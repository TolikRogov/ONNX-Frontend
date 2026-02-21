#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <vector>

namespace onnx {

	enum class TensorDataType {
		UNDEFINED,
		FLOAT,
		UINT8,
		INT8,
		UINT16,
		INT16,
		INT32,
		INT64,
		STRING,
		BOOL,
		FLOAT16,
		DOUBLE,
		UINT32,
		UINT64
	};

	enum class AttributeType {
		UNDEFINED,
		FLOAT,
		INT,
		STRING,
		FLOATS,
		INTS,
		STRINGS,				//|
		TENSOR,					//|
		GRAPH,					//| unsupported set of attribute types
		TENSORS,				//|
		GRAPHS,					//|
	};

	struct Dimension {//dim_value	//dim_param
		std::variant<int64_t, std::string> value;
		bool defined = true;
	};

	struct TensorShapeProto {
		std::vector<Dimension> dim;
	};

	struct TensorTypeProto {
		TensorDataType elem_type = TensorDataType::UNDEFINED;
		std::optional<TensorShapeProto> shape;
	};

	struct TypeProto {
		std::variant<TensorTypeProto> value_type;
		std::string denotation;
	};

	struct ValueInfoProto {
		std::string name;
		TypeProto type;
		std::string doc_string;
	};

	struct OperatorSetIdProto {
		std::string domain;
		int64_t version = 0;
	};

	struct AttributeProto;

	struct TensorProto {
		std::vector<int64_t> dims;
		TensorDataType data_type = TensorDataType::UNDEFINED;

		std::vector<uint8_t> raw_data;

		std::string name;
		std::string doc_string;
	};

	struct NodeProto {
		std::string name;
		std::string op_type;
		std::string domain;
		std::vector<std::string> input;
		std::vector<std::string> output;
		std::vector<AttributeProto> attribute;
		std::string doc_string;
		std::string overload;
		std::unordered_map<std::string, std::string> metadata_props;
	};

	struct GraphProto {
		std::string name;
		std::vector<NodeProto> node;
		std::vector<std::unique_ptr<TensorProto>> initializer;
		std::vector<ValueInfoProto> input;
		std::vector<ValueInfoProto> output;
		std::vector<ValueInfoProto> value_info;
		std::string doc_string;
		std::unordered_map<std::string, std::string> metadata_props;
	};

	struct AttributeProto {
		std::string name;
		std::string doc_string;
		AttributeType type = AttributeType::UNDEFINED;

		float f = 0.0f;
		int64_t i = 0;
		std::string s;
		std::unique_ptr<TensorProto> t;
		std::unique_ptr<GraphProto> g;

		std::vector<float> floats;
		std::vector<int64_t> ints;
		std::vector<std::string> strings;
		std::vector<std::unique_ptr<TensorProto>> tensors;
		std::vector<std::unique_ptr<GraphProto>> graphs;
	};

	struct ModelProto {
		int64_t ir_version = 0;
		std::vector<OperatorSetIdProto> opset_import;

		std::string producer_name;
		std::string producer_version;
		std::string domain;
		int64_t model_version = 0;
		std::string doc_string;

		std::unique_ptr<GraphProto> graph;
		std::unordered_map<std::string, std::string> metadata_props;
	};

	struct OperatorProto {
		std::string op_type;
		int64_t since_version = 0;
		std::string doc_string;
	};

	struct OperatorSetProto {
		std::string magic;
		int32_t ir_version = 0;
		std::string ir_version_prerelease;
		std::string ir_build_metadata;
		std::string domain;
		int64_t opset_version = 0;
		std::string doc_string;
		std::vector<OperatorProto> operator_;
	};

}; //namespace onnx
