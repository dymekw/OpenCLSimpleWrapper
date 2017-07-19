#include "OpenCLOutputArg.h"



OpenCLOutputArg::OpenCLOutputArg(size_t size)
{
	argType = ArgType::OUTPUT;
	this->size = size;
}

