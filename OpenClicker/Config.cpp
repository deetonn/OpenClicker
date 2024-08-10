#include "Config.hpp"
#include "Logger.hpp"

#include "ImGui/imgui.h"

#include <filesystem>
#include <exception>
#include <print>

namespace fs = std::filesystem;

/*
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
*/

#define INSERT(key, value) table->insert_or_assign(get_style_name(key), (value))

static void setup_default_styles(toml::table* table)
{
	// Use the Windows 11 default font.
	table->insert_or_assign("font-family", DEFAULT_FONT_TTF_FILE);
	// The default font size to be used.
	table->insert_or_assign("font-size", DEFAULT_FONT_SIZE);

	INSERT(StyleConfig_WindowPadding, DEFAULT_WINDOW_PADDING);
	INSERT(StyleConfig_WindowRounding, DEFAULT_WINDOW_ROUNDING);
	INSERT(StyleConfig_FramePadding, DEFAULT_FRAME_PADDING);
	INSERT(StyleConfig_FrameRounding, DEFAULT_FRAME_ROUNDING);
	INSERT(StyleConfig_ItemSpacing, DEFAULT_ITEM_SPACING);
	INSERT(StyleConfig_ItemInnerSpacing, DEFAULT_ITEM_INNER_SPACING);
	INSERT(StyleConfig_IndentSpacing, DEFAULT_INDENT_SPACING);
	INSERT(StyleConfig_ScrollBarSize, DEFAULT_SCROLL_BAR_SIZE);
	INSERT(StyleConfig_ScrollBarRounding, DEFAULT_SCROLL_BAR_ROUNDING);
	INSERT(StyleConfig_GrabMinSize, DEFAULT_GRAB_MIN_SIZE);
	INSERT(StyleConfig_GrabRounding, DEFAULT_GRAB_ROUNDING);

	INSERT(StyleConfig_TextColor, DEFAULT_TEXT_COLOR);
	INSERT(StyleConfig_DisabledTextColor, DEFAULT_DISABLED_TEXT_COLOR);
	INSERT(StyleConfig_BackgroundColor, DEFAULT_BACKGROUND_COLOR);
	INSERT(StyleConfig_PopupBackgroundColor, DEFAULT_POPUP_BACKGROUND_COLOR);
	INSERT(StyleConfig_BorderColor, DEFAULT_BORDER_COLOR);
	INSERT(StyleConfig_BorderShadowColor, DEFAULT_BORDER_SHADOW_COLOR);
	INSERT(StyleConfig_FrameBackgroundColor, DEFAULT_FRAME_BACKGROUND_COLOR);
	INSERT(StyleConfig_FrameHoveredBackgroundColor, DEFAULT_FRAME_HOVERED_BACKGROUND_COLOR);
	INSERT(StyleConfig_FrameActiveBackgroundColor, DEFAULT_FRAME_ACTIVE_BACKGROUND_COLOR);
	INSERT(StyleConfig_TitleBackgroundColor, DEFAULT_TITLE_BACKGROUND_COLOR);
	INSERT(StyleConfig_TitleCollapsedColor, DEFAULT_TITLE_COLLAPSED_COLOR);
	INSERT(StyleConfig_TitleActiveBackgroundColor, DEFAULT_TITLE_ACTIVE_COLOR);
	INSERT(StyleConfig_MenuBarBackgroundColor, DEFAULT_MENU_BAR_BACKGROUND_COLOR);
	INSERT(StyleConfig_ScrollBarBackgroundColor, DEFAULT_SCROLLBAR_BACKGROUND_COLOR);
	INSERT(StyleConfig_ScrollBarGrabColor, DEFAULT_SCROLLBAR_GRAB_BACKGROUND_COLOR);
	INSERT(StyleConfig_ScrollBarGrabHovered, DEFAULT_SCROLLBAR_GRAB_HOVERED_COLOR);
	INSERT(StyleConfig_ScrollBarGrabActive, DEFAULT_SCROLLBAR_GRAB_ACTIVE_COLOR);
	INSERT(StyleConfig_CheckMarkColor, DEFAULT_CHECKMARK_COLOR);
	INSERT(StyleConfig_SliderGrabColor, DEFAULT_SLIDER_GRAB_COLOR);
	INSERT(StyleConfig_SliderGrabActiveColor, DEFAULT_SLIDER_GRAB_ACTIVE_COLOR);
	INSERT(StyleConfig_ButtonColor, DEFAULT_BUTTON_COLOR);
	INSERT(StyleConfig_ButtonHoveredColor, DEFAULT_BUTTON_HOVERED_COLOR);
	INSERT(StyleConfig_ButtonActiveColor, DEFAULT_BUTTON_ACTIVE_COLOR);
	INSERT(StyleConfig_HeaderColor, DEFAULT_HEADER_COLOR);
	INSERT(StyleConfig_HeaderHoveredColor, DEFAULT_HEADER_HOVERED_COLOR);
	INSERT(StyleConfig_HeaderActiveColor, DEFAULT_HEADER_ACTIVE_COLOR);
	INSERT(StyleConfig_TextSelectedBackgroundColor, DEFAULT_TEXT_SELECTED_BACKGROUND_COLOR);
}

#undef INSERT

