// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/ImageCore/Public/ImageCore.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "TextureUtilities.generated.h"

#define ALPHA 255
#define CHANNELS 4

/* This structure is used to save the image */
struct ImageInfo
{
	uint8* imageData;
	int64 imageSize;
	int imageWidth;
	int imageHeight;
};

/* Enum for the kind of image we want to save */
UENUM(BlueprintType)
enum AlgorithmType
{
	AT_DiamondSquare UMETA(DisplayName="DiamondSquare"),
	AT_Opening UMETA(DisplayName="Opening"),
	AT_Closing UMETA(DisplayName="Closing")
};

/**
 * 
 */
UCLASS()
class HPCIMAGEPROCESSING_API UTextureUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "TextureUtilities")
		static void SaveToPNG(AlgorithmType algorithm);
	static void SetImageInfo(ImageInfo image);
	static TArray<FString> OpenFileDialog();
	static FImage* LoadImageFromFile(FString file);
private:
	// Fields used to save the new image
	static ImageInfo info;
	static const FString filePath;
	static const FString extensionFile;
	//Fields used to load or save an image
	static const EImageFormat imageFormat;
	static const ERGBFormat RGBFormat;
	static const int bitDepth;
	//Fields used to browse folders
	static const FString title;
	static const FString defaultFolder;
	static const FString filter;
};
