#include <stdlib.h>
#include <string>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <optional>
#include <filesystem>
#include "onnx_utils.hpp"

void sToi(std::string& str, void* var) {
	int* variable = (int*)var;
	*variable = std::stoi(str);
}

void sToopts(std::string& str, void* var) {
	std::optional<std::string>* variable = (std::optional<std::string>*)var;
	*variable = str;
}

void protocDecode(const std::string& onnx_path) {
	if (!std::filesystem::exists(onnx_path))
		throw std::runtime_error(EXCEPTION_INFO "File does not exist!");

	std::string command = PROTOC_CMD_START + onnx_path + PROTOC_CMD_END;
	system(command.c_str());
}

std::vector<std::string> readFile(const std::string& file_path) {
	std::fstream inFile(file_path);
	if (!inFile)
		throw std::runtime_error(EXCEPTION_INFO "Cannot open file!");

	return std::vector<std::string>(std::istream_iterator<std::string>(inFile),
									std::istream_iterator<std::string>());
}
