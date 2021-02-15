#pragma once

#include <string>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

class Injection
{
private:
	char originalBytes[5];
	FARPROC NtOpenFile;
	HANDLE handle;

public:
	int getProcessID(std::string processName);
	DWORD inject(const int procID, const std::string dllPath, const std::string procName);
	bool csgoBypass();
};