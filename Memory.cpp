#include "Memory.h"

Memory::Memory(const wchar_t name[MAX_PATH]) :
	m_name(name)
{
	PROCESSENTRY32 pe32;
	if (listSystemProcesses(name, &pe32) == FALSE)
		throw std::exception("Failed to find process id");

	m_pid = pe32.th32ProcessID;
	m_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
	if (!m_handle) throw std::exception("Failed to open handle");

	MODULEENTRY32 me32;
	if (listSProcessModules(m_pid, name, &me32) == FALSE)
		throw std::exception("Failed to get modules info");

	p_base_address = (uintptr_t)me32.modBaseAddr;
	p_size = me32.modBaseSize;
}

Memory::~Memory()
{
	CloseHandle(m_handle);
}

bool Memory::is_running()
{
	return WaitForSingleObject(m_handle, 0) == WAIT_TIMEOUT;
}

void Memory::set_paused(bool toggle)
{
	toggle ? SuspendThread(m_handle) : ResumeThread(m_handle);
}

uint64_t Memory::allocate(size_t size)
{
	return reinterpret_cast<uint64_t>(
		VirtualAllocEx(m_handle, nullptr, size, MEM_COMMIT, PAGE_READWRITE)
	);
}

std::string Memory::read_str(DWORD_PTR BaseAddress, SIZE_T nSize, std::vector<INT64> offsets)
{
	char* str = new char[nSize];
	memset(str, 0, nSize);
	for (SIZE_T i = 0; i < offsets.size(); i++)
	{
		if (BaseAddress == 0) { delete[] str; return std::string(); }
		read<DWORD_PTR>(LPCVOID(BaseAddress), &BaseAddress);
		BaseAddress += offsets[i];
	}
	read<char>(LPCVOID(BaseAddress), str, nSize);
	std::string ret = str;
	delete[] str;
	return ret;
}

void Memory::write_str(DWORD_PTR BaseAddress, std::string str, SIZE_T nSize, std::vector<INT64> offsets)
{
	for (SIZE_T i = 0; i < offsets.size(); i++)
	{
		if (BaseAddress == 0) return;
		read<DWORD_PTR>(LPCVOID(BaseAddress), &BaseAddress);
		BaseAddress += offsets[i];
	}
	write<char>(LPVOID(BaseAddress), str.c_str(), nSize);
}
