// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class HPCImageProcessing : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string CudaPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../CUDA/")); }
    }

    /// <summary>
    /// It loads the Cuda libraries
    /// </summary>
    public void LoadCudaLib()
    {
        string librariesPath = Path.Combine(CudaPath, "ImageProcessing", "Libraries");
        string cPath = @"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1";
        string cInc = "include";
        string cLib = @"lib\x64";
        PublicAdditionalLibraries.Add(Path.Combine(librariesPath, "DiamondSquareCuda.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(librariesPath, "MathematicalMorphologyCuda.lib"));
        PublicIncludePaths.Add(Path.Combine(CudaPath, "ImageProcessing", "Includes"));
        PublicAdditionalLibraries.Add(Path.Combine(cPath, cLib, "curand.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(cPath, cLib, "cudart_static.lib"));
        PublicIncludePaths.Add(Path.Combine(cPath, cInc));
    }

	public HPCImageProcessing(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "SlateCore",
            "ImageCore",
            "MediaAssets"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
        LoadCudaLib();
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
