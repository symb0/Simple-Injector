#include "file.h"

std::tuple<std::string, std::string> File::read(const std::string fileName, bool& bError)
{
	std::ifstream f;

	std::string procStr;
	std::string dllStr;

	f.open(fileName, std::ios::in);

	if (f.is_open())
	{
		std::getline(f, procStr); std::getline(f, dllStr);

		return { procStr, dllStr };
	}

	else
		bError = true;
}