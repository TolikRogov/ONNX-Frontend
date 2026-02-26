#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <utility>
#include <stdlib.h>
#include "ONNX.hpp"

#define DEF_CMD_VAL_(cmd, var, func) {								\
	cmdset[cmd] = {.cmd_var = (void*)&(var), .cmd_func = (func)};	\
}

#define DEF_CMD_(...) {	\
	__VA_ARGS__			\
}

std::ofstream onnx::ModelProto::dotFile;
std::string onnx::ModelProto::model_path;
onnx::OperatorSetProto onnx::ModelProto::opset;

void onnx::ModelProto::print_opset() {

	std::cout << PRINT_HEAD("Operators set", '-');

	std::unordered_map<std::string, OperatorProto>::iterator iter = opset.operator_.begin();
	std::cout << std::left << std::setw(PRINT_WIDTH) << "Operator type" << std::endl;
	for ( ; iter != opset.operator_.end(); iter++) {
		std::cout << std::left << std::setw(PRINT_LONG_WIDTH)
				  << COLOR_IF_USED((*iter).first, (*iter).second.used) << std::endl;
	}
}

void onnx::ModelProto::print_metadata() {

	if (is_empty()) {
		std::cout << "Model is empty!" << std::endl;
		exit(EXIT_SUCCESS);
	}

	if (!is_filled()) {
		std::cout << "Model is not filled!" << std::endl;
		exit(EXIT_SUCCESS);
	}

	std::cout << PRINT_HEAD("Metadata", '-');

	metadata_t::iterator iter = metadata_props.begin();
	for ( ; iter != metadata_props.end(); iter++) {
		std::cout << std::left << std::setw(PRINT_WIDTH) << (*iter).first
			  << (*iter).second << std::endl;
	}
}

void onnx::ModelProto::print_info() {
	if (is_empty()) {
		std::cout << "Model is empty!" << std::endl;
		exit(EXIT_SUCCESS);
	}

	if (!is_filled()) {
		std::cout << "Model is not filled!" << std::endl;
		exit(EXIT_SUCCESS);
	}

	std::cout << PRINT_HEAD("MODEL INFORMATION", '=');

	std::cout << std::left << std::setw(PRINT_WIDTH) << "model_path:"
			  << model_path << std::endl;
	std::cout << std::left << std::setw(PRINT_WIDTH) << "ir_version:"
			  << ir_version << std::endl;
	std::cout << std::left << std::setw(PRINT_WIDTH) << "producer_name:"
			  << producer_name.value_or("No name") << std::endl;
	std::cout << std::left << std::setw(PRINT_WIDTH) << "producer_version:"
			  << producer_version.value_or("No version") << std::endl;

	std::cout << PRINT_HEAD("Opset Import", '-');
	std::cout << std::left << std::setw(PRINT_WIDTH) << "version:"
			  << opset_import.version << std::endl;
	std::cout << std::left << std::setw(PRINT_WIDTH) << "domain:"
			  << opset_import.domain.value_or("No domain") << std::endl;

	print_metadata();
	print_opset();
}

void onnx::ModelProto::fill() {

	if (is_empty())
		throw std::runtime_error(EXCEPTION_INFO "Model is empty!");

	vec_str_iter_t iter = opset_buffer.begin();
	static map_cmd_set_t cmdset;

	#define CMD_SET_FILL
	#include "commands.txt"
	#undef CMD_SET_FILL

	dotFile << "digraph " DOT_GRAPH_NAME "{\n";
	dotFile << "\tbgcolor = " DOT_COLOR_BACKGROUND ";\n"
			   "\tnode [shape="    	DOT_NODE_SHAPE 	  ","
					   "style="    	DOT_NODE_STYLE 	  ","
					   "fontsize=" 	DOT_NODE_FONTSIZE ","
					   "fontcolor=" DOT_COLOR_FONT	  ","
					   "color="		DOT_COLOR_NODE_BORDER
			   "];\n"
			   "\tedge [color=" DOT_COLOR_EDGE "];\n";

	for ( ; iter != protoc_buffer.end(); iter++) {
		if (iter == opset_buffer.end())
			iter = protoc_buffer.begin();

		if (!cmdset.contains(*iter)) {
			std::cerr << (*iter).c_str() << std::endl;
			throw std::runtime_error(" ^ " EXCEPTION_INFO "Unknown command!");
		}

		try {
			CommandProto& cur_cmd = cmdset[*iter];
			cur_cmd.cmd_func(++iter, cur_cmd.cmd_var);
		}
		catch (const std::exception& err) {
			throw std::runtime_error(err.what());
		}
	}

	dotFile << "}";
	dotFile.close();
	system(DOT_GEN(model_path));
}

void onnx::ModelProto::scope_going(vec_str_iter_t& iter, map_cmd_set_t& cmdset) {

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

void onnx::ModelProto::setOpset(vec_str_iter_t& iter, void* var) {

	OperatorSetProto* opset_p = (OperatorSetProto*)var;
	static map_cmd_set_t cmdset;

	#define CMD_SET_OPSETID
	#include "commands.txt"
	#undef CMD_SET_OPSETID

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}
}

