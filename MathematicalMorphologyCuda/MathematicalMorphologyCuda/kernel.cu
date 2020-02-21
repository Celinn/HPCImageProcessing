
#include "MathematicalMorphologyCuda.h"
#define BLOCK_2D 22
#define WHITE 255
#define BLACK 0

/*
*	Kernel used to split image channels
*		image: input image that has to be split
*		red: matrix of pixels for red channel
*		green: matrix of pixels for green channel
*		blue: matrix of pixels for blue channel
*		width: image width
*		height: image height
*		structWidth: width of structuring element
*		structHeight: height of structuring element
*		ghost: value for ghost cells
*/
__global__ void SplitChannels(uint8_t* image, uint8_t* red,
	uint8_t* green, uint8_t* blue, int width, int height, 
	int structWidth, int structHeight, int ghost)
{
	int j = 0;
	int x = blockIdx.x*blockDim.x + threadIdx.x;
	int y = blockIdx.y*blockDim.y + threadIdx.y;
	int firstX = (structWidth - 1) / 2;
	int firstY = (structHeight - 1) / 2;
	int lastX = width + firstX;
	int lastY = height + firstY;
	int sizeX = width + structWidth - 1;
	int sizeY = height + structHeight - 1;
	int img = x >= firstX && x < lastX &&
		y >= firstY && y < lastY;
	int halo = !img;
	int index = y * sizeX + x;
	int i = (y - firstY)*width + x - firstX;
	if (x < sizeX && y < sizeY)
	{
		blue[index] = image[i*CHANNELS + j] * img + ghost * halo;
		j++;
		green[index] = image[i*CHANNELS + j] * img + ghost * halo;
		j++;
		red[index] = image[i*CHANNELS + j] * img + ghost * halo;
	}
}

/*
*	Kernel that sets the default value for output channels
*		red: red channel
*		green: green channel
*		blue: blue channel
*		width: total channel width, considering also ghost cells
*		height: total channel height, considering also ghost cells
*		value: default value
*/
__global__ void setDefault(uint8_t* red, uint8_t* green, 
	uint8_t* blue, int width, int height, uint8_t value)
{
	int x = blockIdx.x*blockDim.x + threadIdx.x;
	int y = blockIdx.y*blockDim.y + threadIdx.y;
	int index = y * width + x;
	if (x < width && y < height)
	{
		red[index] = value;
		green[index] = value;
		blue[index] = value;
	}
}

/*
*	Kernel that composes an image from its channels
*		image: output image
*		red: red channel
*		green: green channel
*		blue: blue channel
*		structWidth: width of structuring element
*		structHeight: height of structuring element
*		width: image width
*		height: image height
*/
__global__ void ComposeImage(uint8_t* image, uint8_t* red,
	uint8_t* green, uint8_t* blue, int structWidth, 
	int structHeight, int width, int height)
{
	int j = 0;
	int x = blockIdx.x*blockDim.x + threadIdx.x;
	int y = blockIdx.y*blockDim.y + threadIdx.y;
	int firstX = (structWidth - 1) / 2;
	int firstY = (structHeight - 1) / 2;
	int lastX = width + firstX;
	int lastY = height + firstY;
	int sizeW = width + structWidth - 1;
	int index = y * sizeW + x;
	int i = (y - firstY)*width + x - firstX;
	if (x >= firstX && x < lastX &&
		y >= firstY && y < lastY)
	{
		image[i*CHANNELS + j] = blue[index];
		j++;
		image[i*CHANNELS + j] = green[index];
		j++;
		image[i*CHANNELS + j] = red[index];
		j++;
		image[i*CHANNELS + j] = ALPHA;
	}
}

