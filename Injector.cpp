// Most of this code was available on internet forums. 
// I made some changes to it, but I don't claim authorship.
//

#include "Injector.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <cstdio>

Injector::Injector()
{
}

Injector::~Injector()
{
}

bool Injector::Inject(const char* processName, const char* dllPath)
{
	return _inject(processName, dllPath);
}

bool Injector::Inject(DWORD processID, const char* dllPath)
{
	return _inject("", dllPath, processID);
}

bool Injector::_inject(const char* processName, const char* dllPath, DWORD processID)
{
	// If procID is 0, the caller is passing procName as primary argument
	// and we need to find the processID via processName instead.
	if (processID == 0)
		processID = _getProcessID(processName);

	// if procID is still 0, processID couldn't be found.
	if (processID == 0)
		return false;

	HANDLE hProcess = 0;
	LPVOID remoteString, fpLoadLibraryA;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	if (!hProcess)
	{
		printf("\n\n>OpenProcess() failed: %d", GetLastError());
		return false;
	}

	fpLoadLibraryA = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	// Allocate space in the process for our DLL 
	remoteString = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(dllPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	// Write the string name of our DLL in the memory allocated 
	WriteProcessMemory(hProcess, (LPVOID)remoteString, dllPath, strlen(dllPath), NULL);

	// Load our DLL 
	CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)fpLoadLibraryA, (LPVOID)remoteString, NULL, NULL);

	// Let the program regain control of itself
	CloseHandle(hProcess);

	return true;
}

DWORD Injector::_getProcessID(const char* processName)
{
	PROCESSENTRY32 processEntry;
	HANDLE thSnapShot;
	BOOL queNotEmpty = false;

	thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (thSnapShot == INVALID_HANDLE_VALUE)
	{
		printf("\n\n>Error: Unable to create toolhelp snapshot!");
		return false;
	}

	processEntry.dwSize = sizeof(PROCESSENTRY32);

	queNotEmpty = Process32First(thSnapShot, &processEntry);

	while (queNotEmpty)
	{
		char ANSIszExeFile[MAX_PATH] = { 0 };
		
		// Converting WCHAR to char*. This is not needed if compiling with GCC.
		// Use processEntry.szExeFile directly in strcmp instead.
		WideCharToMultiByte(CP_ACP, 
			WC_COMPOSITECHECK, 
			processEntry.szExeFile, 
			-1, 
			ANSIszExeFile, 
			sizeof(ANSIszExeFile), 
			NULL, 
			NULL);

		if (!strcmp(ANSIszExeFile, processName))
		{
			return processEntry.th32ProcessID;
		}
		queNotEmpty = Process32Next(thSnapShot, &processEntry);
	}
	return 0;
}