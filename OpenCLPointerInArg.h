#pragma once

#include "OpenCLArg.h"

class OpenCLPointerInArg : public OpenCLArg
{
public:
	/*
	if data* points to array, size should be equal to total size eg sizeof(int) * 123 for int tab[123]
	*/
	OpenCLPointerInArg(void* data, size_t size);
	void setArgData(cl::Kernel*, int) {};

	void* data;
	size_t size;
};

