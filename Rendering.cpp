#include "Rendering.h"

Rendering::Rendering() noexcept
{
	is_running = true;

	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, nsRendering::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"EXT", NULL };

	g_rendering = this;
}

Rendering::~Rendering() noexcept
{
	g_rendering = nullptr;
}

inline IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall nsRendering::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        
        if (g_rendering->g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            //g_rendering->CleanupRenderTarget();
            g_rendering->g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            //g_rendering->CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
