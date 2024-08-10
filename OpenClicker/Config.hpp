#pragma once

#include "Logger.hpp"

#include <toml.hpp>

#include <string>
#include <optional>
#include <array>
#include <variant>

#include "ImGui/imgui.h"

#define DEFAULT_FONT_SIZE 19
#define DEFAULT_FONT_TTF_FILE "segoeuib.ttf"

#define ROOT_FONT_PATH "C:\\Windows\\Fonts\\"

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

#define STYLE_CFG_KEY(key) "style:" key 

#define DEFAULT_WINDOW_PADDING toml::array(15, 15)
#define DEFAULT_WINDOW_ROUNDING 5.0
#define DEFAULT_FRAME_PADDING toml::array(5, 5)
#define DEFAULT_FRAME_ROUNDING 4.0
#define DEFAULT_ITEM_SPACING toml::array(12, 8)
#define DEFAULT_ITEM_INNER_SPACING toml::array(8, 6)
#define DEFAULT_INDENT_SPACING 25.0
#define DEFAULT_SCROLL_BAR_SIZE 15.0
#define DEFAULT_SCROLL_BAR_ROUNDING 9.0
#define DEFAULT_GRAB_MIN_SIZE 5.0
#define DEFAULT_GRAB_ROUNDING 3.0

#define DEFAULT_TEXT_COLOR toml::array(0.80, 0.80, 0.83, 1.00)
#define DEFAULT_DISABLED_TEXT_COLOR toml::array(0.24, 0.23, 0.29, 1.00)
#define DEFAULT_BACKGROUND_COLOR toml::array(0.06, 0.05, 0.07, 1.00)
#define DEFAULT_POPUP_BACKGROUND_COLOR toml::array(0.07, 0.07, 0.09, 1.00)
#define DEFAULT_BORDER_COLOR toml::array(0.80, 0.80, 0.83, 0.88)
#define DEFAULT_BORDER_SHADOW_COLOR toml::array(0.92, 0.91, 0.88, 0.00)
#define DEFAULT_FRAME_BACKGROUND_COLOR toml::array(0.10, 0.09, 0.12, 1.00)
#define DEFAULT_FRAME_HOVERED_BACKGROUND_COLOR toml::array(0.24, 0.23, 0.29, 1.00)
#define DEFAULT_FRAME_ACTIVE_BACKGROUND_COLOR toml::array(0.56, 0.56, 0.58, 1.00)
#define DEFAULT_TITLE_BACKGROUND_COLOR toml::array(0.10, 0.09, 0.12, 1.00)
#define DEFAULT_TITLE_COLLAPSED_COLOR toml::array(1.00, 0.98, 0.95, 0.75)
#define DEFAULT_TITLE_ACTIVE_COLOR toml::array(0.07, 0.07, 0.09, 1.00)
#define DEFAULT_MENU_BAR_BACKGROUND_COLOR toml::array(0.10, 0.09, 0.12, 1.00)
#define DEFAULT_SCROLLBAR_BACKGROUND_COLOR toml::array(0.10, 0.09, 0.12, 1.00)
#define DEFAULT_SCROLLBAR_GRAB_BACKGROUND_COLOR toml::array(0.80, 0.80, 0.83, 0.31)
#define DEFAULT_SCROLLBAR_GRAB_HOVERED_COLOR toml::array(0.56, 0.56, 0.58, 1.00)
#define DEFAULT_SCROLLBAR_GRAB_ACTIVE_COLOR toml::array(0.06, 0.05, 0.07, 1.00)
#define DEFAULT_CHECKMARK_COLOR toml::array(0.80, 0.80, 0.83, 0.31)
#define DEFAULT_SLIDER_GRAB_COLOR toml::array(0.80, 0.80, 0.83, 0.31)
#define DEFAULT_SLIDER_GRAB_ACTIVE_COLOR toml::array(0.06, 0.05, 0.07, 1.00)
#define DEFAULT_BUTTON_COLOR toml::array(0.10, 0.09, 0.12, 1.00)
#define DEFAULT_BUTTON_HOVERED_COLOR toml::array(0.24, 0.23, 0.29, 1.00)
#define DEFAULT_BUTTON_ACTIVE_COLOR toml::array(0.56, 0.56, 0.58, 1.00)
#define DEFAULT_HEADER_COLOR toml::array(0.10, 0.09, 0.12, 1.00)
#define DEFAULT_HEADER_HOVERED_COLOR toml::array(0.56, 0.56, 0.58, 1.00)
#define DEFAULT_HEADER_ACTIVE_COLOR toml::array(0.06, 0.05, 0.07, 1.00)
#define DEFAULT_TEXT_SELECTED_BACKGROUND_COLOR toml::array(0.25, 1.00, 0.00, 0.43)

