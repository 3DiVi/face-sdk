# Detection and Recognition of Masked Faces

Methods for recognition of masked faces were added in Face SDK 3.4. If you want to detect and recognize masked faces, use the `video_worker_fdatracker_blf_fda.xml` configuration file for detection and one of the configuration files for recognition below:

* `method9v30mask_recognizer.xml`
* `method9v300mask_recognizer.xml`
* `method9v1000mask_recognizer.xml`

These are the configuration files for the methods 9.30, 9.300, and 9.1000 that were optimized to recognize masked faces. See more information about recognition methods and configuration files in [Face Identification](/doc/en/development/face_identification.md).

The options to turn on detection and recognition of masked faces in Face SDK are described below.

## Using the pre-built video_recognition_demo sample

1. When running *video_recognition_demo*, specify the configuration file `video_worker_fdatracker_blf_fda.xml` in the `--vw_config_file` parameter. 
2. [Optional] Specify one of the above recognition methods in the `--method_config` parameter. 

See the complete instructions and detailed info about the parameters in [cpp/video_recognition_demo](/doc/en/samples/cpp/video_recognition_demo.md).

## Building a sample from the source code

1. Specify `video_worker_fdatracker_blf_fda.xml` in the `video_worker_config` parameter when you create a `VideoWorker` object ([C++](/examples/cpp/video_recognition_demo/src/main.cpp#L125), [C#](/examples/csharp/video_recognition_demo/src/Main.cs#L126), [Java](/examples/android/video_recognition_demo/src/main/java/com/vdt/face_recognition/video_recognition_demo/VidRecDemo.java#L106)). 
2. [Optional] Specify one of the above recognition methods in the `recognizer_ini_file` parameter. 
3. Re-build the app.

See an example of creating a `VideoWorker` object and detailed info about the parameters in [Tracking Faces](/doc/en/development/video_stream_processing.md#tracking-faces).