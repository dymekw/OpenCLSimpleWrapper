#include "ParallelExecutionEngine.h"

#define arraySize 3
#define maxValue 5

void randomValues(cl_int* data, int mul)
{
	for (int i = 0; i < arraySize * mul; i++)
	{
		data[i] = rand() % maxValue;
	}
}

void prepareData(int* A, int* B) {
	randomValues(A, arraySize);
	randomValues(B, 1);
}

int main()
{
	int* A = new int[arraySize * arraySize];
	int* B = new int[arraySize];

	prepareData(A, B);

	std::vector<OpenCLArg*> args = {
		new OpenCLPointerInArg(A, sizeof(int)*arraySize*arraySize),
		new OpenCLPointerInArg(B, sizeof(int)*arraySize),
		new OpenCLOutputArg(arraySize),
		new OpenCLSimpleInArg<int>(arraySize)
	};

	std::function<int* (int,  int, std::vector<OpenCLArg*>)> fun = ParallelExecutionEngine::getInstance()->generateOpenCLFunction<int>("test.cl", "multiply");
	int* out = fun(arraySize, 4, args);

	for (int i = 0; i < arraySize; i++) {
		for (int j = 0; j < arraySize; j++) {
			std::cout << A[i*arraySize + j] << " ";
		}
		std::cout << "\t" << B[i] << "\t";
		std::cout << out[i] << std::endl;
	}

	std::cout << "Computed\n";

	std::cin.get();
	return 0;
}