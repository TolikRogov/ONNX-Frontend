#include <vector>
#include <string>
#include <iostream>
#include "ONNX.hpp"
#include "onnx_utils.hpp"

onnx::vec_cmd::iterator onnx::findCmdByName(onnx::vec_cmd& cmds, const std::string& cmd_name) {
	for (onnx::vec_cmd::iterator iter = cmds.begin(); iter < cmds.end(); iter++) {
		if ((*iter).cmd_name == cmd_name)
			return iter;
	}
	return onnx::vec_cmd::iterator();
}

void onnx::ModelProto::fill() {

	HeadProtocCmd IRversion 		= 	{.cmd_name = "ir_version:", 		.cmd_var = (void*)&ir_version, 			.cmd_func = sToi};
	HeadProtocCmd ProducerName 		= 	{.cmd_name = "producer_name:", 		.cmd_var = (void*)&producer_name,		.cmd_func = sToopts};
	HeadProtocCmd ProducerVersion 	= 	{.cmd_name = "producer_version:", 	.cmd_var = (void*)&producer_version,	.cmd_func = sToopts};

	vec_cmd header_cmd = {IRversion, ProducerName, ProducerVersion};

	std::vector<std::string>::iterator iter = protoc_buffer.begin();
	for ( ; iter != protoc_buffer.end(); iter++) {

		vec_cmd::iterator header_cmd_iter = findCmdByName(header_cmd, *iter);

		if (!(header_cmd_iter == vec_cmd::iterator()))
			(*header_cmd_iter).cmd_func(*(++iter), (*header_cmd_iter).cmd_var);
	}

}
