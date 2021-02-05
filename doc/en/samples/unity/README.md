# FaceMask Unity Sample

The `FaceMask` Unity sample displays different masks on a face created with a 3D mesh of a face (see [Anthropometric points](/doc/en/development/face_capturing.md#anthropometric-points)). In this sample, 4 standard face masks and one dynamic mask are available, in which images are selected [depending on your emotions](/doc/en/development/face_estimation.md#emotions):

<p align = "center">
<img width = "250" src = "/doc/img/unity_demo.gif"><br>
<b> An example of running FaceMasks.apk: the displayed mask depends on the emotion </b>
</p>

The pre-built demo is available for Android and Windows 64-bit in the assets to the [release](https://github.com/3DiVi/face-sdk/releases).

## Installing and running the app
 
* **Android**
  1. Download the *FaceMask.apk* file from the releases page and install it on your device (only 64-bit architecture is supported).
  2. Launch the *FaceMask* application.
  3. Allow the application to access photos, media and files on the device, as well as take photos and videos.
* **Windows**
  1. Unpack the *windows_x86_64.zip* archive. This application is located in the directory `bin/unity/FaceMask_demo`.
  2. Add the path to the *bin* folder to the `PATH` environment variable.
  3. Run the *FaceMask.exe* file (`bin/unity/FaceMask_demo/FaceMask.exe`)

## Features

* At the bottom of the screen you can choose one of the standard masks or a dynamic mask (the last one in the list).
* In a dynamic mask, the image depends on your emotions. You can change the set of masks for different emotions: to do this, click on the dynamic mask icon and then click on the emotion icon (NEUTRAL/HAPPY/ANGRY/SURPRISE) to select a mask.
* To change the used device camera, click on the camera icon at the top of the screen.
  
## Settings

To change the sample settings, click on the arrow at the bottom of the screen. The following options are available:
* Opacity - the degree of transparency of the mask. Available values: from 0 to 1
* Motion smoothing. Available values: from 0 to 1
* Resolution. Available values: depends on the device used
