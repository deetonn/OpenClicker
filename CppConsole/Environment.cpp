#include "Environment.hpp"

#include "Logger.hpp"

Environment::Environment(Config& config) noexcept {
	auto* logger = Logger::the();

	// FIXME: We probably shouldn't use this function lmfao.
	char** env_ptr = environ;
	std::size_t position = 0;

	while (env_ptr[position] != NULL) {
		std::string current_string = env_ptr[position++];
		const auto position_of_equals = current_string.find('=');

		if (position_of_equals == std::string::npos) {
			logger->write(LogKind::Warn, "Invalid string found in environment: '{}'", current_string);
			continue;
		}

		auto key = current_string.substr(0, position_of_equals);
		auto value = current_string.substr(position_of_equals + 1, current_string.size());

		m_env_map.insert(
			std::make_pair(
				// We just want to copy the pointer inside, as it points
				// directly into the environment block.
				std::move(key), std::move(value)
			)
		);
	}

	logger->write(LogKind::Info, "Loaded {} system environment variables.", m_env_map.size());

	// Check if the user has requested any custom environment variables.
	auto custom_environment_variable_nodes = config.get_value_as_array("env:autoload_custom_variables");

	if (!custom_environment_variable_nodes) {
		return;
	}

	auto& custom_environment_variables = *custom_environment_variable_nodes;
	for (auto& item : *custom_environment_variables) {
		if (!item.is_string()) {
			logger->write(LogKind::Info, "All custom environment variables must be a string.");
			continue;
		}
		// We expect the usual syntax.
		auto& actual_env_string = item.as_string()->get();
		const std::size_t position_of_equals = actual_env_string.find('=');

		if (position_of_equals == std::string::npos) {
			logger->write(LogKind::Warn, "Custom environment variable has invalid syntax. ({})", actual_env_string);
			continue;
		}

		auto key = actual_env_string.substr(0, position_of_equals);
		auto value = actual_env_string.substr(position_of_equals + 1, actual_env_string.size());

		if (m_env_map.contains(key)) {
			logger->write(LogKind::Warn, "Overwriting already initialized environment variable: {}", key);
		}

		m_env_map.insert_or_assign(key, value);

		logger->write(LogKind::Info, "Loaded custom environment variable {} (Value '{}')", key, value);
	}
}

bool Environment::query(const std::string_view key, std::string* out_ptr) noexcept
{
	if (!m_env_map.contains(key.data())) {
		return false;
	}

	*out_ptr = m_env_map.at(key.data());
	return true;
}

bool Environment::insert(const std::string_view key, const std::string_view value) noexcept
{
	if (m_env_map.contains(key.data())) {
		// Cannot insert something that already exists.
		return false;
	}

	m_env_map.insert(std::make_pair(key.data(), value.data()));
	return true;
}

void Environment::dump_to_console() const noexcept
{
	std::print(" -- DUMPING ENVIRONMENT --\n");

	for (auto& [key, value] : m_env_map) {
		std::print("{}: {}\n", key, value);
	}
}
