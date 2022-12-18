#include "GTAModule.h"

GTAModule::GTAModule(): Memory(L"GTA5.exe")
{
    gta5 = this;
}

GTAModule::~GTAModule()
{
    gta5 = nullptr;
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
        std::string str = read_str(g_pointers->LocalScriptsPTR, MAX_PATH, { i * 0x8, 0xD0 });
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

void GTAModule::to_waypoint(int64_t ped)
{
    int64_t blip;
    if (!get_blip(blip, { 8 }, { 84 })) return;
    int64_t entity = ped_is_in_vehicle(ped) ? ped_get_current_vehicle(ped) : ped;
    Vector3 pos = get_blip_pos(blip);
    pos.z += 1;
    if (pos.z == 21.0) pos.z = -225.0;
    entity_set_position(entity, pos);
}

void GTAModule::to_objective(int64_t ped)
{
    int64_t blip;
    if (!get_blip(blip, { 1 }, { 5, 60, 66 })) return;
    int64_t entity = ped_is_in_vehicle(ped) ? ped_get_current_vehicle(ped) : ped;
    Vector3 pos = get_blip_pos(blip);
    pos.z += 1;
    if (pos.z == 21.0) pos.z = -225.0;
    entity_set_position(entity, pos);
}

Vector3 GTAModule::get_current_location(int64_t ped)
{
    int64_t entity = ped_is_in_vehicle(ped) ? ped_get_current_vehicle(ped) : ped;
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
