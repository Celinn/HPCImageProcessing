// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenMPMMorphology.h"

/*
*	OpenMPMMorphology constructor.
*	It sets the number of threads to use.
*		image: input image
*		size: the length of the structuring element row/column
*		threadNum: the number of thread to use
*/
OpenMPMMorphology::OpenMPMMorphology(FImage* image, int size,
	int threadNum) : MathematicalMorphology(image, size)
{
	omp_set_num_threads(threadNum);
}

/*
*	It executes opening or closing
*		isOpening: true if it has to execute opening
*/
uint8* OpenMPMMorphology::ExecuteOpeningOrClosing(bool isOpening)
{ 
	int32 dataSize, width, height;
	uint8 *redChannel, *greenChannel, *blueChannel;
	uint8 *outRed, *outGreen, *outBlue, *output;
	if (!this->input || !this->structElem.element)
	{
		return NULL;
	}
	dataSize = this->input->SizeX*this->input->SizeY*CHANNELS;
	width = this->input->SizeX + structElem.width - 1;
	height = this->input->SizeY + structElem.height - 1;
	output = (uint8*)malloc(sizeof(uint8)*dataSize);
	redChannel = (uint8*)malloc(sizeof(uint8)*width*height);
	greenChannel = (uint8*)malloc(sizeof(uint8)*width*height);
	blueChannel = (uint8*)malloc(sizeof(uint8)*width*height);
	outRed = (uint8*)malloc(sizeof(uint8)*width*height);
	outGreen = (uint8*)malloc(sizeof(uint8)*width*height);
	outBlue = (uint8*)malloc(sizeof(uint8)*width*height);
	if (!redChannel || !greenChannel || !blueChannel
		|| !outRed || !outGreen || !outBlue || !output)
	{
		return NULL;
	}
#pragma omp parallel
	{
		if (isOpening)
		{
			//VERSION 1: parallelized using sections
/*#pragma omp sections
			{
#pragma omp section
				{
					this->SplitChannels(redChannel, greenChannel,
						blueChannel, WHITE);
				}
#pragma omp section
				{
					this->FillGhostCells(outRed, outGreen, outBlue, BLACK);
				}
			}*/
			//VERSION 2: Parallelized using omp for
			this->SplitChannels(redChannel, greenChannel, 
				blueChannel, WHITE);
			this->FillGhostCells(outRed, outGreen, outBlue, BLACK);
			//VERSION 1: parallelized using sections
/*#pragma omp sections
			{
#pragma omp section
				{
					//Opening on red channel
					this->ExecuteErosion(redChannel, outRed);
					this->ExecuteDilation(outRed, redChannel);
				}
#pragma omp section
				{
					//Opening on green channel
					this->ExecuteErosion(greenChannel, outGreen);
					this->ExecuteDilation(outGreen, greenChannel);
				}
#pragma omp section
				{
					//Opening on blue channel
					this->ExecuteErosion(blueChannel, outBlue);
					this->ExecuteDilation(outBlue, blueChannel);
				}
			}*/
			//VERSION 2: parallelized using omp for
			//Opening on red channel
			this->ExecuteErosion(redChannel, outRed);
			this->ExecuteDilation(outRed, redChannel);
			//Opening on green channel
			this->ExecuteErosion(greenChannel, outGreen);
			this->ExecuteDilation(outGreen, greenChannel);
			//Opening on blue channel
			this->ExecuteErosion(blueChannel, outBlue);
			this->ExecuteDilation(outBlue, blueChannel);
		}
		else
		{
			//VERSION 1: parallelized using sections
/*#pragma omp sections
			{
#pragma omp section
				{
					this->SplitChannels(redChannel, greenChannel,
						blueChannel, BLACK);
				}
#pragma omp section
				{
					this->FillGhostCells(outRed, outGreen, outBlue, WHITE);
				}
			}*/
			//VERSION 2: parallelized using omp for
			this->SplitChannels(redChannel, greenChannel,
				blueChannel, BLACK);
			this->FillGhostCells(outRed, outGreen, outBlue, WHITE);
			//VERSION 1: parallelized using sections
/*#pragma omp sections
			{
#pragma omp section
				{
					//Closing on red channel
					this->ExecuteDilation(redChannel, outRed);
					this->ExecuteErosion(outRed, redChannel);
				}
#pragma omp section
				{
					//Closing on green channel
					this->ExecuteDilation(greenChannel, outGreen);
					this->ExecuteErosion(outGreen, greenChannel);
				}
#pragma omp section
				{
					//Closing on blue channel
					this->ExecuteDilation(blueChannel, outBlue);
					this->ExecuteErosion(outBlue, blueChannel);
				}
			}*/
			//VERSION 2: parallelized using omp for
			//Closing on red channel
			this->ExecuteDilation(redChannel, outRed);
			this->ExecuteErosion(outRed, redChannel);
			//Closing on green channel
			this->ExecuteDilation(greenChannel, outGreen);
			this->ExecuteErosion(outGreen, greenChannel);
			//Closing on blue channel
			this->ExecuteDilation(blueChannel, outBlue);
			this->ExecuteErosion(outBlue, blueChannel);
		}
		this->ComposeImage(redChannel, greenChannel, blueChannel, output);
	}
	free(outRed);
	free(outGreen);
	free(outBlue);
	return output;
}

