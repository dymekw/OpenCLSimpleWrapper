#include "ParallelExecutionEngine.h"

ParallelExecutionEngine* ParallelExecutionEngine::instance = new ParallelExecutionEngine();

ParallelExecutionEngine::ParallelExecutionEngine()
{
	cl_int err;
	cl::Platform::get(&platformList);
	checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");

	for each (cl::Platform platform in platformList) {
		cl_context_properties* cprops = new cl_context_properties[3]{
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)(platform)(),
			0
		};
		contextProperties.push_back(cprops);
	}

	for each (cl_context_properties* context_property in contextProperties) {
		cl::Context context(CL_DEVICE_TYPE_GPU, context_property, NULL, NULL, &err);
		checkErr(err, "Context::Context()");
		contexts.push_back(context);

		std::vector<cl::Device> devices;
		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		this->devices.insert(this->devices.end(), devices.begin(), devices.end());

		for each (cl::Device device in this->devices)
		{
			cl::CommandQueue queue(context, device, 0, &err);
			commandQueues.push_back(queue);
		}
	}
}

ParallelExecutionEngine* ParallelExecutionEngine::getInstance() {
	return instance;
}

inline void ParallelExecutionEngine::checkErr(cl_int err, const char* name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ParallelExecutionEngine::printInfo() {
	for each (cl::Platform platform in platformList)
	{
		std::string platformVendor;
		platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
		std::cerr << "Platform is by: " << platformVendor << "\n";
	}
	for each (cl::Device device in devices)
	{
		std::string name;
		device.getInfo(CL_DEVICE_NAME, &name);
		std::cout << "Device - " << name << std::endl;
	}
}

void ParallelExecutionEngine::execTestCode() {
	cl_int err;
	cl::Context context = contexts.at(0);
	const int arraySize = 10;
	std::ifstream file("hello.cl");
	checkErr(file.is_open() ? CL_SUCCESS : -1, "hello.cl");

	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));
	cl::Program program(context, source);
	err = program.build(devices, "");
	checkErr(err, "Program.build()");

	cl::CommandQueue queue(context, devices[0], 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");

	cl::Buffer inA(context, CL_MEM_READ_ONLY, sizeof(int) * arraySize * arraySize);
	cl::Buffer inB(context, CL_MEM_READ_ONLY, sizeof(int) * arraySize);
	cl::Buffer outCL(context, CL_MEM_WRITE_ONLY, sizeof(int) * arraySize);
	int* A = new int[arraySize * arraySize];
	int* B = new int[arraySize];
	int* out = new int[arraySize];

	for (int i = 0; i < arraySize; i++) {
		for (int j = 0; j < arraySize; j++) {
			A[i*arraySize + j] = i*arraySize + j;
		}
		B[i] = i;
	}

	queue.enqueueWriteBuffer(inA, CL_TRUE, 0, sizeof(int) * arraySize * arraySize, A);
	queue.enqueueWriteBuffer(inB, CL_TRUE, 0, sizeof(int) * arraySize, B);

	cl::Kernel kernel(program, "multiply", &err);
	kernel.setArg(0, inA);
	kernel.setArg(1, inB);
	kernel.setArg(2, outCL);
	kernel.setArg(3, arraySize);

	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(arraySize), cl::NDRange(1), NULL, &event);
	checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

	event.wait();
	err = queue.enqueueReadBuffer(outCL, CL_TRUE, 0, sizeof(int) * arraySize, out);
	checkErr(err, "ComamndQueue::enqueueReadBuffer()");

	for(int i=0; i<arraySize; i++)
	{
		std::cout << out[i] << " ";
	}

	delete[] out;
	delete[] A;
	delete[] B;
}