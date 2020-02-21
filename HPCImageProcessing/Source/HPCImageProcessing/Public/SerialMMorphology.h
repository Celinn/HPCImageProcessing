// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MathematicalMorphology.h"

/**
 *	This class implements a serial version
 *	of mathematical morphology
 */
class HPCIMAGEPROCESSING_API SerialMMorphology
	: public MathematicalMorphology
{
public:
	SerialMMorphology(FImage* image, int size) 
		: MathematicalMorphology(image, size) {}
	~SerialMMorphology() {}
	uint8* ExecuteOpeningOrClosing(bool isOpening);
protected:
	void SplitChannels(uint8* redChannel,uint8* greenChannel,
		uint8* blueChannel, uint8 ghost);
	uint8* ComposeImage(uint8* redChannel,
		uint8* greenChannel, uint8* blueChannel);
	void ExecuteErosion(uint8* in, uint8* out);
	void ExecuteDilation(uint8* in, uint8* out);
	void FillGhostCells(uint8* red, uint8* green, 
		uint8* blue, uint8 value);
};
