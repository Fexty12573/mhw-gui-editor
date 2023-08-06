#include "pch.h"
#include "App.h"
#include "Console.h"

#include <spdlog/spdlog.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
#if defined(_DEBUG)
    const auto& console = Console::get();
	console->initialize();

    const auto log = console->get_logger();
#endif

	App app;
	const int result = app.run();
#if defined(_DEBUG)
    log->info("App::run() returned {}", result);
	console->shutdown();
#endif

	return result;
}
