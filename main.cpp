#include <iostream>
#include <string>
#include <Windows.h>
#include <lmcons.h>

#include "file.h"
#include "injection.h"

int main()
{
	char name[UNLEN + 1];
	DWORD nameSize{ UNLEN + 1 };
	GetUserNameA(name, &nameSize);

	std::cout << "Hello " << name << "!\n\n";

	std::string errorStr{ "Program was unable to open the config file, please check if the name of the file is correct." };
	std::string finalErrorStr{ "\"config.ini\", also keep in mind that the name's CaSe-SeNsItIvE and has to be where the injector is!" };

	File* file{ new File() };
	Injection* injection{ new Injection() };

	bool bError{ false };

	auto [procName, dllPath] { file->read("config.ini", bError) };

	if (!bError)
	{
		int procID{ injection->getProcessID(procName) };

		if (procID)
		{
			DWORD result{ injection->inject(procID, dllPath, procName) };

			if (result)
			{
				std::cout << "Everything went fine, closing the program in 3 seconds...\n";
				Sleep(3000);

				return 0;
			}

			else
			{
				std::cout << "Check if you entered the right dll path to the config file.\n";
				std::cout << "If the process is 64-bit, you need to run the 64-bit version of the program. And vice-versa.\n";
				std::cout << "If you 100% entered the right dll path and the program architecture corresponds to the process's architecture, then the process you wanna inject "
					<< "to / anti-cheat has probably detected the last action of this injector.\n";
			}
		}

		else
		{
			std::cout << "Check if the process you wanna inject to is currently running.\n";
			std::cout << "If the process is running, verify if the name of the process you wanna inject to is correct in the config file.\n";
		}
	}

	else
	{
		std::cout << errorStr << '\n';
		std::cout << finalErrorStr << '\n';
	}

	return 0;
}