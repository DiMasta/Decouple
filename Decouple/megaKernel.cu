#include "processInstruction.h"

extern "C" __global__ void megaKernel(int* result, int textureToBeUsed) {
	*result = processInstruction(textureToBeUsed);
}
