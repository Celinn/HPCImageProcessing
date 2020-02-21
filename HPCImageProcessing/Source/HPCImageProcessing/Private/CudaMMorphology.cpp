// Fill out your copyright notice in the Description page of Project Settings.

#include "CudaMMorphology.h"

/*
*	It executes operations calling the function of
*	MathematicalMorphologyCuda library that uses
*	CUDA kernels
*/
uint8* CudaMMorphology::ExecuteOpeningOrClosing(bool isOpening)
{
	uint8* output = CudaMathMorphology::ExecuteOpeningOrClosing(
		this->structElem.width, this->structElem.height,
		this->input->RawData.GetData(), this->input->SizeX,
		this->input->SizeY, this->ErosionOffsets.offsets,
		this->ErosionOffsets.count, this->DilationOffsets.offsets,
		this->DilationOffsets.count, isOpening);
	return output;
}