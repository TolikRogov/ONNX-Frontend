#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
#include "onnx_utils.hpp"

#define DEFAULT_OPSET_IMPORT_DOMAIN "ai.onnx"

namespace onnx {

	struct HeadProtocCmd;
	typedef std::vector<HeadProtocCmd> vec_cmd;

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

	struct OperatorProto {
		std::string op_type;
		int64_t since_version = 0;
		std::string doc_string;
	};

	struct OperatorSetIdProto {
		std::string domain = DEFAULT_OPSET_IMPORT_DOMAIN;
		int64_t version = 0;
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

	class ModelProto {
		std::vector<std::string> protoc_buffer;
		OperatorSetProto opset;
		std::unique_ptr<GraphProto> graph;

		public:
			int64_t ir_version = 0;
			std::optional<std::string> producer_name;
			std::optional<std::string> producer_version;
			OperatorSetIdProto opset_import;
			std::unordered_map<std::string, std::string> metadata_props;

			ModelProto(const std::string& protoc_path) {
				protoc_buffer = readFile(protoc_path);
			}

			bool is_empty() {
				return protoc_buffer.empty();
			}

			bool is_filled() {
				return (graph != nullptr);
			}

			void fill();
	};

	struct HeadProtocCmd {
		const std::string cmd_name;
		void* cmd_var;
		head_func_t cmd_func;
	};

	vec_cmd::iterator findCmdByName(vec_cmd& cmds, const std::string& cmd_name);

}; //namespace onnx
