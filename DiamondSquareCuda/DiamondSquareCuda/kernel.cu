#include "DiamondSquareCuda.h"
#include <curand.h>
#include <math.h>

#define SIZE 512

/*
*	Kernel that executes the diamond step
*		matrix: the matrix that has to be computed
*		random: random values
*		currentSize: the current size of the matrix rows/columns in which 
*			apply the diamond step
*		matrixSize: the length of matrix rows/columns
*		randValue: random seed
*/
__global__ void DiamondStep(uint8_t* matrix, unsigned *random, 
	int currentSize, int matrixSize, int randValue)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	int half = currentSize / 2;
	int minRand = -randValue;
	int row = y * currentSize + half;
	int col = x * currentSize + half;
	int value;
	value = (matrix[(row - half)*matrixSize + (col - half)] +
		matrix[(row - half)*matrixSize + (col + half)] +
		matrix[(row + half)*matrixSize + (col - half)] +
		matrix[(row + half)*matrixSize + (col + half)] +
		//VERSION 1
		//(random[x*gridDim.x+y] % (randValue - minRand) + minRand)) / 4;
		//VERSION 2
		(random[row*matrixSize + col] % (randValue - minRand) + minRand)) / 4;
	matrix[row*matrixSize + col] = value;
}

/*
*	Kernel that executes the square step
*		matrix: the matrix that has to be computed
*		random: random values
*		currentSize: the current size of the matrix rows/columns in which 
*			apply the diamond step
*		matrixSize: the length of matrix rows/columns
*		maxRowThread: the length of a row, using the x index
*		maxColThread: the length of a column, using the y index
*		randValue: random seed
*/
__global__ void SquareStep(uint8_t* matrix, unsigned* random, int currentSize, 
	int matrixSize, int maxRowThread, int maxColThread, int randValue)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	/*x/y can be greater than maxRowThread/maxColThread because the number
	of created threads might not be multiple of the number of threads in a block*/
	if (x < maxRowThread && y < maxColThread)
	{
		int half = currentSize / 2;
		int minRand = -randValue;
		int value = 0;
		int div = 0;
		int cond;
		int elemX = x * currentSize*(y % 2 == 0) +
			y * half*(y % 2 != 0);
		int elemY = (y*half + half)*(y % 2 == 0) +
			x * currentSize*(y % 2 != 0);
		// CUDA VERSION 2: it uses conditions as variables 
		// to avoid divergent branches
		cond = elemX != 0;
		value += matrix[(elemX - half * cond) *
			matrixSize + elemY] * cond;
		div += cond;
		cond = elemX != matrixSize - 1;
		value += matrix[(elemX + half * cond) *
			matrixSize + elemY] * cond;
		div += cond;
		cond = elemY != 0;
		value += matrix[elemX * matrixSize + 
			elemY - half * cond] * cond;
		div += cond;
		cond = elemY != matrixSize - 1;
		value += matrix[elemX*matrixSize + elemY 
			+ half * cond] * cond;
		div += cond;
		/*
		// CUDA VERSION 1: it uses divergent branches
		if (elemX != 0)
		{
			value += matrix[(elemX - half)*matrixSize + elemY];
			div++;
		}
		if (elemX != matrixSize-1)
		{
			value += matrix[(elemX + half)*matrixSize + elemY];
			div++;
		}
		if (elemY != 0)
		{
			value += matrix[elemX*matrixSize + elemY - half];
			div++;
		}
		if (elemY != matrixSize-1)
		{
			value += matrix[elemX*matrixSize + elemY + half];
			div++;
		}*/
		//VERSION 1: random index is correct for the 
		//first version of random generation but not for the second one
		//value += (minRand + random[x*gridDim.x+y] % (randValue - minRand));
		//VERSION 2
		value += (minRand + random[elemX*matrixSize+elemY] % (randValue - minRand));
		matrix[elemX*matrixSize + elemY] = value / div;
	}
}

