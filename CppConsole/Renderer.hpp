#pragma once

#include <string>
#include <bitset>
#include <format>

#include <D3D11.h>
#include <Windows.h>

#include "RenderBackend.hpp"

#include "Forward.hpp" // For forward declaration of Console.

#define INPUT_BUFFER_SIZE 512

void cc_tooltip(const char* text);
bool cc_button(const char* text, const char* tooltip = NULL);
void cc_checkbox(const char* text, bool* checked, const char* tooltip = NULL);
void cc_int_input(
	const char* text, 
	int* value,
	const bool disabled,
	const char* tooltip = NULL
);
void cc_int2_input(
	const char* text, 
	int values[2], 
	const bool disabled, 
	const char* tooltip = NULL
);

void core_render_function(ImGuiIO& io, Renderer& renderer, OpenClicker& context);

// This struct is only to be passed into the worker thread.
// It must never be modified manually. (Other than when setting it up)
// It is setup by [Renderer.cpp:launch_clicking_thread(...)]
struct ClickInfo {
	int ms_between_clicks;
	int launch_delay;

	/*
	* If this is set to true externally, the thread must stop.
	* The clicking thread must then also set this to false once it has stopped
	* and is about to exit. This is to keep UI synced with actual functionality.
	* 
	* Without this, the user would be able to launch a new thread without the already
	* running one stopping. We acheive this safety by the "Start" button not being visible
	* until the worker thread has actually exited.
	*/
	bool* stop;
};

enum class InputWidget {
	MillisecondBetweenClick,
	LaunchDelay,
	Coordinates,
	CountOfWidgetsDontMove
};

constexpr std::size_t widget_count =
	static_cast<std::size_t>(InputWidget::CountOfWidgetsDontMove);

enum class Button : uint8_t {
	Start, 
	Stop,
	CoordinatesEnabled,
	CountOfButtonsDontMove
};

constexpr std::size_t button_count = 
	static_cast<std::size_t>(Button::CountOfButtonsDontMove);

enum class State {
	Clickable,
	Unclickable
};

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
	std::bitset<button_count> button_states;
	std::bitset<widget_count> widget_states;

	HANDLE clicking_thread{ INVALID_HANDLE_VALUE };
	// Set to true to stop clicking_thread.
	bool stop_click_threadf{ false };
	bool prev_waiting_for_thread_exit{ false };
	bool waiting_for_thread_exit{ false };

	bool first_render_call{ true };
	std::int32_t millis_between_click{ 1 };
	std::int32_t launch_delay{ 1 };
	ClickInfo click_info;

	bool coords_enabled{ false };
	int coords[2] = { 0, 0 };

	std::string log_text{"This is the log text, useful information will live here.\n"};

	template <class... Ts>
	inline void logln(const std::format_string<Ts...> fmt, Ts&&... args) noexcept {
		auto formatted_text = std::format(fmt, std::forward<Ts>(args)...);
		log_text += formatted_text;
		log_text += "\n";
	}

	inline State get_button_state(Button button) const noexcept {
		if (button == Button::CountOfButtonsDontMove)
			return State::Unclickable;
		return static_cast<State>(
			(bool)button_states[static_cast<std::size_t>(button)]
		);
	}
	inline void set_button_state(Button button, State state) noexcept {
		if (button == Button::CountOfButtonsDontMove)
			return;
		button_states.set(static_cast<size_t>(button), static_cast<bool>(state));
	}

	inline State get_widget_state(InputWidget widget) noexcept {
		if (widget == InputWidget::CountOfWidgetsDontMove)
			return State::Unclickable;
		return static_cast<State>(
			(bool)widget_states[static_cast<std::size_t>(widget)]
		);
	}

	inline void set_widget_state(InputWidget widget, State state) noexcept {
		if (widget == InputWidget::CountOfWidgetsDontMove)
			return;
		widget_states.set(static_cast<size_t>(widget), static_cast<bool>(state));
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

	void begin_render_loop(OpenClicker& context, Renderer& renderer) noexcept;
	RenderingContext& render_context() noexcept { return m_render_context; }

	void resize_window(std::size_t x, std::size_t y, UINT flags = 0);

	HWND window() noexcept { return m_backend.window_handle(); }
};