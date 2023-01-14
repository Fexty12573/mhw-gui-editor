#include "pch.h"
#include "App.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

App::App([[maybe_unused]] const std::string& commandline) : m_window("MHW GUI Editor", 1920, 1080) {
	m_swap_chain = m_window.m_swap_chain;
	m_device = m_window.m_device;
	m_context = m_window.m_context;
	m_main_rtv = m_window.m_main_rtv;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(m_window.get_window());
	ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
}

App::~App() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

int App::run() {
	bool running = true;
	constexpr float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };

	while (running) {
		MSG msg{};

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();

		m_context->OMSetRenderTargets(1, m_main_rtv.GetAddressOf(), nullptr);
		m_context->ClearRenderTargetView(m_main_rtv.Get(), clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		m_swap_chain->Present(1, 0);
	}

	return 0;
}

void App::render_frame() {
	
}

void App::handle_input() {
	
}
