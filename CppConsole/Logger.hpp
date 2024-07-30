#pragma once

#include <format>
#include <print>
#include <iostream>

#include <windows.h>

enum class LogKind {
	Info,
	Warn,
	Error,
	Fatal,
};

inline const char* log_kind_to_string(LogKind kind) {
	switch (kind) {
	case LogKind::Info:
		return "INFO";
	case LogKind::Warn:
		return "WARNING";
	case LogKind::Error:
		return "ERROR";
	case LogKind::Fatal:
		return "FATAL";
	}
}

#define LOGGER_FUNC(name, kind) template <class... Args> \
	void name(const std::format_string<Args...> fmt, Args&&... args) noexcept { \
		auto kind_as_string = log_kind_to_string(kind); \
		std::print("[{}] ", kind_as_string); \
		std::print(fmt, std::forward<Args>(args)...); \
		std::print("\n"); \
	} \

class Logger {
public:
	template <class... Args>
	void write(LogKind kind,
		const std::format_string<Args...> fmt,
		Args&&... args) noexcept
	{
		auto kind_as_string = log_kind_to_string(kind);
		std::print("[{}] ", kind_as_string);
		std::print(fmt, std::forward<Args>(args)...);
		std::print("\n");
	}

	void write_wstr(const TCHAR* str) noexcept {
		std::wcout << str;
	}

	LOGGER_FUNC(info, LogKind::Info);
	LOGGER_FUNC(warn, LogKind::Warn);
	LOGGER_FUNC(error, LogKind::Error);
	LOGGER_FUNC(fatal, LogKind::Fatal);

	inline static Logger* the() noexcept {
		static Logger static_logger = {};
		return &static_logger;
	}
};