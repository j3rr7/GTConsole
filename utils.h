#pragma once
#include "framework.h"
#include "utils.h"

namespace hack {

	DWORD pid;
	DWORD64 Base;

	DWORD64 WorldPTR;
	DWORD64 BlipPTR;
	DWORD64 PlayerPTR;
	DWORD64 GlobalPTR;
	DWORD64 Carspawn;
	DWORD64 ObjectSpawn;

	struct module
	{
		DWORD64 dwBase, dwSize;
	};

	module TargetModule;
	HANDLE TargetProcess;
	DWORD64  TargetId;

	uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
	uintptr_t FindPointer(HANDLE hproc, uintptr_t ptr, std::vector<unsigned int> offsets);
	uintptr_t GetPID(const wchar_t* procName);
	uintptr_t GetModuleSize(DWORD pID, const wchar_t* moduleName);
	INT64 readPointer(HANDLE hproc, DWORD64 Address);
	int readInteger(HANDLE hproc, DWORD64 Address);
	HANDLE GetProcess(const wchar_t* processName);
	module GetModule(const wchar_t* moduleName);

	template <typename var>
	bool WriteMemory(DWORD64 Address, var Value);

	template <typename var>
	var ReadMemory(DWORD64 Address);

	bool MemoryCompare(const BYTE* bData, const BYTE* bMask, const char* szMask);
	DWORD64 FindSignature(DWORD64 start, DWORD64 size, const char* sig, const char* mask);
}

class utils
{

};

