#include <stdlib.h>
#include <iostream>
#include <exception>
#include "onnx_utils.hpp"
#include "ONNX.hpp"

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cerr << "Need argument (path to onnx file) to run" << std::endl;
		exit(EXIT_FAILURE);
	}

	try {
		protocDecode(argv[1]);
		onnx::ModelProto model(TEMP_FILE);
		model.fill();
		model.print_info();
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
