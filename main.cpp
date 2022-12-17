#include "common.h"
//#include "GTAModule.h"
#include "Rendering.h"

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
	try
	{
		if (checkRunning())
		{
			MessageBox(NULL, L"Another instance is already running!\nOnly one instance is allowed to running", L"Error", NULL);
			return 1;
		}
		SetConsoleTitle(L"Log Console"); // random jibberish :v

		//auto gta_instance = std::make_unique<GTAModule>();

		//auto pointers_instance = std::make_unique<Pointers>();

		auto gui_instance = std::make_unique<Rendering>();

		/*
		std::cout << "WorldPTR:          " << std::hex << g_pointers->WorldPTR << "\n";
		std::cout << "BlipPTR:           " << std::hex << g_pointers->BlipPTR << "\n";
		std::cout << "ReplayInterfacePTR:" << std::hex << g_pointers->ReplayInterfacePTR << "\n";
		std::cout << "LocalScriptsPTR:   " << std::hex << g_pointers->LocalScriptsPTR << "\n";
		std::cout << "GlobalPTR:         " << std::hex << g_pointers->GlobalPTR << "\n";
		
		
		std::cout << "PlayerCountPTR:    " << std::hex << g_pointers->PlayerCountPTR << "\n";
		std::cout << "PickupDataPTR:     " << std::hex << g_pointers->PickupDataPTR << "\n";
		std::cout << "WeatherADDR:       " << std::hex << g_pointers->WeatherADDR << "\n";
		std::cout << "SettingsPTRs:      " << std::hex << g_pointers->SettingsPTRs << "\n";
		std::cout << "AimCPedPTR:        " << std::hex << g_pointers->AimCPedPTR << "\n";
		std::cout << "FriendlistPTR:     " << std::hex << g_pointers->FriendlistPTR << "\n";
		std::cout << "ThermalADDR:       " << std::hex << g_pointers->ThermalADDR << "\n";
		std::cout << "NightvisionADDR:   " << std::hex << g_pointers->NightvisionADDR << "\n";
		std::cout << "BlackoutADDR:      " << std::hex << g_pointers->BlackoutADDR << "\n";*/

		while (true /*&& gta5->is_running()*/)
		{
			if (GetAsyncKeyState(VK_END) & 0x1) { break; }

			//if (GetAsyncKeyState(VK_F6) & 0x8000) {
			//	gta5->to_waypoint(gta5->get_local_ped());
			//}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		gui_instance.reset();

		//pointers_instance.reset();

		//gta_instance.reset();
	}
	catch (const std::exception& err)
	{
		std::cout << err.what() << std::endl;

		MessageBoxW(
			NULL,
			L"Error Exception",
			L"Error",
			MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND
		);

		exit(1);
	}

	return 0;
}