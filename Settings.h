#pragma once
#include "common.h"
#include "VirtualKeyEnum.h"

class JSONReader {
public:
    JSONReader(const std::string& file_path) : file_path_(file_path) {}

    nlohmann::json Read() {
        nlohmann::json data;

        std::cout << "[+] Reading " << file_path_ << "\n";

        std::ifstream file(file_path_);
        if (!file.is_open()) {
            std::cerr << "[!] Error reading file " << file_path_ << std::endl;
            return data;
        }

        data = nlohmann::json::parse(file);
        file.close();
        return data;
    }

    void Write(const nlohmann::json& data) {
        std::ofstream file(file_path_);
        if (!file.is_open()) {
            std::cerr << "[!] Error writing file " << file_path_ << std::endl;
            return;
        }
        file << std::setw(2) << data << std::endl;
        file.close();
    }

private:
    std::string file_path_;
};

class Config;
inline Config* g_config = nullptr;

class Config {
public:
    Config(const std::string& file_name) : parser_(file_name) {
        g_config = this;
    }
    ~Config() { g_config = nullptr; }

    void PopulateValue()
    {
        auto data = parser_.Read();
        if (data.is_null())
        {
            std::cout << "[+] Writing default settings.json\n";
            nlohmann::json default_data = nlohmann::json::parse(R"(
{"General": {"no_console": false,"enable_global": true},
"Keybinds": {"enable": true,"tp_waypoint": "VK_F6","tp_objective": "VK_F7"},
"Offsets": {"o_CPed": "0x8"}}
            )");
            parser_.Write(default_data);
            data = default_data;
        }
        /*for (auto& el : data.items()) {
            std::cout << el.key() << " : " << el.value() << "\n";
        }*/

        is_console_hidden = data["General"]["no_console"];
        is_globals_enabled = data["General"]["enable_global"];
        is_keybind_active = data["Keybinds"]["enable"];
        vk_hotkey_waypoint = StringToVirtualKeyCode(data["Keybinds"]["tp_waypoint"]);
        vk_hotkey_objective = StringToVirtualKeyCode(data["Keybinds"]["tp_objective"]);

        std::cout << "[+] Done reading settings\n";
    }

public:
    // ToDo: change this into struct or class for better readability and serialization
    bool is_anti_afk = false;
    bool is_console_hidden = false;
    bool is_globals_enabled = true;
    bool is_always_ontop = false;

    bool is_god_mode = false;
    bool is_never_wanted = false;
    bool is_cop_blind = false;
    bool is_cop_bribed = false;
    bool is_disable_collision = false;

    bool is_keybind_active = true;

    bool protect_ceo_kick = false;
    bool protect_kick_crash = false;
    bool protect_ceo_ban = false;
    bool protect_sound_spam = false;
    bool protect_inf_load = false;
    bool protect_colectible = false;
    bool protect_passive_mode = false;
    bool protect_trans_error = false;
    bool protect_remove_money_message = false;
    bool protect_extra_teleport = false;
    bool protect_clear_wanted = false;
    bool protect_off_radar = false;
    bool protect_send_cutscene = false;
    bool protect_remote_godmode = false;
    bool protect_personal_veh_destroy = false;

    int vk_hotkey_waypoint = 0;
    int vk_hotkey_objective = 0;

    struct GOffsets {
        uint64_t GlobalOffset = 262145;
    };
private:
    JSONReader parser_;
};
