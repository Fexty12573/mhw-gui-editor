#pragma once

#include "Window.h"
#include "GUIEditor.h"

#include <wrl.h>
#include <string>

class App {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	App(const std::string& commandline = "");
	~App();

	int run();

private:
	void render_frame();
	void handle_input();

private:
	Window m_window;
	GUIEditor m_editor;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swap_chain;
	ComPtr<ID3D11RenderTargetView> m_main_rtv;
};

