#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <optional>
#include <fstream>
#include <memory>
#include "onnx_utils.hpp"

#define DEFAULT_OPSET_IMPORT_DOMAIN "ai.onnx"

typedef std::vector<std::string>::iterator vec_str_iter_t;
typedef void (*cmd_func_t) (vec_str_iter_t&, void*);

struct CommandProto {
	void* cmd_var;
	cmd_func_t cmd_func;
};

typedef std::unordered_map<std::string, CommandProto> map_cmd_set_t;
typedef std::unordered_map<std::string, std::string> metadata_t;

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
		INTS
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
		std::string name;
		std::string doc_string;
		std::vector<int64_t> dims;
		TensorDataType data_type = TensorDataType::UNDEFINED;
		std::vector<uint8_t> raw_data;
	};

	struct NodeProto {
		std::string name;
		std::string op_type;
		std::string domain;
		std::vector<std::string> input;
		std::vector<std::string> output;
		std::vector<AttributeProto> attribute;
		metadata_t metadata_props;
	};

	struct GraphProto {
		std::string name;
		std::vector<NodeProto> node;
		std::vector<std::unique_ptr<TensorProto>> initializer;
		std::vector<ValueInfoProto> input;
		std::vector<ValueInfoProto> output;
		std::vector<ValueInfoProto> value_info;
		metadata_t metadata_props;
	};

	struct AttributeProto {
		std::string name;
		std::string doc_string;
		AttributeType type = AttributeType::UNDEFINED;

		float f = 0.0f;
		int64_t i = 0;
		std::string s;

		std::vector<float> floats;
		std::vector<int64_t> ints;
	};

	struct OperatorProto {
		std::string op_type;
		int64_t since_version = 0;
		std::string doc_string;
	};

	struct OperatorSetIdProto {
		std::optional<std::string> domain = DEFAULT_OPSET_IMPORT_DOMAIN;
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
		int64_t ir_version = 0;
		std::optional<std::string> producer_name;
		std::optional<std::string> producer_version;
		OperatorSetIdProto opset_import;
		metadata_t metadata_props;

		std::vector<std::string> protoc_buffer;
		std::unique_ptr<GraphProto> graph;
		OperatorSetProto opset;

		public:
			ModelProto(const std::string& protoc_path) {
				protoc_buffer = readFile(protoc_path);

				#ifdef DEBUG
					std::fstream outFile(TEMP_FILE);
					for (auto iter = protoc_buffer.begin(); iter != protoc_buffer.end(); iter++)
						outFile << (*iter).c_str() << std::endl;
				#endif
			}

			bool is_empty() { return protoc_buffer.empty(); }
			bool is_filled() { return (graph != nullptr); }

			void fill();
			void print_info();
	};

}; //namespace onnx
