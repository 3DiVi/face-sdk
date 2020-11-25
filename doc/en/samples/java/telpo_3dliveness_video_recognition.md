# Telpo Liveness Video Recognition 

## Overview

This project demonstrates face identification and liveness estimation on **Telpo Face Recognition Terminals TPS980** with 3D or dual (RGB+IR) cameras.

## Pre-building the Demo Application

Install and run the prebuilt application `apk/telpo_3dliveness_video_recognition.apk` for testing. The app contains an embedded license locked to [Application ID](/doc/en/licenses.md#locking-to-an-application-id) and Android OS.  

## Building the Demo Application from Source Files 

1. [Get the trial license](https://face.3divi.com/products/face_sdk/face_sdk_trial).
2. Copy the trial license `3divi_face_sdk.lic` to `examples/android/telpo_3dliveness_video_recognition/src/main/assets/license`.
3. Open the folder `telpo-android` in Android Studio as an existing project.  
4. Build the project and install the generated appcon your Telpo terminal.

## Identifying Masked Faces 

We provide the new face identification algorithm that is optimized for detection of masked faces and provides better identification quality of masked faces. Follow the steps below to set the algorithm for identification of masked faces: 
1. Open the `AuthAcrivity.java` file. 
2. Replace the line `private final String recognizer_config = "method9v30_recognizer.xml";` with the line `private final String recognizer_config = "method9v30mask_recognizer.xml";`. 
