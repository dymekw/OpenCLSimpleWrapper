#pragma once

#include <CL/cl.hpp>

enum ArgType { OUTPUT, IN_SIMPLE, IN_POINTER };

class OpenCLArg
{
public:
	ArgType getArgType() {return argType;};
	virtual void setArgData(cl::Kernel*, int) = 0;
protected:
	OpenCLArg() {};
	ArgType argType;
};
