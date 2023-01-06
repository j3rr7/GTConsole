#include "GTAModule.h"

GTAModule::GTAModule() : Memory(L"GTA5.exe")
{
    gta5 = this;
}

GTAModule::~GTAModule()
{
    gta5 = nullptr;
}

void GTAModule::on_tick()
{
    if (g_config->is_god_mode)
    {
        // Only write if armor or health below certain threshold
        if (gta5->readMemory<float>(get_local_ped() + 0x280) < 325 ||
            gta5->readMemory<float>(get_local_ped() + 0x150C) < 50
            )
        {
            gta5->writeMemory<float>(get_local_ped() + 0x280, {}, 325);
            gta5->writeMemory<float>(get_local_ped() + 0x284, {}, 325);
            gta5->writeMemory<float>(get_local_ped() + 0x150C, {}, 50); // 50 online | 100 offline
        }
    }
    if (g_config->is_never_wanted)
    {
        // Only write if wanted level more than 0
        if (gta5->readMemory<int>(gta5->get_local_ped() + 0x10A8, { 0x888 }) > 0)
            gta5->writeMemory<int>(gta5->get_local_ped() + 0x10A8, { 0x888 }, 0);
    }
    if (g_config->is_disable_collision)
    {
        auto p1 = gta5->readMemory<int64_t>(gta5->get_local_ped() + 0x30, { 0x10,0x20,0x70,0x0 });
        gta5->writeMemory<float>(p1 + 0x2C, {}, -1.f);
    }
}

int64_t GTAModule::GA(int index)
{
    int a = 8 * ((index >> 0x12) & 0x3F);
    int b = 8 * (index & 0x3FFFF);
    return readMemory<int64_t>(g_pointers->GlobalPTR + a) + b;
}

int64_t GTAModule::LA(std::string name, int index)
{
    for (int i = 0; i < 54; i++)
    {
        std::string str = read_str(g_pointers->LocalScriptsPTR, MAX_PATH, { i * 0x8, 0xD4 });
        if (str == name) { return readMemory<int64_t>(g_pointers->LocalScriptsPTR, { i * 0x8, 0xB0 }) + 8 * index; }
    }
    return 0;
}

bool GTAModule::get_blip(int64_t& blip, std::vector<int> icons, std::vector<int> colors)
{
    for (int i = 0; i < 2000; i++)
    {
        int64_t p = readMemory<int64_t>(g_pointers->BlipPTR + i * 0x8);
        if (p == 0) continue;
        int icon = readMemory<int>(p + 0x40);
        int color = readMemory<int>(p + 0x48);
        if (std::find(icons.begin(), icons.end(), icon) != icons.end())
        {
            if (colors.size() == 0 || std::find(colors.begin(), colors.end(), color) != colors.end())
            {
                blip = p;
                return true;
            }
        }
    }
    return false;
}

Vector3 GTAModule::get_blip_pos(int64_t blip)
{
	return readMemory<Vector3>(blip + 0x10);
}

int64_t GTAModule::get_local_ped()
{
    return readMemory<int64_t>(g_pointers->WorldPTR, { 0x8 });
}

int64_t GTAModule::ped_get_current_vehicle(int64_t ped)
{
    return readMemory<int64_t>(ped + 0xD10);
}

bool GTAModule::ped_is_in_vehicle(int64_t ped)
{
    return readMemory<uint8_t>(ped + 0xE32) == 0 ? false : true;
}

void GTAModule::entity_set_position(int64_t entity, Vector3 pos)
{
    writeMemory<Vector3>(entity + 0x30, { 0x50 }, pos);
    writeMemory<Vector3>(entity + 0x90, {}, pos);
}

void GTAModule::to_waypoint()
{
    int64_t ped = get_local_ped();
    int64_t blip;
    if (!get_blip(blip, { 8 }, { 84 })) return;
    int64_t entity = ped_is_in_vehicle(ped) ? ped_get_current_vehicle(ped) : ped;
    Vector3 pos = get_blip_pos(blip);
    pos.z += 1;
    if (pos.z == 21.0) pos.z = -225.0;
    entity_set_position(entity, pos);
}

void GTAModule::to_objective()
{
    int64_t ped = get_local_ped();
    int64_t blip;
    if (!get_blip(blip, { 1 }, { 5, 60, 66 })) return;
    int64_t entity = ped_is_in_vehicle(ped) ? ped_get_current_vehicle(ped) : ped;
    Vector3 pos = get_blip_pos(blip);
    pos.z += 1;
    if (pos.z == 21.0) pos.z = -225.0;
    entity_set_position(entity, pos);
}

