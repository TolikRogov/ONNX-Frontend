#pragma once

#include <vector>
#include <string>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define EXCEPTION_INFO __FILE_NAME__ ":" TOSTRING(__LINE__) ": "

#define TEMP_DIR "temp/"
#define TEMP_FILE TEMP_DIR "temp.txt"
#define PROTOC_CMD_START "mkdir -p " TEMP_DIR " ; protoc --decode=onnx.ModelProto onnx/onnx.proto3 < "
#define PROTOC_CMD_END " > " TEMP_FILE

typedef void (*head_func_t) (std::string&, void*);

void sToi(std::string& str, void* var);
void sToopts(std::string& str, void* var);

void protocDecode(const std::string& onnx_path);
std::vector<std::string> readFile(const std::string& file_path);
