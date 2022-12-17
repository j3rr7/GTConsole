#pragma once
#include "common.h"

#include <d3d11.h>
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

namespace nsRendering {
	static inline LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

class Rendering
{
public:
	Rendering() noexcept;
	~Rendering() noexcept;

public:
	ID3D11Device* g_pd3dDevice = NULL;
	ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	bool is_running = false;

	HWND hWnd = nullptr;
};

inline Rendering* g_rendering = nullptr;