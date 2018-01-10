#include "processInstruction.h"

// COMPILE_?? flags are passed to nvrtc based on which textures must be used
__device__ int processInstruction(int instruction) {
	int textureResult = -1;

	switch (instruction) {
#ifdef COMPILE_DIRT
#include "dirtUtils.h"
		case I_DIRT: {
			textureResult = evalDirt();
			break;
		}
#endif // COMPILE_DIRT

#ifdef COMPILE_CHECKER
#include "checkerUtils.h"
		case I_CHECKER: {
			textureResult = evalChecker();
			break;
		}
#endif // COMPILE_CHECKER

#ifdef COMPILE_RAMP
#include "rampUtils.h"
		case I_RAMP: {
			textureResult = evalRamp();
			break;
		}
#endif // COMPILE_RAMP

		default: {
			printf("NO TEXTURE USED\n");
		}
	}

	return textureResult;
}
