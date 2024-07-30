#include "ExternalCommand.hpp"

ExternalCommand::ExternalCommand(std::string&& full_path) noexcept
{
	m_full_path = std::move(full_path);
}
