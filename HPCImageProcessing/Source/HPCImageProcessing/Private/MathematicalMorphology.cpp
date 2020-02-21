// Fill out your copyright notice in the Description page of Project Settings.


#include "MathematicalMorphology.h"

// File name of the structuring element
const FString MathematicalMorphology::fileName = 
FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) 
+ "InputImages/StructuringElement";
const FString MathematicalMorphology::extension = ".png";

/*
*	Mathematical morphology constructor.
*		It sets the image field and loads 
*		the structuring element
*/
MathematicalMorphology::MathematicalMorphology(FImage* image, int size)
{
	int elemSize;
	this->input = image;
	this->structElem = StructuringElement();
	if (image)
	{
		this->LoadStructuringElement(size);
		if (this->structElem.element)
		{
			elemSize = this->structElem.width*
				this->structElem.height;
			this->ErosionOffsets = Offset();
			this->DilationOffsets = Offset();
			this->ErosionOffsets.offsets = (int*)malloc(sizeof(int)*elemSize);
			this->DilationOffsets.offsets = (int*)malloc(sizeof(int)*elemSize);
			this->SetOffsets(&ErosionOffsets, false);
			this->SetOffsets(&DilationOffsets, true);
		}
	}
}

/*
*	Mathematical morphology destructor
*/
MathematicalMorphology::~MathematicalMorphology()
{
	this->input = NULL;
	this->structElem.element = NULL;
	free(ErosionOffsets.offsets);
	free(DilationOffsets.offsets);
}

/*	PRIVATE
*	It sets the offsets for the input image
*		offset: array of offsets that has to be set
*		reflect: true if the structuring element has to
*			be reflected (for dilation)
*/
void MathematicalMorphology::SetOffsets(Offset *offset, bool reflect)
{
	int halfWidth = (this->structElem.width - 1) / 2;
	int halfHeight = (this->structElem.height - 1) / 2;
	int imageWidth = this->input->SizeX + this->structElem.width - 1;
	if (offset->offsets)
	{
		for (int row = 0; row < this->structElem.height; row++)
		{
			for (int col = 0; col < this->structElem.width; col++)
			{
				if (this->structElem
					.element[row*this->structElem.width + col] == FOREGROUND)
				{
					int offsetRow = row, offsetCol = col;
					if (reflect)
					{
						offsetRow = this->structElem.width
							- 1 - row;
						offsetCol = this->structElem.height
							- 1 - col;
					}
					offset->offsets[offset->count] =
						(offsetRow - halfWidth)*imageWidth
						+ offsetCol - halfHeight;
					offset->count++;
				}
			}
		}
	}
}

/*	PRIVATE
*	It loads the structuring element
*/
void MathematicalMorphology::LoadStructuringElement(int size)
{
	FString file = MathematicalMorphology::fileName +
		FString::FromInt(size) + MathematicalMorphology::extension;
	FImage* elem = UTextureUtilities::LoadImageFromFile(file);
	this->structElem.element = NULL;
	if (elem)
	{
		this->structElem.width = elem->SizeX;
		this->structElem.height = elem->SizeY;
		this->structElem.element = 
			(uint8*)malloc(sizeof(uint8)*this->structElem.height
			*this->structElem.width);
		FColor* colors = elem->AsBGRA8();
		if (this->structElem.element)
		{
			for (int i = 0; i < this->structElem.height; i++)
			{
				for (int j = 0; j < this->structElem.width; j++)
				{
					this->structElem.element
						[i*this->structElem.width + j] =
						colors[i*this->structElem.width + j].R;
				}
			}
		}
	}
}