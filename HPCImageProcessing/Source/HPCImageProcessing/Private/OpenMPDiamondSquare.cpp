// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenMPDiamondSquare.h"
#include "TextureUtilities.h"
/*
*	OpenMPDiamondSquare constructor.
*	It allocates space for the image matrix
*	and sets the number of threads to use.
*		size: the length of the matrix row/column
*		threadNum: the number of thread to use
*/
OpenMPDiamondSquare::OpenMPDiamondSquare(int size, int threadNumber) 
	: DiamondSquareAlgorithm(size)
{
	this->threadNum = threadNumber;
	omp_set_num_threads(this->threadNum);
}

/*
*	It executes the diamond-square algorithm
*/
uint8 * OpenMPDiamondSquare::ExecuteDiamondSquare()
{
	int last = this->size - 1;
	if (this->image == NULL)
	{
		return NULL;
	}
	srand((unsigned)time(0));
	/*It initializes matrix angles using random values*/
	this->image[0 * size + 0] = rand() % MAX;
	this->image[0 * size + last] = rand() % MAX;
	this->image[last*size + 0] = rand() % MAX;
	this->image[last*size + last] = rand() % MAX;
#pragma omp parallel
	{
		this->DiamondSquare(last, MAX);
	}
	return this->image;
}
	
/*
*	Recursive function.
*	It starts the diamond-square algorithm
*		matrixSize: size of the matrix row/column
*			which the algorithm has to be executed on
*		maxValue: random seed to use
*/
void OpenMPDiamondSquare::DiamondSquare(int matrixSize, int maxValue)
{
	int last = size - 1;
	int half = matrixSize / 2;
	if (matrixSize > 1)
	{
		int startIndex, endSquare;
		int i, j;
		unsigned int seed = (unsigned)time(0)*(omp_get_thread_num() + 1);
		srand(seed);
		//Diamond step
#pragma omp for private(j)
		for (i = half; i < last; i += matrixSize)
		{
			for (j = half; j < last; j += matrixSize)
			{
				this->DiamondStep(i, j, half, maxValue);
			}
		}
		//Square step
#pragma omp for private(j, startIndex, endSquare)
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
*		addingValue: the value to add at the index to set
*			the correct cell
*		maxValue: random seed
*/
void OpenMPDiamondSquare::DiamondStep(int row, int column,
	int adding, int maxValue)
{
	int max = maxValue / 2 > 1 ? maxValue / 2 : 1;
	int min = -max;
	int random = min + rand() % (max - min);
	int value = this->image[(row - adding)*this->size + (column - adding)] +
		this->image[(row - adding)*this->size + column + adding] +
		this->image[(row + adding) * this->size + (column - adding)] +
		this->image[(row + adding) * this->size + column + adding] + random;
	value /= 4;
	this->image[row* this->size + column] = value;
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
void OpenMPDiamondSquare::SquareStep(int row, int column,
	int adding, int maxValue)
{
	int value = 0;
	int div = 0;
	int max = maxValue / 2 > 1 ? maxValue / 2 : 1;
	int min = -max;
	int random = min + rand() % (max - min);
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
	value += random;
	value /= div;

	this->image[row* this->size + column] = value;
}