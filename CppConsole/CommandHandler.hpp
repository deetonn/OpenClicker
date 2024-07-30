#pragma once

#include <vector>
#include <array>

#include "BuiltinCommand.hpp"
#include "ExternalCommand.hpp"

// FIXME: This is causing issues.
#include "Environment.hpp"

class CommandHandler
{
private:
	std::vector<BuiltinCommand> m_builtins;
	std::vector<ExternalCommand> m_externals;
public:
	static std::array<BuiltinCommandData, 1> builtin_commands;

	CommandHandler() = default;
	CommandHandler(Environment& context);

	std::vector<BuiltinCommand>& builtins() noexcept { return m_builtins; }
	std::vector<BuiltinCommand> const& builtins() const noexcept { return m_builtins; }

	std::vector<ExternalCommand>& externals() noexcept { return m_externals; }
	std::vector<ExternalCommand> const& externals() const noexcept { return m_externals; }
};

