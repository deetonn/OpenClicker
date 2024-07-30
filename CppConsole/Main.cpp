#include <toml.hpp>
#include <iostream>
#include <filesystem>
#include <print>
#include <memory>

#include "Config.hpp"
#include "Logger.hpp"
#include "Console.hpp"
#include "Renderer.hpp"

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
	try {
		const std::string current_path = fs::current_path().string();
		Logger::the()->write(LogKind::Info, "Current directory: {}", current_path);
		auto config = std::make_unique<Config>(current_path);
		auto renderer = std::make_unique<Renderer>("CppConsole", 1280, 800);

		auto console = Console(*config, renderer.get());
		return console.run(argc, argv);
	}
	catch (const std::exception& e) {
		std::print("The application has crashed. ({})\n", e.what());
		return -1;
	}
}
