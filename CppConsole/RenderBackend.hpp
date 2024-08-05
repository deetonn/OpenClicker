#pragma once

#include <string>
#include <functional>

#include <D3D11.h>
#include <Windows.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"

LRESULT WINAPI WndProc(
	HWND hWnd, 
	UINT cmd, 
	WPARAM wParam, 
	LPARAM lParam
);

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam
);

struct ResizeInfo {
	std::uint32_t requested_x_size;
	std::uint32_t requested_y_size;
};

// Forward declare Renderer so that RenderCallback can use it.
class Renderer;
// Forward declare Console so that RenderCallback can use it.
class OpenClicker;

using RenderCallback = std::function<
	void(ImGuiIO&, Renderer&, OpenClicker&)
>;

class RenderBackend {
private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_device_context;
	IDXGISwapChain* m_swap_chain;
	ID3D11RenderTargetView* m_target_view;
	bool swap_chain_occluded{ false };

	HWND m_window;
	WNDCLASSEXA m_window_class;

	ResizeInfo m_resize_info;
	bool still_rendering{ true };

	static inline RenderBackend* m_active_instance = NULL;
public:
	RenderBackend(const std::string_view, const int, const int);
	~RenderBackend();

	bool create_d3d_device(HWND device);
	void cleanup_d3d_device();

	void create_render_target();
	void cleanup_render_target();

	void set_resize_dimensions(const ResizeInfo& resize);
	inline HWND window_handle() const noexcept { return m_window; }

	// This will cause the application rendering loop to begin.
	void render(
		const RenderCallback& render_call, 
		OpenClicker& context,
		Renderer& renderer
	) noexcept;

	static RenderBackend* active_instance() noexcept;
};