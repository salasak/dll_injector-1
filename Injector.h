// Most of this code was available on internet forums. 
// I made some changes to it, but I don't claim authorship.
//

#pragma once
#include <Windows.h>

class Injector
{
public:
	Injector(void);
	~Injector(void);

	bool Inject(char* procName, char* dllPath);
	bool Inject(DWORD pID, char* dllPath);

private:
	bool _inject(char* procName, char* dllPath, DWORD pID);
	DWORD GetTargetThreadIDFromProcName(const char* ProcName);
};

