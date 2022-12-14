#include "framework.h"
#include "utils.h"

// our safeguard
bool checkRunning() {
	auto m_hMutex = CreateMutex(NULL, FALSE, L"GTConsole.exe");
	switch (GetLastError())
	{
	case ERROR_SUCCESS:
		break;

	case ERROR_ALREADY_EXISTS: //falltrough
	default:
		return true;
	}
	return false;
}

int main()
{
	if (checkRunning())
	{
		MessageBoxW(NULL, L"Another instance is already running!\nOnly one instance is allowed to running", L"Error", NULL);
		return;
	}
	SetConsoleTitle(L"HWAGYUhVBUY78916786138"); // random jibberish :v
	 
	HWND hWnd = FindWindowA(0, "Grand Theft Auto V"); // find window
	GetWindowThreadProcessId(hWnd, &hack::pid);
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hack::pid); // give administrative rights to read and write data
	hack::Base = hack::GetModuleBaseAddress(hack::pid, L"GTA5.exe"); // get module base addr
	
	// Signature Here
	LPCSTR SignatureWorldPTR = "\x48\x8B\x05\xCC\xCC\x45\xCC\xCC\x48\x8B\x48\x08\x48\x85\xC9\x74\x07";
	LPCSTR MaskWorldPTR = "xxx??x??xxxxxxxxx";

	LPCSTR SignatureBlipPTR = "\x4C\x8D\x05\xCC\xCC\x0F\xB7\xC1";
	LPCSTR MaskBlipPTR = "xxx??xxx";

	LPCSTR SignatureGlobalPTR = "\x4C\x8D\x05\xCC\xCC\x4D\x8B\x08\x4D\x85\xC9\x74\x11";
	LPCSTR MaskGlobalPTR = "xxx??xxxxxxxx";

	LPCSTR SignatureLocalScriptsPTR = "\x48\x8B\x05\xCC\xCC\x8B\xCF\x48\x8B\x0C\xC8\x39\x59\x68";
	LPCSTR MaskLocalScriptsPTR = "xxx??xxxxxxxxx";

	LPCSTR SignatureReplayInterfacePTR = "\x48\x8D\x0D\xCC\xCC\x48\x8B\xD7\xE8\xCC\xCC\x48\x8D\x0D\xCC\xCC\x8A\xD8\xE8";
	LPCSTR MaskLocalReplayInterfacePTR = "xxx??xxxx??xxx??xxx";

	hack::module mod = hack::GetModule(L"GTA5.exe");

	DWORD64 TempWorldPTR = hack::FindSignature(mod.dwBase, mod.dwSize, SignatureWorldPTR, MaskWorldPTR);
	hack::WorldPTR = TempWorldPTR + hack::readInteger(pHandle, TempWorldPTR + 3) + 7;

	DWORD64 TempBlipPTR = hack::FindSignature(mod.dwBase, mod.dwSize, SignatureBlipPTR, MaskBlipPTR);
	hack::BlipPTR = TempBlipPTR + hack::readInteger(pHandle, TempBlipPTR + 3) + 7;

	DWORD64 TempGlobalPTR = hack::FindSignature(mod.dwBase, mod.dwSize, SignatureGlobalPTR, MaskGlobalPTR);
	hack::GlobalPTR = TempGlobalPTR + hack::readInteger(pHandle, TempGlobalPTR + 3) + 7;


}
