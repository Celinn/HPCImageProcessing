// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiamondSquareAlgorithm.h"
#include <omp.h>

/**
 * This class implements a parallel version of
 * Diamond-square algorithm with OpenMP
 */
class HPCIMAGEPROCESSING_API OpenMPDiamondSquare
	: public DiamondSquareAlgorithm
{
public:
	OpenMPDiamondSquare(int size, int threadNumber);
	~OpenMPDiamondSquare() {}
	uint8* ExecuteDiamondSquare();

protected:
	void DiamondSquare(int matrixSize, int maxValue);
	void DiamondStep(int row, int column,
		int adding, int maxValue);
	void SquareStep(int row, int column,
		int adding, int maxValue);

private:
	int threadNum;
};
