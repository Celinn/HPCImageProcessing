// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <ctime>
#define MAX 256

/**
 *	Abstract class parent of the other classes that implement
 *	the diamond-square algorithm
 */
class HPCIMAGEPROCESSING_API DiamondSquareAlgorithm
{
public:
	DiamondSquareAlgorithm(int size);
	virtual ~DiamondSquareAlgorithm();
	virtual uint8* ExecuteDiamondSquare() = 0;
protected:
	virtual void DiamondSquare(int matrixSize, int maxValue) = 0;
	virtual void DiamondStep(int row, int column,
		int adding, int maxValue) = 0;
	virtual void SquareStep(int row, int column,
		int adding, int maxValue) = 0;

	uint8* image;
	int size;
};