enum StyleConfig : std::size_t {
	StyleConfig_WindowPadding,
	StyleConfig_WindowRounding,
	StyleConfig_FramePadding,
	StyleConfig_FrameRounding,
	StyleConfig_ItemSpacing,
	StyleConfig_ItemInnerSpacing,
	StyleConfig_IndentSpacing,
	StyleConfig_ScrollBarSize,
	StyleConfig_ScrollBarRounding,
	StyleConfig_GrabMinSize,
	StyleConfig_GrabRounding,

	StyleConfig_TextColor,
	StyleConfig_DisabledTextColor,
	StyleConfig_BackgroundColor,
	StyleConfig_PopupBackgroundColor,
	StyleConfig_BorderColor,
	StyleConfig_BorderShadowColor,
	StyleConfig_FrameBackgroundColor,
	StyleConfig_FrameHoveredBackgroundColor,

	StyleConfig_FrameActiveBackgroundColor,
	StyleConfig_TitleBackgroundColor,
	StyleConfig_TitleCollapsedColor,
	StyleConfig_TitleActiveBackgroundColor,
	StyleConfig_MenuBarBackgroundColor,
	StyleConfig_ScrollBarBackgroundColor,
	StyleConfig_ScrollBarGrabColor,
	StyleConfig_ScrollBarGrabHovered,
	StyleConfig_ScrollBarGrabActive,
	StyleConfig_CheckMarkColor,
	StyleConfig_SliderGrabColor,
	StyleConfig_SliderGrabActiveColor,
	StyleConfig_ButtonColor,
	StyleConfig_ButtonHoveredColor,
	StyleConfig_ButtonActiveColor,
	StyleConfig_HeaderColor,
	StyleConfig_HeaderHoveredColor,
	StyleConfig_HeaderActiveColor,
	StyleConfig_TextSelectedBackgroundColor,

	StyleConfig_Size
};

toml::table create_default_configuration();

constexpr inline std::array<const char*, StyleConfig_Size> get_style_names()
{
	std::array<const char*, StyleConfig_Size> names{};

	names[StyleConfig_WindowPadding] = "window-padding";
	names[StyleConfig_WindowRounding] = "window-rounding";
	names[StyleConfig_FramePadding] = "frame-padding";
	names[StyleConfig_FrameRounding] = "frame-rounding";
	names[StyleConfig_ItemSpacing] = "item-spacing";
	names[StyleConfig_ItemInnerSpacing] = "item-inner-spacing";
	names[StyleConfig_IndentSpacing] = "indent-spacing";
	names[StyleConfig_ScrollBarSize] = "scrollbar-size";
	names[StyleConfig_ScrollBarRounding] = "scrollbar-rounding";
	names[StyleConfig_GrabMinSize] = "grab-min-size";
	names[StyleConfig_GrabRounding] = "grab-rounding";

	names[StyleConfig_TextColor] = "text-color";
	names[StyleConfig_DisabledTextColor] = "text-disabled-color";
	names[StyleConfig_BackgroundColor] = "background-color";
	names[StyleConfig_PopupBackgroundColor] = "popup-background-color";
	names[StyleConfig_BorderColor] = "border-color";
	names[StyleConfig_BorderShadowColor] = "border-shadow-color";
	names[StyleConfig_FrameBackgroundColor] = "frame-background-color";
	names[StyleConfig_FrameHoveredBackgroundColor] = "frame-hovered-background-color";
	names[StyleConfig_FrameActiveBackgroundColor] = "frame-active-background-color";
	names[StyleConfig_TitleBackgroundColor] = "title-background-color";
	names[StyleConfig_TitleCollapsedColor] = "title-collapsed-color";
	names[StyleConfig_TitleActiveBackgroundColor] = "title-active-background-color";
	names[StyleConfig_MenuBarBackgroundColor] = "menu-bar-background-color";
	names[StyleConfig_ScrollBarBackgroundColor] = "scrollbar-background-color";
	names[StyleConfig_ScrollBarGrabColor] = "scrollbar-grab-color";
	names[StyleConfig_ScrollBarGrabHovered] = "scrollbar-grab-hovered";
	names[StyleConfig_ScrollBarGrabActive] = "scrollbar-grab-active";
	names[StyleConfig_CheckMarkColor] = "checkmark-color";
	names[StyleConfig_SliderGrabColor] = "slider-grab-color";
	names[StyleConfig_SliderGrabActiveColor] = "slider-grab-active-color";
	names[StyleConfig_ButtonColor] = "button-color";
	names[StyleConfig_ButtonHoveredColor] = "button-hovered-color";
	names[StyleConfig_ButtonActiveColor] = "button-active-color";
	names[StyleConfig_HeaderColor] = "header-color";
	names[StyleConfig_HeaderHoveredColor] = "header-hovered-color";
	names[StyleConfig_HeaderActiveColor] = "header-active-color";
	names[StyleConfig_TextSelectedBackgroundColor] = "text-selected-background-color";

	return names;
}
constexpr inline const char* get_style_name(StyleConfig config)
{
	auto style_names = get_style_names();
	return style_names[config];
}

