#include "Renderer.hpp"

#include "Console.hpp"

#include <thread>
#include <tuple>

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
		ImGui::Text("The %s is currently disabled.", text);
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
		ImGui::Text("The %s is currently disabled.", text);
	}
}

static const char* cc_click_type_getter(void* data, int index)
{
	auto click_types = (ClickType*)data;
	if (index == static_cast<int>(ClickType::SingleClick)) {
		return "Single Click";
	}
	else if (index == static_cast<int>(ClickType::DoubleClick)) {
		return "Double Click";
	}

	return "UNHANDLED CLICK TYPE?";
}

static const char* cc_mouse_button_getter(void* data, int index)
{
	if (index == static_cast<int>(MouseButton::LeftClick)) {
		return "Left Mouse Button";
	}
	else if (index == static_cast<int>(MouseButton::RightClick)) {
		return "Right Mouse Button";
	}
	else if (index == static_cast<int>(MouseButton::TopSideButton)) {
		return "Top Side Button";
	}
	else if (index == static_cast<int>(MouseButton::BottomSideButton)) {
		return "Bottom Side Button";
	}
	return "UNHANDLED MOUSE BUTTON?";
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
	ImGui::Text("ClickTypeIdx: %i", rcontext.selected_click_type);
	ImGui::Text("ButtonTypeIdx: %i", rcontext.selected_mouse_button);

	ImGui::End();
}

