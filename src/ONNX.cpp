#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <format>
#include <utility>
#include <iterator>
#include "ONNX.hpp"

#define DEF_CMD_VAL_(cmd, var, func) {								\
	cmdset[cmd] = {.cmd_var = (void*)&(var), .cmd_func = (func)};	\
}

#define DEF_CMD_(...) {	\
	__VA_ARGS__			\
}

static void scope_going(vec_str_iter_t&, map_cmd_set_t&);
static void setOpsetImport(vec_str_iter_t&, void*);
static void setGraph(vec_str_iter_t&, void*);

static void addNode(vec_str_iter_t&, void*);
static void addNodePut(vec_str_iter_t&, void*);

static void addAttribute(vec_str_iter_t&, void*);
static void addAttributeInts(vec_str_iter_t&, void*);

static void setMetadataProps(vec_str_iter_t&, void*);

static void sTos(vec_str_iter_t&, void*);
static void sToi(vec_str_iter_t&, void*);
static void sToOpts(vec_str_iter_t&, void*);

void onnx::ModelProto::print_info() {
	std::cout << "Model Information:" << std::endl;

	std::cout << "ir_version: " << ir_version << std::endl;
	std::cout << "producer_name: " << producer_name.value_or("No name") << std::endl;
	std::cout << "producer_version: " << producer_version.value_or("No version") << std::endl;

	std::cout << "opset_import:" << std::endl;
	std::cout << "version: " << opset_import.version << std::endl;
	std::cout << "domain: " << opset_import.domain.value_or("No domain") << std::endl;
}

void onnx::ModelProto::fill() {

	vec_str_iter_t iter = protoc_buffer.begin();
	static map_cmd_set_t cmdset;

	#define CMD_SET_FILL
	#include "commands"
	#undef CMD_SET_FILL

	for ( ; iter != protoc_buffer.end(); iter++) {
		if (!cmdset.contains(*iter)) {
			std::cerr << (*iter).c_str() << std::format(" ({})",
						std::distance(iter, protoc_buffer.begin())) << std::endl;
			throw std::runtime_error(" ^ " EXCEPTION_INFO "Unknown command for model!");
		}

		try {
			CommandProto& cur_cmd = cmdset[*iter];
			cur_cmd.cmd_func(++iter, cur_cmd.cmd_var);
		}
		catch (const std::exception& err) {
			throw std::runtime_error(err.what());
		}
	}
}

static void scope_going(vec_str_iter_t& iter, map_cmd_set_t& cmdset) {

	int scope_count = (*(iter++) == "{" ? 1 : 0);
	for ( ; scope_count; iter++) {
		if (*iter == "{") {
			scope_count++;
			continue;
		}
		if (*iter == "}") {
			scope_count--;
			continue;
		}
		if (!cmdset.contains(*iter)) {
			std::cerr << (*iter).c_str() << std::endl;
			throw std::runtime_error(" ^ " EXCEPTION_INFO "Unknown command!");
		}

		CommandProto& cur_cmd = cmdset[*iter];
		cur_cmd.cmd_func(++iter, cur_cmd.cmd_var);
	}

	iter--;
};

static void setMetadataProps(vec_str_iter_t& iter, void* var) {

	metadata_t* metadata_props_p = (metadata_t*)var;
	static map_cmd_set_t cmdset;

	#define CMD_SET_METADATA
	#include "commands"
	#undef CMD_SET_METADATA

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}
}

static void setOpsetImport(vec_str_iter_t& iter, void* var) {

	onnx::OperatorSetIdProto* opset_import_p = (onnx::OperatorSetIdProto*)var;
	static map_cmd_set_t cmdset;

	#define CMD_SET_OPSET
	#include "commands"
	#undef CMD_SET_OPSET

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}
}

static void setGraph(vec_str_iter_t& iter, void* var) {

	std::unique_ptr<onnx::GraphProto>* graph_p = (std::unique_ptr<onnx::GraphProto>*)var;
	static map_cmd_set_t cmdset;

	#define CMD_SET_GRAPH
	#include "commands"
	#undef CMD_SET_GRAPH

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}
}

static void addNode(vec_str_iter_t& iter, void* var) {

	std::vector<onnx::NodeProto>* node_p = (std::vector<onnx::NodeProto>*)var;
	onnx::NodeProto node = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_NODE
	#include "commands"
	#undef CMD_SET_NODE

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*node_p).push_back(std::move(node));
}

static void addAttribute(vec_str_iter_t& iter, void* var) {

	std::vector<onnx::AttributeProto>* attribute_p = (std::vector<onnx::AttributeProto>*)var;
	onnx::AttributeProto attribute = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_ATTRIBUTE
	#include "commands"
	#undef CMD_SET_ATTRIBUTE

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*attribute_p).push_back(std::move(attribute));
}

static void addAttributeInts(vec_str_iter_t& iter, void* var) {
	std::vector<int64_t>* ints_p = (std::vector<int64_t>*)var;
	(*ints_p).push_back(std::stoi(*iter));
}

static void addNodePut(vec_str_iter_t& iter, void* var) {
	std::vector<std::string>* put_p = (std::vector<std::string>*)var;
	(*put_p).push_back(*iter);
}

static void sTos(vec_str_iter_t& iter, void* var) {
	std::string* str_p = (std::string*)var;
	*str_p = *iter;
}

static void sToi(vec_str_iter_t& iter, void* var) {
	int* int_p = (int*)var;
	*int_p = std::stoi(*iter);
}

static void sToOpts(vec_str_iter_t& iter, void* var) {
	std::optional<std::string>* opt_str_p = (std::optional<std::string>*)var;
	*opt_str_p = *iter;
}
