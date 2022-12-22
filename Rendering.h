#pragma once
#include "common.h"
#include "ThreadPool.h"
#include "GTAModule.h"
#include "Settings.h"
#include "Teleports.h"

#include <imfont.h>
#include <d3d11.h>
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 420

namespace nsRendering {
	static inline LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

class Rendering final
{
public:
	Rendering();
	~Rendering();

public:
	ID3D11Device* g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	bool is_running = false;

	HWND hWnd = nullptr;
	WNDCLASSEXW wc{};

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

public:
	void on_init();
	void on_tick();
	void on_destroy();

	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();

private:
	void dx_menu();
};

inline Rendering* g_rendering = nullptr;