#pragma once

#include <string>
#include <fstream>
#include <tuple>

class File
{
public:
	std::tuple<std::string, std::string> read(const std::string fileName, bool& bError);
};