static void create_mouse_event_double(
	INPUT* inputs,
	RenderingContext& context, 
	MouseButton button
) noexcept
{
	std::int32_t x = 0, y = 0;

	if (context.coords[0] == 0 || context.coords[1] == 0) {
		POINT point = {};
		GetCursorPos(&point);
		x = point.x;
		y = point.y;
	}
	else {
		x = context.coords[0];
		y = context.coords[1];
	}

	auto* base = inputs;

	if (button == MouseButton::LeftClick) {
		base[0].type = INPUT_MOUSE;
		base[0].mi.dx = x;
		base[0].mi.dy = y;
		base[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		base[1].type = INPUT_MOUSE;
		base[1].mi.dx = x;
		base[1].mi.dy = y;
		base[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

		base[2].type = INPUT_MOUSE;
		base[2].mi.dx = x;
		base[2].mi.dy = y;
		base[2].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		base[3].type = INPUT_MOUSE;
		base[3].mi.dx = x;
		base[3].mi.dy = y;
		base[3].mi.dwFlags = MOUSEEVENTF_LEFTUP;

		context.stats.total_left_clicks += 2;
	}
	else if (button == MouseButton::RightClick) {
		base[0].type = INPUT_MOUSE;
		base[0].mi.dx = x;
		base[0].mi.dy = y;
		base[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

		base[1].type = INPUT_MOUSE;
		base[1].mi.dx = x;
		base[1].mi.dy = y;
		base[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

		base[2].type = INPUT_MOUSE;
		base[2].mi.dx = x;
		base[2].mi.dy = y;
		base[2].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

		base[3].type = INPUT_MOUSE;
		base[3].mi.dx = x;
		base[3].mi.dy = y;
		base[3].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

		context.stats.total_right_clicks += 2;
	}
	else if (button == MouseButton::TopSideButton) {
		// The top mouse button is XBUTTON1
		inputs[0].type = INPUT_MOUSE;
		inputs[0].mi.dx = x;
		inputs[0].mi.dy = y;
		inputs[0].mi.mouseData = XBUTTON1;
		inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;

		inputs[1].type = INPUT_MOUSE;
		inputs[1].mi.dx = x;
		inputs[1].mi.dy = y;
		inputs[1].mi.mouseData = XBUTTON1;
		inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;

		inputs[2].type = INPUT_MOUSE;
		inputs[2].mi.dx = x;
		inputs[2].mi.dy = y;
		inputs[2].mi.mouseData = XBUTTON1;
		inputs[2].mi.dwFlags = MOUSEEVENTF_XDOWN;

		inputs[3].type = INPUT_MOUSE;
		inputs[3].mi.dx = x;
		inputs[3].mi.dy = y;
		inputs[3].mi.mouseData = XBUTTON1;
		inputs[3].mi.dwFlags = MOUSEEVENTF_XUP;

		context.stats.total_top_mb_clicks += 2;
	}
	else if (button == MouseButton::BottomSideButton) {
		inputs[0].type = INPUT_MOUSE;
		inputs[0].mi.dx = x;
		inputs[0].mi.dy = y;
		inputs[0].mi.mouseData = XBUTTON2;
		inputs[0].mi.dwFlags = MOUSEEVENTF_XDOWN;

		inputs[1].type = INPUT_MOUSE;
		inputs[1].mi.dx = x;
		inputs[1].mi.dy = y;
		inputs[1].mi.mouseData = XBUTTON2;
		inputs[1].mi.dwFlags = MOUSEEVENTF_XUP;

		inputs[2].type = INPUT_MOUSE;
		inputs[2].mi.dx = x;
		inputs[2].mi.dy = y;
		inputs[2].mi.mouseData = XBUTTON2;
		inputs[2].mi.dwFlags = MOUSEEVENTF_XDOWN;

		inputs[3].type = INPUT_MOUSE;
		inputs[3].mi.dx = x;
		inputs[3].mi.dy = y;
		inputs[3].mi.mouseData = XBUTTON2;
		inputs[3].mi.dwFlags = MOUSEEVENTF_XUP;

		context.stats.total_bottom_mb_clicks += 2;
	}
	else {
		auto* logger = Logger::the();
		logger->warn("Unhandled mouse button: {}", static_cast<int>(button));
	}
}

// This function returns the actual input event.
static void create_mouse_event(INPUT* inputs, int index, RenderingContext& context, MouseButton button) noexcept
{
	bool is_double_click = context.all_click_types[context.selected_click_type] == ClickType::DoubleClick;

	if (is_double_click) {
		create_mouse_event_double(inputs, context, button);
		return;
	}

	std::int32_t x = 0, y = 0;

	if (context.coords[0] == 0 || context.coords[1] == 0) {
		POINT point = {};
		GetCursorPos(&point);
		x = point.x;
		y = point.y;
	}
	else {
		x = context.coords[0];
		y = context.coords[1];
	}

	// Cast is for clarity
	auto* base = (index >= 2) ? (INPUT*)inputs + 2 : inputs;

	if (button == MouseButton::LeftClick) {
		base[0].type = INPUT_MOUSE;
		base[0].mi.dx = x;
		base[0].mi.dy = y;
		base[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		base[1].type = INPUT_MOUSE;
		base[1].mi.dx = x;
		base[1].mi.dy = y;
		base[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

		context.stats.total_left_clicks += 1;
	}
	else if (button == MouseButton::RightClick) {
		base[0].type = INPUT_MOUSE;
		base[0].mi.dx = x;
		base[0].mi.dy = y;
		base[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

		base[1].type = INPUT_MOUSE;
		base[1].mi.dx = x;
		base[1].mi.dy = y;
		base[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

		context.stats.total_right_clicks += 1;
	}
	else if (button == MouseButton::TopSideButton) {
		// The top mouse button is XBUTTON1
		base[0].type = INPUT_MOUSE;
		base[0].mi.dx = x;
		base[0].mi.dy = y;
		base[0].mi.mouseData = XBUTTON1;
		base[0].mi.dwFlags = MOUSEEVENTF_XDOWN;

		base[1].type = INPUT_MOUSE;
		base[1].mi.dx = x;
		base[1].mi.dy = y;
		base[1].mi.mouseData = XBUTTON1;
		base[1].mi.dwFlags = MOUSEEVENTF_XUP;

		context.stats.total_top_mb_clicks += 1;
	}
	else if (button == MouseButton::BottomSideButton) {
		base[0].type = INPUT_MOUSE;
		base[0].mi.dx = x;
		base[0].mi.dy = y;
		base[0].mi.mouseData = XBUTTON2;
		base[0].mi.dwFlags = MOUSEEVENTF_XDOWN;

		base[1].type = INPUT_MOUSE;
		base[1].mi.dx = x;
		base[1].mi.dy = y;
		base[1].mi.mouseData = XBUTTON2;
		base[1].mi.dwFlags = MOUSEEVENTF_XUP;

		context.stats.total_bottom_mb_clicks += 1;
	}
	else {
		auto* logger = Logger::the();
		logger->warn("Unhandled mouse button: {}", static_cast<int>(button));
	}
}

static void clicking_logic(RenderingContext* info) noexcept
{
	if (info->launch_delay != 0) {
		std::this_thread::sleep_for(
			std::chrono::milliseconds(info->launch_delay)
		);
	}

	auto click_type = info->all_mouse_buttons[info->selected_mouse_button];
	auto button = info->all_mouse_buttons[info->selected_mouse_button];

	while (!info->stop_click_threadf) {
		if (GetAsyncKeyState(VK_F7) & 0x8000) {
			// We have to trigger this to sync the UI.
			info->prev_waiting_for_thread_exit = true;
			info->waiting_for_thread_exit = false;
			// Sleep for 100ms to allow the main thread to catch up with changes.
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			// Once we break, we will sync.
			break;
		}

		auto click_type = info->all_click_types[info->selected_click_type];

		if (click_type == ClickType::SingleClick) {
			INPUT inputs[2] = {};

			create_mouse_event(inputs, NULL, *info, button);

			auto sent =
				SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));

			if (sent != ARRAYSIZE(inputs)) {
				// This click failed...
				std::println("Click failed!");
				std::println("GetLastError() = {}", GetLastError());
			}
			else {
				info->stats.total_clicks += 1;
			}
		}
		else if (click_type == ClickType::DoubleClick) {
			INPUT inputs[4] = {};

			create_mouse_event_double(inputs, *info, button);

			auto sent =
				SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));

			if (sent != ARRAYSIZE(inputs)) {
				// This click failed...
				std::println("Click failed!");
				std::println("GetLastError() = {}", GetLastError());
			}
			else {
				info->stats.total_clicks += 2;
			}
		}

		std::this_thread::sleep_for(
			std::chrono::milliseconds(info->millis_between_click)
		);
	}

	// This signifies to the main thread that the working thread has exited successfully.
	info->stop_click_threadf = false;
	ExitThread(0);
}

static bool launch_clicking_thread(RenderingContext& rcontext) {
	rcontext.stop_click_threadf = false;

	rcontext.logln("Launching auto-clicker with {}ms delay.", 
		rcontext.millis_between_click);

	auto handle = CreateThread(
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)clicking_logic,
		&rcontext,
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
	rcontext.set_widget_state(InputWidget::ClickType, State::Clickable);
	rcontext.set_widget_state(InputWidget::MouseButtonSelection, State::Clickable);
}
static void on_start_disable_input(RenderingContext& rcontext) noexcept {
	rcontext.set_widget_state(InputWidget::LaunchDelay, State::Unclickable);
	rcontext.set_widget_state(InputWidget::MillisecondBetweenClick, State::Unclickable);
	rcontext.set_widget_state(InputWidget::Coordinates, State::Unclickable);
	rcontext.set_widget_state(InputWidget::ClickType, State::Unclickable);
	rcontext.set_widget_state(InputWidget::MouseButtonSelection, State::Unclickable);

	rcontext.set_button_state(Button::CoordinatesEnabled, State::Unclickable);
}
static void on_stop_enable_input(RenderingContext& rcontext) noexcept {
	rcontext.set_widget_state(InputWidget::LaunchDelay, State::Clickable);
	rcontext.set_widget_state(InputWidget::MillisecondBetweenClick, State::Clickable);
	rcontext.set_widget_state(InputWidget::Coordinates, State::Clickable);
	rcontext.set_widget_state(InputWidget::ClickType, State::Clickable);
	rcontext.set_widget_state(InputWidget::MouseButtonSelection, State::Clickable);

	rcontext.set_button_state(Button::CoordinatesEnabled, State::Clickable);
}

void core_render_function(ImGuiIO& io, Renderer& renderer, OpenClicker& context)
{
	auto& config = context.config();

	auto& rcontext = renderer.render_context();
	bool debug_enabled = context.is_debug();

	ImGui::SetNextWindowSize(ImVec2(900, 500));

	ImGui::Begin("OpenClicker", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
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

	auto ct_is_disabled = 
		rcontext.get_widget_state(InputWidget::ClickType) == State::Unclickable;

	if (ct_is_disabled) {
		ImGui::Text("The click type selector is currently disabled. Please wait.");
	}
	else {
		ImGui::ListBox("Click Type",
			&rcontext.selected_click_type,
			cc_click_type_getter,
			rcontext.all_click_types,
			click_type_count
		);
		ImGui::SameLine();
		ImGui::Text(":  %s", cc_click_type_getter(rcontext.all_click_types, rcontext.selected_click_type));
		ImGui::SameLine();
		cc_tooltip("Select what type of click will occur.\n"
				   "Single Click - Send one click\n"
				   "Double Click - Sends two clicks"
		);
	}

	auto mbs_is_disabled = rcontext.get_widget_state(InputWidget::MouseButtonSelection) == State::Unclickable;

	if (mbs_is_disabled) {
		ImGui::Text("The mouse button selector is currently disabled. Please wait.");
	}
	else {
		ImGui::ListBox(
			"Mouse Button",
			&rcontext.selected_mouse_button,
			cc_mouse_button_getter,
			rcontext.all_mouse_buttons,
			mouse_button_count
		);
		ImGui::SameLine();
		ImGui::Text(":  %s", cc_mouse_button_getter(rcontext.all_click_types, rcontext.selected_mouse_button));
		ImGui::SameLine();
		cc_tooltip("Select which mouse button to press.\n"
				   "Left Mouse Button - The large button on the left of your mouse.\n"
				   "Right Mouse Button - The large button on the right of your mouse.\n"
				   "Top Side Button - The top button (furthest from you) on the side of your mouse (if applicable)\n"
				   "Button Side Button - The bottom side button (closest to you) on the side of your mouse (if applicable)");
	}

	// ---- Coordinates widget logic ---- // 

	auto ucc_is_disabled =
		rcontext.get_button_state(Button::CoordinatesEnabled) == State::Unclickable;
	if (!ucc_is_disabled) {
		cc_checkbox("Use custom coordinates", &rcontext.coords_enabled, "Enable custom coordinates for the clicks.");
	}
	else {
		ImGui::Text("The Use Custom Coordinates checkbox is currently disabled while the clicker runs.");
	}

	if (rcontext.coords_enabled) {
		cc_int2_input("Specify custom mouse coordinates", 
			rcontext.coords, 
			rcontext.get_widget_state(InputWidget::Coordinates) == State::Unclickable,
			"These are the custom screen coordinates that will be used for the click.\n"
			"If these are set to (for example), 100 - 100, the click will occur at that position on your screen.\n"
			"IMPORTANT: If both these values are zero, the current mouse position is used.\n"
			"IMPORTANT: If this value is disabled, the current position is used."
		);

		if (cc_button(
			"Capture Mouse Coordinates",
			"Press to automatically capture a position on the screen to be used.\n"
			"After pressing, click F7 once your mouse is in the correct position and those coordinates will be used."))
		{
			rcontext.capturing_mouse_coords = true;
		}
	}
	else {
		rcontext.capturing_mouse_coords = false;
	}

	if (rcontext.capturing_mouse_coords) {
		ImGui::Text("Press F7 once you want to capture the mouse position...");
		if (GetAsyncKeyState(VK_F7) & 0x8000) {
			POINT point = {};
			if (!GetCursorPos(&point)) {
				rcontext.logln("GetCursorPos() returned FALSE. (GetLastError() = {})", GetLastError());
			}
			else {
				rcontext.coords[0] = point.x;
				rcontext.coords[1] = point.y;
			}
			rcontext.capturing_mouse_coords = false;
			rcontext.logln("Captured mouse coordinates and auto-filled coords section. (X={}, Y={})", 
				rcontext.coords[0], 
				rcontext.coords[1]
			);
		}
	}

	ImGui::SeparatorText("Stats");

	ImGui::BulletText("Total Mouse Clicks: %zu", rcontext.stats.total_clicks);
	ImGui::TextDisabled("Left Mouse: %zu", rcontext.stats.total_left_clicks);
	ImGui::SameLine();
	ImGui::TextDisabled("Right Mouse: %zu", rcontext.stats.total_right_clicks);
	ImGui::SameLine();
	ImGui::TextDisabled("Side (Furthest): %zu", rcontext.stats.total_top_mb_clicks);
	ImGui::SameLine();
	ImGui::TextDisabled("Side (Closest): %zu", rcontext.stats.total_bottom_mb_clicks);

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

	if (rcontext.stop_click_threadf && !rcontext.waiting_for_thread_exit) {
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
