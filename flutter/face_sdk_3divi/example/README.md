## 1:1 Matching Example

```dart
/*
1:1 verification example
Pick 2 photos from gallery and process verification
*/

import 'dart:typed_data';

import 'package:face_sdk_3divi/face_sdk_3divi.dart';
import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
import 'package:flutter/services.dart' show DeviceOrientation, SystemChrome;
import 'package:image_picker/image_picker.dart';
import 'dart:math' as math;
import 'package:image/image.dart' as image_lib;

class ImageData {
  int width;
  int height;
  Uint8List bytes;

  ImageData(this.width, this.height, this.bytes);
}

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

late FacerecService service; // Main Face SDK component for creating other components
late AsyncProcessingBlock faceDetector;
late AsyncProcessingBlock faceFitter;
late AsyncProcessingBlock faceTemplateExtractor;
late AsyncProcessingBlock verificationModule;

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

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
  final ImagePicker _picker = ImagePicker();
  final image_lib.PngEncoder pngEncoder = image_lib.PngEncoder(level: 0);
  String _message = "";
  ImageData? _firstPhotoData;
  ImageData? _secondPhotoData;
  Context? _firstPhotoObject;
  Context? _secondPhotoObject;

  Future<void> _process() async {
    _firstPhotoObject = null;
    _secondPhotoObject = null;

    setState(() {});

    final XFile? firstPhoto = await _picker.pickImage(source: ImageSource.gallery);

    if (firstPhoto == null) {
      setState(() {
        _message = "Pick both photos";
      });

      return;
    }

    final XFile? secondPhoto = await _picker.pickImage(source: ImageSource.gallery);

    if (secondPhoto == null) {
      setState(() {
        _message = "Pick both photos";
      });

      return;
    }

    Context data = service.createContextFromEncodedImage(await firstPhoto.readAsBytes()); // creates JSON-like container with data for processing
    Context image = data["image"]; // get image object
    int width = image["shape"][1].get_value(); // get image width
    int height = image["shape"][0].get_value(); // get image height
    int size = width * height * 3; // calculate image size(width * height * 3(RGB channels))

    _firstPhotoData = ImageData(width, height, image["blob"].getBytes(size));

    setState(() {
      _message = "Extract template from first photo";
    });

    _firstPhotoObject = await _extractTemplateObject(data, "first photo");

    if (_firstPhotoObject == null) {
      data.dispose(); // release resources

      setState(() {});

      return;
    }

    data = service.createContextFromEncodedImage(await secondPhoto.readAsBytes()); // creates JSON-like container with data for processing
    image = data["image"]; // get image object
    width = image["shape"][1].get_value(); // get image width
    height = image["shape"][0].get_value(); // get image height
    size = width * height * 3; // calculate image size(width * height * 3(RGB channels))

    _secondPhotoData = ImageData(width, height, image["blob"].getBytes(size));

    setState(() {
      _message = "Extract template from second photo";
    });

    _secondPhotoObject = await _extractTemplateObject(data, "second photo");

    if (_secondPhotoObject == null) {
      data.dispose(); // release resources

      setState(() {});

      return;
    }

    Context verificationData = service.createContext({
      "template1": _firstPhotoObject!["template"],
      "template2": _secondPhotoObject!["template"]
    }); // create context with 2 templates for verification process

    await verificationModule.process(verificationData); // verify

    // verificationData contains information in format
    /*
        {
          "result": {
            "distance": {"long",  "minimum": 0},
            "score": {"double",  "minimum": 0,  "maximum": 1},
            "far": {"double",  "minimum": 0,  "maximum": 1},
            "frr": {"double",  "minimum": 0,  "maximum": 1},
          }
        }
     */

    Context result = verificationData["result"]; // get result as context
    double score = result["score"].get_value(); // recognition score

    setState(() {
      _message = "Verification score: $score. Is same person: ${score > 0.9 ? "true" : "false"}";
    });

    data.dispose(); // release resources
    verificationData.dispose(); // release resources
  }

  Rectangle _createRectangle(Context bbox, int imageWidth, int imageHeight) {
    double x = bbox[0].get_value() * imageWidth; // convert relative coordinate
    double y = bbox[1].get_value() * imageHeight; // convert relative coordinate
    double x1 = bbox[2].get_value() * imageWidth; // convert relative coordinate
    double y1 = bbox[3].get_value() * imageHeight; // convert relative coordinate

    return Rectangle(x.toInt(), y.toInt(), (x - x1).toInt().abs(), (y - y1).toInt().abs());
  }

  Future<Context?> _extractTemplateObject(Context data, String photoName) async {
    int width = data["image"]["shape"][1].get_value(); // get image width
    int height = data["image"]["shape"][0].get_value(); // get image height

    await faceDetector.process(data); // detect faces

    Context objects = data["objects"];

    if (objects.len() == 0) {
      _message = "No faces on $photoName";

      data.dispose(); // release resources

      return null;
    }

    await faceFitter.process(data); // construct face landmarks
    await faceTemplateExtractor.process(data); // extract face template

    Context object = objects[0];

    if (objects.len() != 1) {
      for (int i = 1; i < objects[i].len(); i++) {
        Context temp = objects[i];

        Rectangle currentRectangle = _createRectangle(object, width, height);
        Rectangle tempRectangle = _createRectangle(temp, width, height);

        if (currentRectangle.width * currentRectangle.height < tempRectangle.width * tempRectangle.height) {
          object = temp;
        }
      }
    }

    return service.createContext(object);
  }

  Image _getCrop(Context object, ImageData imageData) {
    final image_lib.Image image = image_lib.Image.fromBytes(
        width: imageData.width, height: imageData.height, bytes: imageData.bytes.buffer, numChannels: 3, order: image_lib.ChannelOrder.rgb);
    Rectangle bbox = _createRectangle(object["bbox"], imageData.width, imageData.height);
    List<int> png = pngEncoder.encode(image_lib.copyCrop(image, x: bbox.x, y: bbox.y, width: bbox.width, height: bbox.height));

    return Image.memory(png as Uint8List);
  }

  @override
  Widget build(BuildContext context) {
    if (_firstPhotoObject == null || _secondPhotoObject == null) {
      return Scaffold(
        body: Center(
          // Center is a layout widget. It takes a single child and positions it
          // in the middle of the parent.
          child: Text(_message),
        ),
        floatingActionButton: FloatingActionButton(
          onPressed: _process,
          tooltip: "Pick 2 photos",
          child: const Icon(Icons.image),
        ), // This trailing comma makes auto-formatting nicer for build methods.
      );
    }

    return PopScope(
      child: Scaffold(
          body: Stack(
            children: [
              Center(child: Text(_message)),
              Container(
                height: 150,
                width: 500,
                margin: const EdgeInsets.symmetric(horizontal: 5.0, vertical: 30.0),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Expanded(
                      child: Transform(
                        alignment: Alignment.center,
                        transform: Matrix4.rotationY(math.pi),
                        child: Center(child: _getCrop(_firstPhotoObject!, _firstPhotoData!)),
                      ),
                    ),
                    Expanded(
                      child: Transform(
                        alignment: Alignment.center,
                        transform: Matrix4.rotationY(math.pi),
                        child: Center(child: _getCrop(_secondPhotoObject!, _secondPhotoData!)),
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),
          floatingActionButton: FloatingActionButton(
            onPressed: _process,
            tooltip: "Pick 2 photos",
            child: const Icon(Icons.image),
          )),
    );
  }

  @override
  void dispose() {
    faceDetector.dispose(); // release resources
    faceFitter.dispose(); // release resources
    faceTemplateExtractor.dispose(); // release resources
    verificationModule.dispose(); // release resources
    service.dispose(); // release resources
    _firstPhotoObject?.dispose();
    _secondPhotoObject?.dispose();

    super.dispose();
  }
}
```

## Video Processing Example

```dart
/*
Video stream example
Detect faces, extract template from first detected single face
Verify first single detected face with other detections
*/

import 'package:camera/camera.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';
import 'package:face_sdk_3divi/utils.dart';
import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
import 'package:flutter/services.dart' show DeviceOrientation, SystemChrome;
import 'dart:io';

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
        Context verificationData = service.createContext({
          "template1": _firstTemplate!,
          "template2": template
        }); // create context with 2 templates for verification process

        await verificationModule.process(verificationData); // verify

        // verificationData contains information in format
        /*
        {
          "result": {
            "distance": {"long",  "minimum": 0},
            "score": {"double",  "minimum": 0,  "maximum": 1},
            "far": {"double",  "minimum": 0,  "maximum": 1},
            "frr": {"double",  "minimum": 0,  "maximum": 1},
          }
        }
        */

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
    } else if (_controller.description.sensorOrientation == 180) {
      _baseAngle = 3; // camera image rotation angle enum
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