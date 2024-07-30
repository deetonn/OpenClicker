#pragma once

#include "Config.hpp"

#include <unordered_map>
#include <string>

class Environment
{
private:
	std::unordered_map<std::string, std::string> m_env_map;
public:
	using value_type = decltype(m_env_map)::value_type;
	using size_type = decltype(m_env_map)::size_type;

	Environment(Config& config) noexcept;

	bool query(const std::string_view key, std::string* out_ptr) noexcept;
	bool insert(const std::string_view key, const std::string_view value) noexcept;

	void dump_to_console() const noexcept;

	inline decltype(m_env_map)::iterator begin() noexcept {
		return m_env_map.begin();
	}
	inline decltype(m_env_map)::iterator end() noexcept {
		return m_env_map.end();
	}
	inline std::size_t size() const noexcept { return m_env_map.size(); }
};