toml::table create_default_configuration() {
	toml::table root_table;

	root_table.insert_or_assign("settings", toml::table{});
	auto settings_table = root_table["settings"].as_table();

	/*
		NOTE: "debug" can obviously be set by the user, but by default 
		in release builds, it is disabled.
	*/
	settings_table->insert_or_assign("debug",
#if _DEBUG
		true
#else
		false
#endif
	);
	settings_table->insert_or_assign("use_experimental_customization", false);

	root_table.insert_or_assign("renderer", toml::table{});
	auto rendering_table = root_table["renderer"].as_table();

	// Enable vsync by default so that we don't do 8000fps for zero
	// reason.
	rendering_table->insert_or_assign("vsync", true);
	rendering_table->insert_or_assign("width", DEFAULT_WINDOW_WIDTH);
	rendering_table->insert_or_assign("height", DEFAULT_WINDOW_HEIGHT);

	root_table.insert_or_assign("style", toml::table{});
	auto style_table = root_table["style"].as_table();

	setup_default_styles(style_table);

	return root_table;
}

ImVec2 array_to_vec2(toml::array* arr)
{
	if (arr->size() < 2) {
		throw std::exception("That array cannot be converted into an ImVec2");
	}
	ImVec2 result = {};
	result.x = arr->at(0).as_integer()->get();
	result.y = arr->at(1).as_integer()->get();
	return result;
}

Config::Config() {
	this->m_core_config = toml::table{};
	this->m_core_config_path = std::string();
}

Config::Config(const std::string& config_directory) {
	if (!fs::exists(config_directory)) {
		// We attempt to create it.
		// NOTE: The usage of create_directories(), we don't know what
		//       config_directory is, so just to be safe we use this.
		if (!fs::create_directories(config_directory)) {
			// We cannot create it or read it.
			throw std::exception("Failed to read/create config directories.");
		}
	}

	this->m_core_config_path =
		(fs::path(config_directory) / fs::path("config.toml")).string();

	if (fs::exists(this->m_core_config_path)) {
		Logger::the()->write(LogKind::Info, "Found configuration file.");
		try {
			this->m_core_config = toml::parse_file(this->m_core_config_path);
		}
		catch (...) {
			throw;
		}
		return;
	}
	else {
		try {
			Logger::the()->write(LogKind::Info, "The configuration file does not exist, creating the default.");

			// The core config file does not exist, create it with its defaults.
			toml::table default_config = create_default_configuration();
			std::ofstream config_file_stream(this->m_core_config_path, 
				std::ofstream::trunc | std::ofstream::out);

			if (config_file_stream.bad()) {
				Logger::the()->write(LogKind::Warn, "Could not create a stream for the default configuration file.");
				return;
			}

			config_file_stream << default_config;
			this->m_core_config = default_config;
		}
		catch (...) {
			throw;
		}
	}
}

Config::~Config() {
	try {
		// The core config file does not exist, create it with its defaults.
		std::ofstream config_file_stream(this->m_core_config_path,
			std::ofstream::out | std::ofstream::trunc);

		config_file_stream << m_core_config;
	}
	catch (const std::exception& exception) {
		// TODO: use a logger instead of std::cout.
		std::print("Failed to save configuration: {}\n", exception.what());
	}
}

// Helper specifically for prepare_assets()
static ImFont* load_font_absolute_path(const fs::path& path, const std::size_t font_size) noexcept {
	auto* logger = Logger::the();
	auto& io = ImGui::GetIO();
	auto ec = std::error_code{};

	auto new_path = fs::current_path() / "assets" / "fonts" / path.filename();

	if (!fs::copy_file(
		path, 
		new_path, ec)
	) 
	{
		if (ec.value() == ERROR_FILE_EXISTS) {
			return io.Fonts->AddFontFromFileTTF(
				(fs::path("./assets") / "fonts" / path.filename()).string().data(),
				static_cast<float>(font_size)
			);
		}

		logger->error("Failed to load specified font ({})", path.string());
		return nullptr;
	}

	return io.Fonts->AddFontFromFileTTF(
		(fs::path("./assets") / "fonts" / path.filename()).string().data(),
		static_cast<float>(font_size)
	);
}

void Config::prepare_assets(Config& config) noexcept
{
	// We need to copy ROOT_FONT_PATH / FONT_FILE into
	// the (CURRENT_DIRECTORY/assets/fonts/FONT_FILE)
	auto* logger = Logger::the();

	auto current_dir = fs::current_path();
	auto assets_root = current_dir / "assets";
	auto fonts_folder = assets_root / "fonts";

	if (!fs::exists(current_dir / "assets")) {
		// Setup the default structure.
		fs::create_directories(assets_root);
		fs::create_directories(fonts_folder);
	}

	auto font_file = config.get_value_as_string("style:font-family")
		.value_or(DEFAULT_FONT_TTF_FILE);
	auto font_size = config.get_value_as_sizet("style:font-size")
		.value_or(DEFAULT_FONT_SIZE);
	ImFont* font = NULL;
	if (fs::exists(font_file)) {
		auto full_font_file_path = fs::path(font_file);
		font = load_font_absolute_path(full_font_file_path, font_size);
	} else {
		auto full_font_file_path = fs::path(ROOT_FONT_PATH) / font_file;
		font = load_font_absolute_path(full_font_file_path, font_size);
	}

	if (!font) {
		// Just load the default.
		auto full_default_path = fs::path(ROOT_FONT_PATH) / DEFAULT_FONT_TTF_FILE;
		font = load_font_absolute_path(full_default_path, font_size);
	}

	if (!font) {
		// We cannot load a font, use the absolute default.
		logger->warn("Exhaused all font options, using the absolute default.");
		auto& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();
	}
}