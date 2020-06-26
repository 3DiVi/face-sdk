=== Build with Visual Studio ===

1. extract windows_x86_64.zip
2. open examples/csharp/demo/vs/csharp_demo.sln in Visual Studio, build and run.

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
dotnet publish\csharp_demo.dll --config_dir=../../../conf/facerec --capturer_config=common_video_capturer_lbf.xml


Linux:
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../../../lib:/path/to/opencvsharp/native/libs dotnet publish/csharp_demo.dll --config_dir=../../../conf/facerec --capturer_config=common_video_capturer_lbf.xml
