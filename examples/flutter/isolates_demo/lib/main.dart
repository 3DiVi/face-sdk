import 'dart:async';
import 'dart:io';
import 'dart:math' as math;
import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:flutter/services.dart';
import 'package:flutter_phoenix/flutter_phoenix.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';

import 'photo.dart';
import 'video.dart';
import 'routes.dart';
import 'home.dart';

late List<CameraDescription> cameras;

Future<Null> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  
  try {
    cameras = await availableCameras();
  } on CameraException catch (e) {
    print("Error: ${e.code}\nError Message: ${e.description}");
  }

  runApp(Phoenix(child: new MyApp()));

  SystemChrome.setPreferredOrientations([
    DeviceOrientation.portraitUp,
  ]);
}

class MyApp extends StatefulWidget {
  String get nextRoute => "/";

  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  FacerecService? _service;
  Context? _template;
  Image? _idPhoto;
  Context? _liveTemplate;
  Image? _livePhoto;
  double _matchScore = 0;
  double _matchDistance = 0;
  double _matchFar = 0;
  double _matchFrr = 0;
  bool? _livenessPassed;
  AsyncProcessingBlock? _templateExtractor;
  AsyncProcessingBlock? _quality_control;
  AsyncProcessingBlock? verification;
  AsyncProcessingBlock? _face_crop;
  double _liveMirror = 0;
  late Matrix4 rotate;

  Future<void> setService(FacerecService service) async {
    _service = service;
    _templateExtractor =
        await _service!.createAsyncProcessingBlock({"unit_type": "FACE_TEMPLATE_EXTRACTOR", "modification": "30"});
    _quality_control = await _service!.createAsyncProcessingBlock({
      "unit_type": "QUALITY_CONTROL",
      "modification": "core",
      "mode": ["recognition"]
    });
    verification =
        await _service!.createAsyncProcessingBlock({"unit_type": "VERIFICATION_MODULE", "modification": "30"});
    _face_crop = await _service!.createAsyncProcessingBlock({
        "unit_type": "UTILITY_MODULE", "modification": "face_cut",
        "cut_type": "FACE_CUT_BASE",
      });
  }

  void setIDTemplate(Context template, Image idPhoto) {
    _template = template;
    _idPhoto = idPhoto;
  }

  void setLiveTemplate(Context liveTemplate, Image livePhoto) {
    _liveTemplate = liveTemplate;
    _livePhoto = livePhoto;
  }

  Future<void> setLiveness(bool liveness, Context template, Image img, double mirror) async {
    _livenessPassed = liveness;
    _liveTemplate = template;
    _livePhoto = img;
    _liveMirror = mirror;
    if (Platform.isIOS) {
      rotate = Matrix4.rotationY(math.pi);
    } else {
      rotate = Matrix4.rotationX(_liveMirror * math.pi);
    }

    if (_template != null && _liveTemplate != null) {
      Context verificationData = _service!.createContext({"template1": _template!, "template2": _liveTemplate!});

      verification!.process(verificationData).whenComplete(() {
        Context result = verificationData["result"];

        _matchDistance = result["distance"].get_value();
        _matchFar = result["far"].get_value();
        _matchFrr = result["frr"].get_value();
        _matchScore = result["score"].get_value();

        verificationData.dispose();
      });
    } else {
      print("Templates not computed");
    }
  }

  Widget getFinalStatus(BuildContext context) {
    return new PopScope(
      child: Scaffold(
        body: Stack(
          children: [
            Center(
                child: Text(
                    "Match score: $_matchScore\nIs same persons: ${_matchScore > 0.9 ? "true" : "false"} \nLiveness passed: $_livenessPassed")),
            Container(
              height: 150,
              width: 500,
              margin: EdgeInsets.symmetric(horizontal: 5.0, vertical: 30.0),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Expanded(
                    child: Transform(
                      alignment: Alignment.center,
                      transform: Matrix4.rotationY(math.pi),
                      child: Center(child: _idPhoto),
                    ),
                  ),
                  Expanded(
                    child: Transform(
                      alignment: Alignment.center,
                      transform: rotate,
                      child: Center(child: _livePhoto),
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
        floatingActionButton: FloatingActionButton(
          heroTag: "btn6",
          child: Icon(Icons.navigate_next),
          onPressed: () {
            setState(() {
              _templateExtractor?.dispose();
              _quality_control?.dispose();
              _template?.dispose();
              _liveTemplate?.dispose();
              verification?.dispose();
              _face_crop?.dispose();

              Phoenix.rebirth(context);
            });
          },
        ),
        floatingActionButtonLocation: FloatingActionButtonLocation.endFloat,
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: "Face SDK example",
      onGenerateRoute: (settings) {
        Widget page;
        if (settings.name == routeHome) {
          page = HomePage(cameras, detectIDRoute, setService);
        } else if (settings.name == detectIDRoute) {
          page = DetectPicture(cameras, _service!, _templateExtractor!, "Take a photo of the document", livenessRoute,
              setIDTemplate, _quality_control!, _face_crop!);
        } else if (settings.name == livenessRoute) {
          page = VideoProcessing(cameras, _service!, _templateExtractor!, _quality_control!, _face_crop!, endRoute, setLiveness);
        } else if (settings.name == endRoute) {
          page = getFinalStatus(context);
        } else
          page = const Text("Wrong route");

        return MaterialPageRoute<dynamic>(
          builder: (context) {
            return Material(child: page);
          },
          settings: settings,
        );
      },
      initialRoute: "/",
      theme: ThemeData(
        hintColor: Colors.blue, // hintColor => accentColor
        brightness: Brightness.dark,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
    );
  }
}
