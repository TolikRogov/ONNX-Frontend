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

#define PRINT_EPS 16
#define PRINT_WIDTH 25
#define PRINT_LINE(el, cnt) std::string(cnt, el)
#define PRINT_HEAD(x, sym) \
	PRINT_LINE(sym, sizeof(x) + PRINT_EPS) << "\n" "\t" x "\n" << PRINT_LINE(sym, sizeof(x) + PRINT_EPS) << "\n"
#define PRINT_HEAD_STR(x, sym) \
	PRINT_LINE(sym, x.size() + PRINT_EPS) << "\n" "\t" x "\n" << PRINT_LINE(sym, x.size() + PRINT_EPS) << "\n"

#define COLOR_GREEN "\033[32m"
#define COLOR_RED	"\033[31m"
#define COLOR_RESET "\033[0m"
#define COLOR_IF_USED(text, used) ((used == 1) ? (COLOR_GREEN + text + COLOR_RESET) : (COLOR_RED + text + COLOR_RESET))

void protocDecode(const std::string& onnx_path);
std::vector<std::string> readFile(const std::string& file_path);
