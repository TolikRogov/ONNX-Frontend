#include <stdlib.h>
#include <iostream>
#include <exception>
#include "ONNX.hpp"

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cerr << "Need argument (path to onnx file) to run" << std::endl;
		exit(EXIT_FAILURE);
	}

	try {
		onnx::ModelProto model(argv[1]);
		model.fill();
		model.print_info();
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