void onnx::ModelProto::addOperator(vec_str_iter_t& iter, void* var) {

	std::unordered_map<std::string, OperatorProto>* operators_p =
		(std::unordered_map<std::string, OperatorProto>*)var;
	std::pair<std::string, OperatorProto> operator_ = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_OPERATOR
	#include "commands.txt"
	#undef CMD_SET_OPERATOR

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*operators_p).insert(operator_);
}

void onnx::ModelProto::setMetadataProps(vec_str_iter_t& iter, void* var) {

	metadata_t* metadata_props_p = (metadata_t*)var;
	std::pair<std::string, std::string> key_val = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_METADATA
	#include "commands.txt"
	#undef CMD_SET_METADATA

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*metadata_props_p).insert(key_val);
}

void onnx::ModelProto::setOpsetImport(vec_str_iter_t& iter, void* var) {

	OperatorSetIdProto* opset_import_p = (OperatorSetIdProto*)var;
	static map_cmd_set_t cmdset;

	#define CMD_SET_OPSET
	#include "commands.txt"
	#undef CMD_SET_OPSET

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}
}

void onnx::ModelProto::setGraph(vec_str_iter_t& iter, void* var) {

	std::unique_ptr<GraphProto>* graph_p = (std::unique_ptr<GraphProto>*)var;
    (*graph_p) = std::make_unique<onnx::GraphProto>();
	static map_cmd_set_t cmdset;

	#define CMD_SET_GRAPH
	#include "commands.txt"
	#undef CMD_SET_GRAPH

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}
}

void onnx::ModelProto::addNode(vec_str_iter_t& iter, void* var) {

	std::vector<NodeProto>* node_p = (std::vector<NodeProto>*)var;
	NodeProto node = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_NODE
	#include "commands.txt"
	#undef CMD_SET_NODE

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*node_p).push_back(std::move(node));
	addNodeToGraphviz((*node_p).back());
}

void onnx::ModelProto::addNodeToGraphviz(NodeProto& node) {
	static std::unordered_map<std::string, std::string> output_nodes;
	static int node_number = 0;
	std::string node_op_type = node.op_type.substr(1, node.op_type.size() - 2);
	node.graphviz_name = node_op_type + std::to_string(node_number++);
	dotFile << "\t" << node.graphviz_name << " [label=" << node_op_type
			<< ",fillcolor=" << opset.operator_[node.op_type].color << "]\n";

	std::vector<std::string>::iterator out_it = node.output.begin(), in_it = node.input.begin();
	for ( ; out_it != node.output.end(); out_it++)
		output_nodes[*out_it] = node.graphviz_name;
	for ( ; in_it != node.input.end(); in_it++) {
		if (!output_nodes.contains(*in_it))
			continue;
		dotFile << "\t" << output_nodes[*in_it]
				<< " -> " << node.graphviz_name << "\n";
	}
}

void onnx::ModelProto::addAttribute(vec_str_iter_t& iter, void* var) {

	std::vector<AttributeProto>* attribute_p = (std::vector<AttributeProto>*)var;
	AttributeProto attribute = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_ATTRIBUTE
	#include "commands.txt"
	#undef CMD_SET_ATTRIBUTE

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*attribute_p).push_back(std::move(attribute));
}

void onnx::ModelProto::addAttributeType(vec_str_iter_t& iter, void* var) {

	AttributeType* type_p = (AttributeType*)var;
	static std::unordered_map<std::string, AttributeType> AtTypeMap = {
		{"FLOAT", 	AttributeType::FLOAT},
		{"INT", 	AttributeType::INT},
		{"STRING", 	AttributeType::STRING},
		{"FLOATS", 	AttributeType::FLOATS},
		{"INTS", 	AttributeType::INTS},
		{"TENSOR",  AttributeType::TENSOR},
	};

	std::unordered_map<std::string, AttributeType>::iterator it = AtTypeMap.find(*iter);
	if (it == AtTypeMap.end())
		throw std::runtime_error(EXCEPTION_INFO "Unkown attribute type!");

	(*type_p) = it->second;
}

void onnx::ModelProto::addInitializer(vec_str_iter_t& iter, void* var) {

	std::vector<std::unique_ptr<TensorProto>>* initializer_p =
		(std::vector<std::unique_ptr<TensorProto>>*)var;
	std::unique_ptr<TensorProto> initializer = std::make_unique<TensorProto>();
	static map_cmd_set_t cmdset;

	#define CMD_SET_INITIALIZER
	#include "commands.txt"
	#undef CMD_SET_INITIALIZER

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*initializer_p).push_back(std::move(initializer));
}

void onnx::ModelProto::addValueInfo(vec_str_iter_t& iter, void* var) {

	std::vector<ValueInfoProto>* val_info_p = (std::vector<ValueInfoProto>*)var;
	ValueInfoProto value_info = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_VALUE_INFO
	#include "commands.txt"
	#undef CMD_SET_VALUE_INFO

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*val_info_p).push_back(std::move(value_info));
}

