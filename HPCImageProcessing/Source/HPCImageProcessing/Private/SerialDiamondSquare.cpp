// Fill out your copyright notice in the Description page of Project Settings.


#include "SerialDiamondSquare.h"

/*
*	It executes the diamond-square algorithm
*/
uint8* SerialDiamondSquare::ExecuteDiamondSquare()
{
	int last = this->size - 1;
	if (this->image == NULL)
	{
		return NULL;
	}
	srand((unsigned)time(0));
	// It inizializes matrix angles
	this->image[0 * this->size + 0] = rand() % MAX;
	this->image[0 * this->size + last] = rand() % MAX;
	this->image[last * this->size + 0] = rand() % MAX;
	this->image[last * this->size + last] = rand() % MAX;

	this->DiamondSquare(last, MAX);
	return this->image;
}

/*
*	Recursive function.
*	It starts the diamond-square algorithm
*		matrixSize: size of the matrix row/column
*			which the algorithm has to be executed on
*		maxValue: random seed to use
*/
void SerialDiamondSquare::DiamondSquare(int matrixSize, int maxValue)
{
	int i, j;
	int startIndex, endSquare;
	int last = this->size - 1;
	int half = matrixSize / 2;

	if (matrixSize > 1)
	{
		//Diamond step
		for (i = 0; i < last; i += matrixSize)
		{
			for (j = 0; j < last; j += matrixSize)
			{
				this->DiamondStep(i, j, half, maxValue);
			}
		}
		//Square step
		for (i = 0; i < this->size; i += half)
		{
			if (i%matrixSize == 0)
			{
				startIndex = half;
				endSquare = last;
			}
			else
			{
				startIndex = 0;
				endSquare = this->size;
			}
			for (j = startIndex; j < endSquare; j += matrixSize)
			{
				this->SquareStep(i, j, half, maxValue);
			}
		}
		this->DiamondSquare(half, maxValue / 2);
	}
}

/*
*	Diamond step: it sets the center cell of a square with the
*	average of the angles plus a random value.
*		row: row index
*		column: column index
*		adding: the value to add at the index to set
*			the correct cell
*		maxValue: random seed
*/
void SerialDiamondSquare::DiamondStep(int row, int column,
	int adding, int maxValue)
{
	int target_r = row + adding;
	int target_c = column + adding;
	int max = maxValue / 2 > 1 ? maxValue / 2 : 1;
	int min = -max;
	int randomValue = min + rand() % (max - min);
	int value = this->image[row*this->size + column] +
		this->image[row*this->size + target_c + adding] +
		this->image[(target_r + adding) * this->size + column] +
		this->image[(target_r + adding) * this->size + target_c + adding];
	value += randomValue;
	value /= 4;
	this->image[target_r* this->size + target_c] = value;
}

/*
*	Square step: it sets the center cell of a diamond with the
*	average of the angles plus a random value.
*		row: row index
*		column: column index
*		addingValue: the value to add at the index to set
*			the cell with the correct value
*		maxValue: random seed
*/
void SerialDiamondSquare::SquareStep(int row, int column,
	int adding, int maxValue)
{
	int value = 0;
	int div = 0;
	int max = maxValue / 2 > 1 ? maxValue / 2 : 1;
	int min = -max;
	int randomValue = min + rand() % (max - min);
	if (row != 0)
	{
		value += this->image[(row - adding) * this->size + column];
		div++;
	}
	if (row != this->size - 1)
	{
		value += this->image[(row + adding) * this->size + column];
		div++;
	}
	if (column != 0)
	{
		value += this->image[row* this->size + column - adding];
		div++;
	}
	if (column != this->size - 1)
	{
		value += this->image[row* this->size + column + adding];
		div++;
	}
	value += randomValue;
	value /= div;
	this->image[row* this->size + column] = value;
}
