# Detection of Masked Faces

Detection of masked faces was added in Face SDK 3.3. To enable this function, use the configuration file `video_worker_fdatracker.mask.xml` (by default, `video_worker_fdatracker.xml` is used in all samples). To do this, choose one of the options below.

## Use the pre-built video_recognition_demo sample

When running *video_recognition_demo*, specify the configuration file `video_worker_fdatracker.mask.xml` in the `--vw_config_file` parameter. See the complete instructions and detailed info about the parameters in [cpp/video_recognition_demo](/doc/en/samples/cpp/video_recognition_demo.md).

## Build a sample from the source code

Specify `video_worker_fdatracker.mask.xml` in the `video_worker_config` parameter when you create a `VideoWorker` object ([С++](/examples/cpp/video_recognition_demo/src/main.cpp#L125), [C#](/examples/csharp/video_recognition_demo/src/Main.cs#L126), [Java](/examples/android/video_recognition_demo/src/main/java/com/vdt/face_recognition/video_recognition_demo/VidRecDemo.java#L106)) and re-build the app. See an example of creating a `VideoWorker` object and detailed info about the parameters in [Tracking Faces](/doc/en/development/video_stream_processing.md#tracking-faces).

## Replace the configuration file

_**Warning:** Replacing the configuration file `video_worker_fdatracker.xml` may affect all the applications that use this configuration file._

As an alternative option, you can enable masked face detection by replacing the configuration file:
* Go to the *conf/facerec* folder.
* Rename the file `video_worker_fdatracker.xml` (for example, to `video_worker_fdatracker_backup.xml`).
* After that, rename the file `video_worker_fdatracker.mask.xml` to `video_worker_fdatracker.xml`.