ImVec2 array_to_vec2(toml::array* arr);

class Config {
private:
	toml::table m_core_config;
	std::string m_core_config_path;
	ImFont* m_font{ NULL };
public:
	Config();
	Config(const std::string& config_directory);
	Config(Config&&) = default;
	inline Config(const Config& other) {
		m_core_config = other.m_core_config;
		m_core_config_path = other.m_core_config_path;
	}

	/*
		The "syntax" of this function is (table_name:key). So, for example,
		if you were to attempt to access the "debug" key within the "settings" table,
		you would call this function like this:

			config->get_value<bool>("settings:debug");

		This avoids code smell and streamlines the fetching process.
		If this function returns std::nullopt, either the table or key
		does not exist.
	*/
	template <class T>
	inline std::optional<T> get_value(const std::string_view syntax) {
		auto* logger = Logger::the();
		
		constexpr auto syntax_delimer = ':';
		const std::size_t position_of_semi = syntax.find(syntax_delimer);

		if (position_of_semi == std::string_view::npos) {
			logger->write(LogKind::Warn, "The syntax passed to get_value() was invalid.");
			logger->write(LogKind::Warn, "Expected ':' within the string, to seperate table_name and key.");
			logger->write(LogKind::Warn, "But instead, got '{}'", syntax);
			return std::nullopt;
		}

		const auto table_name = syntax.substr(0, position_of_semi);
		const auto table_view = this->m_core_config[table_name];

		if (!table_view.is_table()) {
			logger->write(LogKind::Warn, "Within the call to get_value(\"{}\"), the table name does not exist.", syntax);
			logger->write(LogKind::Warn, "This table either doesn't exist, or the value isn't a table.");
			return std::nullopt;
		}

		auto* table = table_view.as_table();
		const auto key_name = syntax.substr(position_of_semi + 1, syntax.size());

		auto* value = table->get(key_name);

		if (!value) {
			logger->write(LogKind::Warn, "The key '{}' does not exist inside of table '{}'.", key_name, table_name);
			return std::nullopt;
		}

		toml::value<T>* wrap_node = value->as<T>();

		if (!wrap_node) {
			logger->write(LogKind::Warn, "Attempted to get '{}:{}' as an invalid type.", table_name, key_name);
			return std::nullopt;
		}

		return wrap_node->get();
	}
	inline std::optional<toml::array*> get_value_as_array(const std::string_view syntax) {
		auto* logger = Logger::the();

		constexpr auto syntax_delimer = ':';
		const std::size_t position_of_semi = syntax.find(syntax_delimer);

		if (position_of_semi == std::string_view::npos) {
			logger->write(LogKind::Warn, "The syntax passed to get_value() was invalid.");
			logger->write(LogKind::Warn, "Expected ':' within the string, to seperate table_name and key.");
			logger->write(LogKind::Warn, "But instead, got '{}'", syntax);
			return std::nullopt;
		}

		const auto table_name = syntax.substr(0, position_of_semi);
		const auto table_view = this->m_core_config[table_name];

		if (!table_view.is_table()) {
			logger->write(LogKind::Warn, "Within the call to get_value_as_array(\"{}\"), the table name does not exist.", syntax);
			logger->write(LogKind::Warn, "This table either doesn't exist, or the value isn't a table.");
			return std::nullopt;
		}

		auto* table = table_view.as_table();
		const auto key_name = syntax.substr(position_of_semi + 1, syntax.size());

		auto* value = table->get(key_name);

		if (!value) {
			logger->write(LogKind::Warn, "The key '{}' does not exist inside of table '{}'.", key_name, table_name);
			return std::nullopt;
		}

		auto array = value->as_array();
		return array;
	}
	// These are because I don't know how to use this library properly yet.
	inline std::optional<std::string_view> get_value_as_string(const std::string_view syntax) {
		auto* logger = Logger::the();

		constexpr auto syntax_delimer = ':';
		const std::size_t position_of_semi = syntax.find(syntax_delimer);

		if (position_of_semi == std::string_view::npos) {
			logger->write(LogKind::Warn, "The syntax passed to get_value() was invalid.");
			logger->write(LogKind::Warn, "Expected ':' within the string, to seperate table_name and key.");
			logger->write(LogKind::Warn, "But instead, got '{}'", syntax);
			return std::nullopt;
		}

		const auto table_name = syntax.substr(0, position_of_semi);
		const auto table_view = this->m_core_config[table_name];

		if (!table_view.is_table()) {
			logger->write(LogKind::Warn, "Within the call to get_value_as_array(\"{}\"), the table name does not exist.", syntax);
			logger->write(LogKind::Warn, "This table either doesn't exist, or the value isn't a table.");
			return std::nullopt;
		}

		auto* table = table_view.as_table();
		const auto key_name = syntax.substr(position_of_semi + 1, syntax.size());

		auto* value = table->get(key_name);

		if (!value) {
			logger->write(LogKind::Warn, "The key '{}' does not exist inside of table '{}'.", key_name, table_name);
			return std::nullopt;
		}

		return value->value<std::string_view>();
	}
	inline std::optional<std::size_t> get_value_as_sizet(const std::string_view syntax) {
		auto* logger = Logger::the();

		constexpr auto syntax_delimer = ':';
		const std::size_t position_of_semi = syntax.find(syntax_delimer);

		if (position_of_semi == std::string_view::npos) {
			logger->write(LogKind::Warn, "The syntax passed to get_value() was invalid.");
			logger->write(LogKind::Warn, "Expected ':' within the string, to seperate table_name and key.");
			logger->write(LogKind::Warn, "But instead, got '{}'", syntax);
			return std::nullopt;
		}

		const auto table_name = syntax.substr(0, position_of_semi);
		const auto table_view = this->m_core_config[table_name];

		if (!table_view.is_table()) {
			logger->write(LogKind::Warn, "Within the call to get_value_as_array(\"{}\"), the table name does not exist.", syntax);
			logger->write(LogKind::Warn, "This table either doesn't exist, or the value isn't a table.");
			return std::nullopt;
		}

		auto* table = table_view.as_table();
		const auto key_name = syntax.substr(position_of_semi + 1, syntax.size());

		auto* value = table->get(key_name);

		if (!value) {
			logger->write(LogKind::Warn, "The key '{}' does not exist inside of table '{}'.", key_name, table_name);
			return std::nullopt;
		}

		return value->value<std::size_t>();
	}
	