void onnx::ModelProto::addValueInfoType(vec_str_iter_t& iter, void* var) {

	TypeProto* type_p = (TypeProto*)var;
	static map_cmd_set_t cmdset;

	#define CMD_SET_VALINFO_TYPE
	#include "commands.txt"
	#undef CMD_SET_VALINFO_TYPE

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}
}

void onnx::ModelProto::addValueInfoTensor(vec_str_iter_t& iter, void* var) {

	valinfo_type_value_t* type_value_p = (valinfo_type_value_t*)var;
	TensorTypeProto tensor = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_VALINFO_TENSOR
	#include "commands.txt"
	#undef CMD_SET_VALINFO_TENSOR

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*type_value_p) = std::move(tensor);
}

void onnx::ModelProto::addTensorShape(vec_str_iter_t& iter, void* var) {

	std::optional<TensorShapeProto>* shape_p = (std::optional<TensorShapeProto>*)var;
	TensorShapeProto shape = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_TENSOR_SHAPE
	#include "commands.txt"
	#undef CMD_SET_TENSOR_SHAPE

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*shape_p) = std::move(shape);
}

void onnx::ModelProto::addTensorDim(vec_str_iter_t& iter, void* var) {

	std::vector<Dimension>* dim_p = (std::vector<Dimension>*)var;
	Dimension dim = {};
	static map_cmd_set_t cmdset;

	#define CMD_SET_TENSOR_DIM
	#include "commands.txt"
	#undef CMD_SET_TENSOR_DIM

	try {
		scope_going(iter, cmdset);
	}
	catch (const std::exception& err) {
		throw std::runtime_error(err.what());
	}

	(*dim_p).push_back(std::move(dim));
}

void onnx::ModelProto::addOpType(vec_str_iter_t& iter, void* var) {

	std::string* op_type_p = (std::string*)var;
	if (!opset.operator_.contains(*iter)) {
		std::cout << *iter << std::endl;
		throw std::runtime_error(" ^ " EXCEPTION_INFO "Unsupported operator!");
	}

	(*op_type_p) = *iter;
	opset.operator_[*iter].used = true;
}

void onnx::ModelProto::addDimParam(vec_str_iter_t& iter, void* var) {

	std::variant<int64_t, std::string>* value_p = (std::variant<int64_t, std::string>*)var;
	std::string overall_string = *iter;

	for (; *iter != "}"; iter++)
		overall_string += " " + *iter;
	(*value_p) = overall_string;

	iter--;
}

void onnx::ModelProto::addMetadataValue(vec_str_iter_t& iter, void* var) {

	std::string* value_p = (std::string*)var;

	for (; *iter != "}"; iter++)
		(*value_p) += " " + *iter;

	iter--;
}

void onnx::ModelProto::addInitializerFloatData(vec_str_iter_t& iter, void* var) {

	std::vector<float>* float_data_p = (std::vector<float>*)var;

	for ( ; true; iter++) {
		try {
			(*float_data_p).push_back(std::stof(*(iter++)));
			if (*iter == "}")
				break;
		}
		catch (const std::exception& err) {
			iter--;
			break;
		}
	}
}

void onnx::ModelProto::addInitializerRawData(vec_str_iter_t& iter, void* var) {

	std::vector<uint8_t>* raw_data_p = (std::vector<uint8_t>*)var;
	std::string overall_raw_data = *iter;

	for (; *iter != "}"; iter++)
		overall_raw_data += " " + *iter;
	(*raw_data_p).assign(overall_raw_data.begin() + 1, overall_raw_data.end() - 1);

	iter--;
}

void onnx::ModelProto::addTensorType(vec_str_iter_t& iter, void* var) {
	TensorDataType* type_p = (TensorDataType*)var;
	(*type_p) = (TensorDataType)std::stoi(*iter);
}

void onnx::ModelProto::addToVectorInts(vec_str_iter_t& iter, void* var) {
	std::vector<int64_t>* ints_p = (std::vector<int64_t>*)var;
	(*ints_p).push_back(std::stoi(*iter));
}

void onnx::ModelProto::addNodePut(vec_str_iter_t& iter, void* var) {
	std::vector<std::string>* put_p = (std::vector<std::string>*)var;
	(*put_p).push_back(*iter);
}

void onnx::ModelProto::sTos(vec_str_iter_t& iter, void* var) {
	std::string* str_p = (std::string*)var;
	*str_p = *iter;
}

void onnx::ModelProto::sTof(vec_str_iter_t& iter, void* var) {
	float* float_p = (float*)var;
	*float_p = std::stof(*iter);
}

void onnx::ModelProto::sToi(vec_str_iter_t& iter, void* var) {
	int* int_p = (int*)var;
	*int_p = std::stoi(*iter);
}

void onnx::ModelProto::sToOpts(vec_str_iter_t& iter, void* var) {
	std::optional<std::string>* opt_str_p = (std::optional<std::string>*)var;
	*opt_str_p = *iter;
}
