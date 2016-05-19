// Most of this code was available on internet forums. 
// I made some changes to it, but I don't claim authorship.
//

#include "Injector.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <cstdio>

bool Injector::_inject(char* procName, char* dllPath, DWORD procID = 0)
{
	// If procID is 0, the caller is passing procName as primary argument
	// and we need to find the procID via procName instead.
	if (procID == 0)
		procID = GetTargetThreadIDFromProcName(procName);

	// if procID is still 0, procID couldn't be found.
	if (procID == 0)
		return false;

	HANDLE Proc = 0;
	LPVOID RemoteString, LoadLibAddy;

	Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if (!Proc)
	{
		printf("\n\n>OpenProcess() failed: %d", GetLastError());
		return false;
	}

	LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	// Allocate space in the process for our DLL 
	RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(dllPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	// Write the string name of our DLL in the memory allocated 
	WriteProcessMemory(Proc, (LPVOID)RemoteString, dllPath, strlen(dllPath), NULL);

	// Load our DLL 
	CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);

	CloseHandle(Proc);

	return true;
}

bool Injector::Inject(char* procName, char* dllPath)
{
	return _inject(procName, dllPath);
}

bool Injector::Inject(DWORD pID, char* dllPath)
{
	return _inject("", dllPath, pID);
}

DWORD Injector::GetTargetThreadIDFromProcName(const char * procName)
{
	PROCESSENTRY32 procEntry;
	HANDLE thSnapShot;
	BOOL queNotEmpty = false;

	thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (thSnapShot == INVALID_HANDLE_VALUE)
	{
		printf("\n\n>Error: Unable to create toolhelp snapshot!");
		return false;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	queNotEmpty = Process32First(thSnapShot, &procEntry);

	while (queNotEmpty)
	{
		char ANSIszExeFile[MAX_PATH] = { 0 };

		// PROCESSENTRY32's string members are WCHAR.
		// Converts WCHAR to char* (required for using strcmp)
		WideCharToMultiByte(CP_ACP, 
			WC_COMPOSITECHECK, 
			procEntry.szExeFile, 
			-1, 
			ANSIszExeFile,
			sizeof(ANSIszExeFile),
			NULL, 
			NULL);

		if (!strcmp(ANSIszExeFile, procName))
		{
			return procEntry.th32ProcessID;
		}
		queNotEmpty = Process32Next(thSnapShot, &procEntry);
	}
	return 0;
}

Injector::Injector(void)
{
}

Injector::~Injector(void)
{
}