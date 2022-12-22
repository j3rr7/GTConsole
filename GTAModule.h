#pragma once
#include "common.h"
#include "Memory.h"
#include "ThreadPool.h"
#include "Pointers.h"
#include "Settings.h"

class GTAModule : public Memory
{
public:
	GTAModule() ;
	~GTAModule() ;

public:
	void on_tick();

public:
	int64_t GA(int index);

	template<typename T> 
	T GG(int index)  
	{ 
		return readMemory<T>(GA(index));
	}

	template<typename T>
	void SG(int index, T value) 
	{ 
		writeMemory<T>(GA(index), {}, value);
	}

	int64_t LA(std::string name, int index);

	template<typename T> 
	T GL(std::string name, int index)
	{ 
		return readMemory<T>(LA(name, index));
	}

	template<typename T> 
	void SL(std::string name, int index, T value)
	{ 
		writeMemory<T>(LA(name, index), {}, value);
	}

	bool get_blip(int64_t& blip, std::vector<int> icons, std::vector<int> colors = {});
	Vector3 get_blip_pos(int64_t blip);

	Vector3 get_entity_location(int64_t entity);
	int64_t get_local_ped();
	int64_t ped_get_current_vehicle(int64_t ped);

	bool ped_is_in_vehicle(int64_t ped);
	void entity_set_position(int64_t entity, Vector3 pos);

	void to_waypoint();
	void to_objective();

	uint32_t joaat(std::string str);

	void create_basic_vehicle(uint32_t modelHash, Vector3 location, bool is_pegasus);
	void simple_vehicle_spawner(uint32_t modelHash, Vector3 location, bool is_pegasus);

	bool Is_Bit_Set(int _value, int _bit) {
		if ((_value >> _bit) & 1LL) 
			return true;
		return false;
	}
	int Set_Bit(int _value, int _bit) {
		return _value |= 1LL << _bit;
	}
	int Clear_Bit(int _value, int _bit) {
		return _value &= ~(1LL << _bit);
	}
};

inline GTAModule* gta5 = nullptr;