#include "Teleports.h"

nlohmann::json Teleports::LoadTeleportData()
{
	auto data = parser_.Read();
	if (data.is_null()) {
		std::cerr << "[!] Couldn't find teleports data\n[+] Creating new...\n";
		nlohmann::json j;
		j["Locations"] = {};
		data = j;
	}
	locations_ = data["Locations"];
	std::cout << "[+] Teleport Data Loaded\n";
	return locations_;
}

void Teleports::SaveTeleportData()
{
	nlohmann::json data;
	data["Locations"] = locations_;
	parser_.Write(data);
}

void Teleports::appendLocation(const std::string& name, Vector3 location)
{
	nlohmann::json j;
	j["name"] = name;
	j["x"] = static_cast<float>(location.x);
	j["y"] = static_cast<float>(location.y);
	j["z"] = static_cast<float>(location.z);
	locations_.push_back(j);
}

void Teleports::deleteLocation(size_t index = -1)
{
	if (locations_.size() >= 1)
	{
		if (index == -1)
			index = locations_.size() - 1;
		locations_.erase(index);
	}
}
