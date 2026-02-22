#include <stdlib.h>
#include <iostream>
#include <exception>
#include <optional>
#include <string>
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
		std::cout << model.ir_version << std::endl;
		std::cout << model.producer_name.value_or("No name") << std::endl;
		std::cout << model.producer_version.value_or("No version") << std::endl;
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