/*
*	Kernel that executes erosion operation
*		input: input channel
*		output: output channel
*		width: image width
*		height: image height
*		structWidth: width of structuring element
*		structHeight: height of structuring element
*		offset: array of offsets
*		offCount: number of element inside offset
*/
__global__ void Erosion(uint8_t* input, uint8_t* output,
	int width, int height, int structWidth, int structHeight,
	int* offset, int offCount)
{
	int x = blockIdx.x*blockDim.x + threadIdx.x;
	int y = blockIdx.y*blockDim.y + threadIdx.y;
	int firstX = (structWidth - 1) / 2;
	int firstY = (structHeight - 1) / 2;
	int lastX = width + firstX;
	int lastY = height + firstY;
	int sizeW = width + structWidth - 1;
	int index = y * sizeW + x;
	uint8_t minValue = WHITE;
	if (x >= firstX && x < lastX &&
		y >= firstY && y < lastY)
	{
		for (int i = 0; i < offCount; i++)
		{
			if (input[index + offset[i]] < minValue)
			{
				minValue = input[index + offset[i]];
			}
		}
		output[index] = minValue;
	}
}

/*
*	Kernel that executes dilation operation
*		input: input channel
*		output: output channel
*		width: image width
*		height: image height
*		structWidth: width of structuring element
*		structHeight: height of structuring element
*		offset: array of offsets
*		offCount: number of element inside offset
*/
__global__ void Dilation(uint8_t* input, uint8_t* output,
	int width, int height, int structWidth, int structHeight,
	int* offset, int offCount)
{
	int x = blockIdx.x*blockDim.x + threadIdx.x;
	int y = blockIdx.y*blockDim.y + threadIdx.y;
	int firstX = (structWidth - 1) / 2;
	int firstY = (structHeight - 1) / 2;
	int lastX = width + firstX;
	int lastY = height + firstY;
	int sizeW = width + structWidth - 1;
	int index = y * sizeW + x;
	uint8_t maxValue = BLACK;
	if (x >= firstX && x < lastX &&
		y >= firstY && y < lastY)
	{
		for (int i = 0; i < offCount; i++)
		{
			if (input[index + offset[i]] > maxValue)
			{
				maxValue = input[index + offset[i]];
			}
		}
		output[index] = maxValue;
	}
}