Vector3 GTAModule::get_entity_location(int64_t entity)
{
    return readMemory<Vector3>(entity + 0x30, { 0x50 });
}

uint32_t GTAModule::joaat(std::string str)
{
    unsigned int Uint32 = 0xffffffff;
    unsigned int hash = 0;
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    for (int i = 0; i < str.length(); i++)
    {
        hash = hash + (unsigned char)str[i];
        hash = hash + (hash << 10);
        hash = hash ^ ((hash & Uint32) >> 6);
    }
    hash = hash + (hash << 3);
    hash = hash ^ ((hash & Uint32) >> 11);
    hash = hash + (hash << 15);
    return hash & Uint32;
}

void GTAModule::create_basic_vehicle(uint32_t modelHash, Vector3 location, bool is_pegasus = false)
{
    if (!g_config->is_globals_enabled)
        return

    SG<float>(2694560 + 7 + 0, location.x);
    SG<float>(2694560 + 7 + 1, location.y);
    SG<float>(2694560 + 7 + 2, location.z);
    SG<uint32_t>(2694560 + 27 + 66, modelHash);
    SG<int>(2694560 + 3, is_pegasus);
    SG(2694560 + 5, 1); // car spawn flag odd
    SG(2694560 + 2, 1); // car spawn toggle
    // SG(2694560 + 27 + 1, 0); // license plate bit
    // SG<std::string>(2694560 + 27 + 1, "F U");
    SG<int>(2694560 + 27 + 5, 159); // primary
    SG<int>(2694560 + 27 + 6, 159); // secondary
    // SG<int>(2694560 + 27 + 7, -1); // pearlescent
    // SG<int>(2694560 + 27 + 8, 123); // wheel color
    // SG<int>(2694560 + 27 + 15, );  // primary weapon
    SG<int>(2694560 + 27 + 19, -1);
    // SG<int>(2694560 + 27 + 20, ); // secondary weapon
    SG<int>(2694560 + 27 + 21, 2); // engine(0 - 3)
    SG<int>(2694560 + 27 + 22, 6); // brakes(0 - 6)
    SG<int>(2694560 + 27 + 23, 9); // transmission(0 - 9)
    SG<int>(2694560 + 27 + 24, 3); // horn(0 - 77)
    SG<int>(2694560 + 27 + 25, 2); // suspension(0 - 13)
    SG<int>(2694560 + 27 + 26, 20); // armor(0 - 18)
    SG<int>(2694560 + 27 + 27, 1); // turbo(0 - 1)
    SG<int>(2694560 + 27 + 28, 1); // weaponised ownerflag
    //SG<int>(2694560 + 27 + 32, ) // colored light(0 - 14)
    //SG<int>(2694560 + 27 + 33, ) // wheel selection
    SG<int>(2694560 + 27 + 60, 1);
    SG<int>(2694560 + 27 + 62, 0); // Tire smoke color R
    SG<int>(2694560 + 27 + 63, 0); // G
    SG<int>(2694560 + 27 + 64, 0); // B
    SG<int>(2694560 + 27 + 65, 0); // Window tint 0 - 6
    // SG<int>(2694560 + 27 + 67, 0); // Livery
    // SG<int>(2694560 + 27 + 69, ); // Wheel type
    SG<int>(2694560 + 27 + 74, 0); // Neon R 0 - 255
    SG<int>(2694560 + 27 + 75, 0); // G
    SG<int>(2694560 + 27 + 76, 0); // B
    SG<int64_t>(2694560 + 27 + 77, 4030726305); // vehstate
    writeMemory<int8_t>(GA(2694560 + 27 + 77) + 1, {}, 2); // 2:bulletproof 0 : false
    SG<int>(2694560 + 27 + 95, 14); // ownerflag
    SG<int>(2694560 + 27 + 94, 2); // personal car ownerflag
}

void GTAModule::load_session(int session_id)
{
    if (!g_config->is_globals_enabled)
        return;

    switch (session_id)
    {
        case -1:
            gta5->SG<int>(1574589 + 2, -1);
            gta5->SG<int>(1574589, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            gta5->SG<int>(1574589, 0);
            break;
        case 0: // join public
        case 1: // new public
        case 2: // closed crew
        case 3: // crew session
        case 6: // closed friend
        case 9: // find friend
        case 10: // solo
        case 11: // invite only
        case 12: // join crew
            gta5->SG<int>(1575017, session_id);
            gta5->SG<int>(1574589, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            gta5->SG<int>(1574589, 0);
            break;
        default:
            std::cout << "[~] Session Type: " << gta5->GG<int>(1574589 + 2) << ", Transition: " << gta5->GG<int>(1574589) << "\n";
    }
}
