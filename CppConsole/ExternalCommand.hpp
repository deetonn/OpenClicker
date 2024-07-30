#pragma once

#include <string>

class ExternalCommand
{
private:
	std::string m_full_path;
public:
	ExternalCommand(std::string&& full_path) noexcept;
};

