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
	


    std::cout << "Test" << std::endl;
}
