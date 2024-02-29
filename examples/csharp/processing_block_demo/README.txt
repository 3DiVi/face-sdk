=== Build with Visual Studio ===

1. extract windows_x86_64.zip
1.1 please make sure the libonnxruntime.so library can be found when building the project
2. open examples/csharp/processing_block_demo/vs/csharp_processing_block_demo.sln in Visual Studio, build and run.

=== Build with dotnet ===

Requirements:
  * dotnet https://docs.microsoft.com/en-us/dotnet/core/get-started
  * opencvsharp https://github.com/shimat/opencvsharp
  * command line parser https://github.com/commandlineparser/commandline


Build
cd examples/csharp/demo
dotnet publish -o publish


Run

Windows:
set PATH=%PATH%;..\..\..\bin
dotnet publish\csharp_processing_block_demo.dll --sdk_path=../../../ --unit_type=face --modification uld --min_size 50


Linux:
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../../../lib:/path/to/opencvsharp/native/libs dotnet publish/csharp_processing_block_demo.dll --sdk_path=../../../ --unit_type=face --modification uld --min_size 50
