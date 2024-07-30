#pragma once

#include <string>

#include <D3D11.h>

#include "RenderBackend.hpp"

#include "Forward.hpp" // For forward declaration of Console.

void cc_tooltip(const char* text);
bool cc_button(const char* text, const char* tooltip = NULL);
void cc_checkbox(const char* text, bool* checked, const char* tooltip = NULL);

void core_render_function(ImGuiIO& io, Renderer& renderer, Console& context);

/*
* This struct contains all definitions that the core rendering depends on.
* 
* This contains things such as if windows are opened, or if options are checked.
* It must exist before the rendering loop begins, so we don't reset state.
* 
* This will be passed into core_render_function via the renderer.
* 
* NOTE: add default values where possible.
*/
struct RenderingContext {
	bool metrics_window_open{ false };
	bool debug_log_window_open{ false };
	
	// Any output stream begins empty.
	std::string std_output{};
	std::string std_err{};
	std::string std_in{};

	inline void output(const std::string_view data) noexcept {
		std_output += data;
	}
};

class Renderer {
private:
	RenderBackend m_backend;
	RenderingContext m_render_context;
public:
	Renderer(const std::string_view title,
		const std::uint32_t x,
		const std::uint32_t y
	)
		: m_backend(title, x, y)
	{}

	void begin_render_loop(Console& context, Renderer& renderer) noexcept;
	RenderingContext& render_context() noexcept { return m_render_context; }

	void resize_window(std::size_t x, std::size_t y, UINT flags = 0);

	HWND window() noexcept { return m_backend.window_handle(); }
};