/*
*	Function that will be called by the Unreal Engine classes to 
*	execute opening or closing operations with CUDA
*		structWidth: width of structuring element
*		structHeight: height of structuring element
*		image: input image
*		width: image width
*		height: image height
*		erOffset: array of offsets for erosion operations
*		erCount: number of elements in erOffset
*		dilOffset: array of offsets for dilation operations
*		dilCount: number of elements in dilOffset
*		isOpening: true if we have to execute opening operations,
*			false otherwise
*/
uint8_t* CudaMathMorphology::ExecuteOpeningOrClosing(int structWidth,
	int structHeight, uint8_t* image, int width,
	int height, int* erOffset, int erCount,
	int* dilOffset, int dilCount, bool isOpening)
{
	int32_t imageSize = width * height * CHANNELS;
	int32_t size = imageSize * sizeof(uint8_t);
	int sizeX = width + structWidth - 1;
	int sizeY = height + structHeight - 1;
	int32_t channelSize = sizeX * sizeY * sizeof(uint8_t);
	int32_t erSize = erCount * sizeof(int);
	int32_t dilSize = dilCount * sizeof(int);
	uint8_t *d_Image, *d_output;
	uint8_t* output = (uint8_t*)malloc(size);
	//image channels
	uint8_t *d_red, *d_green, *d_blue;
	uint8_t *d_outRed, *d_outGreen, *d_outBlue;
	//structuring element offsets
	int *d_erosion, *d_dilation;
	dim3 grid_2D((sizeX + BLOCK_2D - 1) / BLOCK_2D, 
		(sizeY + BLOCK_2D - 1) / BLOCK_2D);
	dim3 block_2D(BLOCK_2D, BLOCK_2D);

	if (!output)
	{
		return NULL;
	}
	//image allocation
	cudaMalloc((void**)&d_Image, size);
	cudaMalloc((void**)&d_output, size);
	cudaMemcpy(d_Image, image, size, cudaMemcpyHostToDevice);
	//image channels allocation
	cudaMalloc((void**)&d_red, channelSize);
	cudaMalloc((void**)&d_green, channelSize);
	cudaMalloc((void**)&d_blue, channelSize);
	cudaMalloc((void**)&d_outRed, channelSize);
	cudaMalloc((void**)&d_outGreen, channelSize);
	cudaMalloc((void**)&d_outBlue, channelSize);
	//offset allocation
	cudaMalloc((void**)&d_erosion, erSize);
	cudaMemcpy(d_erosion, erOffset, erSize, cudaMemcpyHostToDevice);
	cudaMalloc((void**)&d_dilation, dilSize);
	cudaMemcpy(d_dilation, dilOffset, dilSize, cudaMemcpyHostToDevice);
	
	if (isOpening)
	{
		//split channels
		SplitChannels<<<grid_2D, block_2D>>>(d_Image, d_red, d_green, 
			d_blue, width, height, structWidth, structHeight, WHITE);
		//it sets default values for channels and output channels
		setDefault << <grid_2D, block_2D >> > (d_outRed, d_outGreen,
			d_outBlue, sizeX, sizeY, BLACK);
		//red channel
		Erosion <<<grid_2D, block_2D>>> (d_red,
			d_outRed, width, height, structWidth,
			structHeight, d_erosion, erCount);
		Dilation<<<grid_2D, block_2D>>>(d_outRed,
			d_red, width, height, structWidth,
			structHeight, d_dilation, dilCount);
		//green channel
		Erosion <<<grid_2D, block_2D>>> (d_green,
			d_outGreen, width, height, structWidth, 
			structHeight, d_erosion, erCount);
		Dilation<<<grid_2D, block_2D>>>(d_outGreen,
			d_green, width, height, structWidth, 
			structHeight, d_dilation, dilCount);
		//blue channel
		Erosion <<<grid_2D, block_2D>>> (d_blue,
			d_outBlue, width, height, structWidth, 
			structHeight, d_erosion, erCount);
		Dilation<<<grid_2D, block_2D>>>(d_outBlue, 
			d_blue, width, height, structWidth, 
			structHeight, d_dilation, dilCount);
	}
	else
	{
		//split channels
		SplitChannels <<<grid_2D, block_2D >>> (d_Image, d_red, d_green, 
			d_blue, width, height, structWidth, structHeight, BLACK);
		//it sets default values for channels and output channels
		setDefault << <grid_2D, block_2D >> > (d_outRed, d_outGreen,
			d_outBlue, sizeX, sizeY, WHITE);
		//red channel
		Dilation <<<grid_2D, block_2D >>> (d_red,
			d_outRed, width, height, structWidth,
			structHeight, d_dilation, dilCount);
		Erosion <<<grid_2D, block_2D >>> (d_outRed,
			d_red, width, height, structWidth,
			structHeight, d_erosion, erCount);
		//green channel
		Dilation <<<grid_2D, block_2D >>> (d_green,
			d_outGreen, width, height, structWidth, 
			structHeight, d_dilation, dilCount);
		Erosion <<<grid_2D, block_2D >>> (d_outGreen,
			d_green, width, height, structWidth, 
			structHeight, d_erosion, erCount);
		//blue channel
		Dilation <<<grid_2D, block_2D >>> (d_blue,
			d_outBlue, width, height, structWidth,
			structHeight, d_dilation, dilCount);
		Erosion <<<grid_2D, block_2D >>> (d_outBlue,
			d_blue, width, height, structWidth, 
			structHeight, d_erosion, erCount);
	}
	//compose channels
	ComposeImage<<<grid_2D, block_2D>>>(d_output, 
		d_red, d_green, d_blue, structWidth,
		structHeight, width, height);
	cudaMemcpy(output, d_output, size, cudaMemcpyDeviceToHost);
	cudaFree(d_Image);
	cudaFree(d_red);
	cudaFree(d_green);
	cudaFree(d_blue);
	cudaFree(d_outRed);
	cudaFree(d_outGreen);
	cudaFree(d_outBlue);
	cudaFree(d_erosion);
	cudaFree(d_dilation);
	return output;
}