/*
*	It split image channels
*		redChannel: red channel
*		greenChannel: green channel
*		blueChannel: blue channel
*		ghost: value for ghost cells
*/
void OpenMPMMorphology::SplitChannels(uint8* redChannel,
	uint8* greenChannel, uint8* blueChannel, uint8 ghost)
{
	FColor* colors = this->input->AsBGRA8();
	int firstRow = (this->structElem.height - 1) / 2;
	int firstCol = (this->structElem.width - 1) / 2;
	int lastRow = this->input->SizeY + firstRow;
	int lastCol = this->input->SizeX + firstCol;
	int width = this->input->SizeX + this->structElem.width - 1;
	int height = this->input->SizeY + this->structElem.height - 1;
#pragma omp for // only for VERSION 2
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i < firstRow || i >= lastRow
				|| j < firstCol || j >= lastCol)
			{
				redChannel[i*width + j] = ghost;
				greenChannel[i*width + j] = ghost;
				blueChannel[i*width + j] = ghost;
			}
			else
			{
				redChannel[i*width + j] =
					colors[(i - firstRow)*this->input->SizeX
					+ j - firstCol].R;
				greenChannel[i*width + j] =
					colors[(i - firstRow)*this->input->SizeX
					+ j - firstCol].G;
				blueChannel[i*width + j] =
					colors[(i - firstRow)*this->input->SizeX
					+ j - firstCol].B;
			}
		}
	}
}

/*
*	It fills the uninitialized ghost cells
*		red: red channel
*		green: green channel
*		blue: blue channel
*		value: value for ghost cells
*/
void OpenMPMMorphology::FillGhostCells(uint8* red, uint8* green,
	uint8* blue, uint8 value)
{
	int width = this->input->SizeX + this->structElem.width - 1;
	int height = this->input->SizeY + this->structElem.height - 1;
	int halfWidth = (this->structElem.width - 1) / 2;
	int halfHeight = (this->structElem.height - 1) / 2;
#pragma omp for // only for VERSION 2
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i < halfHeight || i >= height - halfHeight
				|| j < halfWidth || j >= width - halfWidth)
			{
				red[i*width + j] = value;
				green[i*width + j] = value;
				blue[i*width + j] = value;
			}
		}
	}
}

/*
*	It composes the image using channels
*		redChannel: red channel
*		greenChannel: green channel
*		blueChannel: blue channel
*		output: output image
*/
void OpenMPMMorphology::ComposeImage(uint8* redChannel,
	uint8* greenChannel, uint8* blueChannel, uint8* output)
{
	int32 size = this->input->SizeX
		*this->input->SizeY;
	int firstRow = (this->structElem.height - 1) / 2;
	int firstCol = (this->structElem.width - 1) / 2;
	int lastRow = this->input->SizeY + firstRow;
	int lastCol = this->input->SizeX + firstCol;
	int width = this->input->SizeX + this->structElem.width - 1;
#pragma omp for
	for (int i = firstRow; i < lastRow; i++)
	{
		for (int k = firstCol; k < lastCol; k++)
		{
			int index = (i - firstRow)*this->input->SizeX + k - firstCol;
			int j = 0;
			//blue channel
			output[index*CHANNELS + j] = blueChannel[i*width + k];
			j++;
			//green channel
			output[index*CHANNELS + j] = greenChannel[i*width + k];
			j++;
			//red channel
			output[index*CHANNELS + j] = redChannel[i*width + k];
			j++;
			//alpha channel
			output[index*CHANNELS + j] = ALPHA;
			j++;
		}
	}
}

/*
*	It executes the erosion operation
*		in: input channel
*		out: output channel
*/
void OpenMPMMorphology::ExecuteErosion(uint8* in, uint8* out)
{
	int width = this->input->SizeX + this->structElem.width - 1;
	int firstRow = (this->structElem.height - 1) / 2;
	int firstCol = (this->structElem.width - 1) / 2;
	int rowSize = this->input->SizeY + firstRow;
	int colSize = this->input->SizeX + firstCol;
#pragma omp for // only for version 2
	for (int row = firstRow; row < rowSize; row++)
	{
		for (int col = firstCol; col < colSize; col++)
		{
			uint8 minValue = 255;
			for (int i = 0; i < this->ErosionOffsets.count; i++)
			{
				if (in[row*width + col
					+ this->ErosionOffsets.offsets[i]] < minValue)
				{
					minValue = in[row*width + col + 
						this->ErosionOffsets.offsets[i]];
				}
			}
			out[row*width + col] = minValue;
		}
	}
}

/*
*	It executes the dilation operation
*		in: input channel
*		out: output channel
*/
void OpenMPMMorphology::ExecuteDilation(uint8* in, uint8* out)
{
	int width = this->input->SizeX + this->structElem.width - 1;
	int firstRow = (this->structElem.height - 1) / 2;
	int firstCol = (this->structElem.width - 1) / 2;
	int rowSize = this->input->SizeY + this->structElem.height / 2;
	int colSize = this->input->SizeX + this->structElem.width / 2;
#pragma omp for // only for VERSION 2
	for (int row = firstRow; row < rowSize; row++)
	{
		for (int col = firstCol; col < colSize; col++)
		{
			uint8 maxValue = 0;
			for (int i = 0; i < this->DilationOffsets.count; i++)
			{
				if (in[row*width + col
					+ this->DilationOffsets.offsets[i]] > maxValue)
				{
					maxValue = in[row*width + col
						+ this->DilationOffsets.offsets[i]];
				}
			}
			out[row*width + col] = maxValue;
		}
	}
}
