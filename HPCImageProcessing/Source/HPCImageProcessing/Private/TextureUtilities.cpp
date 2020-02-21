// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureUtilities.h"

ImageInfo UTextureUtilities::info = ImageInfo();
//File path in which we want to save the image
FString const UTextureUtilities::filePath =
FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "OutputImages/";
//Extension of the image we want to save
FString const UTextureUtilities::extensionFile = ".png";

EImageFormat const UTextureUtilities::imageFormat = EImageFormat::PNG;
ERGBFormat const UTextureUtilities::RGBFormat = ERGBFormat::RGBA;
int const UTextureUtilities::bitDepth = 8;

FString const UTextureUtilities::title = "Scegli immagine";
FString const UTextureUtilities::defaultFolder =
FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "InputImages/";
FString const UTextureUtilities::filter = "Image files (*.png) | *.png";

/*	
*	It saves the matrix of bytes as PNG image
*/
void UTextureUtilities::SaveToPNG(AlgorithmType algorithm)
{
	int counter = 0;
	FString algType = "";
	FString fileName;
	IPlatformFile &platform = FPlatformFileManager::Get().GetPlatformFile();
	IImageWrapperModule &w_module =
		FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> wrapper = w_module.CreateImageWrapper(UTextureUtilities::imageFormat);
	wrapper->SetRaw(UTextureUtilities::info.imageData, UTextureUtilities::info.imageSize,
		UTextureUtilities::info.imageWidth, UTextureUtilities::info.imageHeight,
		UTextureUtilities::RGBFormat, UTextureUtilities::bitDepth);
	switch (algorithm)
	{
	case AlgorithmType::AT_DiamondSquare:
		algType = "DiamondSquare";
		break;
	case AlgorithmType::AT_Opening:
		algType = "Opening";
		break;
	case AlgorithmType::AT_Closing:
		algType = "Closing";
		break;
	default:
		break;
	}
	fileName = UTextureUtilities::filePath + algType
		+ UTextureUtilities::extensionFile;
	/*It checks if there is already a file with the specified path and changes the
	name of the image to save it without overwrite the previous file*/
	while (platform.FileExists(*fileName))
	{
		counter++;
		fileName = UTextureUtilities::filePath + algType +
			FString::FromInt(counter) + UTextureUtilities::extensionFile;
	}
	FFileHelper::SaveArrayToFile(wrapper->GetCompressed(), *fileName);
}

/*
*	It is used to set the info field for saving the image
*/
void UTextureUtilities::SetImageInfo(ImageInfo image)
{
	UTextureUtilities::info = image;
}

/*
*	It opens the file dialog used to find the image we want to use as input
*/
TArray<FString> UTextureUtilities::OpenFileDialog()
{
	void* parentWindow;
	IDesktopPlatform* platform;
	TArray<FString> files;
	if (GEngine && GEngine->GameViewport)
	{
		parentWindow = GEngine->GameViewport->GetWindow()->
			GetNativeWindow()->GetOSWindowHandle();
		platform = FDesktopPlatformModule::Get();
		if (platform)
		{
			platform->OpenFileDialog(parentWindow, UTextureUtilities::title,
				UTextureUtilities::defaultFolder, "", 
				UTextureUtilities::filter, 0, files);
		}
	}
	return files;
}

/*
*	It loads an image from file
*/
FImage* UTextureUtilities::LoadImageFromFile(FString file)
{
	TSharedPtr<IImageWrapper> wrapper;
	TArray<uint8> fileData;
	const TArray<uint8>* imageData;
	FImage* image = NULL;
	IImageWrapperModule &module = FModuleManager::LoadModuleChecked
		<IImageWrapperModule>(FName("ImageWrapper"));
	wrapper = module.CreateImageWrapper(UTextureUtilities::imageFormat);
	if (FPaths::FileExists(*file))
	{
		if (FFileHelper::LoadFileToArray(fileData, *file))
		{
			if (wrapper.IsValid() &&
				wrapper->SetCompressed(fileData.GetData(), fileData.Num()))
			{
				if (wrapper->GetRaw(UTextureUtilities::RGBFormat,
					UTextureUtilities::bitDepth, imageData))
				{
					image = new FImage();
					image->SizeX = wrapper->GetWidth();
					image->SizeY = wrapper->GetHeight();
					image->RawData = *imageData;
					image->Format = ERawImageFormat::Type::BGRA8;
					image->GammaSpace = EGammaSpace::sRGB;
				}
			}
		}
	}
	return image;
}
