#include "pch.h"
#include "App.h"
#include "IconFontAwesome.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>


App::App([[maybe_unused]] const std::string& commandline)
	: m_window("MHW GUI Editor", 1920, 1080, WS_EX_ACCEPTFILES), m_editor(this) {
	m_swap_chain = m_window.m_swap_chain;
	m_device = m_window.m_device;
	m_context = m_window.m_context;

	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(m_window.get_window());
	ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());

	m_window.add_resize_callback([this](u16, u16) {
        ImGui_ImplDX11_InvalidateDeviceObjects();
        ImGui_ImplDX11_CreateDeviceObjects();
	});

	m_editor.add_menu_item("File", { "Exit", "Alt+F4", [](Editor*) { PostQuitMessage(0); } });
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

        handle_input();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		render_frame();

		ImGui::Render();

		m_context->OMSetRenderTargets(1, m_window.m_main_rtv.GetAddressOf(), nullptr);
		m_context->ClearRenderTargetView(m_window.m_main_rtv.Get(), clear_color);
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
	const ImGuiID dockspace_id = ImGui::DockSpaceOverViewport();

	m_editor.render(dockspace_id);
}

void App::handle_input() {
#define CLEAR_KEY(key) ImGui::GetIO().KeysData[ImGuiKey_##key].Down = false

	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
	    if (ImGui::IsKeyPressed(ImGuiKey_S)) {
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
                CLEAR_KEY(S);
                m_editor.save_file_as();
            } else {
                m_editor.save_file();
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_O)) {
            CLEAR_KEY(O);
            m_editor.open_file();
        }

		if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_P)) {
            CLEAR_KEY(P);
            m_editor.open_preview();
		}
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftAlt)) {
        if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
            PostQuitMessage(0);
        }
    }
}
