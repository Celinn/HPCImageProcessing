// Fill out your copyright notice in the Description page of Project Settings.


#include "SerialMMorphology.h"

/*	
*	It executes opening or closing
*		isOpening: true if it has to execute opening
*/
uint8* SerialMMorphology::ExecuteOpeningOrClosing(bool isOpening)
{
	int32 size, width, height;
	uint8 *redChannel, *greenChannel, *blueChannel;
	uint8 *outRed, *outGreen, *outBlue;
	if (!this->input || !this->structElem.element)
	{
		return NULL;
	}
	width = this->input->SizeX + structElem.width-1;
	height = this->input->SizeY + structElem.height-1;
	size = width * height;
	redChannel = (uint8*)malloc(sizeof(uint8)*size);
	greenChannel = (uint8*)malloc(sizeof(uint8)*size);
	blueChannel = (uint8*)malloc(sizeof(uint8)*size);
	outRed = (uint8*)malloc(sizeof(uint8)*size);
	outGreen = (uint8*)malloc(sizeof(uint8)*size);
	outBlue = (uint8*)malloc(sizeof(uint8)*size);
	if (!redChannel || !greenChannel || !blueChannel
		|| !outRed || !outGreen || !outBlue)
	{
		return NULL;
	}
	if (isOpening)
	{
		this->SplitChannels(redChannel, greenChannel, 
			blueChannel, WHITE);
		this->FillGhostCells(outRed, outGreen, outBlue, BLACK);
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
		this->SplitChannels(redChannel, greenChannel,
			blueChannel, BLACK);
		this->FillGhostCells(outRed, outGreen, outBlue, WHITE);
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
	free(outRed);
	free(outGreen);
	free(outBlue);
	return this->ComposeImage(redChannel, greenChannel, blueChannel);
}

/*
*	It split image channels
*		redChannel: red channel
*		greenChannel: green channel
*		blueChannel: blue channel
*		ghost: value for ghost cells
*/
void SerialMMorphology::SplitChannels(uint8* redChannel, 
	uint8* greenChannel, uint8* blueChannel, uint8 ghost)
{
	FColor* colors = this->input->AsBGRA8();
	int firstRow = (this->structElem.height - 1) / 2;
	int firstCol = (this->structElem.width - 1) / 2;
	int lastRow = this->input->SizeY + firstRow;
	int lastCol = this->input->SizeX + firstCol;
	int width = this->input->SizeX + this->structElem.width-1;
	int height = this->input->SizeY + this->structElem.height - 1;
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
void SerialMMorphology::FillGhostCells(uint8* red, uint8* green, 
	uint8* blue, uint8 value)
{
	int width = this->input->SizeX + this->structElem.width-1;
	int height = this->input->SizeY + this->structElem.height-1;
	int halfWidth = (this->structElem.width - 1) / 2;
	int halfHeight = (this->structElem.height - 1) / 2;
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
*/
uint8* SerialMMorphology::ComposeImage(uint8* redChannel,
	uint8* greenChannel, uint8* blueChannel)
{
	int32 size = this->input->SizeX
		*this->input->SizeY;
	int32 datasize = size * CHANNELS;
	uint8* output = (uint8*)malloc(sizeof(uint8)*datasize);
	int firstRow = (this->structElem.height - 1) / 2;
	int firstCol = (this->structElem.width - 1) / 2;
	int lastRow = this->input->SizeY + firstRow;
	int lastCol = this->input->SizeX + firstCol;
	int width = this->input->SizeX + this->structElem.width-1;
	int j = 0;
	if(!output)
	{
		return NULL;
	}
	for (int i = firstRow; i < lastRow; i++)
	{
		for (int k = firstCol; k < lastCol; k++)
		{
			//blue channel
			output[j] = blueChannel[i*width + k];
			j++;
			//green channel
			output[j] = greenChannel[i*width + k];
			j++;
			//red channel
			output[j] = redChannel[i*width + k];
			j++;
			//alpha channel
			output[j] = ALPHA;
			j++;
		}
	}
	return output;
}

/*
*	It executes the erosion operation
*		in: input channel
*		out: output channel
*/
void SerialMMorphology::ExecuteErosion(
	uint8* in, uint8* out)
{
	int width = this->input->SizeX + this->structElem.width-1;
	int firstRow = (this->structElem.height - 1) / 2;
	int firstCol = (this->structElem.width - 1) / 2;
	int rowSize = this->input->SizeY + firstRow;
	int colSize = this->input->SizeX + firstCol;
	for (int row = firstRow; row < rowSize; row++)
	{
		for (int col = firstCol; col < colSize; col++)
		{
			uint8 minValue = 255;
			for (int i = 0; i < this->ErosionOffsets.count; i++)
			{
				if (in[row*width + col + 
					this->ErosionOffsets.offsets[i]] < minValue)
				{
					minValue = in[row*width + col 
						+ this->ErosionOffsets.offsets[i]];
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
void SerialMMorphology::ExecuteDilation(
	uint8* in, uint8* out)
{
	int width = this->input->SizeX + this->structElem.width-1;
	int firstRow = (this->structElem.height - 1) / 2;
	int firstCol = (this->structElem.width - 1) / 2;
	int rowSize = this->input->SizeY + this->structElem.height / 2;
	int colSize = this->input->SizeX + this->structElem.width / 2;
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