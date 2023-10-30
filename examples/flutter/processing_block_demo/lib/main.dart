import 'dart:async';
import 'dart:ui';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:image/image.dart' as image_lib;
import 'dart:ui' as ui;

import 'package:image_picker/image_picker.dart';

import 'package:face_sdk_3divi/face_sdk_3divi.dart';

import 'loading.dart';
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
  late ProcessingBlock qualityAssessment;
  late ProcessingBlock qualityEstimator;

  Image? pickedImage;
  Text? currentStateText;
  bool isInitialized = false;

  image_lib.Decoder? _createDecoder(String fileName) {
    if (fileName.endsWith(".png")) {
      return image_lib.PngDecoder();
    }

    List<String> jpgExtensions = [".jpg", ".jpeg", ".JPG", ".JPEG", ".jpe", ".JPE"];

    for (String extension in jpgExtensions) {
      if (fileName.endsWith(extension)) {
        return image_lib.JpegDecoder();
      }
    }

    return null;
  }

  void _pickImage() async {
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

    final image_lib.Decoder? decoder = _createDecoder(file.path);

    if (decoder == null) {
      setState(() {
        currentStateText = const Text("Not supported image format");
      });

      return;
    }

    setState(() {
      currentStateText = const Text("Processing image...");
    });

    final Uint8List bytes = await file.readAsBytes();

    final ImageDescriptor descriptor = await ImageDescriptor.encoded(await ImmutableBuffer.fromUint8List(bytes));
    final Context data = service.createContext({
      "objects": [],
      "image": {
        "blob": decoder.decodeImage(bytes)!.getBytes(format: image_lib.Format.rgb),
        "dtype": "uint8_t",
        "format": "NDARRAY",
        "shape": [descriptor.height, descriptor.width, 3]
      }
    });

    Context result = faceDetector.process(data);

    if (result["objects"].len() == 0) {
      setState(() {
        currentStateText = const Text("No faces");
      });

      return;
    }

    result = fitter.process(result);
    result = qualityAssessment.process(result);
    result = qualityEstimator.process(result);

    Context objects = result["objects"];
    Completer<ui.Image> completer = Completer();
    Map<BBox, TextInformation> results = {};

    ui.decodeImageFromList(bytes, (image) => completer.complete(image));

    for (int i = 0; i < objects.len(); i++) {
      Context object = objects[i];
      Context bbox = object["bbox"];
      Context quality = object["quality"];

      double x1 = bbox[0].get_value() * descriptor.width;
      double y1 = bbox[1].get_value() * descriptor.height;
      double x2 = bbox[2].get_value() * descriptor.width;
      double y2 = bbox[3].get_value() * descriptor.height;

      results[BBox(x1: x1, y1: y1, x2: x2, y2: y2)] = TextInformation(quality, quality["total_score"].get_value());
    }

    PictureRecorder recorder = PictureRecorder();
    Canvas canvas = Canvas(recorder);

    final Paint paint = Paint()
      ..color = Colors.red
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2.0;

    canvas.drawImage(await completer.future, const Offset(0.0, 0.0), Paint());

    results.forEach((bbox, textInformation) {
      TextPainter painter = TextPainter()..textDirection = TextDirection.ltr;
      StringBuffer buffer = StringBuffer();

      buffer.write("Quality: ${textInformation.getTotalQualityScoreWithPrecision()}\n");
      buffer.write(textInformation.getQualityAssessmentResults());

      canvas.drawRect(Rect.fromLTRB(bbox.x1, bbox.y1, bbox.x2, bbox.y2), paint);

      painter.text = TextSpan(text: buffer.toString(), style: const TextStyle(color: Colors.red));

      painter.layout();

      painter.paint(canvas, Offset(bbox.x2, bbox.y1));
    });

    ByteData? pngBytes = await (await recorder.endRecording().toImage(descriptor.width, descriptor.height))
        .toByteData(format: ImageByteFormat.png);

    setState(() {
      pickedImage = Image.memory(Uint8List.view(pngBytes!.buffer));
      currentStateText = null;
    });
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

    getDirs().then((dirs) => createService(dirs[0], dirs[1]));
  }

  void createService(String dataDir, String libDir) {
    service = FaceSdkPlugin.createFacerecService("$dataDir/conf/facerec", "$dataDir/license",
        libPath: "$libDir/${FaceSdkPlugin.nativeLibName}");

    faceDetector = service.createProcessingBlock(
        {"unit_type": "FACE_DETECTOR", "modification": "uld", "model_path": "", "@sdk_path": dataDir, "min_size": 50});
    fitter = service
        .createProcessingBlock({"unit_type": "FACE_FITTER", "modification": "tddfa_faster", "@sdk_path": dataDir});
    qualityAssessment = service.createProcessingBlock({
      "unit_type": "QUALITY_ASSESSMENT_ESTIMATOR",
      "modification": "assessment",
      "config_name": "quality_assessment.xml",
      "@sdk_path": dataDir
    });
    qualityEstimator = service.createProcessingBlock(
        {"unit_type": "QUALITY_ASSESSMENT_ESTIMATOR", "modification": "estimation", "@sdk_path": dataDir});

    setState(() {
      currentStateText = const Text("Initializing finished");
      isInitialized = true;
    });
  }
}
