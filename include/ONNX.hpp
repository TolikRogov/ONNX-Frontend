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
#define DEFAULT_OPSET_MAGIC "ONNXOPSET"

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
		INTS,
		TENSOR,
	};

	struct Dimension {//dim_value	//dim_param
		std::variant<int64_t, std::string> value;
	};

	struct TensorShapeProto {
		std::vector<Dimension> dim;
	};

	struct TensorTypeProto {
		TensorDataType elem_type = TensorDataType::UNDEFINED;
		std::optional<TensorShapeProto> shape;
	};

	typedef std::variant<TensorTypeProto> valinfo_type_value_t;

	struct TypeProto {
		valinfo_type_value_t value_type;
	};

	struct ValueInfoProto {
		std::string name;
		TypeProto type;
	};

	struct AttributeProto;

	struct TensorProto {
		std::string name;
		std::vector<int64_t> dims;
		TensorDataType data_type = TensorDataType::UNDEFINED;
		std::vector<uint8_t> raw_data;
		std::vector<float> float_data;
	};

	struct NodeProto {
		std::string name;
		std::string graphviz_name;
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
		TensorProto t;

		std::vector<float> floats;
		std::vector<int64_t> ints;
	};

	struct OperatorProto {
		int64_t since_version = 0;
		std::string color;
		bool used = 0;
		std::string doc_string;
	};

	struct OperatorSetIdProto {
		std::optional<std::string> domain = DEFAULT_OPSET_IMPORT_DOMAIN;
		int64_t version = 0;
	};

	struct OperatorSetProto {
		std::string magic = DEFAULT_OPSET_MAGIC;
		std::string domain = DEFAULT_OPSET_IMPORT_DOMAIN;
		int64_t opset_version = 0;
		std::unordered_map<std::string, OperatorProto> operator_;
	};

	class ModelProto {
		int64_t ir_version = 0;
		std::optional<std::string> producer_name;
		std::optional<std::string> producer_version;
		OperatorSetIdProto opset_import;
		metadata_t metadata_props;

		std::vector<std::string> protoc_buffer;
		std::vector<std::string> opset_buffer;
		std::unique_ptr<GraphProto> graph;

		static std::ofstream dotFile;
		static std::string model_path;
		static OperatorSetProto opset;
		static void print_opset();

		static void scope_going(vec_str_iter_t&, map_cmd_set_t&);
		static void setOpset(vec_str_iter_t&, void*);
		static void addOperator(vec_str_iter_t&, void*);

		static void setOpsetImport(vec_str_iter_t&, void*);
		static void setGraph(vec_str_iter_t&, void*);

		static void addNode(vec_str_iter_t&, void*);
		static void addNodeToGraphviz(NodeProto&);
		static void addNodePut(vec_str_iter_t&, void*);
		static void addOpType(vec_str_iter_t&, void*);

		static void addAttribute(vec_str_iter_t&, void*);
		static void addAttributeType(vec_str_iter_t&, void*);
		static void addToVectorInts(vec_str_iter_t&, void*);

		static void addInitializer(vec_str_iter_t&, void*);
		static void addInitializerRawData(vec_str_iter_t&, void*);
		static void addInitializerFloatData(vec_str_iter_t&, void*);

		static void addTensorType(vec_str_iter_t&, void*);
		static void addTensorShape(vec_str_iter_t&, void*);
		static void addTensorDim(vec_str_iter_t&, void*);
		static void addDimParam(vec_str_iter_t&, void*);

		static void addValueInfo(vec_str_iter_t&, void*);
		static void addValueInfoType(vec_str_iter_t&, void*);
		static void addValueInfoTensor(vec_str_iter_t&, void*);

		static void setMetadataProps(vec_str_iter_t&, void*);
		static void addMetadataValue(vec_str_iter_t&, void*);

		static void sTof(vec_str_iter_t&, void*);
		static void sTos(vec_str_iter_t&, void*);
		static void sToi(vec_str_iter_t&, void*);
		static void sToOpts(vec_str_iter_t&, void*);

		public:
			ModelProto(const std::string& onnx_path) {
				model_path = onnx_path;
				protocDecode(onnx_path);
				protoc_buffer = readFile(TEMP_FILE);
				opset_buffer = readFile(ONNX_OPERATORS);
				dotFile.open(model_path + DOT_EXTENSION);
			}

			void fill();

			bool check_optype(std::string& op_type) { return opset.operator_.contains(op_type); }
			bool is_empty() { return protoc_buffer.empty(); }
			bool is_filled() { return (graph != nullptr); }

			void print_info();
			void print_metadata();
	};

}; //namespace onnx
