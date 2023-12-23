#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (file)
	{
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> fileBuffer(fileSize);
		file.seekg(0);
		file.read(fileBuffer.data(), fileSize);
		file.close();
		return fileBuffer;
	}
}