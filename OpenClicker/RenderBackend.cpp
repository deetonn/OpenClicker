#include "RenderBackend.hpp"
#include "ImGui/imgui_impl_dx11.h"
#include "Logger.hpp"
#include "comdef.h"
#include "Console.hpp"

#include <filesystem>
namespace fs = std::filesystem;
using namespace std::string_literals;

RenderBackend::RenderBackend(
	const std::string_view window_title,
	const int window_x = 1280,
	const int window_y = 800
)
{
	// NOTE: This MUST be initialized here.
	m_active_instance = this;

	auto* logger = Logger::the();
	auto module = GetModuleHandleA(NULL);

	if (!module) {
		throw std::exception("Failed to get a handle to the current module.");
	}

	m_window_class = WNDCLASSEXA{
		.cbSize = sizeof(WNDCLASSEXA),
		.style = CS_CLASSDC,
		.lpfnWndProc = WndProc,
		.cbClsExtra = NULL,
		.cbWndExtra = NULL,
		.hInstance = module,
		.hIcon = nullptr,
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = "CppConsole",
		.hIconSm = nullptr
	};
	auto atom = RegisterClassExA(&m_window_class);
	if (atom == NULL) {
		throw std::exception("Failed to register window class");
	}
	m_window = CreateWindowExA(
		0L,
		m_window_class.lpszClassName,
		window_title.data(),
		WS_OVERLAPPEDWINDOW,
		100, 100, window_x, window_y,
		nullptr, nullptr,
		m_window_class.hInstance,
		nullptr
	);

	if (m_window == NULL) {
		const char* message = "Failed to create main window.";
		logger->fatal("{} ({})", message, GetLastError());
		UnregisterClassA(m_window_class.lpszClassName, m_window_class.hInstance);
		throw std::exception(message);
	}

	if (!create_d3d_device(m_window)) {
		logger->fatal("Failed to create d3d device for the window!!");
		UnregisterClassA(m_window_class.lpszClassName, m_window_class.hInstance);
		throw std::exception("Failed to initialized d3d device for main window.");
	}

	ShowWindow(m_window, SW_SHOWDEFAULT);
	UpdateWindow(m_window);

	ImGui::CreateContext();
	auto& io = ImGui::GetIO();

	// TODO: Actually have these options configurable.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_EnableDpiAwareness();
	ImGui_ImplWin32_Init(m_window);
	ImGui_ImplDX11_Init(m_device, m_device_context);
}

RenderBackend::~RenderBackend()
{
	auto* logger = Logger::the();
	logger->info("The render backend has shutdown!");
}

bool RenderBackend::create_d3d_device(HWND device)
{
	auto* logger = Logger::the();

	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_window;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_swap_chain, &m_device, &featureLevel, &m_device_context);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
	{
		_com_error com_error(res);
		logger->warn("Failed to create D3D device using the hardware... attempting to use WARP.");
		logger->write_wstr(com_error.ErrorMessage());
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_swap_chain, &m_device, &featureLevel, &m_device_context);
	}
	if (res != S_OK)
	{
		_com_error com_error(res);
		logger->error("Cannot create D3D device and swapchain.");
		logger->write_wstr(com_error.ErrorMessage());
		return false;
	}

	this->create_render_target();
	return true;
}

void RenderBackend::cleanup_d3d_device()
{
	this->cleanup_render_target();
	if (m_swap_chain) { 
		m_swap_chain->Release(); 
		m_swap_chain = nullptr; 
	}
	if (m_device_context) { 
		m_device_context->Release(); 
		m_device_context = nullptr;
	}
	if (m_device) { 
		m_device->Release(); 
		m_device = nullptr; 
	}
}

void RenderBackend::create_render_target()
{
	ID3D11Texture2D* back_buffer;
	m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	m_device->CreateRenderTargetView(back_buffer, nullptr, &m_target_view);
	back_buffer->Release();
}

void RenderBackend::cleanup_render_target()
{
	if (m_target_view) { 
		m_target_view->Release(); 
		m_target_view = nullptr; 
	}
}

void RenderBackend::set_resize_dimensions(const ResizeInfo& resize)
{
	m_resize_info.requested_x_size = resize.requested_x_size;
	m_resize_info.requested_y_size = resize.requested_y_size;
}

