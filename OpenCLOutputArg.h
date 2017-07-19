#pragma once

#include "OpenCLArg.h"

class OpenCLOutputArg : public OpenCLArg
{
public:
	OpenCLOutputArg(size_t);
	void setArgData(cl::Kernel*, int) {};

	size_t size;
};

