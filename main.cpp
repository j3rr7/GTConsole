#include "common.h"
#include "ThreadPool.h"
#include "GTAModule.h"
#include "Rendering.h"
#include "Settings.h"

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

		SetConsoleTitle(L"Log Console");

		std::cout << "[+] Reading Settings...\n";
		auto thread_pool = std::make_unique<ThreadPool>(4);
		auto config_instance = std::make_unique<Config>("settings.ini");
		g_config->IsConsoleHidden() ? ::ShowWindow(::GetConsoleWindow(), SW_HIDE) : ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
		std::cout << "[+] Settings Loaded\n";
		
		bool isRunning = true;

		auto gta_instance = std::make_unique<GTAModule>();
		auto pointers_instance = std::make_unique<Pointers>();

		auto game_thread = [&]
		{
			// Log Game Content
			std::cout << "======= DEBUG =======\n";
			std::cout << "[-] WorldPTR:          " << std::hex << HEX_TO_UPPER(g_pointers->WorldPTR) << "\n";
			std::cout << "[-] BlipPTR:           " << std::hex << HEX_TO_UPPER(g_pointers->BlipPTR) << "\n";
			std::cout << "[-] ReplayInterfacePTR:" << std::hex << HEX_TO_UPPER(g_pointers->ReplayInterfacePTR) << "\n";
			std::cout << "[-] LocalScriptsPTR:   " << std::hex << HEX_TO_UPPER(g_pointers->LocalScriptsPTR) << "\n";
			std::cout << "[-] GlobalPTR:         " << std::hex << HEX_TO_UPPER(g_pointers->GlobalPTR) << "\n";
			std::cout << "======= END DEBUG =======\n";

			//std::cout << "PlayerCountPTR:    " << std::hex << g_pointers->PlayerCountPTR << "\n";
			//std::cout << "PickupDataPTR:     " << std::hex << g_pointers->PickupDataPTR << "\n";
			//std::cout << "WeatherADDR:       " << std::hex << g_pointers->WeatherADDR << "\n";
			//std::cout << "SettingsPTRs:      " << std::hex << g_pointers->SettingsPTRs << "\n";
			//std::cout << "AimCPedPTR:        " << std::hex << g_pointers->AimCPedPTR << "\n";
			//std::cout << "FriendlistPTR:     " << std::hex << g_pointers->FriendlistPTR << "\n";
			//std::cout << "ThermalADDR:       " << std::hex << g_pointers->ThermalADDR << "\n";
			//std::cout << "NightvisionADDR:   " << std::hex << g_pointers->NightvisionADDR << "\n";
			//std::cout << "BlackoutADDR:      " << std::hex << g_pointers->BlackoutADDR << "\n";

			while (isRunning && gta5->is_running())
			{
				if (GetAsyncKeyState(VK_END) & 0x1) { break; }

				if (GetAsyncKeyState(VK_F6) & 0x8000) {
					gta5->to_waypoint(gta5->get_local_ped());
				}

				if (GetAsyncKeyState(VK_F7) & 0x8000) {
					gta5->to_objective(gta5->get_local_ped());
				}

				gta5->on_tick();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			pointers_instance.reset();
			gta_instance.reset();
		};

		thread_pool->enqueue(game_thread);

		auto gui_instance = std::make_unique<Rendering>();
		auto rendering_thread = [&]
		{
			gui_instance->on_init();

			while (isRunning && gui_instance->is_running)
			{
				if (GetAsyncKeyState(VK_INSERT) & 0x1) {
					SetWindowPos(gui_instance->hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}

				gui_instance->on_tick();

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			gui_instance->on_destroy();

			gui_instance.reset();

			isRunning = false;
		};
		thread_pool->enqueue(rendering_thread);

		while (isRunning) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }	

		config_instance.reset();
	}
	catch (const std::exception& err)
	{
		std::cout << "[!] " << err.what() << std::endl;

		MessageBoxA(
			NULL,
			err.what(),
			"Error",
			MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND
		);

		exit(1);
	}

	return 0;
}
