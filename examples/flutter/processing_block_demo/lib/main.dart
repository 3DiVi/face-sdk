import 'dart:async';
import 'dart:ui';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'dart:ui' as ui;

import 'package:image_picker/image_picker.dart';

import 'package:face_sdk_3divi/face_sdk_3divi.dart';

import 'structs.dart';

void main() {
  runApp(const App());
}

class App extends StatelessWidget {
  const App({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: "Processing Block Demo",
      theme: ThemeData(
        // This is the theme of your application.
        //
        // Try running your application with "flutter run". You'll see the
        // application has a blue toolbar. Then, without quitting the app, try
        // changing the primarySwatch below to Colors.green and then invoke
        // "hot reload" (press "r" in the console where you ran "flutter run",
        // or simply save your changes to "hot reload" in a Flutter IDE).
        // Notice that the counter didn't reset back to zero; the application
        // is not restarted.
        primarySwatch: Colors.blue,
      ),
      home: const HomePage(title: "Processing Block Demo"),
    );
  }
}

class HomePage extends StatefulWidget {
  const HomePage({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final ImagePicker picker = ImagePicker();
  late FacerecService service;
  late ProcessingBlock faceDetector;
  late ProcessingBlock fitter;
  late ProcessingBlock qualityControl;
  late ProcessingBlock ageEstimator;
  late ProcessingBlock genderEstimator;
  late ProcessingBlock emotionEstimator;
  late ProcessingBlock maskEstimator;
  late ProcessingBlock eyeOpennesEstimator;
  late ProcessingBlock glassesEstimator;
  late ProcessingBlock deepfakeEstimator;
  late ProcessingBlock livenessEstimator;

  Image? pickedImage;
  Text? currentStateText;
  bool isInitialized = false;

  Future<void> _pickImage() async {
    final XFile? file = await picker.pickImage(source: ImageSource.gallery);

    setState(() {
      pickedImage = null;
    });

    if (file == null) {
      setState(() {
        currentStateText = const Text("No image");
      });

      return;
    }

    setState(() {
      currentStateText = const Text("Processing image...");
    });

    final Uint8List bytes = await file.readAsBytes();

    final Context data = service.createContextFromEncodedImage(bytes);
    final image_width = data["image"]["shape"][1].get_value();
    final image_height = data["image"]["shape"][0].get_value();

    faceDetector.process(data);

    if (data["objects"].len() == 0) {
      setState(() {
        currentStateText = const Text("No faces");
      });

      return;
    }

    fitter.process(data);
    qualityControl.process(data);
    ageEstimator.process(data);
    genderEstimator.process(data);
    emotionEstimator.process(data);
    maskEstimator.process(data);
    eyeOpennesEstimator.process(data);
    glassesEstimator.process(data);
    deepfakeEstimator.process(data);

    if (data["objects"].len() == 1) {
      livenessEstimator.process(data);
    }

    Context objects = data["objects"];
    Completer<ui.Image> completer = Completer();
    Map<BBox, TextInformation> results = {};

    ui.decodeImageFromList(bytes, (image) => completer.complete(image));

    for (int i = 0; i < objects.len(); i++) {
      Context object = objects[i];
      Context bbox = object["bbox"];

      double x1 = bbox[0].get_value() * image_width;
      double y1 = bbox[1].get_value() * image_height;
      double x2 = bbox[2].get_value() * image_width;
      double y2 = bbox[3].get_value() * image_height;

      results[BBox(x1: x1, y1: y1, x2: x2, y2: y2)] = TextInformation(object);
    }

    PictureRecorder recorder = PictureRecorder();
    Canvas canvas = Canvas(recorder);

    final Paint paint = Paint()
      ..color = Colors.red
      ..style = PaintingStyle.stroke
      ..strokeWidth = (image_width * 0.005).clamp(8.0, 20.0);

    canvas.drawImage(await completer.future, const Offset(0.0, 0.0), Paint());

    final fontSize = (image_width * 0.018).clamp(12.0, 70.0);

    final textStyle = TextStyle(
      color: Colors.white,
      fontSize: fontSize,
    );

    final bgPaint = Paint()
      ..color = Colors.black.withValues(alpha: 0.7);

    List<Rect> occupiedRects = [];

    results.forEach((bbox, textInformation) {
      final painter = TextPainter(
        textDirection: TextDirection.ltr,
      );

      final info = textInformation.getInfo();

      painter.text = TextSpan(
        text: info,
        style: textStyle,
      );

      painter.layout(
        maxWidth: image_width * 0.25,
      );

      final double textWidth = painter.width + 10;
      final double textHeight = painter.height + 10;

      canvas.drawRect(
        Rect.fromLTRB(
          bbox.x1,
          bbox.y1,
          bbox.x2,
          bbox.y2,
        ),
        paint,
      );

      double textX = bbox.x1;
      double textY = bbox.y1 - textHeight - 8;
      bool placeBelow = textY < 0;

      late Rect rect;

      if (!placeBelow) {
        rect = Rect.fromLTWH(
          textX,
          textY,
          textWidth,
          textHeight,
        );

        bool overlaps = occupiedRects.any(
              (r) => r.overlaps(rect),
        );

        if (overlaps) {
          placeBelow = true;
        }
      }

      if (placeBelow) {
        textY = bbox.y2 + 8;

        rect = Rect.fromLTWH(
          textX,
          textY,
          textWidth,
          textHeight,
        );

        while (
        occupiedRects.any((r) => r.overlaps(rect))) {
          textY += textHeight + 8;

          rect = Rect.fromLTWH(
            textX,
            textY,
            textWidth,
            textHeight,
          );
        }
      }

      occupiedRects.add(rect);

      canvas.drawRRect(
        RRect.fromRectAndRadius(
          rect,
          const Radius.circular(8),
        ),
        bgPaint,
      );

      painter.paint(
        canvas,
        Offset(textX + 5, textY + 5),
      );
    });

    ByteData? pngBytes = await (await recorder.endRecording().toImage(image_width, image_height))
        .toByteData(format: ImageByteFormat.png);

    setState(() {
      pickedImage = Image.memory(Uint8List.view(pngBytes!.buffer));
      currentStateText = null;
    });

    data.dispose();
  }

  @override
  Widget build(BuildContext context) {
    Widget child;
    void Function()? onPressed;

    if (pickedImage != null) {
      child = pickedImage!;
    } else if (currentStateText != null) {
      child = currentStateText!;
    } else {
      child = const Card();
    }

    if (isInitialized) {
      onPressed = _pickImage;
    } else {
      onPressed = null;
    }

    // This method is rerun every time setState is called, for instance as done
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the HomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text(widget.title),
      ),
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: InteractiveViewer(child: child),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: onPressed,
        tooltip: "Pick image",
        child: const Icon(Icons.image),
      ), // This trailing comma makes auto-formatting nicer for build methods.
    );
  }

  @override
  void initState() {
    super.initState();

    setState(() {
      currentStateText = const Text("Initializing...");
      isInitialized = false;
    });

    initializeFaceSDK();
  }

  @override
  void dispose() {
    faceDetector.dispose();
    fitter.dispose();
    qualityControl.dispose();
    ageEstimator.dispose();
    genderEstimator.dispose();
    emotionEstimator.dispose();
    maskEstimator.dispose();
    eyeOpennesEstimator.dispose();
    glassesEstimator.dispose();
    deepfakeEstimator.dispose();
    livenessEstimator.dispose();
    service.dispose();

    super.dispose();
  }

  Future<void> initializeFaceSDK() async {
    service = await FaceSdkPlugin.createFacerecService();

    faceDetector = service.createProcessingBlock({
      "unit_type": "FACE_DETECTOR",
      "modification": "ssyv_light",
      "version": 1
    });
    fitter = service.createProcessingBlock({
      "unit_type": "FACE_FITTER",
      "modification": "fda"
    });
    qualityControl = service.createProcessingBlock({
      "unit_type": "QUALITY_CONTROL",
      "modification": "core",
      "mode": ["recognition"]
    });
    ageEstimator = service.createProcessingBlock({
      "unit_type": "AGE_ESTIMATOR",
      "modification": "light",
      "version": 2
    });
    genderEstimator = service.createProcessingBlock({
      "unit_type": "GENDER_ESTIMATOR",
      "modification": "light",
      "version": 2
    });
    emotionEstimator = service.createProcessingBlock({
      "unit_type": "EMOTION_ESTIMATOR",
      "modification": "heavy",
      "version": 1
    });
    maskEstimator = service.createProcessingBlock({
      "unit_type": "MASK_ESTIMATOR",
      "modification": "light",
      "version": 2
    });
    eyeOpennesEstimator = service.createProcessingBlock({
      "unit_type": "EYE_OPENNESS_ESTIMATOR",
      "modification": "light",
      "version": 1
    });
    glassesEstimator = service.createProcessingBlock({
      "unit_type": "GLASSES_ESTIMATOR",
      "modification": "sunglasses",
      "version": 1
    });
    deepfakeEstimator = service.createProcessingBlock({
      "unit_type": "DEEPFAKE_ESTIMATOR",
      "modification": "egn",
      "version": 3
    });
    livenessEstimator = service.createProcessingBlock({
      "unit_type": "LIVENESS_ESTIMATOR",
      "modification": "2d_ensemble_light",
      "version": 4
    });

    setState(() {
      currentStateText = const Text("Initializing finished");
      isInitialized = true;
    });
  }
}