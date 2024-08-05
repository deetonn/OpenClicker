#include "Console.hpp"
#include "Config.hpp"
#include "Renderer.hpp"

#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	auto current_path = fs::current_path();
	auto config = std::make_unique<Config>(current_path.string());

	auto desired_width = config->get_value_as_sizet("renderer:window_width")
		.value_or(DEFAULT_WINDOW_WIDTH);
	auto desired_height = config->get_value_as_sizet("renderer:window_height")
		.value_or(DEFAULT_WINDOW_HEIGHT);

	auto renderer = std::make_unique<Renderer>("OpenClicker", desired_width, desired_height);
	auto clicker = OpenClicker(*config, &*renderer);

	return clicker.run(argc, argv);
}