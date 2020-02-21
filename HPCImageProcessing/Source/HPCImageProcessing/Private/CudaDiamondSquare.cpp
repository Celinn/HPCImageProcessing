// Fill out your copyright notice in the Description page of Project Settings.


#include "CudaDiamondSquare.h"

/*
*	It executes the diamond-square algorithm
*/
uint8 * CudaDiamondSquare::ExecuteDiamondSquare()
{
	int last = this->size - 1;
	if (this->image == NULL)
	{
		return NULL;
	}
	/*It initializes matrix angles using random values*/
	this->image[0 * size + 0] = rand() % MAX;
	this->image[0 * size + last] = rand() % MAX;
	this->image[last*size + 0] = rand() % MAX;
	this->image[last*size + last] = rand() % MAX;

	this->DiamondSquare(last, MAX);
	return this->image;
}

/*
*	It starts the diamond-square algorithm calling the function
*	of the CudaDiamondSquare library that will call the kernels
*		matrixSize: size of the matrix row/column, but for this
*			version is useless because the algorithm is computed
*			by the CudaDiamondSquare library
*		maxValue: random seed
*/
void CudaDiamondSquare::DiamondSquare(int matrixSize, int maxValue)
{
	CudaAlgorithm::CudaDiamondSquare(this->image, this->size, maxValue);
}
