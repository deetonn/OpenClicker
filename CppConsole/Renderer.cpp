#include "Renderer.hpp"

#include "Console.hpp"

#include <thread>

void Renderer::begin_render_loop(OpenClicker& context, Renderer& renderer) noexcept
{
	// Simple enough, start the backend with our core_render_function
	m_backend.render(core_render_function, context, renderer);
}

void Renderer::resize_window(std::size_t x, std::size_t y, UINT flags)
{
	// We add 0.15 back, to give room.
	std::size_t adjusted_x = static_cast<std::size_t>(x + (x * 0.15));
	std::size_t adjusted_y = static_cast<std::size_t>(y + (y * 0.15));

	SetWindowPos(window(),
		NULL, NULL, NULL,
		adjusted_x, adjusted_y, flags);

	ImGui::SetWindowSize(ImVec2(adjusted_x, adjusted_y));
}

void cc_tooltip(const char* text) {
	if (text == NULL) {
		return;
	}
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool cc_button(const char* text, const char* tooltip)
{
	bool clicked = ImGui::Button(text);
	ImGui::SameLine();
	cc_tooltip(tooltip);
	return clicked;
}

void cc_checkbox(const char* text, bool* checked, const char* tooltip)
{
	ImGui::Checkbox(text, checked);
	ImGui::SameLine();
	cc_tooltip(tooltip);
}

void cc_int_input(const char* text, int* value, const bool disabled, const char* tooltip) 
{
	if (!disabled) {
		ImGui::InputInt(text, value, 10);
		if (tooltip) {
			ImGui::SameLine();
			cc_tooltip(tooltip);
		}
	}
	else {
		cc_tooltip("This input is disabled while the auto-clicker is running.\n"
				   "Press \"Stop\" to change settings.");
	}
}

void cc_int2_input(const char* text, int values[2], const bool disabled, const char* tooltip)
{
	if (!disabled) {
		ImGui::InputInt2(text, values, 10);
		if (tooltip) {
			ImGui::SameLine();
			cc_tooltip(tooltip);
		}
	}
	else {
		cc_tooltip("This input is disabled while the auto-clicker is running.\n"
			"Press \"Stop\" to change settings.");
	}
}

/* 
 Beginning of function that only exist in the source and need no definition. 
 These function are specifically called by core_render_function(...) and should
 never be called anywhere else.
*/

static void debug_window(RenderingContext& rcontext) noexcept {
	ImGui::Begin("Debug Information");

	// Button States
	for (size_t i = 0; i < button_count; ++i) {
		Button button = static_cast<Button>(i);
		auto state = rcontext.get_button_state(button);

		ImGui::Text("Button #%zu: %s", 
			button, 
			state == State::Clickable ? "Clickable" : "Unclickable"
		);
	}

	ImGui::NewLine();

	// Widget States
	for (size_t i = 0; i < widget_count; ++i) {
		InputWidget widget = static_cast<InputWidget>(i);
		auto state = rcontext.get_widget_state(widget);

		ImGui::Text("Widget #%zu: %s",
			widget,
			state == State::Clickable ? "Enabled" : "Disabled");
	}

	ImGui::Text("MillisBetweenClick: %i", rcontext.millis_between_click);
	ImGui::Text("Coords: (%i, %i)", rcontext.coords[0], rcontext.coords[1]);
	ImGui::Text("ThreadExitSignal: %s", rcontext.stop_click_threadf ? "Yes" : "No");
	ImGui::Text("ThreadHasExited: %s", rcontext.stop_click_threadf == false ? "Yes" : "No");
	ImGui::Text("ThreadSignals: (WaitingForThreadExit: %s, PrevWaitingForThreadExit: %s)",
		rcontext.waiting_for_thread_exit ? "Yes" : "No",
		rcontext.prev_waiting_for_thread_exit ? "Yes" : "No"
	);

	ImGui::End();
}

static void clicking_logic(ClickInfo info) noexcept
{
	if (info.launch_delay != 0) {
		std::this_thread::sleep_for(
			std::chrono::milliseconds(info.launch_delay)
		);
	}

	while (!*info.stop) {
		if (GetAsyncKeyState(VK_F7) & 0x8000) {
			*info.stop = true;
			break;
		}

		INPUT inputs[2] = {};

		POINT cursor_pos = {};
		GetCursorPos(&cursor_pos);

		inputs[0].type = INPUT_MOUSE;
		inputs[0].mi.dx = cursor_pos.x;
		inputs[0].mi.dy = cursor_pos.y;
		inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		inputs[1].type = INPUT_MOUSE;
		inputs[1].mi.dx = cursor_pos.x;
		inputs[1].mi.dy = cursor_pos.y;
		inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

		auto sent = 
			SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));

		if (sent != ARRAYSIZE(inputs)) {
			// This click failed...
			std::println("Click failed!");
			std::println("GetLastError() = {}", GetLastError());
		}

		std::this_thread::sleep_for(
			std::chrono::milliseconds(info.ms_between_clicks)
		);
	}

	// This signifies to the main thread that the working thread has exited successfully.
	*info.stop = false;
	ExitThread(0);
}

