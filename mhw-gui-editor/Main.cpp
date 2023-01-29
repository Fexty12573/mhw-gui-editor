#include "pch.h"
#include "App.h"
#include "Console.h"

#include <spdlog/spdlog.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    const auto& console = Console::get();
	console->initialize();

    const auto log = console->get_logger();

	App app;
	const int result = app.run();
	console->shutdown();

	return result;
}
