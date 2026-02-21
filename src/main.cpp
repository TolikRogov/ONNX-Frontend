#include <variant>
#include <iostream>
#include <__ostream/basic_ostream.h>
#include <stdint.h>
#include "ONNX.hpp"

int main() {

	onnx::Dimension dim = {};
	dim.value = 64;
	std::cout << std::get<int64_t>(dim.value) << std::endl;

	return 0;
}
