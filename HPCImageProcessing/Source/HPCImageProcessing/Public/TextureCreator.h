// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SerialDiamondSquare.h"
#include "OpenMPDiamondSquare.h"
#include "CudaDiamondSquare.h"
#include "SerialMMorphology.h"
#include "OpenMPMMorphology.h"
#include "CudaMMorphology.h"
#include "TextureUtilities.h"
#include <Math.h>
#include "TextureCreator.generated.h"

//It is used to specify the algorithm to use to compute the texture
UENUM(BlueprintType)
enum ImplementationType
{
	IT_Serial UMETA(DisplayName = "Serial"),
	IT_OpenMP UMETA(DisplayName = "OpenMP"),
	IT_Cuda UMETA(DisplayName = "Cuda"),
};

/**
 * 
 */
UCLASS()
class HPCIMAGEPROCESSING_API UTextureCreator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "DiamondSquare")
		static UTexture2D* CreateProceduralTexture(ImplementationType implementationType,
			int size, int threadNumber, float &executionTime);
	UFUNCTION(BlueprintCallable, Category = "MathematicalMorphology")
		static UTexture2D* LoadImage();
	UFUNCTION(BlueprintCallable, Category = "MathematicalMorphology")
		static UTexture2D* ExecuteMMOperation(ImplementationType implementationType,
			int threadNumber, float &executionTime, bool isOpening, int structElemSize);
private:
	static UTexture2D* CreateChannels(uint8* matrix);
	static UTexture2D* CreateTexture();
	static void CreateImageInfo();

	static const EPixelFormat pixelFormat;
	static uint8* imageData;
	static int32 imageSize;
	static int sizeX;
	static int sizeY;
	static FImage* image;
};
