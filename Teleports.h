#pragma once
#include "common.h"
#include "Settings.h"

class Teleports;
inline Teleports* g_teleports = nullptr;

class Teleports
{
public:
	Teleports(const std::string& file_name) : parser_(file_name) {
		g_teleports = this;
	}
	~Teleports() {
		SaveTeleportData();
		g_teleports = nullptr;
	}
public:
	nlohmann::json LoadTeleportData();
	void SaveTeleportData();

	void appendLocation(const std::string& name, Vector3 location);
	void deleteLocation(size_t index);

	nlohmann::json GetTeleportList()
	{
		return locations_;
	}

public:
	int sel_index_tp = -1;

private:
	JSONReader parser_;
	nlohmann::json locations_;
};

