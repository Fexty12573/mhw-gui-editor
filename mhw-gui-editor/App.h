#pragma once

#include "Window.h"
#include "Editor.h"

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

    void add_window_message_callback(const MessageCallback& callback) {
        m_window.add_message_callback(callback);
    }

private:
	void render_frame();
	void handle_input();

private:
	Window m_window;
	Editor m_editor;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swap_chain;
};

