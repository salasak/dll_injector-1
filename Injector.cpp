// Most of this code was available on internet forums. 
// I made some changes to it, but I don't claim authorship.

#include "Injector.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <io.h> // only for using _access()

Injector::Injector()
{
}

Injector::~Injector()
{
}

StatusCode Injector::Inject(const char* procName, const char* dllPath)
{
	return _inject(procName, dllPath);
}

StatusCode Injector::Inject(DWORD procID, const char* dllPath)
{
	return _inject("", dllPath, procID);
}

StatusCode Injector::_inject(const char* procName, const char* dllPath, DWORD procID)
{
	// Check if the DLL exists 
	if (_access(dllPath, 0))
		return StatusCode::INVALID_DLL_PATH;

	// If procID is 0, we need to find a valid process ID using procName.
	if (procID == 0) 
	{
		StatusCode status;
		procID = _getProcID(procName, &status);

		if (status != StatusCode::OK)
			return status;
	}
	
	HANDLE procHandle = 0;
	LPVOID remoteString;
	LPVOID fnLoadLibraryA;

	procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if (!procHandle)
		return StatusCode::CANT_OPEN_PROCESS;

	fnLoadLibraryA = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	// Allocate memory in the remote process for our string argument 
	remoteString = (LPVOID)VirtualAllocEx(procHandle, NULL, strlen(dllPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	// Write the string in the allocated memory 
	WriteProcessMemory(procHandle, (LPVOID)remoteString, dllPath, strlen(dllPath), NULL);
	
	// Call LoadLibrary remotely and pass our string (DLL path) as argument to it.
	CreateRemoteThread(procHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)fnLoadLibraryA, (LPVOID)remoteString, NULL, NULL);
	
	// Let the program regain control of itself 
	CloseHandle(procHandle);

	return StatusCode::OK;
}

DWORD Injector::_getProcID(const char* procName, StatusCode* status)
{
	*status = StatusCode::OK;

	PROCESSENTRY32 processEntry;
	HANDLE hProcSnapShot;
	BOOL isNotEmpty = false;

	hProcSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcSnapShot == INVALID_HANDLE_VALUE)
	{
		*status = StatusCode::CANT_CREATE_PROC_SNAPSHOT;
		return 0;
	}

	processEntry.dwSize = sizeof(PROCESSENTRY32);

	isNotEmpty = Process32First(hProcSnapShot, &processEntry);

	while (isNotEmpty)
	{
		// Converts WCHAR to char*. Only needed if building in Unicode, apparently
		char szExeFileANSI[MAX_PATH] = { 0 };
		WideCharToMultiByte(CP_ACP, 
			WC_COMPOSITECHECK, 
			processEntry.szExeFile, 
			-1, 
			szExeFileANSI, 
			sizeof(szExeFileANSI), 
			NULL, 
			NULL);

		// If we find the process we are looking for, return its ID
		if (!strcmp(szExeFileANSI, procName))
			return processEntry.th32ProcessID;

		// Else, search next process
		isNotEmpty = Process32Next(hProcSnapShot, &processEntry);
	}

	*status = StatusCode::CANT_FIND_PROC_ID;

	return 0;
}