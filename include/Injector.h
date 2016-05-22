// Most of this code was available on internet forums. 
// I made some changes to it, but I don't claim authorship.

#ifndef _INJECTOR_H
#define _INJECTOR_H

#include <Windows.h>
#include "StatusCode.h"

class Injector
{
public:
	Injector();
	~Injector();

	StatusCode Inject(const char* procName, const char* dllPath);
	StatusCode Inject(DWORD procID, const char* dllPath);

private:
	StatusCode _inject(const char* procName, const char* dllPath, DWORD procID = 0);
	DWORD _getProcID(const char* procName, StatusCode* status);
};

#endif