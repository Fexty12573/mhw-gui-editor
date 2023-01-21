#include "pch.h"
#include "Window.h"
#include "HrException.h"
#include "imgui_impl_win32.h"

#include <fmt/format.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::Window(std::string title, int width, int height)
	: m_window(nullptr), m_title(std::move(title)), m_width(width), m_height(height) {

	HR_INIT(S_OK);
	WNDCLASSEX wc{};

	RECT rc = { 0, 0, m_width, m_height };
	if (!AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE)) {
		OutputDebugStringA(fmt::format("AdjustWindowRect failed: {}\n", GetLastError()).c_str());
		throw;
	}

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
	wc.lpfnWndProc = WndProcInit;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = nullptr;
	wc.hCursor = LoadIcon(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = "GuiEditor";
	wc.hIconSm = nullptr;

	if (!RegisterClassEx(&wc)) {
		OutputDebugStringA(fmt::format("Failed to create window class: {}\n", GetLastError()).c_str());
		throw;
	}

	m_window = CreateWindow(
		wc.lpszClassName,
		m_title.c_str(), 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr, nullptr, 
		wc.hInstance, 
		this
	);

	if (!m_window) {
		OutputDebugStringA(fmt::format("Failed to create window: {}\n", GetLastError()).c_str());
		throw;
	}

	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferDesc.Width = m_width;
	scd.BufferDesc.Height = m_height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = m_window;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

#ifdef _DEBUG
	constexpr UINT flags = 0;
#else
	constexpr UINT flags = 0;
#endif

	HR_ASSERT(D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		flags, 
		nullptr, 
		0,
		D3D11_SDK_VERSION, 
		&scd, 
		&m_swap_chain, 
		&m_device, 
		nullptr, 
		&m_context
	));

	ShowWindow(m_window, SW_SHOW);

	ComPtr<ID3D11Texture2D> back_buffer;
	HR_ASSERT(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)));
	HR_ASSERT(m_device->CreateRenderTargetView(back_buffer.Get(), nullptr, &m_main_rtv));

	UpdateWindow(m_window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
}

Window::~Window() {
	DestroyWindow(m_window);
}

LRESULT CALLBACK Window::WndProcInit(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_NCCREATE) {

		const auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		auto window = static_cast<Window*>(cs->lpCreateParams);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcThunk));
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::WndProcThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	const auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	return window->WndProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HR_INIT(S_OK);

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	/*case WM_SIZE:
		if (m_device && wParam != SIZE_MINIMIZED) {
			OutputDebugStringA("Resizing\n");
			m_swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			ComPtr<ID3D11Texture2D> back_buffer;
			HR_ASSERT(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)));
			HR_ASSERT(m_device->CreateRenderTargetView(back_buffer.Get(), nullptr, &m_main_rtv));
		}

		break;*/
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}
