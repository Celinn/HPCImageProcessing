// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MathematicalMorphology.h"
#include <omp.h>

/**
 * This class implements a parallel version of
 * mathematical morphology with OpenMP
 */
class HPCIMAGEPROCESSING_API OpenMPMMorphology
	: public MathematicalMorphology
{
public:
	OpenMPMMorphology(FImage* image, int size, int threadNum);
	~OpenMPMMorphology() {}
	uint8* ExecuteOpeningOrClosing(bool isOpening);
protected:
	void SplitChannels(uint8* redChannel,uint8* greenChannel,
		uint8* blueChannel, uint8 ghost);
	void ComposeImage(uint8* redChannel,
		uint8* greenChannel, uint8* blueChannel, uint8* output);
	void ExecuteErosion(uint8* in, uint8* out);
	void ExecuteDilation(uint8* in, uint8* out);
	void FillGhostCells(uint8* red, uint8* green,
		uint8* blue, uint8 value);
};