static bool launch_clicking_thread(RenderingContext& rcontext) {
	rcontext.click_info.ms_between_clicks = rcontext.millis_between_click;
	rcontext.click_info.stop = &rcontext.stop_click_threadf;
	rcontext.click_info.launch_delay = rcontext.launch_delay;
	rcontext.stop_click_threadf = false;

	rcontext.logln("Launching auto-clicker with {}ms delay.", 
		rcontext.millis_between_click);

	auto handle = CreateThread(
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)clicking_logic,
		&rcontext.click_info,
		NULL,
		NULL);

	rcontext.clicking_thread = handle;
	return handle != INVALID_HANDLE_VALUE;
}

static void setup_default_states(RenderingContext& rcontext) noexcept {
	rcontext.set_button_state(Button::Start, State::Clickable);
	rcontext.set_button_state(Button::Stop, State::Unclickable);

	rcontext.set_widget_state(InputWidget::LaunchDelay, State::Clickable);
	rcontext.set_widget_state(InputWidget::MillisecondBetweenClick, State::Clickable);
	// NOTE: The state of Coordinates must always be State::Clickable; the logic on whether on not to
	//       show this widget is handled with a checkbox (rcontext.coords_enabled)
	rcontext.set_widget_state(InputWidget::Coordinates, State::Clickable);
}
static void on_start_disable_input(RenderingContext& rcontext) noexcept {
	rcontext.set_widget_state(InputWidget::LaunchDelay, State::Unclickable);
	rcontext.set_widget_state(InputWidget::MillisecondBetweenClick, State::Unclickable);
	rcontext.set_widget_state(InputWidget::Coordinates, State::Unclickable);
}
static void on_stop_enable_input(RenderingContext& rcontext) noexcept {
	rcontext.set_widget_state(InputWidget::LaunchDelay, State::Clickable);
	rcontext.set_widget_state(InputWidget::MillisecondBetweenClick, State::Clickable);
	rcontext.set_widget_state(InputWidget::Coordinates, State::Clickable);
}

