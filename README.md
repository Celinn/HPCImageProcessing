# HPCImageProcessing
This software is used to execute diamond-square algorithm and mathematical morphology operation.
There are three possible versions: the serial one, one with OpenMP and one with CUDA.

This is an Unreal Engine project using version UE4.22, but to use OpenMP you need to recompile
the engine code, so, instead of using Epic Games version, you have to download this one from GitHub
and put the parameter for OpenMP in makefile (VCToolChain.cs file)

In the repository there are also two libraries used for the CUDA versions of the algorithms:
they have to be compiled: their .h files has to be put in CUDA\ImageProcessing\Includes folder inside 
the Unreal Engine project and .lib files has to be put in CUDA\ImageProcessing\Libreries