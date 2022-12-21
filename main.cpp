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

	case ERROR_ALREADY_EXISTS:
	default:
		return true;
	}
	return false;
}

int main()
{
	if (checkRunning())
	{
		MessageBoxA(NULL, "Another instance is already running!\nOnly one instance is allowed to running", "Error", MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND);
		return 1;
	}
	try
	{
		SetConsoleTitle(L"Log Console");

		std::cout << "[+] Reading Settings...\n";
		auto thread_pool = std::make_unique<ThreadPool>(4);
		auto config_instance = std::make_unique<Config>("settings.ini");

		g_config->getBoolean("General", "no_console") ? ::ShowWindow(::GetConsoleWindow(), SW_HIDE) : ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
		g_config->is_keybind_active = g_config->getBoolean("Keybinds", "enable");
		g_config->vk_hotkey_waypoint = VkKeyScanA(g_config->GetValue("Keybinds", "key_waypoint")[0]);
		g_config->vk_hotkey_objective = VkKeyScanA(g_config->GetValue("Keybinds", "key_objective")[0]);
		std::cout << "[+] Settings Loaded\n";

		bool isRunning = true;

		auto gta_instance = std::make_unique<GTAModule>();
		auto pointers_instance = std::make_unique<Pointers>();
		auto gui_instance = std::make_unique<Rendering>();

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
					if (GetAsyncKeyState(VK_END) & 0x1) { break; }

					if (GetAsyncKeyState(g_config->vk_hotkey_waypoint) & 0x8000) {
						gta5->to_waypoint(gta5->get_local_ped());
					}

					if (GetAsyncKeyState(g_config->vk_hotkey_objective) & 0x8000) {
						gta5->to_objective(gta5->get_local_ped());
					}
				}

				gta5->on_tick();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			isRunning = false;
		};

		auto rendering_thread = [&]
		{
			gui_instance->on_init();

			while (isRunning && gui_instance->is_running)
			{
				gui_instance->on_tick();

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			gui_instance->on_destroy();

			isRunning = false;
		};

		thread_pool->enqueue(game_thread);
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
