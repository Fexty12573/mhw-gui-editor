#pragma once

#include "Window.h"
#include "GUIEditor.h"

#include <wrl.h>
#include <string>

class App {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    explicit App(const std::string& commandline = "");
	~App();

	int run();

    const ComPtr<ID3D11Device>& get_device() const { return m_device; }
    const ComPtr<ID3D11DeviceContext>& get_context() const { return m_context; }
    const ComPtr<IDXGISwapChain>& get_swapchain() const { return m_swap_chain; }

private:
	void render_frame();
	void handle_input();

private:
	Window m_window;
	GUIEditor m_editor;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swap_chain;
};

