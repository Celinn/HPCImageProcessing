// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DiamondSquareAlgorithm.h"

/**
 * This class implements a serial version of Diamond-square algorithm
 */
class HPCIMAGEPROCESSING_API SerialDiamondSquare
	: public DiamondSquareAlgorithm
{
public:
	SerialDiamondSquare(int size)
		: DiamondSquareAlgorithm(size) {}
	~SerialDiamondSquare() {}
	uint8* ExecuteDiamondSquare();

protected:
	void DiamondSquare(int matrixSize, int maxValue);
	void DiamondStep(int row, int column,
		int adding, int maxValue);
	void SquareStep(int row, int column,
		int adding, int maxValue);
};
