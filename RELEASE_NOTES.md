# Release Notes

## 3.6.1 (18 Jan 2021)

### Bug Fixes and Improvements

* Fixed fake detections when processing video from multiple streams
* Fixed the reading error from 3divi_face_sdk.lic when using a USB token


## 3.5.1 (18 Jan 2021)

### Bug Fixes and Improvements

* Fixed fake detections when processing video from multiple streams


## 3.4.2 (18 Jan 2021)

### Bug Fixes and Improvements

* Fixed fake detections when processing video from multiple streams


## v3.6.0 (25 Nov 2020)

### What's new

* Added Python API (Windows, Linux)
* Added a new detector providing the highest quality of detection – refa (recommended for expert systems)
* Added GPU usage support on Windows x86 64-bit
* Added a video recognition demo for Telpo Face Recognition Terminals TPS980
* Added a new primitive for evaluating Liveness based on RGB images – `Liveness2DEstimator` (previously available in `VideoWorker`)

### Bug Fixes and Improvements

* Added support for the AVX2 instruction set (only for Linux x86 64-bit) that can be used to speed up the template creation in 9.x identification methods (on expert systems). [See more info in the section Performance Parameters](/doc/en/performance_parameters.md#timing-characteristics-for-core-i7-45-ghz).


## v3.5.0 (21 Oct 2020)

### What's new

* Beta version of [GPU support](doc/en/development/gpu_usage.md) for linux-x86-64 and Android
* Added a new module for estimation of age and gender: `age_gender_estimator_v2.xml`. This is a pre-release of a new model.

### Bug Fixes and Improvements

* Improved the speed and quality of the 2D Liveness module
* Improved the methods for quality estimation: `quality_estimator_iso.xml`, `quality_estimator.xml`


## v3.4.1 (07 Oct 2020)

### Bug Fixes and Improvements

* Updated the tutorial project for iOS
* Fixed demo for Android 11: added the use of scoped storage


## v3.4.0 (10 Aug 2020)

### What's new

* Added the IR Liveness module (beta)
* Added the BLF detector
* Added the *mesh* set of points
* Added methods for recognition of masked faces: 
    * `method9v30mask_recognizer.xml` 
    * `method9v300mask_recognizer.xml`
    * `method9v1000mask_recognizer.xml`
* Added a new improved method for quality estimation: `quality_estimator_iso.xml`


## v3.3.0.1 (03 June 2020)

### Bug Fixes and Improvements

* Added config `video_worker_fdatracker.mask.xml` for masked faces


## v3.3.0.0 (27 April 2020)

### What's new

* Added new improved identification algorithms: `method9v30`, `method9v300`, `method9v1000`
* Fixed Android demos for Android 10


## v3.2.2.4 (02 April 2020)

### Bug Fixes and Improvements

* Fixed iOS framework


## v3.2.2.3 (27 March 2020)

### Bug Fixes and Improvements

* Fixed building C++ examples on Linux


## v3.2.2.2 (23 March 2020)

### Bug Fixes and Improvements

* Fixed license issue in the android apps


## v3.2.2.1 (10 March 2020)

### Bug Fixes and Improvements

* Fixed Android 10 support


## v3.2.2.0 (17 February 2020)

### What's new

* Added iOS support
* Added C# example for 3D-liveness with RealSense
* Added permissions request to Android demo apps


## v3.2.1.1 (12 January 2020)

### Bug Fixes and Improvements

* Added astra linux 1.5 support


## v3.2.1.0 (05 December 2019)

### What's new

* Added beta version of 2D-liveness in `VideoWorker`
* Added Android Studio samples for android
* Added MS Visual Studio samples for C#
* Added license status logs
* Added match score


## v3.2.0.0 (17 October 2019)

### What's new

* Added licensing by USB-token 
* Added Age, Gender and Emotions to `VideoWorker`
* Added API to get license status
* Fixed android signature generation error


## v3.1.1.1 (29 July 2019)

### Bug Fixes and Improvements

* Added tutorial "Face Recognition in a Video Stream"


## v3.1.1.0 (09 July 2019)

### What's new

* Added binaries for linux astra 1.11 x86_64 (linux astra 1.5 compatible)


## v3.1.0.2 (26 June 2019)

### Bug Fixes and Improvements

* Fix memory leak in `VideoWorker` (from v2.4.0.9)


## v3.1.0.1 (20 June 2019)

### Bug Fixes and Improvements

* Fixed error "0xe078ff45" in `VideoWorker`


## v3.1.0.0 (10 June 2019)

### What's new

* Fixed behaviour of `VideoWorker` "max_frames_queue_size" parameter (from v3.0.1.10)
* Added tutorial "Estimating age, gender, and emotions" (from v3.0.1.9)
* Fixed using multiple licenses on the same device (from v3.0.1.8)

* Added new improved identification algorithms: `method7.7`, `method6.7`, `method8.7`.

NOTE:
        These methods require results of FDA facial landmarks estimation algorithm,
        so if sample was created with different point set, facial landmarks will be
        re-estimated by FDA algorithm during `Recognizer::processing`.
        Also note that result of FDA is more than points position, so samples that created with
        `Capturer::manualCapture(..., <vector of points>)` functions will be treated the same way
        even if that <vector of points> contain FDA points set.
        So better use FDA capturers configurations, and use `RawSample::saveWithoutImage`
        and `FacerecService::loadRawSampleWithoutImage` instead of `Capturer::manualCapture` if possible.

* Added new serialization functions: `RawSample::saveWithoutImage` and `FacerecService::loadRawSampleWithoutImage`.
* Changed format of RawSample serialization. New sdk versions can load old format, old sdk version can't load new format.
* Added new landmarks estimation algorithm - see capturer and `VideoWorker` config files with 'fda' substring.


## v3.0.1.7 (7 May 2019)

### Bug Fixes and Improvements

* Added tutorial "Displaying Anthropometric Points and Head Rotation Angles"


## v3.0.1.6 (24 April 2019)

### Bug Fixes and Improvements

* Fixed working of online license on linux arm64


## v3.0.1.5 (23 April 2019)

### Bug Fixes and Improvements

* Added tutorial "Face Detection and Tracking in a Video Stream"
* Added 'need_stable_results' `VideoWorker` config parameter


## v3.0.1.4 (18 April 2019)

### Bug Fixes and Improvements

* Fixed precompiled C# demos
* Fixed deadlock in `VideoWorker` (from v2.5.3.7)
* Fixed error "0xf4d482c2" which occurred during call of `VideoWorker.resetStream` (from v2.5.3.7)


## v3.0.1.3 (07 March 2019)

### Bug Fixes and Improvements

* Fixed a bug in the online license when the path to the SDK contains spaces (fixes from v2.5.3.7)


## v3.0.1.2 (15 February 2019)

### Bug Fixes and Improvements

* Fixed local reference table overflow error in Java api (fixes from v2.5.3.6)


## v3.0.1.1 (11 February 2019)

### Bug Fixes and Improvements

* Fixed premature disposing of objects in C# api (from v2.5.3.5)
* Fixed using of disposed objects in C# api (from v2.5.3.4)
* Fixed error "use of disposed object - zero pointer `videoWorker_ptr`, code: 0x44cdb4de" in  `com.vdt.face_recognition.sdk.VideoWorker` (from v2.5.3.3)
* Fixed locking at asynchronous call of `VideoWorker.addDepthFrame` and `VideoWorker.resetStream/VideoWorker.resetTrackerOnStream` (from v2.5.3.2)


## v3.0.1.0 (16 January 2019)

### What's new

* `FacerecNetWrapper.dll` and `FacerecNetDemo` have been deprecated
* Fixes of examples build (fixes from v2.5.2.4)
* Added `resetStream` method in `VideoWorker` (from v2.5.3.1)


## v3.0.0.0 (26 November 2018)

### What's new

* Changed license version. Warning: license of previos versions not compatible with this version of sdk.
* Added new imporved identification algorithm, called `Method8.6` - fastest method
* Fixed fail in "finalize" method if "dispose" was called early for Java API (from v2.4.1.5)


## v2.5.3.0 (19 November 2018)

### What's new

* Added `"max_occlusion_time_wait"` for trackers.
* Added `"max_occlusion_time_wait"`, `"offline_work_i_e_dont_use_time"`, `"max_occlusion_count_wait"`, `"squeeze_match_found_callback_groups"`, `"debug_log_enabled"` for `VideoWorker`.


## v2.5.2.2 (12 November 2018)

### Bug Fixes and Improvements

* Fixed online license for linux armhf x86 platform. (fixes from v2.5.0.6)


## v2.5.2.1 (29 October 2018)

### Bug Fixes and Improvements

* Fixed compilation error in `include/pbio/DllHandle.h` when using `mingw` compiler for windows. (fixes from v2.4.3.8)


## v2.5.2.0 (18 October 2018)

### What's new

* Added support of linux aarch64
* Fixed: do not show the UAC dialog in windows


## v2.5.1.0 (03 October 2018)

### What's new

* Added new interface for `VideoWorker` callbacks (old interface of callbacks marked as deprecated)
* Improved face tracking multithreading in `VideoWorker`


## v2.5.0.5 (22 September 2018)

### Bug Fixes and Improvements

* Fix full face cut for `RawSample` with `singlelbf` points. (fixes from v2.2.0.4)


## v2.5.0.4 (05 September 2018)

### Bug Fixes and Improvements

* Fixed a default recognition threshold for C # demonstrations due to the use of new recognition methods.


## v2.5.0.3 (04 September 2018)

### Bug Fixes and Improvements

* Fixed dependencies of C# demos in `examples/csharp`


## v2.5.0.2 (04 September 2018)

### Bug Fixes and Improvements

* Changed versions of recognizer in demos and samples from `6v5` and `7v3` to `6v6` and `7v6`.


## v2.5.0.1 (28 August 2018)

### Bug Fixes and Improvements

* Optimized template size in RAM
* Fix bug in `test_sdk` and `template_generator`.
* Fix rare bug in face tracker (from v2.1.1.4).


## v2.5.0.0 (10 August 2018)

### What's new

* Removed dynamic dependencies to `libpng`, `libtiff`, `libjpeg` from linux x86, linux x86_64 versions of `libfacerec.so`
* Added a new improved identification algorithm, called `method7.6`.
* Added a new improved identification algorithm, called `method6.6`.
* `VideoWorker` now use `video_capturer` license if `processing_threads_count` is zero
* Added trial license into distributive
* Added method to force updating of online license `FacerecService::forceOnlineLicenseUpdate`


## v2.4.5.8 (27 July 2018)

### Bug Fixes and Improvements

* Fixed Video Recognition Demo (fixes from v2.4.3.7)


## v2.4.5.7 (27 July 2018)

### Bug Fixes and Improvements

* Fixed `DepthLivenessEstimator` for `depth_liveness_estimator_cnn.xml` config file (fixes from v2.4.3.6)


## v2.4.5.5 (16 July 2018)

### Bug Fixes and Improvements

* Fixed value of `VideoWorker.MATCH_NOT_FOUND_ID` constant in .Net wrapper (fixes from v2.4.4.8)


## v2.4.5.4 (12 July 2018)

### Bug Fixes and Improvements

* Fixed overriding of parameters using `Config.overrideParameter` in .Net wrapper (fixes from v2.4.4.7)


## v2.4.5.2 (04 July 2018)

### Bug Fixes and Improvements

* Fixed logic of frame skipping in `VideoWorker` (fixes from v2.4.3.4)
* Fixed throwing of wrong exception: "object can't be runned in two threads"


## v2.4.5.1 (25 June 2018)

### Bug Fixes and Improvements

* Fixed updating online license on android arm (fixes from v2.4.0.8)


## v2.4.5.0 (20 June 2018)

### What's new

* Added demo for .Net wrapper
* Added `setParameter` method for Capturer


## v2.4.4.3 (13 June 2018)

### Bug Fixes and Improvements

* Fixed error `'CallbackOnCollectedDelegate'` in .NET wrapper in `VideoWorker`
* Fixed visibility of `VideoWorker.MATCH_NOT_FOUND_ID `


## v2.4.4.2 (15 May 2018)

### Bug Fixes and Improvements

* fixed fail "EXCEPTION_ILLEGAL_INSTRUCTION", which occur on some CPUs (from v2.4.2.2)
* Fixed: disabled automatic creation of thread pool on windows (from v2.1.1.3)


## v2.4.4.1 (11 May 2018)

### Bug Fixes and Improvements

* Fixed error `'CallbackOnCollectedDelegate'` in .NET wrapper


## v2.4.4.0 (04 May 2018)

### What's new

* Added fully-functionally wrapper for .NET


## v2.4.3.0 (24 April 2018)

### What's new

* Integrated depth liveness in `videoworker`


## v2.4.2.1 (11 April 2018)

### Bug Fixes and Improvements

* fixed bug when get cameras list in android demo applications (from v2.4.1.2)
* fixed bug in android `video_recognition_demo`


## v2.4.2.0 (26 March 2018)

### What's new

* Speedup template creation for all methods, emotions, age and gender estimation up to 3 times on x86 CPU architecture, up to 2 times on ARM CPU architecture.


## v2.4.1.1 (12 March 2018)

### Bug Fixes and Improvements

* fixed bug in `VideoWorker` that sometimes deny template generation from face when it was not found in database by first N templates (N = `consecutive_match_count_for_match_found_callback`) (fixes from v2.4.0.6)


## v2.4.1.0 (22 February 2018)

### What's new

* Added Java interface


## v2.4.0.5 (21 February 2018)

### Bug Fixes and Improvements

* fix face rectangle compute in `pbio::Capturer::manualCapture(image, left_eye_x, left_eye_y, right_eye_x, right_eye_y)` (fixes from v2.3.0.9)


## v2.4.0.4 (19 February 2018)

### Bug Fixes and Improvements

* changed distance  between the eyes in `pbio::RawSample` for frontal capturers (110 pixels) (from v2.3.0.8)
* changed model of filling borders in `pbio::RawSample` (black background instead of repeating pixels) (from v2.3.0.8)
* added time measurement in test_identify
* fixed "bus error" with zero license count


## v2.4.0.3 (13 February 2018)

### Bug Fixes and Improvements

* fix depth liveness detector for close distance
* fix depth liveness detector for reflections


## v2.4.0.2 (07 February 2018)

### Bug Fixes and Improvements

* fixed of hardware signature generation inside docker container.


## v2.4.0.1 (19 December 2017)

### Bug Fixes and Improvements

* fixed bug that cause crash in `VideoWorker::resetTrackerOnStream` (fixes from v2.3.0.7)
* fix accelerated search for methods 6.5 and 7.3 - results were completely wrong for large databases; slight fix of search time in docs. (fixes from v2.3.0.7)


## v2.4.0.0 (06 December 2017)

### What's new
 
* Fixed checking hardware info for linux
* Added age number in years estimation by `pbio::AgeGenderEstimator`.
* Added `TemplateCreatedCallback` in `pbio::VideoWorker`.
* `MatchFound` callback for not recognized persons in `pbio::VideoWorker` (toggle by the <not_found_match_found_callback> tag).
* Added `pbio::DepthLivenessEstimator`.
* Added flag `processing_less_memory_consumptio`n parameter in `pbio::FacerecService::createRecognizer`, and corresponding tag <recognizer_processing_less_memory_consumption> in `VideoWorker` config.
* Added `getROCCurvePointByDistanceThreshold`, `getROCCurvePointByFAR` and `getROCCurvePointByFRR` member functions in `pbio::Recognizer`
* Added `<store_original_frame_in_raw_sample>` tag in `VideoWorker` config, and `hasOriginalImage` and `getOriginalImage` member functions in `pbio::RawSample`.
* Online licenses - see page Registration in docs.
* Added build for android arm


## v2.3.0.6 (29 November 2017)

### Bug Fixes and Improvements

* .Net wrapper ,  NULL pointer exception fixed, cused by incorrect processing of empty template into the tracking lost callback.


## v2.3.0.5 (11 November 2017)

### Bug Fixes and Improvements

* .Net wrapper: fixed lost callback usage
* .Net wrapper: fixed enable/display return cropped faces in thracking callbacks
* .Net demo: example how to display unknown faces history using lost callback


## v2.3.0.4 (21 October 2017)

### Bug Fixes and Improvements

* .Net demo, added displaying identification history.
* `VideoWorker`, fixed work with disabled processing, broken templates were generated before.


## v2.3.0.3 (05 October 2017)

### Bug Fixes and Improvements

* License memory leaks fixed 
* Multithreaded license issue fixed
* .Net module speedup
* .Net module, fixed dead lock for multiple video streams
* .Net module, return cropped faces in identification and verification callbacks
* .Net wrapper, added FPS parameter to constructor, now able to change processing FPS.


## v2.3.0.0 (21 August 2017)

### What's new

* Added a new improved identification algorithm, called `method7.3`.
* Added a new improved identification algorithm, called `method6.5`.
* Added index creation for fast search in large databases, size of indexes database is limited by the license.
* Added `pbio::FacerecService::Config` that can be used for runtime override numerical parameters of configuration files for `pbio::Capturer` and `pbio::VideoWorker` creating.
* Added `pbio::VideoWorker::disableProcessingOnStream` and `pbio::VideoWorker::enableProcessingOnStream` member functions.
* New anthropometric points sets are now fully compatible with all estimators.
* Fixed template matching time in documentation (new results measured on larger dataset).
* Added `pbio::RawSample::getFrameID` - for samples returned by `pbio::VideoWorer` (see docs).
* Added `matching_queue_size_limit` and `processing_queue_size_limit` parameters in `VideoWorker`.
* Added `pbio::CameraCalibrator` - for calibrate cameras and undistort images.


## v2.2.0.3 (27 June 2017)

### Bug Fixes and Improvements

* fix checking licenses from differrent users
* add .Net wrapper and example


## v2.2.0.2 (11 May 2017)

### Bug Fixes and Improvements

* fix `VideoWorker` license.


## v2.2.0.1 (25 April 2017)

### Bug Fixes and Improvements

* fix bug in `VideoWorker` (error code 0x7721f65c).


## v2.2.0.0 (19 April 2017)

### What's new

* EmotionsEstimator added.
* Demo programms explanation added in documentation.
* Licenses work explanation added in documentation.
* `video_recognition_show` rewrited and renamed to `video_recognition_demo`, source code added in examples.
* `VideoWorker` added.


## v2.1.3.1 (28 February 2017)

### Bug Fixes and Improvements

* Fixed bug with wrong dinamic linkage, that causes an error when using certain opencv versions.


## v2.1.3.0 (16 February 2017)

### What's new

* Added a new improved identification algorithm, called `method6.4`.


## v2.1.2.1 (26 January 2017)

### Bug Fixes and Improvements

* fix rare license bug on Windows


## v2.1.2.0 (25 January 2017)

### What's new

* ARM Linux build and NEON optimizations added.
* Added a new improved identification algorithm, called `method7.2`.
* Methods `5.1` and `5.2` removed.


## v2.1.1.3 (18 November 2016)

### Bug Fixes and Improvements

* Fixed bug in face detector.


## v2.1.1.1 (26 April 2016)

### Bug Fixes and Improvements

* Fixed licenses generation and checking bug.


## v2.1.1.0 (20 April 2016)

### What's new

* Added a new improved identification algorithm, called `method7`


## v2.1.0.0 (22 January 2016)

### What's new

* Added a new improved identification algorithm, called `method6.3`


## v2.0.0.0 (01 August 2015)

### What's new

* Added a new improved identification algorithm, called `method6.2`


## v1.0.2.0 (16 March 2015)

### What's new

* Changed a protection mode, switched to license file.
* Added a new improved identification algorithm, called `method5`. 


## v1.0.1.2 (12 November 2014)

### Bug Fixes and Improvements

* Fix auth code checking under .Net application. 


## v1.0.1.2 (01 October 2014)

### Bug Fixes and Improvements

* Fixed output of eyes position into the `Estimator::get_pose()`.


## v1.0.1 (04 September 2014)

### What's new

* Removed dependency from QT
* Added 4th method of face identification, the most perspective for identification usage.  


## v0.1.18 (29 July 2014)

### What's new

* Fix points detector instability (libesr)


## v0.1.16 (18 July 2014)

### What's new

* pbio: new error codes added 
* Capturer now returns objects in descending size order


## v0.1.15 (11 July 2014)

### What's new

* SDK initialization simplified. Algorithm pre-initialisation implemented in services. Added helper class `pbio::facerec::Service`
* Identification example added

## v0.1.14 (02 July 2014)

### What's new

* RAW image support added
* `pbio::facerec::Estimator` interface changed
* Synthes and preprocess algorithms updated
