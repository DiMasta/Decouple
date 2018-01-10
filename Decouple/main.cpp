#include <iostream>
#include <nvrtc.h>
#include <cuda.h>

using namespace std;

#include "nvrtcProgram.h"

const int TEXTURES_COUNT = 3;
const string TEXTURE_COMPILE_OPTIONS[TEXTURES_COUNT] = {
	"-D COMPILE_DIRT",
	"-D COMPILE_CHECKER",
	"-D COMPILE_RAMP"
};

const string TEXTURE_FILE_NAMES[TEXTURES_COUNT] = {
	"dirt.cu",
	"checker.cu",
	"ramp.cu"
};

const string DEVICE_C_OPTION = "--device-c";

//************************************************************************************************************************
//************************************************************************************************************************

int main(int argc, char *argv[]) {
	// This is the host main func (consider V-Ray)

	// Here we'll understand what texture the user wants
	int textureToBeUsed = -1;
	cout << "Tell us what texture {(0, 1, 2) = (dirt, checker, ramp)} you need:";
	cin >> textureToBeUsed;

	// After we know what texture the user wants
	// we have to compile the main "processInstruction" function
	// and the functions needed for the wanted texture.
	// The V-Ray is running, so we have to use nvrtc to runtime compile the needed code

	StringList compileOptions = { DEVICE_C_OPTION };

	// using nvrtc to compile "megaKernel" function
	const string megaKernelFileName = "megaKernel.cu";
	NVRTCProgram megaKernelProgram;
	megaKernelProgram.createAndCompile(megaKernelFileName, compileOptions);
	
	compileOptions.push_back(TEXTURE_COMPILE_OPTIONS[textureToBeUsed]);

	// using nvrtc to compile "processInstruction" function
	const string processInstructionFileName = "processInstruction.cu";
	NVRTCProgram processInstructionProgram;
	processInstructionProgram.createAndCompile(processInstructionFileName, compileOptions);

	// using nvrtc to compile the given texture code
	const string textureFileName = TEXTURE_FILE_NAMES[textureToBeUsed];
	NVRTCProgram textureProgram;
	textureProgram.createAndCompile(textureFileName, compileOptions);

	// link megaKernel with processInstruction and texture code
	CUdevice cuDevice;
	CUcontext context;
	CUlinkState linkState;
	CUmodule module;
	CUfunction kernel;
	cuInit(0);
	cuDeviceGet(&cuDevice, 0);
	cuCtxCreate(&context, 0, cuDevice);
	cuLinkCreate(0, nullptr, nullptr, &linkState);

	CUresult addDataRes;
	addDataRes = cuLinkAddData(linkState, CU_JIT_INPUT_PTX, (void *)megaKernelProgram.getPtx(), megaKernelProgram.getPtxSize(), "megaKernel.ptx", 0, 0, 0);
	addDataRes = cuLinkAddData(linkState, CU_JIT_INPUT_PTX, (void *)processInstructionProgram.getPtx(), processInstructionProgram.getPtxSize(), "processInstruction.ptx", 0, 0, 0);
	addDataRes = cuLinkAddData(linkState, CU_JIT_INPUT_PTX, (void *)textureProgram.getPtx(), textureProgram.getPtxSize(), "texture.ptx", 0, 0, 0);

	size_t cubinSize;
	void *cubin;
	CUresult linkResult = cuLinkComplete(linkState, &cubin, &cubinSize);

	// Invoke megaKernel
	CUresult moduleLoadResult = cuModuleLoadData(&module, cubin);
	CUresult getFuncResult = cuModuleGetFunction(&kernel, module, "megaKernel");

	int* hostMegaKernelRes = new int[1];
	CUdeviceptr deviceMegaKernelRes;

	size_t buffSize = sizeof(int);

	CUresult mallocResult = cuMemAlloc(&deviceMegaKernelRes, buffSize);

	void* megaKernelArgs[] = { &deviceMegaKernelRes, &textureToBeUsed };

	CUresult launchResult =
		cuLaunchKernel(
			kernel,
			1, 1, 1,		// grid dim
			1, 1, 1,		// block dim
			0, nullptr,		// shared mem and stream
			megaKernelArgs, 0
		);

	CUresult synchResult = cuCtxSynchronize();

	CUresult memCpyResult = cuMemcpyDtoH(hostMegaKernelRes, deviceMegaKernelRes, buffSize);

	cout << "The result from texture is: " << *hostMegaKernelRes << endl;

	cuMemFree(deviceMegaKernelRes);
	cuModuleUnload(module);
	cuLinkDestroy(linkState);
	CUresult destroyResult = cuCtxDestroy(context);

	delete[] hostMegaKernelRes;

	system("pause");

	return 0;
}
