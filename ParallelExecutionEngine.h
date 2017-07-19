#pragma once

#include <iostream>
#include <functional>
#include <fstream>
#include <string>

#include "OpenCLOutputArg.h"
#include "OpenCLPointerInArg.h"
#include "OpenCLSimpleInArg.h"

class ParallelExecutionEngine
{
public:
	static ParallelExecutionEngine* getInstance();
	void printInfo();
	void execTestCode();

	template <class OUTPUT>
	std::function<OUTPUT*(int, std::vector<OpenCLArg*>)> generateOpenCLFunction(const char*, const char*);

private:
	//	FIELDS
	static ParallelExecutionEngine* instance;
	std::vector<cl::Platform> platformList;
	std::vector<cl_context_properties*> contextProperties;
	std::vector<cl::Context> contexts;
	std::vector<cl::Device> devices;
	std::vector<cl::CommandQueue> commandQueues;

	//	METHODS
	ParallelExecutionEngine();

	inline void checkErr(cl_int, const char*);
};

template <class OUTPUT>
std::function<OUTPUT*(int, std::vector<OpenCLArg*>)> ParallelExecutionEngine::generateOpenCLFunction(const char* openCLFile, const char* functionName) {
	cl_int err;
	std::ifstream file(openCLFile);
	checkErr(file.is_open() ? CL_SUCCESS : -1, openCLFile);

	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
	cl::Program program(contexts[0], source);
	err = program.build(devices, "");
	cl::Kernel* kernel = new cl::Kernel(program, functionName, &err);
	checkErr(err, "Program.build()");

	//TODO
	cl::CommandQueue queue = commandQueues[0];
	cl::Context context = contexts[0];


	return [queue, context, kernel](int global, std::vector<OpenCLArg*> args) -> OUTPUT* {
		cl_int err;

		std::vector<cl::Buffer> buffers;
		cl::Buffer* outBuffer = NULL;
		OpenCLOutputArg* outArg = NULL;

		int i = 0;
		for each (OpenCLArg* arg in args)
		{
			if (arg->getArgType() == ArgType::IN_POINTER) {
				cl::Buffer buffer(context, CL_MEM_READ_ONLY, ((OpenCLPointerInArg*)arg)->size);
				buffers.push_back(buffer);
				queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, ((OpenCLPointerInArg*)arg)->size, ((OpenCLPointerInArg*)arg)->data);
				kernel->setArg(i++, buffer);
			}
			else if (arg->getArgType() == ArgType::OUTPUT) {
				outArg = (OpenCLOutputArg*)arg;
				cl::Buffer buffer(context, CL_MEM_WRITE_ONLY, outArg->size * sizeof(OUTPUT));
				outBuffer = &buffer;
				buffers.push_back(buffer);
				kernel->setArg(i++, buffer);
			}
			else if (arg->getArgType() == ArgType::IN_SIMPLE) {
				arg->setArgData(kernel, i++);
			}
		}

		if (outBuffer == NULL) {
			std::cerr << "No output buffer defined!!!";
			return NULL;
		}

		cl::Event event;
		err = queue.enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(global), cl::NDRange(1), NULL, &event);

		OUTPUT* out = new OUTPUT[outArg->size];
		event.wait();
		err = queue.enqueueReadBuffer(*outBuffer, CL_TRUE, 0, sizeof(OUTPUT) * outArg->size, out);

		return out;
	};
}