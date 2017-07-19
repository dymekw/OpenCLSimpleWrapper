#pragma once

#include "OpenCLArg.h"

template <class T>
class OpenCLSimpleInArg : public OpenCLArg
{
public:
	OpenCLSimpleInArg(T data) {
		argType = ArgType::IN_SIMPLE;
		this->data = data;
	}
	void setArgData(cl::Kernel*, int);

	T data;
};

template<class T>
inline void OpenCLSimpleInArg<T>::setArgData(cl::Kernel* kernel, int id)
{
	if (int p = static_cast<int>(data)) {
		kernel->setArg(id, p);
	}
	else if (double p = static_cast<double>(data)) {
		kernel->setArg(id, p);
	}
	else {
		std::cerr << "OpenCLSimpleInArg::setArgData - unsupported argument type";
	}
}