void core_render_function(ImGuiIO& io, Renderer& renderer, OpenClicker& context)
{
	auto& config = context.config();

	auto& rcontext = renderer.render_context();
	bool debug_enabled = context.is_debug();

	renderer.resize_window(1920, 1080);

	ImGui::Begin("CppConsole *Alpha", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
	if (config.font())
		ImGui::PushFont(config.font());

	if (rcontext.first_render_call) {
		setup_default_states(rcontext);
		rcontext.first_render_call = false;
	}

	ImGui::SeparatorText("Options");

	// ---- Input for milliseconds between mouse clicks ---- //

	auto mbc_is_disabled =
		rcontext.get_widget_state(InputWidget::MillisecondBetweenClick) == State::Unclickable;

	cc_int_input("Milliseconds between clicks",
		&rcontext.millis_between_click,
		mbc_is_disabled,
		"The amount of milliseconds that will pass between each automated click."
		"\nNOTE: This number cannot be below zero."
		"\nNOTE: 1000ms = 1s (So for example, a 2 second delay would be 2000ms)"
	);

	auto ld_is_disabled =
		rcontext.get_widget_state(InputWidget::LaunchDelay) == State::Unclickable;

	cc_int_input("Launch Delay",
		&rcontext.launch_delay,
		ld_is_disabled,
		"How many milliseconds to wait before launching the auto-clicker.\n"
		"For example, if this is set to 2000, once you press start it will take 2 seconds for it to start clicking.\n"
	);

	// Sanitize the millis to never go below zero.
	// A zero/negative timeout would cause serious issues with Sleep(...)
	if (rcontext.millis_between_click <= 0) {
		rcontext.millis_between_click = 0;
	}

	// ---- Coordinates widget logic ---- // 

	cc_checkbox("Use custom coordinates", &rcontext.coords_enabled, "Enable custom coordinates for the clicks.");

	if (rcontext.coords_enabled) {
		ImGui::SameLine();
		cc_int2_input("Specify custom mouse coordinates", 
			rcontext.coords, 
			rcontext.get_widget_state(InputWidget::Coordinates) == State::Unclickable,
			"These are the custom screen coordinates that will be used for the click.\n"
			"If these are set to (for example), 100 - 100, the click will occur at that position on your screen.\n"
			"IMPORTANT: If both these values are zero, the current mouse position is used.\n"
			"IMPORTANT: If this value is disabled, the current position is used."
		);
	}

	ImGui::SeparatorText("Actions");

	// ---- Start button logic ---- //

	if (rcontext.get_button_state(Button::Start) == State::Clickable && rcontext.waiting_for_thread_exit != true) {
		if (cc_button("Start", "Begin auto-clicking.")) {
			// Begin auto-clicking thread..
			rcontext.set_button_state(Button::Start, State::Unclickable);
			rcontext.set_button_state(Button::Stop, State::Clickable);

			on_start_disable_input(rcontext);

			if (!launch_clicking_thread(rcontext)) {
				// We failed to launch the thread.
				rcontext.set_button_state(Button::Start, State::Unclickable);
				rcontext.set_button_state(Button::Stop, State::Clickable);
				on_stop_enable_input(rcontext);

				rcontext.logln("Failed to launch clicking thread. (GetLastError() returned 0x{:x})", GetLastError());
			}
		}
	}

	// ---- Stop button logic ---- //

	if (rcontext.get_button_state(Button::Stop) == State::Clickable && rcontext.waiting_for_thread_exit != true) {
		if (cc_button("Stop (F7)", "Stop auto-clicking.")) {
			rcontext.stop_click_threadf = true;
			// The working thread will set the above variable to false once it has exited.
			// We must disable all buttons until this has happened.
			rcontext.waiting_for_thread_exit = true;

			rcontext.logln("Signaled clicking thread to exit, please wait...");
		}
	}

	ImGui::SameLine();

	if (cc_button("Clear Log", "Clear the log below this button.")) {
		rcontext.log_text.clear();
		rcontext.logln("The log was cleared.");
	}

	if (rcontext.waiting_for_thread_exit) {
		ImGui::Text("Waiting for clicker thread to exit, please wait...");
		rcontext.prev_waiting_for_thread_exit = rcontext.waiting_for_thread_exit;
		rcontext.waiting_for_thread_exit = rcontext.stop_click_threadf == true;
	}
	else {
		if (rcontext.prev_waiting_for_thread_exit) {
			rcontext.prev_waiting_for_thread_exit = false;
			on_stop_enable_input(rcontext);
			rcontext.set_button_state(Button::Start, State::Clickable);
			rcontext.set_button_state(Button::Stop, State::Unclickable);

			CloseHandle(rcontext.clicking_thread);
			rcontext.clicking_thread = INVALID_HANDLE_VALUE;
		}
	}

	// We check if the thread has terminated itself (Via F7) 
	// And sort the state out.

	if (rcontext.stop_click_threadf) {
		if (rcontext.get_button_state(Button::Start) == State::Unclickable) 
		{
			// We need to clean.
			rcontext.set_button_state(Button::Stop, State::Unclickable);
			rcontext.set_button_state(Button::Start, State::Clickable);
			on_stop_enable_input(rcontext);
		}
	}

	ImGui::BeginChild("LogSection");
	ImGui::TextUnformatted(rcontext.log_text.data());
	ImGui::EndChild();

	if (debug_enabled) {
		debug_window(rcontext);
	}

	if (config.font())
		ImGui::PopFont();
	ImGui::End();
}
