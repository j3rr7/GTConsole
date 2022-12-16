#pragma once
#include "common.h"

class Memory
{
public:
	std::wstring m_name;
	HANDLE m_handle;
	uint32_t m_pid;
	uintptr_t p_base_address;
	uint32_t p_size;

public: 
	Memory(const wchar_t name[MAX_PATH]);
	~Memory();

public:
	bool is_running();
	void set_paused(bool toggle);
	uint64_t allocate(size_t size);

	template<typename T>
	BOOL read(LPCVOID baseAddr, LPVOID pbuffer, SIZE_T size = NULL, SIZE_T* numOfByteRead = NULL) 
	{
		size = (size == NULL) ? sizeof(T) : size;
		return ReadProcessMemory(m_handle, baseAddr, pbuffer, size, numOfByteRead);
	}

	template<typename T> 
	BOOL write(LPVOID baseAddr, LPCVOID pbuffer, SIZE_T size = NULL, SIZE_T* numOfByteWrite = NULL)
	{
		size = (size == NULL) ? sizeof(T) : size;
		return WriteProcessMemory(m_handle, baseAddr, pbuffer, size, numOfByteWrite);
	}

	template<typename T> 
	T readMemory(DWORD_PTR BaseAddress, std::vector<INT64> offsets = {})
	{
		T ret;
		for (SIZE_T i = 0; i < offsets.size(); i++)
		{
			if (BaseAddress == 0) return T();
			read<DWORD_PTR>(LPCVOID(BaseAddress), &BaseAddress);
			BaseAddress += offsets[i];
		}
		read<T>(LPCVOID(BaseAddress), &ret);
		return ret;
	}

	template<typename T> 
	void writeMemory(DWORD_PTR BaseAddress, std::vector<INT64> offsets, T value)
	{
		for (SIZE_T i = 0; i < offsets.size(); i++)
		{
			if (BaseAddress == 0) return;
			read<DWORD_PTR>(LPCVOID(BaseAddress), &BaseAddress);
			BaseAddress += offsets[i];
		}
		write<T>(LPVOID(BaseAddress), &value);
	}
	std::string read_str(DWORD_PTR BaseAddress, SIZE_T nSize, std::vector<INT64> offsets = {});
	void write_str(DWORD_PTR BaseAddress, std::string str, SIZE_T nSize, std::vector<INT64> offsets = {});

public:
	BOOL listSystemProcesses(const wchar_t szExeFile[MAX_PATH], LPPROCESSENTRY32 pe32)
	{
		HANDLE m_handleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (m_handleSnap == INVALID_HANDLE_VALUE) {
			MessageBox(
				NULL,
				(LPCWSTR)L"CreateToolhelp32Snapshot (of processes)",
				(LPCWSTR)L"Error",
				MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND
			);
			return(FALSE);
		}
		pe32->dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(m_handleSnap, pe32))
		{
			MessageBox(
				NULL,
				(LPCWSTR)L"Process32First",
				(LPCWSTR)L"Error",
				MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND
			);
			CloseHandle(m_handleSnap);
			return(FALSE);
		}

		do
		{
			if (lstrcmp(szExeFile, pe32->szExeFile) == 0)
			{
				CloseHandle(m_handleSnap);
				return(TRUE);
			}
		} while (Process32Next(m_handleSnap, pe32));
		CloseHandle(m_handleSnap);
		return(FALSE);
	}

	BOOL listSProcessModules(uint32_t dwProcessId, const wchar_t szModule[MAX_MODULE_NAME32 + 1], LPMODULEENTRY32 me32)
	{
		HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcessId);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			MessageBox(
				NULL,
				(LPCWSTR)L"CreateToolhelp32Snapshot (of modules)",
				(LPCWSTR)L"Error",
				MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND
			);
			return(FALSE);
		}
		me32->dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(hModuleSnap, me32))
		{
			MessageBox(
				NULL,
				(LPCWSTR)L"Module32First",
				(LPCWSTR)L"Error",
				MB_ICONSTOP | MB_ICONERROR | MB_ICONHAND
			);
			CloseHandle(hModuleSnap);
			return(FALSE);
		}

		do
		{
			if (lstrcmp(szModule, me32->szModule) == 0)
			{
				CloseHandle(hModuleSnap);
				return(TRUE);
			}
		} while (Module32Next(hModuleSnap, me32));
		CloseHandle(hModuleSnap);
		return(FALSE);
	}
};

