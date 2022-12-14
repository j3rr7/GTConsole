#include "common.h"
#include "ThreadPool.h"
#include "GTAModule.h"
#include "Rendering.h"
#include "Settings.h"
#include "Teleports.h"

int main()
{
	try
	{
		// Check first if thread already exist
		// Note: Exe name muse be GTConsole please change this accordingly
		auto m_hMutex = CreateMutex(NULL, FALSE, L"GTConsole.exe");
		switch (GetLastError())
		{
		case ERROR_SUCCESS:
			break;

		case ERROR_ALREADY_EXISTS:
		default:
			MessageBoxA(NULL, "Another instance is already running!\nOnly one instance is allowed to running", "Error", MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND);
			return TRUE;
		}

		SetConsoleTitle(L"Log Console");
		auto thread_pool = std::make_unique<ThreadPool>(4);

		auto config_instance = std::make_unique<Config>("settings.json");
		config_instance->PopulateValue();

		auto teleports_instance = std::make_unique<Teleports>("teleports.json");
		teleports_instance->LoadTeleportData();

		bool isRunning = true;
		auto gui_instance = std::make_unique<Rendering>();
		auto gta_instance = std::make_unique<GTAModule>();
		auto pointers_instance = std::make_unique<Pointers>();

		auto game_thread = [&]
		{
			std::cout << "======= DEBUG =======\n";
			std::cout << "[-] WorldPTR:          " << HEX_TO_UPPER(g_pointers->WorldPTR) << "\n";
			std::cout << "[-] BlipPTR:           " << HEX_TO_UPPER(g_pointers->BlipPTR) << "\n";
			std::cout << "[-] ReplayInterfacePTR:" << HEX_TO_UPPER(g_pointers->ReplayInterfacePTR) << "\n";
			std::cout << "[-] LocalScriptsPTR:   " << HEX_TO_UPPER(g_pointers->LocalScriptsPTR) << "\n";
			std::cout << "[-] GlobalPTR:         " << HEX_TO_UPPER(g_pointers->GlobalPTR) << "\n";
			std::cout << "[-] PlayerCountPTR:    " << HEX_TO_UPPER(g_pointers->PlayerCountPTR) << "\n";
			std::cout << "[-] PickupDataPTR:     " << HEX_TO_UPPER(g_pointers->PickupDataPTR) << "\n";
			std::cout << "[-] WeatherADDR:       " << HEX_TO_UPPER(g_pointers->WeatherADDR) << "\n";
			std::cout << "[-] SettingsPTRs:      " << HEX_TO_UPPER(g_pointers->SettingsPTRs) << "\n";
			std::cout << "[-] AimCPedPTR:        " << HEX_TO_UPPER(g_pointers->AimCPedPTR) << "\n";
			std::cout << "[-] FriendlistPTR:     " << HEX_TO_UPPER(g_pointers->FriendlistPTR) << "\n";
			std::cout << "[-] ThermalADDR:       " << HEX_TO_UPPER(g_pointers->ThermalADDR) << "\n";
			std::cout << "[-] NightvisionADDR:   " << HEX_TO_UPPER(g_pointers->NightvisionADDR) << "\n";
			std::cout << "[-] BlackoutADDR:      " << HEX_TO_UPPER(g_pointers->BlackoutADDR) << "\n";
			std::cout << "======= END DEBUG =======\n";

			while (isRunning && gta5->is_running())
			{
				if (g_config->is_keybind_active)
				{
					if (GetAsyncKeyState(g_config->vk_hotkey_waypoint) & 0x8000) {
						gta5->to_waypoint();
					}

					if (GetAsyncKeyState(g_config->vk_hotkey_objective) & 0x8000) {
						gta5->to_objective();
					}
				}

				gta5->on_tick();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			isRunning = false;
		};

		thread_pool->enqueue(game_thread);

		auto rendering_thread = [&]
		{
			gui_instance->on_init();

			while (isRunning && gui_instance->is_running())
			{
				gui_instance->on_tick();

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			gui_instance->on_destroy();

			isRunning = false;
		};

		thread_pool->enqueue(rendering_thread);

		while (isRunning) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }	
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
