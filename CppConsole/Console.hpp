#pragma once

#include "Config.hpp"
#include "Environment.hpp"
#include "CommandHandler.hpp"

class Renderer;

class Console {
private:
	Config& m_config;
	Environment m_env;
	CommandHandler m_command_handler;
	Renderer* m_renderer;

	bool m_still_running{ true };
public:
	inline Console(Config& config, Renderer* renderer)
		: m_config(config)
		, m_env(config)
	{
		auto is_debug_node = config.get_value<bool>("settings:debug");
		if (!is_debug_node) {
			// ?? The config has not been initialized correctly.
			throw std::exception("The configuration has not been initialized correctly. (THIS IS A BUG!)");
		}
		bool is_debug = *is_debug_node;
		Logger::the()->write(LogKind::Info, "Booting (Debug : {})", is_debug);

		if (!is_debug) {
			// Clear the screen without debug, to clear mess.
			this->clear();
		}

		m_command_handler = CommandHandler(this->env());
		m_renderer = renderer;
	}

	int32_t run(int argc, char** argv) noexcept;

	void clear() noexcept;

	bool still_running() const noexcept { return m_still_running; }

	inline Config& config() noexcept {
		return m_config;
	}
	inline Environment& env() noexcept {
		return m_env;
	}
	inline Renderer& renderer() noexcept {
		return *m_renderer;
	}
};