static void setup_imgui_styles(ImGuiStyle* style, Config& config)
{
	// Loading style things and not color related things.

	auto* style_table = config.get_styles_table();

	style->WindowPadding = config.get_vec2_from(style_table, get_style_name(StyleConfig_WindowPadding), DEFAULT_WINDOW_PADDING);
	style->WindowRounding = config.get_double_from(style_table, get_style_name(StyleConfig_WindowRounding), DEFAULT_WINDOW_ROUNDING);
	style->FramePadding = config.get_vec2_from(style_table, get_style_name(StyleConfig_FramePadding), DEFAULT_WINDOW_PADDING);
	style->FrameRounding = config.get_double_from(style_table, get_style_name(StyleConfig_FrameRounding), DEFAULT_FRAME_ROUNDING);
	style->ItemSpacing = config.get_vec2_from(style_table, get_style_name(StyleConfig_ItemSpacing), DEFAULT_ITEM_SPACING);
	style->ItemInnerSpacing = config.get_vec2_from(style_table, get_style_name(StyleConfig_ItemInnerSpacing), DEFAULT_ITEM_INNER_SPACING);
	style->IndentSpacing = config.get_config_value<double>(StyleConfig_IndentSpacing, DEFAULT_INDENT_SPACING);
	style->ScrollbarSize = config.get_config_value<double>(StyleConfig_ScrollBarSize, DEFAULT_SCROLL_BAR_SIZE);
	style->ScrollbarRounding = config.get_config_value<double>(StyleConfig_ScrollBarRounding, DEFAULT_SCROLL_BAR_ROUNDING);
	style->GrabMinSize = config.get_config_value<double>(StyleConfig_GrabMinSize, DEFAULT_GRAB_MIN_SIZE);
	style->GrabRounding = config.get_config_value<double>(StyleConfig_GrabRounding, DEFAULT_GRAB_ROUNDING);

	style->Colors[ImGuiCol_Text] = config.get_config_value<ImVec4>(StyleConfig_TextColor, DEFAULT_TEXT_COLOR);
	style->Colors[ImGuiCol_TextDisabled] = config.get_config_value<ImVec4>(StyleConfig_DisabledTextColor, DEFAULT_DISABLED_TEXT_COLOR);;
	style->Colors[ImGuiCol_WindowBg] = config.get_config_value<ImVec4>(StyleConfig_BackgroundColor, DEFAULT_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_PopupBg] = config.get_config_value<ImVec4>(StyleConfig_PopupBackgroundColor, DEFAULT_POPUP_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_Border] = config.get_config_value<ImVec4>(StyleConfig_BorderColor, DEFAULT_BORDER_COLOR);
	style->Colors[ImGuiCol_BorderShadow] = config.get_config_value<ImVec4>(StyleConfig_BorderShadowColor, DEFAULT_BORDER_SHADOW_COLOR);
	style->Colors[ImGuiCol_FrameBg] = config.get_config_value<ImVec4>(StyleConfig_FrameBackgroundColor, DEFAULT_FRAME_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_FrameBgHovered] = config.get_config_value<ImVec4>(StyleConfig_FrameHoveredBackgroundColor, DEFAULT_FRAME_HOVERED_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_FrameBgActive] = config.get_config_value<ImVec4>(StyleConfig_FrameActiveBackgroundColor, DEFAULT_FRAME_ACTIVE_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_TitleBg] = config.get_config_value<ImVec4>(StyleConfig_TitleBackgroundColor, DEFAULT_TITLE_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_TitleBgCollapsed] = config.get_config_value<ImVec4>(StyleConfig_TitleCollapsedColor, DEFAULT_TITLE_COLLAPSED_COLOR);
	style->Colors[ImGuiCol_TitleBgActive] = config.get_config_value<ImVec4>(StyleConfig_TitleActiveBackgroundColor, DEFAULT_TITLE_ACTIVE_COLOR);
	style->Colors[ImGuiCol_MenuBarBg] = config.get_config_value<ImVec4>(StyleConfig_MenuBarBackgroundColor, DEFAULT_MENU_BAR_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_ScrollbarBg] = config.get_config_value<ImVec4>(StyleConfig_ScrollBarBackgroundColor, DEFAULT_SCROLLBAR_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_ScrollbarGrab] = config.get_config_value<ImVec4>(StyleConfig_ScrollBarGrabColor, DEFAULT_SCROLLBAR_GRAB_BACKGROUND_COLOR);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = config.get_config_value<ImVec4>(StyleConfig_ScrollBarGrabHovered, DEFAULT_SCROLLBAR_GRAB_HOVERED_COLOR);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
}

void RenderBackend::render(const RenderCallback& render_call, OpenClicker& context, Renderer& renderer) noexcept
{
	auto& io = ImGui::GetIO();
	auto clear_colour = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	auto& config = context.config();
	config.prepare_assets(config);

	setup_imgui_styles(&ImGui::GetStyle(), context.config());

	while (still_rendering) {
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0L, 0L, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				still_rendering = false;
			}
		}

		if (!still_rendering)
			break;

		if (m_resize_info.requested_x_size != 0
			&& m_resize_info.requested_y_size != 0) 
		{
			this->cleanup_render_target();
			m_swap_chain->ResizeBuffers(
				0,
				m_resize_info.requested_x_size,
				m_resize_info.requested_y_size,
				DXGI_FORMAT_UNKNOWN, 0);
			m_resize_info.requested_x_size = 0;
			m_resize_info.requested_y_size = 0;
			this->create_render_target();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Now the frame has begun, we call the actual renderer.
		render_call(io, renderer, context);

		ImGui::Render();

		const float clear_colour_with_alpha[4] =
		{
			clear_colour.x * clear_colour.w, 
			clear_colour.y * clear_colour.w, 
			clear_colour.z * clear_colour.w, 
			clear_colour.w
		};

		m_device_context->OMSetRenderTargets(1, &m_target_view, nullptr);
		m_device_context->ClearRenderTargetView(m_target_view, clear_colour_with_alpha);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		const bool vsync_enabled =
			context.config().get_value<bool>("renderer:vsync").value_or(false);

		HRESULT result = m_swap_chain->Present(
			static_cast<UINT>(vsync_enabled), 
			0
		);
		swap_chain_occluded = (result == DXGI_STATUS_OCCLUDED);
	}

	// NOTE: do any cleanup here, such as saving stuff blah blah
		// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	this->cleanup_d3d_device();
	DestroyWindow(m_window);
	UnregisterClassA(m_window_class.lpszClassName, 
		m_window_class.hInstance);
}

RenderBackend* RenderBackend::active_instance() noexcept
{
	return RenderBackend::m_active_instance;
}

LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto* renderer = RenderBackend::active_instance();

	if (!renderer)
		return false;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		renderer->set_resize_dimensions({ 
			(UINT)LOWORD(lParam), 
			(UINT)HIWORD(lParam) 
		});
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcA(hWnd, msg, wParam, lParam);
}
