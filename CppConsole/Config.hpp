#pragma once

#include "Logger.hpp"

#include <toml.hpp>

#include <string>
#include <optional>

#include "ImGui/imgui.h"

#define DEFAULT_FONT_SIZE 19
#define DEFAULT_FONT_TTF_FILE "segoeuib.ttf"

#define ROOT_FONT_PATH "C:\\Windows\\Fonts\\"

toml::table create_default_configuration();

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