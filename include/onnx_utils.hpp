#pragma once

#include <vector>
#include <string>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define EXCEPTION_INFO __FILE_NAME__ ":" TOSTRING(__LINE__) ": "

#define TEMP_DIR "temp/"
#define ONNX_DIR "onnx/"
#define ONNX_PROTO3 ONNX_DIR "onnx.proto3"
#define ONNX_OPERATORS ONNX_DIR "operators.txt"
#define TEMP_FILE TEMP_DIR "temp.txt"
#define PROTOC_CMD_START "mkdir -p " TEMP_DIR " ; protoc --decode=onnx.ModelProto " ONNX_PROTO3 " < "
#define PROTOC_CMD_END " > " TEMP_FILE

#define DOT_EXTENSION ".dot"
#define DOT_OUTPUT_EXTENSION "svg"
#define DOT_GEN(file) ("dot -T" DOT_OUTPUT_EXTENSION " " + file + DOT_EXTENSION " -o " + file + "." DOT_OUTPUT_EXTENSION).c_str()

#define DOT_COLOR_BACKGROUND "\"#404040\""
#define DOT_COLOR_FONT "\"#DFDFDF\""
#define DOT_COLOR_EDGE "\"#878787\""
#define DOT_COLOR_NODE_BACK "\"#2D2D2D\""
#define DOT_COLOR_NODE_BORDER "\"#1D1D1D\""
#define DOT_GRAPH_NAME "main_graph"
#define DOT_NODE_SHAPE "box"
#define DOT_NODE_STYLE "\"rounded,filled\""
#define DOT_NODE_FONTSIZE TOSTRING(10)

#define PRINT_EPS 16
#define PRINT_WIDTH 26
#define PRINT_LONG_WIDTH 35
#define PRINT_LINE(el, cnt) std::string(cnt, el)
#define PRINT_HEAD(x, sym) \
	PRINT_LINE(sym, sizeof(x) + PRINT_EPS) << "\n" "\t" x "\n" << PRINT_LINE(sym, sizeof(x) + PRINT_EPS) << "\n"
#define PRINT_HEAD_STR(x, sym) \
	PRINT_LINE(sym, x.size() + PRINT_EPS) << "\n" "\t" x "\n" << PRINT_LINE(sym, x.size() + PRINT_EPS) << "\n"

#define ESC_COLOR_GREEN "\033[32m"
#define ESC_COLOR_RED	"\033[31m"
#define ESC_COLOR_RESET "\033[0m"
#define COLOR_IF_USED(text, used) ((used == 1) ? (ESC_COLOR_GREEN + text + ESC_COLOR_RESET) : (ESC_COLOR_RED + text + ESC_COLOR_RESET))

void protocDecode(const std::string& onnx_path);
std::vector<std::string> readFile(const std::string& file_path);
