#pragma once

#include "Config.hpp"

class Renderer;
class RenderingContext;

class OpenClicker {
private:
	Config& m_config;
	Renderer* m_renderer;

	bool m_still_running{ true };
	bool m_is_debug{ false };
public:
	inline OpenClicker(Config& config, Renderer* renderer)
		: m_config(config)
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
			this->clear_console();
		}

		m_renderer = renderer;

		m_is_debug = config.get_value<bool>("settings:debug").value_or(false);
	}

	int32_t run(int argc, char** argv) noexcept;

	void clear_console() noexcept;

	bool still_running() const noexcept { return m_still_running; }
	bool is_debug() const noexcept { return m_is_debug; }

	inline Config& config() noexcept {
		return m_config;
	}
	inline Renderer& renderer() noexcept {
		return *m_renderer;
	}
};