// Most of this code was available on internet forums. 
// I made some changes to it, but I don't claim authorship.

#include "Injector.h"
#include <Windows.h>
#include <cstdio>

int main(int argc, char** argv)
{
	if (argc != 3) 
	{
		printf("======= DLL Injector =======\n\nUsage: binary_name target_process_name dll_relative_path\n\n");
		system("PAUSE");
		return 1;
	}

	char currentDir[MAX_PATH] = { 0 };
	char dllFullPath[MAX_PATH] = { 0 };
	char dllRealPath[MAX_PATH] = { 0 };
	Injector injector;

	GetCurrentDirectoryA(MAX_PATH, currentDir);
	
	strcpy_s(dllFullPath, currentDir);
	strcat_s(dllFullPath, "\\");
	strcat_s(dllFullPath, argv[2]);
	
	// Handles directory navigation commands, like ../../dir
	GetFullPathNameA(dllFullPath, MAX_PATH, dllRealPath, NULL);

	printf("\n>Current directory: \t%s\n", currentDir);
	printf(">DLL relative path: \t%s\n", argv[2]);
	printf(">DLL real path: \t%s\n", dllRealPath);
	printf(">Target process: \t%s\n", argv[1]);
	
	StatusCode status = injector.Inject(argv[1], dllRealPath);

	switch (status)
	{
	case StatusCode::OK:
		printf("\n>DLL injection SUCCEEDED!\n"); 
		break;
	case StatusCode::CANT_FIND_PROC_ID:
		printf("\n>DLL injection FAILED...\n>Can't find process ID!"); 
		printf("\n>Error code: %d\n", GetLastError());
		break;
	case StatusCode::CANT_OPEN_PROCESS:
		printf("\n>DLL injection FAILED...\n>Can't open remote process!"); 
		printf("\n>Error code: %d\n", GetLastError());
		break;
	case StatusCode::CANT_CREATE_PROC_SNAPSHOT:
		printf("\n>DLL injection FAILED...\n>Can't create process snapshot!");
		printf("\n>Error code: %d\n", GetLastError());
		break;
	case StatusCode::INVALID_DLL_PATH:
		printf("\n>DLL injection FAILED...\n>Invalid DLL path!");
		printf("\n>Error code: %d\n", GetLastError());
		break;
	}

	return 0;
}