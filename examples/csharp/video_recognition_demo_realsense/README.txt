=== RealSense ===
 - obtain Intel.Realsense.dll and realsense2.dll/librealsense2.so :
   - on Windows install RealSense from https://github.com/IntelRealSense/librealsense/releases - the Intel.Realsense.dll and realsense2.dll will be at "C:\Program Files (x86)\Intel RealSense SDK 2.0\bin\x64\"
   - on Linux see the install options on https://github.com/IntelRealSense/librealsense/blob/master/doc/distribution_linux.md for librealsense2.so and https://github.com/IntelRealSense/librealsense/tree/master/wrappers/csharp for Intel.Realsense.dll
 - copy Intel.Realsense.dll and realsense2.dll/librealsense2.so to the bin/ directory
 - at run time you will need Intel.Realsense.dll and realsense2.dll/librealsense2.so in your PATH (LD_LIBRARY_PATH for linux)

=== Build with Visual Studio ===

1. extract windows_x86_64.zip
1.1 please make sure the libonnxruntime.so library can be found when building the project
2. open examples/csharp/video_recognition_demo_realsense/vs/csharp_video_recognition_demo.sln in Visual Studio, build and run.

=== Build with dotnet ===

Requirements:
  * dotnet https://docs.microsoft.com/en-us/dotnet/core/get-started
  * opencvsharp https://github.com/shimat/opencvsharp
  * command line parser https://github.com/commandlineparser/commandline


Build
cd examples/csharp/video_recognition_demo_realsense
dotnet publish -o publish


Run

Windows:
set PATH=%PATH%;..\..\..\bin
dotnet publish/csharp_video_recognition_demo.dll --config_dir=../../../conf/facerec --database_dir=../../../bin/base --method_config=method6v7_recognizer.xml --recognition_distance_threshold=7400 --frame_fps_limit=25 0


Linux:
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../../../lib:/path/to/opencvsharp/native/libs dotnet publish/csharp_video_recognition_demo.dll --config_dir=../../../conf/facerec --database_dir=../../../bin/base --method_config=method6v7_recognizer.xml --recognition_distance_threshold=7400 --frame_fps_limit=25 0
