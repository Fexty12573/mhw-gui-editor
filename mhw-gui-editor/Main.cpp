#include "pch.h"

#include "App.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	App app;
	const int result = app.run();

	return result;
}
