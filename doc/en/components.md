# Components

## Face Detector 

**Face Detector** is a basic Face SDK component, which allows to detect faces on input images or videos (when used with *VEE*, see below). The result of data processing is a special internal object representing a user's face, which is a normalized (normally rotated in the frame plane and cropped) face image standardized for further biometric processing. Due to the built-in tracking mechanism, a face is tracked and treated as the same object even after a person had left and entered the frame. For streaming video data, it's recommended to use the *VideoEngine* component instead of *FaceDetector*.  
**Licensing**: by Data Channel (photo or video).  
See the detailed info in [Tracking Faces](development/video_stream_processing.md#tracking-faces) and [Capturer Class Reference](development/face_capturing.md#capturer-class-reference).

## Encoder 

**Encoder** is a basic Face SDK component, which allows to create the Face biometric template. *Encoder* uses the normalized face image obtained from *FaceDetector* as input data.
The result of processing by *Encoder* is a biometric template Face (numerical array of identification features), which is stored in a biometric database and/or used for biometric operations. Internal composition and accordingly the size of the Face template depend on the selected method of biometric processing.  
**Licensing**: by Data Channel (photo or video).  
See the detailed info in [Face Identification](development/face_identification.md).

## Video Engine Standard (VES)

**Video Engine Standard (VES)** is a macro component (component bundle) of Face SDK, which provides basic processing of video stream in 1 channel (for example, real-time video data from 1 camera). *VES* processes video in cycles. Each cycle includes:
  * detection and tracking of faces in a frame
  * selecting the best shot and encoding the selected face (computing the Face biometric template)
  * estimation of age, gender, and emotions (separately licensed component)

*VES* receives the sequence of video frames as input data. It means that a video stream should be decompressed. *VES* by default includes the following components: *Face Detector*, *Encoder*, one load balancer (to manage the sequence of input video frames). For complex tasks of real-time video processing in high-load systems, the internal composition of *VES* can be extended: you can add additional components to improve performance. As compared to *VideoEngine Extended*, *VES* does not support face matching feature.  
**Licensing**: by Data Channel (photo or video).  
See the detailed info in [Video Stream Processing](development/video_stream_processing.md).

## Video Engine Extended (VEE)

**Video Engine Extended (VEE)** is a macro component (component bundle) of Face SDK, which provides basic video stream processing in 1 channel (for example, real-time video stream from 1 camera) and makes the data sequence for further search (in the watchlist or in the database). *VEE* processes video in cycles. Each cycle includes:
 * detection and tracking of faces in a frame
 * selecting the best shot and encoding the selected face (computing the Face biometric template)
 * comparing the template with the database (separately licensed component)
 * estimation of age, gender, and emotions (separately licensed component)

*VEE* receives the sequence of video frames as input data. It means that a video stream should be decompressed. *VEE* by default includes the following components: *Face Detector*, *Encoder*, and *MatcherDB(N)* where *N* is the size of the search index, which is typically equal to the number of faces (biometric templates) in the database. *VEE* also includes two internal load balancers: the first one is designed to manage the sequence of input video frames, the second one is designed to manage the sequence of biometric database search queries based on the face template. For complex tasks of real-time data stream processing in high-load systems, the internal composition of *VEE* can be extended: you can add additional components to improve performance.  
**Licensing**: by Data Channel (photo or video) and size of the search index of *MatcherDB*.  
See the detailed info in [Video Stream Processing](development/video_stream_processing.md).  

## MatcherDB

**MatcherDB** is a basic component of Face SDK, which implements matching of biometric templates: 1:1 (verification), 1:N (identification) or their combination. *MatcherDB* uses the Face biometric template calculated by the *Encoder* component as input data. For verification (1:1), it's sufficient to use *MatcherDB(1)*. Identification (1:N) is performed in the array of biometric templates. The *MatcherDB* size determines the search index size (permitted search boundaries) but doesn't limit the size of the biometric database.  
**Licensing**: by the number of biometric face templates in search index of a biometric database.  
See the detailed info in [Face Identification](development/face_identification.md).

## 3D Liveness Detector

**3D Liveness Detector** is an optional Face SDK component, which allows to estimate the liveness of the subject. Liveness is estimated by means of detecting 3D surface of a real face and can detect spoofing attacks by means of presenting a photo or a video image of a face. As input data, *3D Liveness Detector* uses the normalized image obtained from *FaceDetector* and a depth map received from a 3D (RGBD) sensor.  
**Licensing**: by Data Channel (photo or video).  
See the detailed info in [Liveness](development/face_estimation.md#liveness).

## Gender-Age Detector and Emotions Detector

**Gender-Age Detector** and **Emotions Detector** are optional Face SDK components, which allow to estimate gender and age of a person and get a rough estimate of the prevailing emotional state of the face at a given time. As input data, *Gender-Age Detector* and *Emotions Detector* use the normalized face images obtained from the *FaceDetector* module. The age is detected within the range of +/- 5 years. The emotional states are Happy, Surprise, Neutral, Angry.  
**Licensing**: by Data Channel (photo or video).  
See the detailed info in [Age & Gender](development/face_estimation.md#age--gender) and [Emotions](development/face_estimation.md#emotions).