/*
*	Function that will be called by the Unreal Engine classes to execute
*	the diamond-square algorithm with CUDA
*		matrix: the matrix that has to be computed
*		matrixSize: the length of rows/columns of the matrix that has to be computed
*		randomValue: the random seed at the beginning of the execution
*/
void CudaAlgorithm::CudaDiamondSquare(uint8_t* matrix, 
	int matrixSize, int randomValue)
{
	uint8_t *d_matrix;
	unsigned *d_random;
	int size = matrixSize * matrixSize * sizeof(uint8_t);
	int last = matrixSize - 1;
	int half; 
	int count = 0;
	int diamondSize = CudaAlgorithm::PowerInt(2, count);
	int squareRowSize = 0;
	int squareColSize = 0;
	int randValue = randomValue;
	curandGenerator_t generator;
	//Allocation of device pointer
	cudaMalloc((void**)&d_matrix, size);
	//Copy of the values from host pointer to the device
	cudaMemcpy(d_matrix, matrix, size, cudaMemcpyHostToDevice);
	//VERSION 2
	curandCreateGenerator(&generator, CURAND_RNG_PSEUDO_DEFAULT);
	curandSetGeneratorOrdering(generator, CURAND_ORDERING_PSEUDO_SEEDED);
	cudaMalloc((void**)&d_random, matrixSize*matrixSize);
	curandGenerate(generator, d_random, matrixSize*matrixSize);

	while (last > 1)
	{
		int d_length = CudaAlgorithm::PowerInt(2, count);
		int S_row, S_column;
		half = last / 2;
		randValue = randValue / 2 > 1 ? randValue / 2 : 1;
		//It checks if the number of threads is less than the maximum
		if (pow(d_length, 2) < SIZE)
		{
			diamondSize = d_length;
		}
		if (last != matrixSize - 1)
		{
			// VERSION 1
			//curandDestroyGenerator(generator);
			cudaFree(d_random);
		}
		//cudaMalloc((void**)&d_random, sizeof(unsigned)*d_length*d_length);
		dim3 D_block(diamondSize, diamondSize);
		dim3 D_grid(d_length / diamondSize, d_length / diamondSize);
		//VERSION 1
		//Creation of the generator of random numbers
		//curandCreateGenerator(&generator, CURAND_RNG_PSEUDO_DEFAULT);
		//It creates random numbers
		//curandGenerate(generator, d_random, d_length*d_length);
		DiamondStep <<<D_grid, D_block>>> (d_matrix, d_random, last,
			matrixSize, randValue);
		count++;
		S_row = CudaAlgorithm::PowerInt(2, count-1)+1;
		S_column = CudaAlgorithm::PowerInt(2, count);
		if (S_row*S_column < SIZE)
		{
			squareRowSize = S_row;
			squareColSize = S_column;
		}
		//VERSION 1
		//cudaFree(d_random);
		//curandDestroyGenerator(generator);
		cudaMalloc((void**)&d_random, sizeof(int)*S_row*S_column);
		dim3 S_block(squareRowSize, squareColSize);
		dim3 S_grid((S_row + squareRowSize - 1) / squareRowSize, 
			S_column / squareColSize);
		//VERSION 1: It creates other random numbers for square step
		//curandCreateGenerator(&generator, CURAND_RNG_PSEUDO_DEFAULT);
		//curandGenerate(generator, (unsigned*)d_random, S_row*S_column);
		SquareStep <<<S_grid, S_block>>> (d_matrix, d_random, last, matrixSize,
			S_row, S_column, randValue);
		last = half;
	}
	//Copy of the values from device pointes to host
	cudaMemcpy(matrix, d_matrix, size, cudaMemcpyDeviceToHost);
	//It frees the allocated device memory
	cudaFree(d_matrix);
	curandDestroyGenerator(generator);
	cudaFree(d_random);
}

/*
*	Private function used to calculate the power of integer values
*	because the pow function uses double values and creates warnings
*	when we try to cast them to integer.
*		base: the base of the power
*		exp: the exponent
*/
int CudaAlgorithm::PowerInt(int base, int exp)
{
	int power = 1;
	for (int i = 1; i <= exp; i++)
	{
		power *= base;
	}
	return power;
}
