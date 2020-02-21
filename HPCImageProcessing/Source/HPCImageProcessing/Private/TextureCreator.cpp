// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureCreator.h"

//Pixel format to use to produce the texture and the image
EPixelFormat const UTextureCreator::pixelFormat = EPixelFormat::PF_R8G8B8A8;
//Array of uint8 that will contain the image
uint8* UTextureCreator::imageData = NULL;
//Size of data used to create the image (4 bytes for each pixel)
int32 UTextureCreator::imageSize = 0;
//Width of the image
int UTextureCreator::sizeX = 0;
//Height of the image
int UTextureCreator::sizeY = 0;
FImage* UTextureCreator::image = NULL;

/* 
*	It creates the procedural texture using the selected algorithm
*		implementationType: the algorithm we want to use
*		size: the length of the matrix row/column
*		threadNumber: the number of thread we want to use in a OpenMP
*			implementation
*		executionTime: time the algorithm takes to produce the matrix
*/
UTexture2D* UTextureCreator::CreateProceduralTexture
(ImplementationType implementationType, int matrixSize, int threadNumber, float &executionTime)
{
	uint8* matrix = NULL;
	DiamondSquareAlgorithm *implementation = NULL;
	UTexture2D* texture = NULL;
	clock_t start, end;
	int textureSize = sizeof(uint8)*matrixSize*matrixSize * 4;
	switch (implementationType)
	{
	case ImplementationType::IT_Serial:
		implementation = new SerialDiamondSquare(matrixSize);
		break;
	case ImplementationType::IT_OpenMP:
		implementation = new OpenMPDiamondSquare(matrixSize, threadNumber);
		break;
	case ImplementationType::IT_Cuda:
		implementation = new CudaDiamondSquare(matrixSize);
		break;
	default:
		break;
	}
	start = clock();
	matrix = implementation->ExecuteDiamondSquare();
	end = clock();
	executionTime = (double)(end - start) / CLOCKS_PER_SEC;
	if (matrix)
	{
		UTextureCreator::sizeX = matrixSize;
		UTextureCreator::sizeY = matrixSize;
		texture = UTextureCreator::CreateChannels(matrix);
	}
	delete implementation;
	UTextureCreator::CreateImageInfo();
	return texture;
}

/* 
*	It loads the image from file and creates the texture to show
*/
UTexture2D* UTextureCreator::LoadImage()
{
	TArray<FString> files = UTextureUtilities::OpenFileDialog();
	if (files.IsValidIndex(0))
	{
		UTextureCreator::image = UTextureUtilities::LoadImageFromFile(files[0]);
		UTextureCreator::sizeX = image->SizeX;
		UTextureCreator::sizeY = image->SizeY;
		UTextureCreator::imageData = image->RawData.GetData();
		UTextureCreator::imageSize = image->RawData.Num();
		return UTextureCreator::CreateTexture();
	}
	return NULL;
}

/*
*	It creates the texture as result of mathematical morphology 
*	operations using the selected algorithm version
*		implementationType: the algorithm we want to use
*		threadNumber: the number of thread we want to use in a OpenMP
*			implementation
*		executionTime: time the algorithm takes to produce the matrix
*		isOpening: true if we want to execute an opening
*		structElemSize: size of the structuring element
*/
UTexture2D* UTextureCreator::ExecuteMMOperation(
	ImplementationType implementationType, int threadNumber,
	float &executionTime, bool isOpening, int structElemSize)
{
	UTexture2D* texture = NULL;
	uint8* output = NULL;
	clock_t start, end;
	MathematicalMorphology* implementation = NULL;
	switch (implementationType)
	{
	case ImplementationType::IT_Serial:
		implementation = new SerialMMorphology(UTextureCreator::image,
			structElemSize);
		break;
	case ImplementationType::IT_OpenMP:
		implementation = new OpenMPMMorphology(UTextureCreator::image,
			structElemSize, threadNumber);
		break;
	case ImplementationType::IT_Cuda:
		implementation = new CudaMMorphology(UTextureCreator::image,
			structElemSize);
		break;
	default:
		break;
	}
	start = clock();
	output = implementation->ExecuteOpeningOrClosing(isOpening);
	end = clock();
	executionTime = (double)(end - start) / CLOCKS_PER_SEC;
	if (output)
	{
		UTextureCreator::imageData = output;
		UTextureCreator::imageSize = sizeX * sizeY * CHANNELS;
		texture = UTextureCreator::CreateTexture();
	}
	delete implementation;
	UTextureCreator::CreateImageInfo();
	return texture;
}

/*	
*	It creates the color channels using the computed matrix and returns
*	the created texture.
*		matrix -> matrix of uint8 computed by the selected algorithm
*/
UTexture2D* UTextureCreator::CreateChannels(uint8* matrix)
{
	int i, j;
	int32 matrixSize = sizeX * sizeY;
	UTextureCreator::imageSize = matrixSize * CHANNELS;
	UTextureCreator::imageData = (uint8*)malloc(
		UTextureCreator::imageSize * sizeof(uint8));
	if (UTextureCreator::imageData != NULL)
	{
		for (i = 0; i < matrixSize; i++)
		{
			for (j = 0; j < CHANNELS; j++)
			{
				if (j == CHANNELS - 1)
				{
					UTextureCreator::imageData[i*CHANNELS + j] = ALPHA;
				}
				else
				{
					UTextureCreator::imageData[i*CHANNELS + j] = matrix[i];
				}
			}
		}
		if (sizeX < pow((double)2, (double)14) + 1)
		{
			return UTextureCreator::CreateTexture();
		}
	}
	return NULL;
}

/*	
*	It saves the matrix of bytes as texture to show in the widget
*/
UTexture2D* UTextureCreator::CreateTexture()
{
	void* textureData;
	UTexture2D* texture = UTexture2D::CreateTransient(UTextureCreator::sizeX,
		UTextureCreator::sizeY, UTextureCreator::pixelFormat);
	if (texture)
	{
		textureData = texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(textureData, UTextureCreator::imageData,
			UTextureCreator::imageSize);
		texture->PlatformData->Mips[0].BulkData.Unlock();
		texture->UpdateResource();
	}
	return texture;
}

/*	
*	It creates the info structure used to save the image
*/
void UTextureCreator::CreateImageInfo()
{
	ImageInfo info = ImageInfo();
	info.imageData = UTextureCreator::imageData;
	info.imageSize = UTextureCreator::imageSize;
	info.imageWidth = UTextureCreator::sizeX;
	info.imageHeight = UTextureCreator::sizeY;
	UTextureUtilities::SetImageInfo(info);
}
