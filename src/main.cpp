#include <__ostream/basic_ostream.h>
#include <_stdlib.h>
#include <variant>
#include <iostream>
#include <stdint.h>
#include <string>
#include "ONNX.hpp"
#include "onnx_utils.hpp"

inline void protocDecode(const char* onnx_path) {
	std::string path(onnx_path);
	std::string command = COMMAND_START + path + COMMAND_END;
	system(command.c_str());
}

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cout << "Error: need path to onnx file!" << std::endl;
		exit(1);
	}

	protocDecode(argv[1]);

	return 0;
}
