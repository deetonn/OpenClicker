#pragma once

#include "Environment.hpp"
#include "Forward.hpp"

#include <string>
#include <vector>
#include <optional>

using BuiltinCommandFunction = int32_t(*)(
	Console*,
	std::vector<std::string_view>*);

struct BuiltinCommandOptionalData {
	std::string_view description;
};

struct BuiltinCommandData {
	std::string_view name;
	BuiltinCommandFunction function;
	std::optional<BuiltinCommandOptionalData> optional_data;
};

class BuiltinCommand
{
private:
	BuiltinCommandData m_command_info;
public:
	BuiltinCommand(BuiltinCommandData data)
		: m_command_info(data)
	{}
};

