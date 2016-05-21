// Most of this code was available on internet forums. 
// I made some changes to it, but I don't claim authorship.
//

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
	char dllDir[MAX_PATH] = { 0 };
	char dllRealPath[MAX_PATH] = { 0 };
	Injector injector;

	GetCurrentDirectoryA(MAX_PATH, currentDir);
	
	strcpy_s(dllDir, currentDir);
	strcat_s(dllDir, "\\");
	strcat_s(dllDir, argv[2]);
	
	// Handling directory navigation commands, like ../../dir
	GetFullPathNameA(dllDir, MAX_PATH, dllRealPath, NULL);

	printf("\n>Current directory: \t%s\n", currentDir);
	printf(">DLL relative path: \t%s\n", argv[2]);
	printf(">DLL real path: \t%s\n", dllRealPath);
	printf(">Target process: \t%s\n", argv[1]);
	
	if (injector.Inject(argv[1], dllRealPath)) {
		printf("\n>DLL injection SUCCEEDED!\n");
	}
	else {
		printf("\n>DLL injection FAILED...\n");
	}

	return 0;
}