# Face Estimation

* [Age & Gender](#age--gender)
* [Quality](#quality) 
* [Liveness (2D and 3D)](#liveness)
* [Emotions](#emotions)

## Age & Gender

_**Note:** If you need to estimate age and gender on a video stream, see [Estimation of age, gender, and emotions](video_stream_processing.md#estimation-of-age-gender-and-emotions) in the section [Video Stream Processing](video_stream_processing.md)._

For age and gender estimation, create the `AgeGenderEstimator` class by calling the `FacerecService.createAgeGenderEstimator` method, providing the configuration file. Currently, there is only one config file available, which is *age_gender_estimator.xml*. With `AgeGenderEstimator` you can estimate age and gender of a captured face using `AgeGenderEstimator.estimateAgeGender`. The result is the `AgeGenderEstimator.AgeGender` struct containing the number of ages (in years), age group (`AgeGenderEstimator.Age`) and gender (`AgeGenderEstimator.Gender`). See the example of using the `AgeGenderEstimator` in [demo.cpp](../../../examples/cpp/demo/demo.cpp). 

Learn how to estimate Age & Gender in an image in our tutorial [Estimating Age, Gender, and Emotions](../tutorials/estimating_age_gender_and_emotions.md).

## Quality

At the moment, there are two quality estimation interfaces: `QualityEstimator` and `FaceQualityEstimator`. 
 * `QualityEstimator` provides **discrete grade of quality** for flare, lighting, noise and sharpness. 
 * `FaceQualityEstimator` provides quality as **a single real value** that aggregates sample usability for face recognition (i.e. pose, occlusion, noise, blur and lighting), which is very useful for comparing samples of one person from video tracking.  

### QualityEstimator

To create the `QualityEstimator` object, call the `FacerecService.createQualityEstimator` method by passing the configuration file. Currently, two configuration files are available:
* `quality_estimator.xml` – first implementation of the *QualityEstimator* quality estimation interface 
* `quality_estimator_iso.xml` (recommended) – improved version of the *QualityEstimator* quality estimation interface, provides higher accuracy of quality estimation 

With `QualityEstimator` you can estimate the quality of a captured face using `QualityEstimator.estimateQuality`. The result is the `QualityEstimator.Quality` structure that contains estimated flare, lighting, noise, and sharpness level. 

See the example of using the `QualityEstimator` in [demo.cpp](../../../examples/cpp/demo/demo.cpp).

### FaceQualityEstimator

To create the `FaceQualityEstimator` object, call the `FacerecService.createFaceQualityEstimator` method by passing the configuration file. Currently, there is only one configuration file available, which is *face_quality_estimator.xml*. With `FaceQualityEstimator` you can estimate the quality of a captured face using `FaceQualityEstimator.estimateQuality`. This results in a real number (the greater it is, the higher the quality), which aggregates sample usability for face recognition. See the example of using the `FaceQualityEstimator` in [demo.cpp](../../../examples/cpp/demo/demo.cpp).

## Liveness

The main purpose of liveness estimation is to prevent spoofing attacks (using a photo of a person instead of a real face). Currently, you can estimate liveness in one of three ways - [by processing a depth map](#depthlivenessestimator), [by processing an IR image](#irlivenessestimator) or [by processing an RGB image](#livenessestimator) from your camera.

Learn how to estimate liveness of a face in our tutorial [Liveness Detection](../tutorials/liveness_detection.md).

### DepthLivenessEstimator

To estimate liveness with a depth map, you should create the `DepthLivenessEstimator` object using `FacerecService.createDepthLivenessEstimator`.

The following configuration files are available:
  * *depth_liveness_estimator.xml* – the first implementation (not recommended; used only for backward compatibility);
  * *depth_liveness_estimator_cnn.xml* – implementation based on neural networks (recommended, used in `VideoWorker` by default).

To use this algorithm, it is necessary to obtain synchronized and registered frames (color image + depth map) and use a color image for face tracking / detection and to pass the corresponding depth map to the `DepthLivenessEstimator.estimateLiveness` method.

To get an estimated result, you can call the `pbio.DepthLivenessEstimator.estimateLiveness` method. You will get one of the following results:
  * `DepthLivenessEstimator.NOT_ENOUGH_DATA` – too many missing depth values on the depth map.
  * `DepthLivenessEstimator.REAL` – the observed face belongs to a living person.
  * `DepthLivenessEstimator.FAKE` – the observed face is taken from a photo.

### IRLivenessEstimator 

To estimate liveness using an infrared image from a camera, you should create the `IRLivenessEstimator` object using the `FacerecService.createIRLivenessEstimator` method. Currently, only one configuration file is available – *ir_liveness_estimator_cnn.xml* (implementation based on neural networks). To use this algorithm, you have to get color frames from the camera in addition to the IR frames.

To get an estimated result, you can call the `IRLivenessEstimator.estimateLiveness` method. You will get one of the following results:
* `IRLivenessEstimator.Liveness.NOT_ENOUGH_DATA` – too many missing values in the IR image.
* `IRLivenessEstimator.Liveness.REAL` – the observed face belongs to a living person.
* `IRLivenessEstimator.Liveness.FAKE` – the observed face is taken from a photo.

### LivenessEstimator

To estimate liveness with an RGB map, you should create the `LivenessEstimator` object using the `FacerecService.createLivenessEstimator` method. To do that, you need to create one `LivenessEstimator` object per each unique track id and provide the tracked `RawSample` object to the corresponding `LivenessEstimator` object using the `LivenessEstimator.addSample` method.

To get an estimated result, you can call the `LivenessEstimator.estimateLiveness` method. As a result you will have one of the following:
  * `LivenessEstimator.NOT_ENOUGH_DATA` – not enough samples or face movements to make a decision.
  * `LivenessEstimator.REAL` – the observed face belongs to a living person.
  * `LivenessEstimator.FAKE` – the observed face is taken from a photo.

See an example of using `LivenessEstimator` in [demo.cpp](../../../examples/cpp/demo/demo.cpp).

## Emotions

_**Note:** If you need to estimate emotions on a video stream, see [Estimation of age, gender, and emotions](video_stream_processing.md#estimation-of-age-gender-and-emotions) in the section [Video Stream Processing](video_stream_processing.md)._

To estimate emotions, create the `EmotionsEstimator` object using `FacerecService.createEmotionsEstimator` and pass the configuration file. Currently, there is only one configuration file, which is *emotions_estimator.xml*.  
With the `EmotionsEstimator` object you can estimate the emotion of a captured face using the `EmotionsEstimator.estimateEmotions` method. The result is a vector with the `EmotionsEstimator.EmotionConfidence` elements containing emotions with a confidence value. See the example of using the `EmotionsEstimator` object in [demo.cpp](../../../examples/cpp/demo/demo.cpp).
