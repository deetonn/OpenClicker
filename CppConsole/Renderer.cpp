#include "Renderer.hpp"

#include "Console.hpp"

void Renderer::begin_render_loop(Console& context, Renderer& renderer) noexcept
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

/* 
 Beginning of function that only exist in the source and need no definition. 
 These function are specifically called by core_render_function(...) and should
 never be called anywhere else.
*/

static void render_output_functionality(RenderingContext& rcontext) 
{
	ImGui::BeginChild("##OutputWindow");

	ImGui::TextUnformatted(rcontext.std_output.data(),
		rcontext.std_output.data() + rcontext.std_output.size());

	ImGui::EndChild();
}

static void render_debug_functionality(bool debug, RenderingContext& rcontext)
{
	if (!debug) return;

	bool metrics_window_open = false;
	bool debug_log_window_open = false;

	cc_checkbox("Enable Metrics Window",
		&rcontext.metrics_window_open,
		"Enable the metrics window. This is useful for development has lots of rendering information."
	);

	cc_checkbox("Enable Debug Logging",
		&rcontext.debug_log_window_open,
		"Enable/Disable specific logging features. All logs will be directed towards the windows console."
	);

	if (rcontext.metrics_window_open)
		ImGui::ShowMetricsWindow(&rcontext.metrics_window_open);
	if (rcontext.debug_log_window_open)
		ImGui::ShowDebugLogWindow(&rcontext.debug_log_window_open);

	if (cc_button("Insert Text", "Debug tool for inserting text into stdout.")) {
		rcontext.output("Hello, Stdout!\n");
	}

	ImGui::SameLine();
	// Show information about the stdout string.
	ImGui::Text("Stdout(Size=%zu, Capacity=%zu, Ptr=0x%p)",
		rcontext.std_output.size(), 
		rcontext.std_output.capacity(),
		rcontext.std_output.data()
	);

	ImGui::NewLine();
}

void core_render_function(ImGuiIO& io, Renderer& renderer, Console& context)
{
	auto& config = context.config();

	auto& rcontext = renderer.render_context();
	bool debug_enabled = config.get_value<bool>("settings:debug").value_or(false);

	ImGui::Begin("CppConsole *Alpha", NULL, ImGuiWindowFlags_NoSavedSettings);
	if (config.font())
		ImGui::PushFont(config.font());

	// This will render debug functions if settings:debug is enabled.
	render_debug_functionality(debug_enabled, rcontext);
	// This renders the stdout window.
	render_output_functionality(rcontext);

	if (config.font())
		ImGui::PopFont();
	ImGui::End();
}
