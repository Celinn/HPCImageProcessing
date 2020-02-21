// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MathematicalMorphology.h"
#include "MathematicalMorphologyCuda.h"

/*
 * This class implements a parallel version of
 * mathematical morphology with CUDA
 */
class HPCIMAGEPROCESSING_API CudaMMorphology
	: public MathematicalMorphology
{
public:
	CudaMMorphology(FImage* image, int size)
		:MathematicalMorphology(image, size) {}
	~CudaMMorphology() {}
	uint8* ExecuteOpeningOrClosing(bool isOpening);
protected:
	void SplitChannels(uint8* redChannel, uint8* greenChannel, 
		uint8* blueChannel, uint8 ghost) {}
	void ExecuteErosion(uint8* in, uint8* out) {}
	void ExecuteDilation(uint8* in, uint8* out) {}
	void FillGhostCells(uint8* red, uint8* green,
		uint8* blue, uint8 value) {};
};
