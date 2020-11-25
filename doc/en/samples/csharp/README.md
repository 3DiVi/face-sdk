# C# Demo Programs

The *bin/csharp_demo* directory contains the following folders with executable files of the demo programs:

* [demo](demo.md) – an example of face tracking and face estimation
* [video_recognition_demo](video_recognition_demo.md) – an example of using the `pbio.VideoWorker` object

The following dependencies are required to build and run C# demos:

*  [.NET Core Platform](https://docs.microsoft.com/en-us/dotnet/core/get-started) (recommended version: 2.1)
*  [.NET Wrapper for OpenCV](https://github.com/shimat/opencvsharp) (recommended version: 3.4.1)
*  Manual build of OpenCvSharp for Linux is required.

To build the demo project in VisualStudio:

* Create an empty C# console application.
* Add the source files of the demo to the project.
* Add links to `FacerecCSharpWrapper.dll` and [OpenCvSharp3-AnyCPU](https://www.nuget.org/packages/OpenCvSharp3-AnyCPU).
