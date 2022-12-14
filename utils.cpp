#include "utils.h"

uintptr_t hack::GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t hack::FindPointer(HANDLE hproc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(hproc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

uintptr_t hack::GetPID(const wchar_t* procName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	uintptr_t pID = NULL;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szExeFile, procName) == 0) {
				pID = entry.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &entry));
	}
	CloseHandle(snapshot);
	return pID;
}

uintptr_t hack::GetModuleSize(DWORD pID, const wchar_t* moduleName)
{
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);
	uintptr_t moduleSize = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

	if (Module32First(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szModule, moduleName) == 0) {
				moduleSize = (uintptr_t)entry.modBaseSize;
				break;
			}
		} while (Module32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return moduleSize;
}

INT64 hack::readPointer(HANDLE hproc, DWORD64 Address)
{
	INT64 value;
	ReadProcessMemory(hproc, (INT64*)Address, &value, sizeof(value), 0);
	return value;
}

int hack::readInteger(HANDLE hproc, DWORD64 Address)
{
	int value;
	ReadProcessMemory(hproc, (BYTE*)Address, &value, sizeof(value), 0);
	return value;
}

HANDLE hack::GetProcess(const wchar_t* processName)
{
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);

	do
		if (!_wcsicmp(entry.szExeFile, processName)) {
			hack::TargetId = entry.th32ProcessID;
			CloseHandle(handle);
			hack::TargetProcess = OpenProcess(PROCESS_ALL_ACCESS, false, static_cast<DWORD>(hack::TargetId));
			return hack::TargetProcess;
		}
	while (Process32Next(handle, &entry));

	return nullptr;
}

hack::module hack::GetModule(const wchar_t* moduleName) {
	HANDLE hmodule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, static_cast<DWORD>(hack::TargetId));
	MODULEENTRY32 mEntry;
	mEntry.dwSize = sizeof(mEntry);

	do {
		if (!_wcsicmp(mEntry.szModule, (const wchar_t*)moduleName)) {
			CloseHandle(hmodule);

			hack::TargetModule = { (DWORD64)mEntry.hModule, mEntry.modBaseSize };
			return hack::TargetModule;
		}
	} while (Module32Next(hmodule, &mEntry));

	hack::module mod = { (DWORD64)false, (DWORD64)false };
	return mod;
}

template <typename var>
bool hack::WriteMemory(DWORD64 Address, var Value) {
	return WriteProcessMemory(hack::TargetProcess, (LPVOID)Address, &Value, sizeof(var), 0);
}

template <typename var>
var hack::ReadMemory(DWORD64 Address) {
	var value;
	ReadProcessMemory(hack::TargetProcess, (LPCVOID)Address, &value, sizeof(var), NULL);
	return value;
}

bool hack::MemoryCompare(const BYTE* bData, const BYTE* bMask, const char* szMask) {
	for (; *szMask; ++szMask, ++bData, ++bMask) {
		if (*szMask == 'x' && *bData != *bMask) {
			return false;
		}
	}
	return (*szMask == NULL);
}

DWORD64 hack::FindSignature(DWORD64 start, DWORD64 size, const char* sig, const char* mask)
{
	BYTE* data = new BYTE[size];
	SIZE_T bytesRead;

	ReadProcessMemory(hack::TargetProcess, (LPVOID)start, data, size, &bytesRead);

	for (DWORD64 i = 0; i < size; i++)
	{
		if (hack::MemoryCompare((const BYTE*)(data + i), (const BYTE*)sig, mask)) {
			return start + i;
		}
	}
	delete[] data;
	return NULL;
}