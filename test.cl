__kernel void multiply(__global int* inA, __global int* inB, __global int* out, int size)
{
	size_t id = get_global_id(0);
	if(id < size)
	{
		out[id] = 0;
		for (int j = 0; j < size; j++) {
			out[id] += inA[id*size + j] * inB[j];
		}
	}
}