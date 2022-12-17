#include "Rendering.h"

Rendering::Rendering() noexcept
{
	is_running = true;

	//WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, Rendering::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"EXT", NULL };

	g_rendering = this;
}

Rendering::~Rendering() noexcept
{
	g_rendering = nullptr;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	return true;
}
