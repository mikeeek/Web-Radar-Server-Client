#pragma once
#include <windows.h>
#include <string>
#include <TlHelp32.h>


struct Module
{
	uintptr_t module_base;
	DWORD module_size;
};

class ProcessMemory
{

public:
	uintptr_t	process_id;
	HANDLE		process_handle;
	Module	client_dll;

	ProcessMemory();
	~ProcessMemory();

	Module get_module(std::string module) const;

	bool initialize(std::string process);

	template<typename T>
	T read_memory(uintptr_t address);

	std::string read_string(uintptr_t address);

};

extern ProcessMemory processMemory;

template<typename T>
inline T ProcessMemory::read_memory(uintptr_t address)
{
	T buffer;

	ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), nullptr);

	return buffer;
}



inline std::string ProcessMemory::read_string(uintptr_t address)
{
	char buffer[512];

	if (ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(address), buffer, sizeof(buffer), nullptr))
	{
		buffer[sizeof(buffer) - 1] = '\0';
		return std::string(buffer);
	}
	else
	{
		return "";
	}
}