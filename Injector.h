// Most of this code was available on internet forums. 
// I made some changes to it, but I don't claim authorship.
//

#ifndef _INJECTOR_H
#define _INJECTOR_H

#include <Windows.h>

class Injector
{
public:
	Injector();
	~Injector();

	bool Inject(const char* processName, const char* dllPath);
	bool Inject(DWORD processID, const char* dllPath);

private:
	bool _inject(const char* processName, const char* dllPath, DWORD processID = 0);
	DWORD _getProcessID(const char* processName);
};

#endif