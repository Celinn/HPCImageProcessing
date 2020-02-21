// Fill out your copyright notice in the Description page of Project Settings.


#include "DiamondSquareAlgorithm.h"

/*
*	DiamondSquareAlgorithm constructor.
*	It allocates the space for the matrix
*		size: the length of the matrix row/column
*/
DiamondSquareAlgorithm::DiamondSquareAlgorithm(int size)
{
	int imageSize = sizeof(uint8)*size*size;
	this->image = (uint8*)malloc(imageSize);
	this->size = size;
}

/*
*	DiamondSquareAlgorithm destructor.
*	It frees the matrix allocated space
*/
DiamondSquareAlgorithm::~DiamondSquareAlgorithm()
{
	free(this->image);
}
