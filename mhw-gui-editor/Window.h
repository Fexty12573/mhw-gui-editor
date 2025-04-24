#pragma once
#include "MinWindows.h"

#include <functional>
#include <string>
#include <vector>

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

class App;

using MessageCallback = std::function<void(UINT, WPARAM, LPARAM)>;

class Window {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Window(std::string title, int width, int height, DWORD ex_style = 0);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	[[nodiscard]] HWND get_window() const { return m_window; }

	void add_resize_callback(const std::function<void(u16, u16)>& callback);
    void add_message_callback(const MessageCallback& callback) {
        m_message_callbacks.push_back(callback);
    }

private:
	static LRESULT CALLBACK WndProcInit(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	HWND m_window;

	std::string m_title;
	int m_width;
	int m_height;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swap_chain;
	ComPtr<ID3D11RenderTargetView> m_main_rtv;

    std::vector<std::function<void(u16, u16)>> m_resize_callbacks;
    std::vector<MessageCallback> m_message_callbacks;

	friend class App;
};

