#pragma once

#define COMMAND_START "mkdir temp ; protoc --decode=onnx.ModelProto onnx/onnx.proto3 < "
#define COMMAND_END " > temp/temp.txt"


