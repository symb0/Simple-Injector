#include "injection.h"

bool Injection::csgoBypass()
{
	NtOpenFile = GetProcAddress(LoadLibrary("ntdll"), "NtOpenFile");

	if (NtOpenFile)
	{
		std::cout << "Bypassing csgo's retarded NtOpenFile hook...\n\n";

		char bytes[5];

		std::memcpy(bytes, NtOpenFile, 5);
		ReadProcessMemory(handle, NtOpenFile, originalBytes, 5, nullptr);
		WriteProcessMemory(handle, NtOpenFile, bytes, 5, nullptr);

		return true;
	}

	else return false;
}

int Injection::getProcessID(const std::string processName)
{
	int procID{ 0 };

	HANDLE snapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };

	if (snapshot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(snapshot, &procEntry))
		{
			do
			{
				if (!strcmp(processName.c_str(), procEntry.szExeFile))
				{
					procID = procEntry.th32ProcessID;
					break;
				}

			} while (Process32Next(snapshot, &procEntry));
		}
	}

	CloseHandle(snapshot);
	return procID;
}

DWORD Injection::inject(const int procID, const std::string dllPath, const std::string procName)
{
	DWORD threadID{ 0 };

	handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	if (handle)
	{
		std::cout << "Successfully opened the process object.\n\n";

		void* alloc{ VirtualAllocEx(handle, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE) };

		if (alloc)
		{
			std::cout << "Successfully made space for the dll.\n\n";

			BOOL writeMem{ WriteProcessMemory(handle, alloc, dllPath.c_str(), strlen(dllPath.c_str()) + 1, NULL) };

			if (writeMem)
			{
				std::cout << "Successfully copied the data to the specified address.\n\n";

				if (procName == "csgo.exe")
				{
					if (csgoBypass())
						std::cout << "Successfully bypassed csgo's retarded NtOpenFile hook.\n\n";

					else
						std::cout << "Something went wrong with getting the address of the NtOpenFile subroutine, last error code: " << GetLastError() << '\n' << '\n';
				}

				HANDLE createThread{ CreateRemoteThread(handle, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, alloc, 0, &threadID) };

				if (createThread)
				{
					std::cout << "Successfully created the thread with ID " << threadID << ".\n" << '\n';

					if (procName == "csgo.exe")
						WriteProcessMemory(handle, NtOpenFile, originalBytes, 5, nullptr);

					return threadID;
				}

				else
				{
					std::cout << "Something went wrong with creating the thread, last error code: " << GetLastError() << '\n' << '\n';
					return 0;
				}
			}

			else
			{
				std::cout << "Something went wrong with copying the data to the specified address, last error code: " << GetLastError() << '\n' << '\n';
				return 0;
			}
		}

		else
		{
			std::cout << "Something went wrong with making space for the dll, last error code: " << GetLastError() << '\n' << '\n';
			return 0;
		}
	}

	else
	{
		std::cout << "Something went wrong with opening the process object, last error code: " << GetLastError() << '\n' << '\n';
		return 0;
	}
}