	inline ImVec2 get_value_as_vec2(const std::string_view syntax, const toml::array& default_array) noexcept {
		auto array = get_value_as_array(syntax);
		ImVec2 default_value;
		// Risky, relying on the defaults being correct.
		auto& thing = default_array.at(0);
		default_value.x = default_array.at(0).as_floating_point()->get();
		default_value.y = default_array.at(1).as_floating_point()->get();
		if (!array) {
			return default_value;
		}
		auto elements = array.value();
		ImVec2 value = {};

		try {
			value.x = elements->at(0).as_floating_point()->get();
			value.y = elements->at(1).as_floating_point()->get();
		}
		catch (...) {
			Logger::the()->error("The config value '{}' is invalid. (Failed to read Vec2 from it)", syntax);
			return default_value;
		}

		return value;
	}
	inline ImVec4 get_value_as_vec4(const std::string_view syntax, const toml::array& default_array)
	{
		auto array = get_value_as_array(syntax);
		ImVec4 default_value;
		// Risky, relying on default values being correct.
		default_value.x = default_array.at(0).as<double>()->get();
		default_value.y = default_array.at(1).as<double>()->get();
		default_value.z = default_array.at(2).as<double>()->get();
		default_value.w = default_array.at(3).as<double>()->get();
		if (!array) {
			return default_value;
		}
		auto elements = array.value();
		ImVec4 value = {};

		try {
			value.x = elements->at(0).as_floating_point()->get();
			value.y = elements->at(1).as_floating_point()->get();
			value.z = elements->at(2).as_floating_point()->get();
			value.y = elements->at(3).as_floating_point()->get();
		}
		catch (...) {
			Logger::the()->error("The config value '{}' is invalid. (Failed to read Vec4 from it)", syntax);
			return default_value;
		}

		return value;
	}
	inline float get_value_as_float(const std::string_view syntax, float default_value) noexcept {
		auto* logger = Logger::the();

		constexpr auto syntax_delimer = ':';
		const std::size_t position_of_semi = syntax.find(syntax_delimer);

		if (position_of_semi == std::string_view::npos) {
			logger->write(LogKind::Warn, "The syntax passed to get_value() was invalid.");
			logger->write(LogKind::Warn, "Expected ':' within the string, to seperate table_name and key.");
			logger->write(LogKind::Warn, "But instead, got '{}'", syntax);
			return default_value;
		}

		const auto table_name = syntax.substr(0, position_of_semi);
		const auto table_view = this->m_core_config[table_name];

		if (!table_view.is_table()) {
			logger->write(LogKind::Warn, "Within the call to get_value_as_array(\"{}\"), the table name does not exist.", syntax);
			logger->write(LogKind::Warn, "This table either doesn't exist, or the value isn't a table.");
			return default_value;
		}

		auto* table = table_view.as_table();
		const auto key_name = syntax.substr(position_of_semi + 1, syntax.size());

		auto* value = table->get(key_name);

		if (!value) {
			logger->write(LogKind::Warn, "The key '{}' does not exist inside of table '{}'.", key_name, table_name);
			return default_value;
		}

		return static_cast<float>(value->value<double>().value_or(default_value));
	}

