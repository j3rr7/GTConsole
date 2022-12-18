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
		// Check If Another Instance is running
		if (checkRunning())
		{
			MessageBox(NULL, L"Another instance is already running!\nOnly one instance is allowed to running", L"Error", NULL);
			return 1;
		}

		// Set Console Title
		SetConsoleTitle(L"Log Console");
		
		// Initialize Thread Pool 
		ThreadPool pool(2);

		// Reading Settings (ToDo: Add This)
		Config g_settings("settings.ini");

		// Grab our pointers and classes
		auto gta_instance = std::make_unique<GTAModule>();
		auto pointers_instance = std::make_unique<Pointers>();
		auto gui_instance = std::make_unique<Rendering>();

		bool isRunning = true; // our global for now

		// Set Up Game Thread for game content
		auto game_thread = [&] 
		{
			// Log Game Content
			std::cout << "WorldPTR:          " << std::hex << g_pointers->WorldPTR << "\n";
			std::cout << "BlipPTR:           " << std::hex << g_pointers->BlipPTR << "\n";
			std::cout << "ReplayInterfacePTR:" << std::hex << g_pointers->ReplayInterfacePTR << "\n";
			std::cout << "LocalScriptsPTR:   " << std::hex << g_pointers->LocalScriptsPTR << "\n";
			std::cout << "GlobalPTR:         " << std::hex << g_pointers->GlobalPTR << "\n";

			/*std::cout << "PlayerCountPTR:    " << std::hex << g_pointers->PlayerCountPTR << "\n";
			std::cout << "PickupDataPTR:     " << std::hex << g_pointers->PickupDataPTR << "\n";
			std::cout << "WeatherADDR:       " << std::hex << g_pointers->WeatherADDR << "\n";
			std::cout << "SettingsPTRs:      " << std::hex << g_pointers->SettingsPTRs << "\n";
			std::cout << "AimCPedPTR:        " << std::hex << g_pointers->AimCPedPTR << "\n";
			std::cout << "FriendlistPTR:     " << std::hex << g_pointers->FriendlistPTR << "\n";
			std::cout << "ThermalADDR:       " << std::hex << g_pointers->ThermalADDR << "\n";
			std::cout << "NightvisionADDR:   " << std::hex << g_pointers->NightvisionADDR << "\n";
			std::cout << "BlackoutADDR:      " << std::hex << g_pointers->BlackoutADDR << "\n"; */

			while (isRunning && gta5->is_running())
			{
				// Setup Keybind

				// Keybinds Start
				if (GetAsyncKeyState(VK_END) & 0x1) { break; }

				if (GetAsyncKeyState(VK_F6) & 0x8000) {
					gta5->to_waypoint(gta5->get_local_ped());
				}

				if (GetAsyncKeyState(VK_F7) & 0x8000) {
					gta5->to_objective(gta5->get_local_ped());
				}
				// Keybinds End
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			MessageBoxA(
				NULL,
				"GTA5 Exitted, Program will still running...",
				"Error",
				MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND
			);
		};

		// Set Up Rendering Thread for gui content
		auto rendering_thread = [&] 
		{
			gui_instance->on_init();

			while (isRunning)
			{
				gui_instance->on_tick();

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			
			gui_instance->on_destroy();
		};

		pool.enqueue(game_thread);
		pool.enqueue(rendering_thread);

		// To Do Attach this into its own fiber thread or better leave it like this :v
		while (isRunning)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		gui_instance.reset();
		pointers_instance.reset();
		gta_instance.reset();

	}
	catch (const std::exception& err)
	{
		std::cout << err.what() << std::endl;

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
