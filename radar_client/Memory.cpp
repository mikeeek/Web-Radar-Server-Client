#include "Memory.hpp"
#include <iostream>
#include <vector>
#include <tlhelp32.h>
#include <tchar.h>
typedef long NTSTATUS;

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

#ifndef STATUS_ACCESS_DENIED
#define STATUS_ACCESS_DENIED ((NTSTATUS)0xC0000022L)
#endif

#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#endif

ProcessMemory::ProcessMemory() = default;  
ProcessMemory::~ProcessMemory() = default; 

ProcessMemory processMemory;


Module ProcessMemory::get_module(std::string moduleName) const
{

	HANDLE localProcessHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);

	if (localProcessHandle == INVALID_HANDLE_VALUE)
		return Module();

	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(localProcessHandle, &moduleEntry))
	{
		do
		{
			if (std::string(moduleEntry.szModule) == moduleName)
			{
				CloseHandle(localProcessHandle);

				Module returnModule;
				returnModule.module_base = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
				returnModule.module_size = moduleEntry.modBaseSize;
				return returnModule;
			}
		} while (Module32Next(localProcessHandle, &moduleEntry));
	}

	CloseHandle(localProcessHandle);

	return Module();
}



DWORD MyGetProcessId2(LPCTSTR ProcessName)
{

	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) { // must call this first
		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap); // close handle on failure

	return 0;
}

bool EnableDebugPrivilege() {
	HANDLE hToken = nullptr;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return false;
	}

	TOKEN_PRIVILEGES tp = {};
	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		return false;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	BOOL ok = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	CloseHandle(hToken);
	return ok && GetLastError() == ERROR_SUCCESS;
}

HANDLE OpenProcessFullAccess(DWORD pid) {

	EnableDebugPrivilege();

	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!h) {
		DWORD err = GetLastError();
		printf("OpenProcess(PROCESS_ALL_ACCESS) failed for pid %u: %u\n", pid, err);
	}
	return h;
}

bool ProcessMemory::initialize(std::string process)
{
	int processIdCs2 = NULL;
	while (!processIdCs2)
	{
		processIdCs2 = MyGetProcessId2(TEXT("cs2.exe"));
		Sleep(1000);

	}
	process_id = processIdCs2;

	DWORD desired = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD | PROCESS_DUP_HANDLE;
	process_handle = OpenProcess(desired, FALSE, processIdCs2);

	client_dll = get_module("client.dll");

	return(client_dll.module_base);
}
