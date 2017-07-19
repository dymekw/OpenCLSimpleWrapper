#include "OpenCLPointerInArg.h"

OpenCLPointerInArg::OpenCLPointerInArg(void* data, size_t size) {
	argType = ArgType::IN_POINTER;
	this->data = data;
	this->size = size;
}
