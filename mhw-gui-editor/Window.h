#pragma once
#include "MinWindows.h"

#include <string>

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

class App;

class Window {
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Window(std::string title, int width, int height);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	[[nodiscard]] HWND get_window() const { return m_window; }

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

	friend class App;
};

