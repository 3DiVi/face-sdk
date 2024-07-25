# FaceSDK Plugin


## Features
* Face detection
* Face recognition


## Installation
First, add `face_sdk_3divi` as a [dependency in your pubspec.yaml file](https://flutter.dev/using-packages/).

### Requirements
Physical device with minimal Android API level 21(Android 5.0) or iOS 10.0


### Android
* Create flutter project with Java
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
  // With FacerecService you can create other Face SDK components. Such as ProcessingBlock, Context, Recognizer, Capturer
  FacerecService service = await FaceSdkPlugin.createFacerecService();
}
```
#### Create Face SDK async processing blocks
```dart
import 'package:face_sdk_3divi/face_sdk_3divi.dart';

// AsyncProcessingBlock uses isolate

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
Context? detect(CameraImage image, FacerecService service, AsyncProcessingBlock faceDetector, AsyncProcessingBlock faceFitter, int baseAngle) {
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
    } catch (e) {
      print("Exception: $e");
    }
    
    // data contains information in format
    /*
    {
        "objects": [{
            "id": {"type": "long", "minimum": 0},
            "class": "face",
            "confidence": {"double",  "minimum": 0,  "maximum": 1},
            "bbox": [x1, y2, x2, y2],
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


### Example
```dart
import 'package:camera/camera.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';
import 'package:face_sdk_3divi/utils.dart';
import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
import 'package:path_provider/path_provider.dart';
import 'package:flutter/services.dart'
    show DeviceOrientation, MethodChannel, PlatformException, SystemChrome, rootBundle;
import 'dart:io';
import 'dart:convert';
import "dart:typed_data";

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

late List<CameraDescription> cameras;
late FacerecService service; // Main Face SDK component for creating other components
late AsyncProcessingBlock faceDetector;
late AsyncProcessingBlock faceFitter;
late AsyncProcessingBlock faceTemplateExtractor;
late AsyncProcessingBlock verificationModule;

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  try {
    cameras = await availableCameras();
  } on CameraException catch (e) {
    print("Error: ${e.code}\nError Message: ${e.description}");
  }

  // Creating main Face SDK component
  // With FacerecService you can create other Face SDK components. Such as ProcessingBlock, Context, Recognizer, Capturer
  service = await FaceSdkPlugin.createFacerecService(); // 

  faceDetector = await createFaceDetector(service);
  faceFitter = await createFaceFitter(service);
  faceTemplateExtractor = await createTemplateExtractor(service);
  verificationModule = await createVerificationModule(service);

  runApp(const MaterialApp(home: App()));

  SystemChrome.setPreferredOrientations([
    DeviceOrientation.portraitUp,
  ]);
}

class App extends StatefulWidget {
  const App({super.key});

  @override
  AppState createState() => AppState();
}

class AppState extends State<App> {
  late CameraController _controller;
  Context? _firstTemplate;
  bool _processing = false;
  int _baseAngle = 0;

  Future<void> _processStream(CameraImage image) async {
    if (_processing || !mounted) {
      return;
    }

    _processing = true;

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

        _processing = false;

        return;
    }

    Context data = service.createContextFromFrame(nativeData.bytes!, image.width, image.height,
        format: format, baseAngle: _baseAngle); // creates JSON-like container with data for processing

    try {
      await faceDetector.process(data); // detect faces

      // all detected objects stored in data["objects"] array

      if (data["objects"].len() > 1) {
        data.dispose(); // release resources

        print("More than 1 face detected");

        _processing = false;

        return;
      } else if (data["objects"].len() == 0) {
        data.dispose(); // release resources

        print("No face detected");

        _processing = false;

        return;
      }

      await faceFitter.process(data); // construct face landmarks
      await faceTemplateExtractor.process(data); // extract face template

      Context template = service.createContext(data["objects"][0]["template"]); // copy template

      if (_firstTemplate == null) {
        _firstTemplate = template;

        print("Template created");
      } else {
        Context verificationData = service.createContext({"template1": _firstTemplate!, "template2": template}); // create context with 2 templates for verification process

        await verificationModule.process(verificationData); // verify

        Context result = verificationData["result"]; // get result as context
        double score = result["score"].get_value(); // recognition score

        print("Match score: $score. Is same persons: ${score > 0.9 ? "true" : "false"}");
        
        print(result.toMap()); // context can be converted to map

        verificationData.dispose(); // release resources
        template.dispose(); // release resources
      }
    } catch (e) {
      print("Exception: $e");
    }

    data.dispose(); // release resources

    _processing = false;
  }

  @override
  void initState() {
    super.initState();

    if (cameras.isEmpty) {
      print("No camera is found");
    } else {
      CameraDescription? camera;

      for (CameraDescription description in cameras) {
        if (description.lensDirection == CameraLensDirection.front) {
          camera = description;

          break;
        }
      }

      if (camera == null) {
        print("No front-facing camera found");

        camera = cameras.first;
      }

      _controller = CameraController(camera, ResolutionPreset.high);
      _controller.initialize().then((_) {
        if (!mounted) {
          return;
        }

        setState(() {});

        _controller.startImageStream(_processStream);
      });
    }

    if (_controller.description.sensorOrientation == 90) {
      _baseAngle = 1; // camera image rotation angle enum
    } else if (_controller.description.sensorOrientation == 270) {
      _baseAngle = 2; // camera image rotation angle enum
    }

    if (Platform.isIOS) {
      _baseAngle = 0; // camera image rotation angle enum
    }
  }

  @override
  Widget build(BuildContext context) {
    if (!_controller.value.isInitialized) {
      return Container();
    }

    return PopScope(
        child: Scaffold(
            body: Stack(
          children: [
            Center(
              child: Padding(padding: const EdgeInsets.all(1.0), child: CameraPreview(_controller)),
            )
          ],
        )));
  }

  @override
  void dispose() {
    _controller.dispose(); // release resources
    faceDetector.dispose(); // release resources
    faceFitter.dispose(); // release resources
    faceTemplateExtractor.dispose(); // release resources
    verificationModule.dispose(); // release resources
    _firstTemplate?.dispose(); // release resources
    service.dispose(); // release resources

    super.dispose();
  }
}
```

For a more information see [here](https://github.com/3DiVi/face-sdk)
For a more examples see [here](https://github.com/3DiVi/face-sdk/tree/master/examples/flutter)
