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

	strcpy(dllFullPath, currentDir);
	strcat(dllFullPath, "\\");
	strcat(dllFullPath, argv[2]);

	// Handles directory navigation commands, like ../../dir
	GetFullPathNameA(dllFullPath, MAX_PATH, dllRealPath, NULL);

	printf("\n>Current directory: \t%s\n", currentDir);
	printf(">DLL relative path: \t%s\n", argv[2]);
	printf(">DLL real path: \t%s\n", dllRealPath);
	printf(">Target process: \t%s\n", argv[1]);

	StatusCode status = injector.Inject(argv[1], dllRealPath);

	if (status == StatusCode::OK)
	{
		printf("\n>DLL injection SUCCEEDED!\n");
	}
	else 
	{
		printf("\n>DLL injection FAILED...");

		switch (status)
		{
		case StatusCode::CANT_FIND_PROC_ID:
			printf("\n>Can't find process ID!");
			break;
		case StatusCode::CANT_OPEN_PROCESS:
			printf("\n>Can't open remote process!");
			break;
		case StatusCode::CANT_CREATE_PROC_SNAPSHOT:
			printf("\n>Can't create process snapshot!");
			break;
		case StatusCode::INVALID_DLL_PATH:
			printf("\n>Invalid DLL path!");
			break;
		}

		printf("\n>Error code: %d\n", GetLastError());
	}

	return 0;
}