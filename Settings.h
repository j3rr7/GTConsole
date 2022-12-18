#pragma once
#include "common.h"

class InIParser final {
public:
    InIParser() = default;
    InIParser(const std::string& file_name) { Load(file_name); }

public:
    void Load(const std::string& file_name) {
        config_.clear();
        std::ifstream ini_file(file_name);
        std::string line;
        while (std::getline(ini_file, line)) {
            std::size_t section_start = line.find('[');
            if (section_start != std::string::npos) {
                std::size_t section_end = line.find(']');
                if (section_end != std::string::npos) {
                    std::string section = line.substr(section_start + 1, section_end - section_start - 1);
                    while (std::getline(ini_file, line)) {
                        if (line[0] == '[') {
                            break;
                        }
                        std::size_t separator_pos = line.find('=');
                        if (separator_pos != std::string::npos) {
                            std::string key = line.substr(0, separator_pos);
                            std::string value = line.substr(separator_pos + 1);
                            config_[section + '.' + key] = value;
                        }
                    }
                }
            }
        }
        ini_file.close();
    }

    void Save(const std::string& file_name) {
        std::ofstream ini_file(file_name);
        for (const auto& [key, value] : config_) {
            std::size_t separator_pos = key.find('.');
            std::string section = key.substr(0, separator_pos);
            std::string k = key.substr(separator_pos + 1);
            ini_file << '[' << section << "]\n" << k << '=' << value << '\n';
        }
        ini_file.close();
    }

    std::string GetValue(const std::string& section, const std::string& key) const {
        std::string full_key = section + '.' + key;
        auto it = config_.find(full_key);
        if (it == config_.end()) {
            return std::string();
        }
        return it->second;
    }

    void SetValue(const std::string& section, const std::string& key, const std::string& value) {
        config_[section + '.' + key] = value;
    }
private:
    std::unordered_map<std::string, std::string> config_;
};

class Config;
inline Config* g_config = nullptr;

class Config {
public:
    Config(const std::string& file_name) : parser_(file_name) {
        g_config = this;
    }
    ~Config() { g_config = nullptr; }

    std::string GetValue(const std::string& section, const std::string& key) const {
        return parser_.GetValue(section, key);
    }

    void SetValue(const std::string& section, const std::string& key, const std::string& value) {
        parser_.SetValue(section, key, value);
    }

    void Save(const std::string& file_name) {
        parser_.Save(file_name);
    }

private:
    bool stringToBool(const std::string& str) {
        std::string lowercaseStr = str;
        std::transform(lowercaseStr.begin(), lowercaseStr.end(), lowercaseStr.begin(), ::tolower);
        std::istringstream stream(lowercaseStr);
        bool value;
        stream >> std::boolalpha >> value;
        return value;
    }

public:
    bool is_console_hidden = false;

private:
    InIParser parser_;
};
