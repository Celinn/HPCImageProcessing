// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextureUtilities.h"
#define FOREGROUND 255
#define BLACK 0
#define WHITE 255

/* structure that contains informations 
for structuring elements */
struct StructuringElement
{
	uint8* element;
	int width;
	int height;
};

/* structure that contains informations
for offsets */
struct Offset
{
	int* offsets;
	int count = 0;
};

/**
 *	Abstract class parent of the other classes that implement
 *	mathematical morphology operations
 */
class HPCIMAGEPROCESSING_API MathematicalMorphology
{
public:
	MathematicalMorphology(FImage* image, int size);
	virtual ~MathematicalMorphology();
	virtual uint8* ExecuteOpeningOrClosing(bool isOpening) = 0;
protected:
	virtual void SplitChannels(uint8* redChannel, uint8* greenChannel, 
		uint8* blueChannel, uint8 ghost) = 0;
	virtual void ExecuteErosion(uint8* input, uint8* output) = 0;
	virtual void ExecuteDilation(uint8* input, uint8* output) = 0;
	virtual void FillGhostCells(uint8* red, uint8* green,
		uint8* blue, uint8 value) = 0;
	FImage* input;
	StructuringElement structElem;
	Offset ErosionOffsets;
	Offset DilationOffsets;
private:
	void LoadStructuringElement(int size);
	void SetOffsets(Offset *offset, bool reflect);
	static const FString fileName;
	static const FString extension;
};
