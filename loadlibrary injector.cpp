#include <string>
#include <Windows.h>
#include <iostream>
#include <Tlhelp32.h>

#undef max

std::string path(std::string &dllpath)
{

	do
	{
		std::cout << "dll path: ";
		std::getline(std::cin, dllpath);

		if (!dllpath.ends_with(".dll"))
		{
			std::cout << "dll path is invalid\n\n";
		}

	} while (!dllpath.ends_with(".dll"));

	std::cout << '\n';

	return dllpath;
}

DWORD getProcessID(LPCTSTR processName)
{

	PROCESSENTRY32 pt;

	HANDLE snapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };

	pt.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &pt))
	{

		std::cout << "looking for csgo.exe...\n";

		do
		{

			if (!lstrcmpi(pt.szExeFile, processName))
			{
				CloseHandle(snapshot);
				return pt.th32ProcessID;

			}

		} while (Process32Next(snapshot, &pt));
	}

	CloseHandle(snapshot);
	return 0;

}

LPVOID bypassPrevention(HANDLE &processHandle)
{

	LPVOID ntOpenFile{ GetProcAddress(LoadLibraryW(L"ntdll"), "NtOpenFile") };

	if (ntOpenFile)
	{
		char originalBytes[5];
		memcpy(originalBytes, ntOpenFile, 5);
		WriteProcessMemory(processHandle, ntOpenFile, originalBytes, 5, NULL);
	}
		
	return 0;

}

int main()
{

	SetConsoleTitle(TEXT("bruh"));

	std::cout << "simple csgo injector by symb0#6969\n\n";
	std::cout << "path example: C:\\cheat\\ayyware.dll (with double parentheses) \n\n";

	repeat:

	std::string dllpath{};
	path(dllpath);

	DWORD pid{ getProcessID(TEXT("csgo.exe"))};

	if (pid)
	{

		std::cout << "csgo.exe successfully found\n\n";

	}
	else
	{
		std::cout << "csgo.exe wasn't found\n\n";
		goto repeat;
	}

	HANDLE processHandle{ OpenProcess(PROCESS_ALL_ACCESS, false, pid) };

	LPVOID memoryShit{ VirtualAllocEx(processHandle, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };
	
	DWORD numberOfBytes{};
	WriteProcessMemory(processHandle, memoryShit, dllpath.c_str(), dllpath.size() + 1, &numberOfBytes);

	if (numberOfBytes)
	{
		std::cout << numberOfBytes << " bytes were transferred into the process\n";
	}
	else
	{
		std::cout << "no bytes were transferred into the process\n";
	}

	bypassPrevention(processHandle);

	DWORD threadID{};
	CreateRemoteThread(processHandle, NULL, 0, LPTHREAD_START_ROUTINE(LoadLibraryA), memoryShit, 0, &threadID);

	if (threadID)
	{
		std::cout << "thread creation was successful, the id of the thread is " << threadID << '\n';
	}
	else
	{
		std::cout << "thread creation failed, error code: " << GetLastError() << '\n' << '\n';
	}

	CloseHandle(processHandle);

	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cin.get();
		
	return 0;
}