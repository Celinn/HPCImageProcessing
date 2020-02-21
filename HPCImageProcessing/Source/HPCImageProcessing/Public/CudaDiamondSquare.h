// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//Library that contains the code with CUDA kernel
#include "DiamondSquareCuda.h"
#include "DiamondSquareAlgorithm.h"

/*
 * This class implements a parallel version of
 * Diamond-square algorithm with CUDA
 */
class HPCIMAGEPROCESSING_API CudaDiamondSquare
	: public DiamondSquareAlgorithm
{
public:
	CudaDiamondSquare(int size)
		: DiamondSquareAlgorithm(size) {}
	~CudaDiamondSquare() {}
	uint8* ExecuteDiamondSquare();

protected:
	void DiamondSquare(int matrixSize, int maxValue);
	void DiamondStep(int row, int column,
		int adding, int maxValue) {}
	void SquareStep(int row, int column,
		int adding, int maxValue) {}
};
