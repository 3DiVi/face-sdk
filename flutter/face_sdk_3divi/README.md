# 3DiVi FaceSDK Plugin

This plugin offers a trial of the core functionalities of 3DiVi Face SDK, featuring advanced Face Recognition technology ranked highly by NIST. Key features include:

* Face Detection
* Facial Keypoints Detection
* Face Verification (1:1)

Please, note that this package contains only a demo license, so your runtime usage is limited in time. For a trial license, please [contact us](https://3divi.ai/products/software/face-sdk?utm_source=flutter&utm_medium=plugin&utm_campaign=site).

## Installation
First, add `face_sdk_3divi` and `face_sdk_3divi_models` as a [dependency in your pubspec.yaml file](https://flutter.dev/using-packages/).

### Requirements
Physical device with minimal Android API level 21(Android 5.0) or iOS 10.0


### Android
* Create Flutter project with Java
* Add imports to your ```android/app/src/main/java/com.../MainActivity.java```
```java
import android.content.pm.ApplicationInfo;
import androidx.annotation.NonNull;
import io.flutter.embedding.android.FlutterActivity;
import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.embedding.engine.loader.FlutterLoader;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.FlutterInjector;
```
* Replace your ```MainActivity``` class with
```java
public class MainActivity extends FlutterActivity {
    static {
        System.loadLibrary("facerec");
    }

    private static final String CHANNEL = "samples.flutter.dev/facesdk";

    private String _getNativeLibDir() {
        return getApplicationInfo().nativeLibraryDir;
    }

    @Override
    public void configureFlutterEngine(@NonNull FlutterEngine flutterEngine) {
        super.configureFlutterEngine(flutterEngine);
        new MethodChannel(flutterEngine.getDartExecutor().getBinaryMessenger(), CHANNEL)
                .setMethodCallHandler(
                        (call, result) -> {
                            if (call.method.equals("getNativeLibDir")) {
                                String nativeLibDir = _getNativeLibDir();
                                result.success(nativeLibDir);
                            } else {
                                result.notImplemented();
                            }
                        }
                );
    }
} 
```
* Add these lines to android/app/build.gradle in android section
```
sourceSets {
    main {
            jniLibs.srcDirs = ["${projectDir}/../../assets/lib"]
    }
}
```


### iOS
Copy libraries
* Create directory assets/lib in your project directory  
* Copy face_sdk_3divi/assets/lib/facerec.framework and face_sdk_3divi/assets/lib/onnxruntime.framework to assets/lib
* Open ios/Runner.xcworkspace in XCode
* In Target Navigator select "Runner", go to "General" tab, "Frameworks, Libraries, and Embedded Content" section and click "+". In the opened window "Add Other..."->"Add Files" and select facerec.framework, onnxruntime.framework in Finder
* Remove facerec.framework, onnxruntime.framework in "Build Phases" tab, "Link Binary With Libraries" section
Setup project
* Add permissions to ```ios/Runner/Info.plist``` in ```dict``` tag
```xml
<key>NSCameraUsageDescription</key>
<string>Camera description</string>
<key>NSMicrophoneUsageDescription</key>
<string>Microphone description</string>
```
* Replace your ```ios/Runner/AppDelegate.swift``` with
```swift
import UIKit
import Flutter

@UIApplicationMain
@objc class AppDelegate: FlutterAppDelegate {
  override func application(
    _ application: UIApplication,
    didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?
  ) -> Bool {
    let controller : FlutterViewController = window?.rootViewController as! FlutterViewController
    let batteryChannel = FlutterMethodChannel(name: "samples.flutter.dev/facesdk",
                                              binaryMessenger: controller.binaryMessenger)
    batteryChannel.setMethodCallHandler({
      (call: FlutterMethodCall, result: @escaping FlutterResult) -> Void in
      // Note: this method is invoked on the UI thread.
        guard call.method == "getNativeLibDir" else {
          result(FlutterMethodNotImplemented)
          return
        }
        result(Bundle.main.privateFrameworksPath)
    })

    GeneratedPluginRegistrant.register(with: self)
    return super.application(application, didFinishLaunchingWithOptions: launchOptions)
  }
}
```


### Creating Face SDK components
#### Create Face SDK service
```dart
import 'package:face_sdk_3divi/face_sdk_3divi.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // Creating main Face SDK component
  // With FacerecService you can create other Face SDK components, such as ProcessingBlock, Context, Recognizer, Capturer
  FacerecService service = await FaceSdkPlugin.createFacerecService();
}
```
#### Create Face SDK async processing blocks
```dart
import 'package:face_sdk_3divi/face_sdk_3divi.dart';

// AsyncProcessingBlock uses Isolate

// Creating AsyncProcessingBlock component for detecting faces
Future<AsyncProcessingBlock> createFaceDetector(FacerecService service) async {
  return await service.createAsyncProcessingBlock({"unit_type": "FACE_DETECTOR", "modification": "blf_front"});
}

// Creating AsyncProcessingBlock component for constructing face landmarks after detecting
Future<AsyncProcessingBlock> createFaceFitter(FacerecService service) async {
  return await service.createAsyncProcessingBlock({"unit_type": "FACE_FITTER", "modification": "tddfa_faster"});
}

// Creating AsyncProcessingBlock component for extracting face template after detecting and constructing face landmarks
// With this templates you can recognize faces
Future<AsyncProcessingBlock> createTemplateExtractor(FacerecService service) async {
  return await service.createAsyncProcessingBlock({"unit_type": "FACE_TEMPLATE_EXTRACTOR", "modification": "30"});
}

// Creating AsyncProcessingBlock component for recognizing faces
Future<AsyncProcessingBlock> createVerificationModule(FacerecService service) async {
  return await service.createAsyncProcessingBlock({"unit_type": "VERIFICATION_MODULE", "modification": "30"});
}
```
#### Create Face SDK processing blocks
```dart
import 'package:face_sdk_3divi/face_sdk_3divi.dart';

// Creating ProcessingBlock component for detecting faces
Future<ProcessingBlock> createFaceDetector(FacerecService service) async {
  return service.createProcessingBlock({"unit_type": "FACE_DETECTOR", "modification": "blf_front"});
}

// Creating ProcessingBlock component for constructing face landmarks after detecting
Future<ProcessingBlock> createFaceFitter(FacerecService service) async {
  return service.createProcessingBlock({"unit_type": "FACE_FITTER", "modification": "tddfa_faster"});
}

// Creating ProcessingBlock component for extracting face template after detecting and constructing face landmarks
Future<ProcessingBlock> createTemplateExtractor(FacerecService service) async {
  return service.createProcessingBlock({"unit_type": "FACE_TEMPLATE_EXTRACTOR", "modification": "30"});
}

// Creating ProcessingBlock component for recognizing faces
Future<ProcessingBlock> createVerificationModule(FacerecService service) async {
  return service.createProcessingBlock({"unit_type": "VERIFICATION_MODULE", "modification": "30"});
}
```


### Using Face SDK processing blocks
```dart
import 'package:face_sdk_3divi/face_sdk_3divi.dart';

/**
* @param image Image for camera plugin
* @param baseAngle Native camera image rotation(0 - no rotation, 1 - 90°, 2 - 270°, 3 - 180°)
*/
Context? createTemplate(
    CameraImage image, 
    FacerecService service, 
    AsyncProcessingBlock faceDetector, 
    AsyncProcessingBlock faceFitter, 
    AsyncProcessingBlock faceTemplateExtractor, 
    int baseAngle
) {
    NativeDataStruct nativeData = NativeDataStruct(); // hold image bytes
    ContextFormat format; // image format

    convertRAW(image.planes, nativeData); // fill nativeData struct

    switch (image.format.group) {
      case ImageFormatGroup.yuv420:
        format = ContextFormat.FORMAT_YUV420;
        break;

      case ImageFormatGroup.bgra8888:
        format = ContextFormat.FORMAT_BGRA8888;
        break;

      default:
        print("Unsupported format");

        return null;
    }

    Context data = service.createContextFromFrame(nativeData.bytes!, image.width, image.height,
        format: format, baseAngle: baseAngle); // creates JSON-like container with data for processing

    try {
      await faceDetector.process(data); // detect faces
      
      // all detected objects stored in data["objects"] array
      // you can access to any object by index

      if (data["objects"].len() > 1) {
        data.dispose(); // release resources

        print("More than 1 face detected");

        return null;
      } else if (data["objects"].len() == 0) {
        data.dispose(); // release resources

        print("No face detected");

        return null;
      }

      await faceFitter.process(data); // construct face landmarks
      
      await faceTemplateExtractor.process(data); // extract face template
    } catch (e) {
      print("Exception: $e");
    }
    
    // data Context information format
    // please note, all x and y are normalized coordinates with respect to image width/height
    /*
    {
        "objects": [{
            "id": {"type": "long", "minimum": 0},
            "class": "face",
            "confidence": {"double",  "minimum": 0,  "maximum": 1},
            "bbox": [x1, y2, x2, y2],
            "template": {
                "face_template_extractor_{modification}_{version}": {
                    "format": "NDARRAY",
                    "blob": "data pointer",
                    "dtype": "uint8_t",
                    "shape": [size]
                }
            }
            "keypoints": {
                "left_eye_brow_left":   {"proj" : [x, y]},
                "left_eye_brow_up":     {"proj" : [x, y]},
                "left_eye_brow_right":  {"proj" : [x, y]},
                "right_eye_brow_left":  {"proj" : [x, y]},
                "right_eye_brow_up":    {"proj" : [x, y]},
                "right_eye_brow_right": {"proj" : [x, y]},
                "left_eye_left":        {"proj" : [x, y]},
                "left_eye":             {"proj" : [x, y]},
                "left_eye_right":       {"proj" : [x, y]},
                "right_eye_left":       {"proj" : [x, y]},
                "right_eye":            {"proj" : [x, y]},
                "right_eye_right":      {"proj" : [x, y]},
                "left_ear_bottom":      {"proj" : [x, y]},
                "nose_left":            {"proj" : [x, y]},
                "nose":                 {"proj" : [x, y]},
                "nose_right":           {"proj" : [x, y]},
                "right_ear_bottom":     {"proj" : [x, y]},
                "mouth_left":           {"proj" : [x, y]},
                "mouth":                {"proj" : [x, y]},
                "mouth_right":          {"proj" : [x, y]},
                "chin":                 {"proj" : [x, y]},
                "points": ["proj": [x, y]]
            }
        }]
    }
    */

    return data;
}
```

## Support
Please do not hesitate to [contact us](https://3divi.ai/resources/support) if you need any assistance or want to report a bug / suggest an improvement.

## Advanced SDK
Advanced SDK version is presented [here](https://3divi.ai/products/software/face-sdk?utm_source=flutter&utm_medium=plugin&utm_campaign=site).
It offers a comprehensive suite of features, including:
* Face Identification (1:N)
* Liveness / Face Antispoofing algorithms to prevent fraud
* Face Image Quality  to enhance recognition accuracy
* Age, Gender and Emotions Estimation
* Face Recognition on server side
* Flexible licensing 
* Advance Tech Support
* and much more !
