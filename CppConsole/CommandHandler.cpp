#include "CommandHandler.hpp"
#include "Logger.hpp"
#include "Helpers.hpp"

// The actual builtin commands.
#include "HelpCommand.hpp"

#include <ranges>
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

constexpr std::string_view PATH_ENVIRONMENT_VARIABLE_NAME = "Path";

std::array<BuiltinCommandData, 1> CommandHandler::builtin_commands = {
	HelpCommand::load()
};

CommandHandler::CommandHandler(Environment& env)
{
	auto* logger = Logger::the();
	std::string path_environment_variable;
	
	if (!env.query(PATH_ENVIRONMENT_VARIABLE_NAME, &path_environment_variable)) {
		env.dump_to_console();
		throw std::exception("The PATH environment variable does not exist on this system.");
	}

	// This is designed for windows, so don't worry about different path
	// delimeters.

	auto paths = string_split(path_environment_variable, ";");

	for (const auto& path : paths) {
		WIN32_FIND_DATAA find_data = {};
		CHAR file_path[MAX_PATH] = {};

		// NOTE: +2 for the \\* we have to append.
		if (path.size() + 2 > MAX_PATH) {
			logger->warn("The path '{}' is too long to process file information.", path.data());
			continue;
		}

		std::memcpy(file_path, path.data(), path.size());
		std::memcpy(file_path + path.size(), "\\*", 2);

		HANDLE handle_to_finder = FindFirstFileA(file_path, &find_data);

		if (INVALID_HANDLE_VALUE == handle_to_finder) {
			logger->warn("Failed to create handle to finder for path '{}'", path.data());
			continue;
		}

		do {
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			auto actual_file_name =
				(fs::path(path) / find_data.cFileName).string();
			if (!actual_file_name.ends_with(".exe")) {
				continue;
			}
			// This is the path to a PATH exe.
			m_externals.emplace_back(std::move(actual_file_name));
		} while (FindNextFileA(handle_to_finder, &find_data));

		FindClose(handle_to_finder);
	}

	logger->info("Initialized {} externally loaded commands", m_externals.size());

	// Now we load the builtin commands.
	for (auto& item : builtin_commands) {
		m_builtins.emplace_back(item);
	}

	logger->info("Initialized {} builtin (internally) loaded commands.", m_builtins.size());
}
