#pragma once

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <cstdint>
#include <stdlib.h>
//Number of image channels
#define CHANNELS 4
//Value for alpha channel
#define ALPHA 255

/*
*	This class contains the function used to execute
*	opening and closing operations using CUDA
*/
class CudaMathMorphology
{
public:
	static uint8_t* ExecuteOpeningOrClosing(int structWidth,
		int structHeight, uint8_t* image, int width, 
		int height, int* erOffset, int erCount,
		int* dilOffset, int dilCount, bool isOpening);
};