	template <class CfgValue>
	inline CfgValue get_config_value(StyleConfig config_value, const std::variant<toml::array, double> default_value) {
		const bool is_experimental_styles_enabled = get_value<bool>("settings:use_experimental_customization").value_or(false);
		std::string style_name;
		if (is_experimental_styles_enabled) {
			style_name = std::string("style:") + get_style_name(config_value);
		}
		else {
			style_name = "DISABLED";
		}
		if constexpr (std::is_same_v<CfgValue, ImVec2>) {
			return get_value_as_vec2(style_name, std::get<0>(default_value));
		}
		else if constexpr (std::is_same_v<CfgValue, ImVec4>) {
			return get_value_as_vec4(style_name, std::get<0>(default_value));
		}
		else if constexpr (std::is_same_v<CfgValue, double>) {
			return get_value_as_float(style_name, std::get<1>(default_value));
		}

		throw std::exception("That type is not a valid style configuration type.");
	}

	inline toml::table* get_styles_table() noexcept {
		return m_core_config.get("style")->as_table();
	}

	inline ImVec2 get_vec2_from(toml::table* table, const std::string_view key_name, const toml::array& default_value) noexcept
	{
		auto& node = table->at(key_name);
		if (node.is_array()) {
			return array_to_vec2(node.as_array());
		}
		return array_to_vec2(node.as_array());
	}

	inline double get_double_from(toml::table* table, const std::string_view key_name, const float default_value) noexcept
	{
		auto& node = table->at(key_name);
		if (node.is_floating_point()) {
			return node.as_floating_point()->get();
		}
		return default_value;
	}

	inline ImFont* font() noexcept { return m_font; }

	template <class T>
	inline bool set_value(const std::string_view syntax, const T& value) noexcept {
		auto* logger = Logger::the();

		constexpr auto syntax_delimer = ':';
		const std::size_t position_of_semi = syntax.find(syntax_delimer);

		if (position_of_semi == std::string_view::npos) {
			logger->write(LogKind::Warn, "The syntax passed to get_value() was invalid.");
			logger->write(LogKind::Warn, "Expected ':' within the string, to seperate table_name and key.");
			logger->write(LogKind::Warn, "But instead, got '{}'", syntax);
			return false;
		}

		const auto table_name = syntax.substr(0, position_of_semi);
		const auto table_view = this->m_core_config[table_name];

		if (!table_view.is_table()) {
			logger->write(LogKind::Warn, "Within the call to get_value(), the table name does not exist.");
			logger->write(LogKind::Warn, "This table either doesn't exist, or the value isn't a table.");
			return false;
		}

		auto* table = table_view.as_table();
		const auto key_name = syntax.substr(position_of_semi + 1, syntax.size());

		toml::node* value = table->get(key_name);

		if (!value) {
			logger->write(LogKind::Warn, "The key '{}' does not exist inside of table '{}'.", key_name, table_name);
			return false;
		}

		if (!value->is<T>()) {
			logger->write(LogKind::Warn, "From set_value(), cannot set '{}:{}' due to incompatible types.", table_name, key_name);
			return false;
		}

		table->insert_or_assign(key_name, value);
		return true;
	}

	void prepare_assets(Config& config) noexcept;

	~Config();
};