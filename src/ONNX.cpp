#include <vector>
#include <string>
#include <unordered_map>
#include "ONNX.hpp"

#define DEF_CMD_(cmd, var, func)									\
	cmdset[cmd] = {.cmd_var = (void*)&(var), .cmd_func = (func)};	\

void onnx::ModelProto::fill() {

	#include "commands"

	std::vector<std::string>::iterator iter = protoc_buffer.begin();
	for ( ; iter != protoc_buffer.end(); iter++) {
		if (cmdset.contains(*iter)) {
			CommandProto& cur_cmd = cmdset[*iter];
			cur_cmd.cmd_func(++iter, cur_cmd.cmd_var);
		}
	}

}

void onnx::sToi(std::vector<std::string>::iterator iter, void* var) {
	int* variable = (int*)var;
	*variable = std::stoi(*iter);
}

void onnx::sToOpts(std::vector<std::string>::iterator iter, void* var) {
	std::optional<std::string>* variable = (std::optional<std::string>*)var;
	*variable = *iter;
}
