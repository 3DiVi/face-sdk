# Video Stream Processing

The `VideoWorker` interface object is used to:

* [track faces on video streams](#tracking-faces)
* [create templates](#creating-templates)
* [match templates with the database](#recognizing-faces)
* [estimate age, gender, and emotions](#estimation-of-age-gender-and-emotions)
* [match the faces detected in a specified period with each other](#short-time-identification)

The `VideoWorker` object is responsible for thread control and synchronization routine, you only need to provide decoded video frames and register a few callback functions.

See an example of using `VideoWorker` in [video_recognition_demo](../samples/cpp/video_recognition_demo.md).  
Learn how to detect and track faces in a video stream in our tutorial [Face Detection and Tracking in a Video Stream](../tutorials/face_detection_and_tracking_in_a_video_stream.md).  
Learn how to recognize faces in a video stream in our tutorial [Face Recognition in a Video Stream](../tutorials/face_recognition_in_a_video_stream.md).  

## Tracking Faces

`VideoWorker` can be created with `FacerecService.createVideoWorker`.

### C ++ Example

<details>
  <summary>Click to expand</summary>
    
```cpp
pbio::FacerecService::Config video_worker_config("video_worker_lbf.xml");
video_worker_config.overrideParameter("search_k", 3);
pbio::VideoWorker::Ptr video_worker = service->createVideoWorker(
    pbio::VideoWorker::Params()
        .video_worker_config(video_worker_config)
        .recognizer_ini_file(recognizer_config)
        .streams_count(streams_count)
        .processing_threads_count(processing_threads_count)
        .matching_threads_count(matching_threads_count)
        .age_gender_estimation_threads_count(age_gender_estimation_threads_count)
        .emotions_estimation_threads_count(emotions_estimation_threads_count)
        .short_time_identification_enabled(enable_sti)
        .short_time_identification_distance_threshold(sti_recognition_threshold)
        .short_time_identification_outdate_time_seconds(sti_outdate_time)
    );
```
</details>

### C# Example

<details>
  <summary>Click to expand</summary>

```cs
FacerecService.Config video_worker_config = new FacerecService.Config("video_worker_lbf.xml");
video_worker_config.overrideParameter("search_k", 3);
VideoWorker video_worker = service.createVideoWorker(
    new VideoWorker.Params()
        .video_worker_config(video_worker_config)
        .recognizer_ini_file(recognizer_config)
        .streams_count(streams_count)
        .processing_threads_count(processing_threads_count)
        .matching_threads_count(matching_threads_count)
        .age_gender_estimation_threads_count(age_gender_estimation_threads_count)
        .emotions_estimation_threads_count(emotions_estimation_threads_count)
        .short_time_identification_enabled(enable_sti)
        .short_time_identification_distance_threshold(sti_recognition_threshold)
        .short_time_identification_outdate_time_seconds(sti_outdate_time)
    );
```
</details>

### Java Example 

<details>
  <summary>Click to expand</summary>

```java
FacerecService.Config video_worker_config = service.new Config("video_worker_lbf.xml");
video_worker_config.overrideParameter("search_k", 3);
VideoWorker video_worker = service.createVideoWorker(
    new VideoWorker.Params()
        .video_worker_config(video_worker_config)
        .recognizer_ini_file(recognizer_config)
        .streams_count(streams_count)
        .processing_threads_count(processing_threads_count)
        .matching_threads_count(matching_threads_count)
        .age_gender_estimation_threads_count(age_gender_estimation_threads_count)
        .emotions_estimation_threads_count(emotions_estimation_threads_count)
        .short_time_identification_enabled(enable_sti)
        .short_time_identification_distance_threshold(sti_recognition_threshold)
        .short_time_identification_outdate_time_seconds(sti_outdate_time)
    );
```
</details>

Where:

* `video_worker_config` – path to the configuration file for `VideoWorker` or `FacerecService.Config` object
* `recognizer_config` – the configuration file for the recognizer used (see [Face Identification](face_identification.md))
* `streams_count` – the number of video streams; a tracking stream is created for each stream
* `processing_threads_count` – the number of threads for template creation. These threads are common to all video streams and they distribute resources evenly across all video streams regardless of their workload (except for the video streams without faces in the frame)
* `matching_threads_count` – the number of threads for comparison of templates created from video streams with the database. Like processing threads, they distribute the workload evenly across all video streams
* `age_gender_estimation_threads_count` – the number of threads for age and gender estimation. Like processing threads, they distribute the workload evenly across all video streams
* `emotions_estimation_threads_count` – the number of threads for emotions estimation. Like processing threads, they distribute the workload evenly across all video streams
* `enable_sti` – the flag enabling [short time identification](#short-time-identification)
* `sti_recognition_threshold` – the recognition distance threshold for [short time identification](#short-time-identification)
* `sti_outdate_time` – time period in seconds for [short time identification](#short-time-identification)

Currently, there are three configuration files with the tracking method from `common_video_capturer.xml`:

* `video_worker.xml` with the *esr* points set
* `video_worker_lbf.xml` with the *singlelbf* points set
* `video_worker_fda.xml` with the *fda* points set

and two configuration files with the tracking method from `fda_tracker_capturer.xml`:

* `video_worker_fdatracker.xml` with the *fda* points set
* `video_worker_fdatracker_fake_detector.xml` with the *fda* points set

(see [Anthropometric Points](face_capturing.md#anthropometric-points), [Capturer Class Reference](face_capturing.md#capturer-class-reference)).

If `VideoWorker` is used only for face tracking, it should be created with `matching_thread=0 and processing_thread=0` and the standard [Face Detector license](../components.md#facedetector) is used. To create [Face Detector](../components.md#facedetector) for one stream, specify the `streams_count=1` parameter.

To provide video frames, you should call `VideoWorker.addVideoFrame`. This method is thread-safe, so you can provide frames from different streams created for each video stream, without additional synchronization. The method returns an integer frame id that will be used to identify this frame in the callback.

You have to use two callbacks for face tracking:

* `VideoWorker.TrackingCallbackU` provides the tracking results. This callback is called every time the frame has been processed by the tracking conveyor. Tracking callback will be called with `frame_id` equal to `X` not earlier than `VideoWorker.addVideoFrame` returns the value of `X + N - 1`, where `N` is the value returned by `VideoWorker.getTrackingConveyorSize`. Tracking callbacks with the same `stream_id` are called in ascending `frame_id` order. Therefore, if a callback with `stream_id = 2` and `frame_id = 102` was received immediately after a callback with `stream_id = 2` and `frame_id = 100`, then the frame with `frame_id = 101` was skipped for the video stream 2. Most of the samples are created from the `frame_id` frame, but some samples can be obtained from previous frames. Use the `RawSample.getFrameID` method to determine which frame the sample actually belongs to. To subscribe to this callback, use the `VideoWorker.addTrackingCallbackU` method. To unsubscribe from this method, use the `VideoWorker.removeTrackingCallback` method by submitting the `callback_id` you received from the `VideoWorker.addTrackingCallbackU` method.

* `VideoWorker.TrackingLostCallbackU` returns the best sample and face template when tracking is lost (for example, when a person leaves the frame). The best sample can be empty if the `weak_tracks_in_tracking_callback` configuration parameter is enabled. It is guaranteed that this is the last callback for the pair `<stream_id, track_id>` (`track_id` is equal to `sample.getID()` for a sample given in any `VideoWorker` callback). That is, after this callback, no `Tracking`, `MatchFound` or `TrackingLost` callback for this `stream_id` can contain a sample with the same `track_id` identifier. It is also guaranteed that for each pair `<stream_id, track_id>`, which was mentioned in the `Tracking` callback, there is exactly one `TrackingLost` callback, except for the tracks removed during `VideoWorker.resetStream` – the `TrackingLost` callback won't be called for these tracks. Use the `return` value of `VideoWorker.resetStream` to release the memory allocated for these tracks. To subscribe to this callback, use the `VideoWorker.addTrackingLostCallbackU` method. To unsubscribe from this callback, use the `VideoWorker.removeTrackingLostCallback` method by providing the `callback_id` that you received from the `VideoWorker.addTrackingLostCallbackU` method.

_**Note:** Exceptions that are thrown in the callbacks will be catched and rethrown in the `VideoWorker.checkExceptions` member function. Therefore, do not forget to call the `VideoWorker.checkExceptions` method from time to time to check for errors._

_**WARNING:** Do not call the methods that change the state of `VideoWorker` inside the callbacks in order to avoid a deadlock. That is, only the `VideoWorker.getMethodName` and `VideoWorker.getStreamsCount` member functions are safe for calling in callbacks._

## Creating Templates

If besides detection, the creation of templates is required, `VideoWorker` should be created with `matching_thread=0` and `processing_thread>0` and the [Video Engine Standard license](../components.md#video-engine-standard-ves) is used. To create [Video Engine Standard](../components.md#video-engine-standard-ves) for one stream, specify the parameters `streams_count=1`, `processing_threads_count=1`, `matching_threads_count=0`.

You can disable / enable the creation of templates for a specific video stream using the `VideoWorker.disableProcessingOnStream` and `VideoWorker.enableProcessingOnStream` member functions. At start, template creation is enabled for all video streams.

`VideoWorker.TemplateCreatedCallbackU` provides template generation results. This callback is called whenever a template is created within the `VideoWorker`. It is guaranteed that this callback will be called after at least one `Tracking` callback and before a `TrackingLost` callback with the same `stream_id` and `track_id` (`track_id = sample->getID()`). To subscribe to this callback, use the `VideoWorker.addTemplateCreatedCallbackU` method. To unsubscribe from this callback, use the `VideoWorker.removeTemplateCreatedCallback` method by providing the `callback_id` that you received from the `VideoWorker.addTemplateCreatedCallbackU` method.

## Recognizing Faces

If face tracking, template creation and matching with the database are required, `VideoWorker` should be created with `matching_thread>0` and `processing_thread>0` and the [Video Engine Extended license](../components.md#video-engine-extended-vee) is used. To create [Video Engine Extended](../components.md#video-engine-extended-vee) for one stream, specify the parameters `streams_count=1`, `processing_threads_count=1`, `matching_threads_count=1`.

Use the `VideoWorker.setDatabase` member function to setup or change the database. It can be called at any time.

`VideoWorker.MatchFoundCallbackU` returns the result of the matching with the database. When a template is created for the tracked face, it is compared with each template from the database, and if the distance to the closest element is less than  `distance_threshold` specified in this element, then a match is fixed. This callback is called after `N` consecutive matches with the elements belonging to the same person.

You can set the `<not_found_match_found_callback>` tag to `1` to enable this callback after `N` sequential not-found hits (i.e. when the closest element is beyond its `distance_threshold`.) In this case, `match_result` of the first element in `VideoWorker.MatchFoundCallbackData.search_results` will be at zero distance, and the `person_id` and `element_id` identifiers will be equal to `VideoWorker.MATCH_NOT_FOUND_ID`. The `N` number can be set in the configuration file in the `<consecutive_match_count_for_match_found_callback>` tag.

It is guaranteed that this callback will be called after at least one `Tracking` callback and before a `TrackingLost` callback with the same `stream_id` and `track_id` (`track_id = sample->getID()`). To subscribe to this callback, use the `VideoWorker.addMatchFoundCallbackU` method. To unsubscribe from this callback, use the `VideoWorker.removeMatchFoundCallback` method by providing the `callback_id` that you received from the `VideoWorker.addMatchFoundCallbackU` method. The maximum number of elements returned in the `VideoWorker.MatchFoundCallbackData.search_results` is set in the configuration file in the `search_k` tag and can be changed by the `FacerecService.Config.overrideParameter` object, for example: `video_worker_config.overrideParameter("search_k", 3);`

## Estimation of age, gender, and emotions

To estimate age and gender, specify the parameter `age_gender_estimation_threads_count > 0`. To estimate emotions, specify the parameter `emotions_estimation_threads_count > 0`. The information about age, gender, and emotions is returned in `VideoWorker.TrackingCallbackU`. The information about emotions is constantly updated. The information about age and gender is updated only if there is a sample of better quality. By default the estimation of age, gender, and emotions is enabled after you create `VideoWorker`.

To disable estimation of age, gender, and emotions on a specified stream, use the following methods:

* `VideoWorker.disableAgeGenderEstimationOnStream` (age and gender)
* `VideoWorker.disableEmotionsEstimationOnStream` (emotions)

To enable estimation of age, gender, and emotions on a specified stream again, use the following methods:

* `VideoWorker.enableAgeGenderEstimationOnStream` (age and gender)
* `VideoWorker.enableEmotionsEstimationOnStream` (emotions)

## Short time identification

Short time identification (STI) is used to recognize a track as a person who has been in front of a camera not long ago, even if this person is not in the database and even if matching is disabled. For example, if a person is detected, tracked, lost, and then detected and tracked again during, for example, one minute, he/she will be considered as the same person.

If short time identification is enabled, `VideoWorker` matches the tracks, where a face is lost, with other tracks, where a face was lost not longer than `sti_outdate_time` seconds ago. Matched tracks are grouped as `sti_person`. ID of this group (`sti_person_id`) is returned in `VideoWorker.TrackingLostCallbackU`. The value of `sti_person_id` is equal to the `track_id` value of the first element that formed the group `sti_person`. When a specific group `sti_person` exceeds the specified period `sti_outdate_time`, then `VideoWorker.StiPersonOutdatedCallbackU` is called.

Short time identification does not affect the usage of the license. To use this function, there should be at least one thread for template creation (`processing_thread>0`).
