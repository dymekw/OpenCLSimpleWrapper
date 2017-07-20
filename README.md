# OpenCLSimpleWrapper

_OpenCLSimpleWrapper_ makes developing OpenCL apps faster and simplier.
Thanks _OpenCLSimpleWrapper_ You are able to invoke kernel like any other functions (almost ;))

## USAGE
> Full example -> main.cpp and text.cl

> Program computes product of 3x3 matrix and 3dim vector
### Creating kernel
OpenCL needs to parameters: name of file with kernel code, and name of the method.

To create _wrapped kernel_ get instance of **ParallelExecutionEngine** and invoke **generateOpenCLFunction\<class T\>**, example:
```c++
ParallelExecutionEngine::getInstance()->generateOpenCLFunction<int>("test.cl", "multiply");
```
As the result of above line, engine will return 
```c++ 
std::function<int* (int, std::vector<OpenCLArg*>)>
```
```int*``` - type of data, which is returned by Your kernel; it depands on type which You pass as **class T** during generateOpenCLFunction invocation (passing _int_ suggest that kernel will return array of integers).

```int``` - OpenCL _grobal_ parameter, passed to _enqueueNDRangeKernel_

```std::vector<OpenCLArg*>``` - collection of all arguments, described in next section

### Invoking kernel
Initialize all input arguments:
```c++
#define arraySize 3

int* A = new int[arraySize * arraySize];
int* B = new int[arraySize];

prepareData(A, B);
```
#### Passing primitive value
Create _OpenCLSimpleInArg_ object, pass type and value as argument:
```c++
new OpenCLSimpleInArg<int>(arraySize);
```

#### Passing object by pointer
Create _OpenCLPointerInArg_ object, pass Your pointer and **total** size of Your object as arguments:
```c++
new OpenCLPointerInArg(A, sizeof(int) * arraySize * arraySize);
new OpenCLPointerInArg(B, sizeof(int) * arraySize);
```

#### Prepare an _output object_
Create _OpenCLOutputArg_ object, pass expected **number of objects** as argument:
```c++
new OpenCLOutputArg(arraySize);
```

**Put all arguments in std::vector in proper order**
```c++
std::vector<OpenCLArg*> args = {
		new OpenCLPointerInArg(A, sizeof(int)*arraySize*arraySize),
		new OpenCLPointerInArg(B, sizeof(int)*arraySize),
		new OpenCLOutputArg(arraySize),
		new OpenCLSimpleInArg<int>(arraySize)
	};
```

#### Finally, invoke Your kernel
For:
```c++
std::function<int* (int, std::vector<OpenCLArg*>)> fun = ParallelExecutionEngine::getInstance()->generateOpenCLFunction<int>("test.cl", "multiply");
```
Simply:
```c++
int* out = fun(arraySize, args);
```


## TODO
1. Now, engine support only int and double primitive values (OpenCLSimpleInArg).
2. Engine uses only one available device.
