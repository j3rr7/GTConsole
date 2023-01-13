#include "Rendering.h"

Rendering::Rendering()
{
	is_running_ = true;
	g_rendering = this;
}

Rendering::~Rendering()
{
	g_rendering = nullptr;
}

void Rendering::on_init()
{
    wc = { sizeof(wc), CS_CLASSDC, nsRendering::WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"GTConsole", NULL };
    ::RegisterClassExW(&wc);
    hWnd = ::CreateWindowW(wc.lpszClassName, L"GTConsole GUI", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hWnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        throw std::exception("Failed to allocate D3D11 Device Renderer");
    }

    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    //SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED); scrap this
    //SetLayeredWindowAttributes(hWnd, 0, 250, LWA_ALPHA);
    ::UpdateWindow(hWnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL; // sorry but let's disable this for a while
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // ImGui::StyleColorsDark(); // ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    
    io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, Roboto_compressed_size, 16);

    style.WindowPadding = ImVec2(8.f, 8.f);
    style.FramePadding = ImVec2(8.f, 4.f);
    style.WindowRounding = 8.f;
    style.FrameRounding = 8.f;

    int hue = 150;
    float col_main_sat = 180.f / 255.f;
    float col_main_val = 161.f / 255.f;
    float col_area_sat = 124.f / 255.f;
    float col_area_val = 100.f / 255.f;
    float col_back_sat = 59.f / 255.f;
    float col_back_val = 40.f / 255.f;

    ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
    ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
    ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
    ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

    style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x + 0.3f, col_main.y + 0.3f, col_main.z, 0.90f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x + 0.3f, col_main.y, col_main.z + 0.3f, 0.24f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x + 0.3f, col_main.y, col_main.z + 0.3f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

void Rendering::on_tick()
{
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
        {
            is_running_ = false;
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

#ifdef IMGUI_HAS_VIEWPORT
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
#else 
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    {
        ImGui::Begin("Options Window", nullptr,ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

        dx_menu();

        ImGui::End();
    }
    ImGui::PopStyleVar(2);

    ImGui::Render();
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0); // Present with vsync
    //g_pSwapChain->Present(0, 0); // Present without vsync
}

void Rendering::on_destroy()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hWnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

bool Rendering::CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void Rendering::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void Rendering::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    if (FAILED(g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
        throw std::exception("Failed to get backbuffer");
    
    if (FAILED(g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView)))
        throw std::exception("Couldn't allocate Render Target View");
    pBackBuffer->Release();
}

void Rendering::CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void Rendering::dx_menu()
{
    if (ImGui::BeginTabBar("Sub Menu", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Self"))
        {
            ImGui::Checkbox("Semi God Mode", &g_config->is_god_mode);

            if (ImGui::Checkbox("Disable Collision", &g_config->is_disable_collision))
            {
                if (!g_config->is_disable_collision)
                {
                    g_thread_pool->enqueue([] {
                        if (auto p1 = gta5->readMemory<int64_t>(gta5->get_local_ped() + 0x30, { 0x10,0x20,0x70,0x0 }))
                        {
                            gta5->writeMemory<float>(p1 + 0x2C, {}, 0.25f);
                        }
                        });
                }
            }

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::Checkbox("No Wanted", &g_config->is_never_wanted);
            
            if (auto wanted_level_ptr = g_thread_pool->enqueue([] {

                return gta5->readMemory<int>(gta5->get_local_ped() + 0x10A8, { 0x888 });

                }); wanted_level_ptr.valid())
            {
                static int wanted_level = static_cast<int>(wanted_level_ptr.get());

                if (ImGui::SliderInt("Wanted Level", &wanted_level, 0, 5))
                {
                    g_thread_pool->enqueue([] {
                        gta5->writeMemory<int>(gta5->get_local_ped() + 0x10A8, { 0x888 }, wanted_level);
                        });
                }
            }

            ImGui::Dummy(ImVec2(0, 10));

            if (ImGui::CollapsingHeader("Session"))
            {
                if (ImGui::Button("Create Public Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(1);
                        });
                }
                if (ImGui::Button("Join Public Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(0);
                        });
                }
                if (ImGui::Button("Invite Only Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(11);
                        });
                }
                if (ImGui::Button("Find Friend Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(9);
                        });
                }
                if (ImGui::Button("Closed Friend Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(6);
                        });
                }
                if (ImGui::Button("Create Crew Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(3);
                        });
                }
                if (ImGui::Button("Create Closed Crew Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(2);
                        });
                }
                if (ImGui::Button("Join Crew Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(12);
                        });
                }
                if (ImGui::Button("Solo Session"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(10);
                        });
                }
                if (ImGui::Button("Leave Online"))
                {
                    g_thread_pool->enqueue([] {
                        gta5->load_session(-1);
                        });
                }
            }

            ImGui::Separator();
            ImGui::Text("");
            ImGui::Dummy(ImVec2(0, 15));

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Teleport"))
        {
            if (ImGui::Button("Teleport to Waypoint (Hotkey)"))
                g_thread_pool->enqueue([] { gta5->to_waypoint(); });

            ImGui::SameLine();

            if (ImGui::Button("Teleport to Objective (Hotkey)"))
                g_thread_pool->enqueue([] { gta5->to_objective(); });
            ImGui::EndTabItem();

            ImGui::Dummy(ImVec2(0, 15));

            if (ImGui::Button("Tp to personal vehicle"))
            {
                g_thread_pool->enqueue([] {
                    gta5->SG<int>(2639783 + 8, 1);
                    });
            }
        }

        if (ImGui::BeginTabItem("Custom Teleport"))
        {
            ImGui::BeginChild("##teleport_location", ImVec2(225.f, -30.f), true, NULL);

            auto list_teleport = g_teleports->GetTeleportList();
            for (auto it = list_teleport.begin(); it != list_teleport.end(); it++) {
                auto index = std::abs(std::distance(list_teleport.begin(), it));
                const bool is_selected = (g_teleports->sel_index_tp == index);
                std::string name = it->at("name");
                name.append("##" + std::to_string(index));
                if (ImGui::Selectable(name.c_str(), is_selected))
                {
                    g_teleports->sel_index_tp = static_cast<int>(index);
                    ImGui::OpenPopup("Teleport?");
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            if (ImGui::BeginPopupModal("Teleport?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                Vector3 teleLocation = {
                    list_teleport[g_teleports->sel_index_tp].at("x"),
                    list_teleport[g_teleports->sel_index_tp].at("y"),
                    list_teleport[g_teleports->sel_index_tp].at("z")
                };
                ImGui::Text("Teleport to this location?\nX:%.3f Y:%.3f, Z:%.3f", 
                    teleLocation.x,
                    teleLocation.y,
                    teleLocation.z
                    );
                ImGui::Separator();

                if (ImGui::Button("YES", ImVec2(120, 0))) { 
                    g_thread_pool->enqueue([&] {
                        gta5->entity_set_position(gta5->get_local_ped(),
                            { list_teleport[g_teleports->sel_index_tp].at("x"),
                              list_teleport[g_teleports->sel_index_tp].at("y"),
                              list_teleport[g_teleports->sel_index_tp].at("z") }
                            );
                        });
                    ImGui::CloseCurrentPopup(); 
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0, 25));
            ImGui::Text("Teleport Name");
            static char teleport_name[128];
            ImGui::InputText("##TeleportName", teleport_name, IM_ARRAYSIZE(teleport_name));

            if (ImGui::Button("Append Current Location"))
            {
                if (auto fEntityLoc = g_thread_pool->enqueue([] { 
                    return gta5->get_entity_location(gta5->get_local_ped()); 
                    }); fEntityLoc.valid())
                {
                    Vector3 currentLocation = fEntityLoc.get();
                    if (strlen(teleport_name) == 0)
                        g_teleports->appendLocation("Unnamed Location", currentLocation);
                    else
                        g_teleports->appendLocation(teleport_name, currentLocation);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                if (g_teleports->sel_index_tp != -1)
                    g_teleports->deleteLocation(g_teleports->sel_index_tp);
                else
                    g_teleports->deleteLocation(-1);
                g_teleports->sel_index_tp = -1;
            }

            ImGui::EndGroup();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Vehicle Spawn"))
        {
            static char model_hash[128];
            ImGui::InputText("Vehicle Model", model_hash, IM_ARRAYSIZE(model_hash));
            if (ImGui::Button("Spawn Vehicle"))
            {
                g_thread_pool->enqueue([] {
                    Vector3 ped_pos = gta5->get_entity_location(gta5->get_local_ped());
                    uint32_t modelHash = gta5->joaat(model_hash);
                    Vector2 heading = gta5->readMemory<Vector2>(gta5->get_local_ped() + 0x30, { 0x20 });
                    Vector3 new_pos{
                        ped_pos.x - (heading.y * 5.f),
                        ped_pos.y + (heading.x * 5.f),
                        ped_pos.z + 0.5f
                    };
                    gta5->create_basic_vehicle(modelHash, new_pos, false);
                    });
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Online"))
        {
            if (ImGui::Button("GetCurPlayerOffset"))
            {
                std::cout << "Player: " << gta5->GG<int>(1574918) << "\n";
            }
            if (ImGui::Button("Disable AFK interfal"))
            {
                g_thread_pool->enqueue([] {
                    gta5->SG<int>(1653913 + 1156, -1);
                    gta5->SG<int>(1653913 + 1172, -1);
                    });
            }

            if (ImGui::Button("Airstrike"))
            {
                g_thread_pool->enqueue([] {
                    gta5->SG<int>(2793044 + 4485, 1);
                    });
            }

            if (ImGui::Checkbox("Blind Cop", &g_config->is_cop_blind))
            {
                g_config->is_cop_blind ? g_thread_pool->enqueue([] {

                    auto GET_NETWORK_TIME = gta5->GG<int>(1574757 + 11);
                    gta5->SG<int>(2793044 + 4654 + 1, 1);
                    gta5->SG<int>(2793044 + 4654 + 3, GET_NETWORK_TIME + 3420000);
                    gta5->SG<int>(2793044 + 4654, 5);

                    }) : g_thread_pool->enqueue([] {
                    gta5->SG<int>(2793044 + 4654 + 1, 0);
                    gta5->SG<int>(2793044 + 4654 + 3, 0);
                    gta5->SG<int>(2793044 + 4654, 0);
                    });
            }

            ImGui::SameLine();

            if (ImGui::Checkbox("Bribe Cop", &g_config->is_cop_bribed))
            {
                g_config->is_cop_bribed ? g_thread_pool->enqueue([] {

                    auto GET_NETWORK_TIME = gta5->GG<int>(1574757 + 11);
                    gta5->SG<int>(2793044 + 4654 + 1, 1);
                    gta5->SG<int>(2793044 + 4654 + 3, GET_NETWORK_TIME + 3420000);
                    gta5->SG<int>(2793044 + 4654, 21);

                    }) : g_thread_pool->enqueue([] {
                    gta5->SG<int>(2793044 + 4654 + 1, 0);
                    gta5->SG<int>(2793044 + 4654 + 3, 0);
                    gta5->SG<int>(2793044 + 4654, 0);
                        });                
            }

            if (ImGui::CollapsingHeader("Protection"))
            {
                if (ImGui::Checkbox("Block Ceo Kick", &g_config->protect_ceo_kick))
                {
                    if (g_config->protect_ceo_kick)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669984, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669984, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Ceo Ban", &g_config->protect_ceo_ban)) {
                    if (g_config->protect_ceo_ban)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1670006, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1670006, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Some Kicks/Crashes", &g_config->protect_kick_crash)) {
                    if (g_config->protect_kick_crash)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1670036, 1);
                            gta5->SG<bool>(1670051, 1);
                            gta5->SG<bool>(1669951, 1);
                            gta5->SG<bool>(1670028, 1);
                            gta5->SG<bool>(1670238, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1670036, 0);
                            gta5->SG<bool>(1670051, 0);
                            gta5->SG<bool>(1669951, 0);
                            gta5->SG<bool>(1670028, 0);
                            gta5->SG<bool>(1670238, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Sound Spam", &g_config->protect_sound_spam)) {
                    if (g_config->protect_sound_spam)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669879, 1);
                            gta5->SG<bool>(1670243, 1);
                            gta5->SG<bool>(1669394, 1);
                            gta5->SG<bool>(1670529, 1);
                            gta5->SG<bool>(1670058, 1);
                            gta5->SG<bool>(1669421, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669879, 0);
                            gta5->SG<bool>(1670243, 0);
                            gta5->SG<bool>(1669394, 0);
                            gta5->SG<bool>(1670529, 0);
                            gta5->SG<bool>(1670058, 0);
                            gta5->SG<bool>(1669421, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Infinite Loadingscreen", &g_config->protect_inf_load)) {
                    if (g_config->protect_inf_load)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669947, 1);
                            gta5->SG<bool>(1670076, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669947, 0);
                            gta5->SG<bool>(1670076, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Give Collectibles", &g_config->protect_colectible)) {
                    if (g_config->protect_colectible)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1670208, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1670208, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Passive Mode", &g_config->protect_passive_mode))
                {
                    if (g_config->protect_passive_mode)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669996, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669996, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Transaction Error", &g_config->protect_trans_error))
                {
                    if (g_config->protect_trans_error)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669797, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669797, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Modded Notifys/SMS", &g_config->protect_remove_money_message))
                {
                    if (g_config->protect_remove_money_message)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669880, 1);
                            gta5->SG<bool>(1669426, 1);
                            gta5->SG<bool>(1670057, 1);
                            gta5->SG<bool>(1669428, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669880, 0);
                            gta5->SG<bool>(1669426, 0);
                            gta5->SG<bool>(1670057, 0);
                            gta5->SG<bool>(1669428, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Cayo && Beach Teleport", &g_config->protect_extra_teleport))
                {
                    if (g_config->protect_extra_teleport)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669741, 1);
                            gta5->SG<bool>(1670138, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669741, 0);
                            gta5->SG<bool>(1670138, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Clear Wanted", &g_config->protect_clear_wanted))
                {
                    if (g_config->protect_clear_wanted)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669938, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669938, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Off The Radar", &g_config->protect_off_radar))
                {
                    if (g_config->protect_off_radar)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669940, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669940, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Send to Cutscene", &g_config->protect_send_cutscene))
                {
                    if (g_config->protect_send_cutscene)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1670198, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1670198, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Remove Godmode", &g_config->protect_remote_godmode))
                {
                    if (g_config->protect_remote_godmode)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669396, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669396, 0);
                            });
                    }
                }

                if (ImGui::Checkbox("Block Personal Vehicle Destroy", &g_config->protect_personal_veh_destroy))
                {
                    if (g_config->protect_personal_veh_destroy)
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669480, 1);
                            gta5->SG<bool>(1670063, 1);
                            gta5->SG<bool>(1669947, 1);
                            });
                    }
                    else
                    {
                        g_thread_pool->enqueue([] {
                            gta5->SG<bool>(1669480, 0);
                            gta5->SG<bool>(1670063, 0);
                            gta5->SG<bool>(1669947, 0);
                            });
                    }
                }
            }

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Special Cargo"))
            {
                if (ImGui::Button("No Sell Cooldown"))
                {
                    gta5->SG<int>(262145 + 15554, 0);
                }

                if (ImGui::Button("Instant finish special crago sell mission"))
                {
                    gta5->SL("GB_CONTRABAND_SELL", 540 + 1, 99999);
                }

                if (ImGui::Button("Instant finish buy/steal mission"))
                {
                    gta5->SL("GB_CONTRABAND_BUY", 598 + 5, 1);
                    gta5->SL("GB_CONTRABAND_BUY", 598 + 191, 6);
                    gta5->SL("GB_CONTRABAND_BUY", 598 + 192, 4);
                }

                static int cargo_sell_mult = 100;
                ImGui::SliderInt("Cargo Sell Price", &cargo_sell_mult, 0, 5000000);
                if (ImGui::Button("Set Sell Price (3.2M)"))
                {
                    auto base_address = 15788;
                    gta5->SG<int>(262145 + base_address, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 1, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 2, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 3, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 4, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 5, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 6, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 7, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 8, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 9, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 10, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 11, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 12, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 13, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 14, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 15, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 16, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 17, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 18, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 19, cargo_sell_mult);
                    gta5->SG<int>(262145 + base_address + 20, cargo_sell_mult);
                }
            }

            if (ImGui::CollapsingHeader("Nightclub"))
            {
                static int nightclub_sale_price=100; //138000
                ImGui::SliderInt("Nightclub Sales Price", &nightclub_sale_price, 0, 140000);
                if (ImGui::Button("NightClub Sell Change Price"))
                {
                    auto base_address = 24381;// --offset for v 1.64 nightclub goods
                    gta5->SG<int>(262145 + base_address, nightclub_sale_price);
                    gta5->SG<int>(262145 + base_address + 1, nightclub_sale_price);
                    gta5->SG<int>(262145 + base_address + 2, nightclub_sale_price);
                    gta5->SG<int>(262145 + base_address + 3, nightclub_sale_price);
                    gta5->SG<int>(262145 + base_address + 4, nightclub_sale_price);
                    gta5->SG<int>(262145 + base_address + 5, nightclub_sale_price);
                    gta5->SG<int>(262145 + base_address + 6, nightclub_sale_price);
                }
            }

            ImGui::EndTabItem();
        }

        //if (ImGui::BeginTabItem("Stats"))
        //{
        //    static char stats_name[256];
        //    ImGui::InputText("Name", stats_name, IM_ARRAYSIZE(stats_name));
        //    /*
        //    static std::string stat_name;
        //    gta5->GG<int>(1574918); grab mp0 or mp1 player
        //    auto stat_hash = rage::joaat(stat_name);
        //    auto oldhash = gta5->GG<int>(1665454+4)
        //    auto oldvalue = gta5->GG<int>(1010831+5525)
        //    gta5->SG<int>(1665454+4,hash)
        //    gta5->SG<int>(1010831+5525,value)
        //    gta5->SG<int>(1653913+1139,-1)
        //    gta5->SG<int>(1665454+4,oldhash)
        //    gta5->SG<int>(1010831+5525,oldvalue)
        //    */
        //    ImGui::EndTabItem();
        //}

        if (ImGui::BeginTabItem("Unlocks"))
        {
            if (ImGui::Button("Unlock all Drug Wars content"))
            {
                for (int i = 33974; i < 34112; i++)
                {
                    gta5->SG<int>(262145 + i, 1);
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Unsafe!")) // enjoy ban :D
        {
            if (ImGui::Button("Refund Orbital 500k"))
            {
                g_thread_pool->enqueue([] {
                    gta5->SG<int>(1968313, 1);
                    });
            }

            if (ImGui::Button("Refund Orbital 750k"))
            {
                g_thread_pool->enqueue([] {
                    gta5->SG<int>(1968313, 2);
                    });
            }
            
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Options"))
        {
            if (ImGui::Checkbox("Always On Top", &g_config->is_always_ontop))
            {
                if (g_config->is_always_ontop)
                {
                    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }
                else
                {
                    SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }
            }

            ImGui::Checkbox("Globals Enabled", &g_config->is_globals_enabled);

            ImGui::Checkbox("Keybinds Enabled", &g_config->is_keybind_active);

            if (ImGui::Button("DEBUG BUTTON"))
            {
                for (int i = 0; i < 54; i++)
                {
                    std::string str = gta5->read_str(g_pointers->LocalScriptsPTR, MAX_PATH, { i * 0x8, 0xD4 });
                    std::cout << "Str Name: " << "[" << str << "]" << "\n";
                }
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::SetCursorPosY((ImGui::GetWindowSize().y + ImGui::GetScrollY() - 27.f));
    ImGui::Separator();
    ImGui::Text("Frame %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall nsRendering::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!g_rendering)
        return false;

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg)
    {
    case WM_SIZE:
        
        if (g_rendering->g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_rendering->CleanupRenderTarget();
            g_rendering->g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            g_rendering->CreateRenderTarget();
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
