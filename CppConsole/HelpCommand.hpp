#pragma once

#include <vector>

#include "Console.hpp"
#include "BuiltinCommand.hpp"

static int32_t builtin_help_command_impl(
	Console* console,
	std::vector<std::string_view>* args
)
{
	return 1;
}

struct HelpCommand {
	inline static BuiltinCommandData load() noexcept {
		auto data = BuiltinCommandData{};

		data.name = "help";
		data.function = builtin_help_command_impl;
		
		auto optional_data = BuiltinCommandOptionalData{};
		optional_data.description = "The builtin help command to display information and commands.";
		data.optional_data = optional_data;

		return data;
	}
};