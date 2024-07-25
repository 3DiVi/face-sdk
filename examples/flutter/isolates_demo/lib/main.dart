import 'dart:async';
import 'dart:ui';
import 'dart:io';
import 'dart:math' as math;
import 'dart:convert';
import "dart:typed_data";
import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'package:flutter/services.dart';
import 'package:path_provider/path_provider.dart';
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
  MatchResult? _match;
  bool? _livenessPassed;
  AsyncProcessingBlock? _templateExtractor;
  AsyncProcessingBlock? _qaa;
  AsyncProcessingBlock? verification;
  double _liveMirror = 0;
  late Matrix4 rotate;

  Future<void> setService(FacerecService service) async {
    _service = service;
    _templateExtractor =
        await _service!.createAsyncProcessingBlock({"unit_type": "FACE_TEMPLATE_EXTRACTOR", "modification": "30"});
    _qaa = await _service!.createAsyncProcessingBlock({
      "unit_type": "QUALITY_ASSESSMENT_ESTIMATOR",
      "modification": "assessment",
      "config_name": "quality_assessment.xml"
    });
    verification =
        await _service!.createAsyncProcessingBlock({"unit_type": "VERIFICATION_MODULE", "modification": "30"});
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

        _match = MatchResult(result["distance"].get_value(), result["far"].get_value(), result["frr"].get_value(),
            result["score"].get_value());

        verificationData.dispose();
      });
    } else {
      print("Templates not computed");
    }
  }

  Widget getFinalStatus(BuildContext context) {
    return new WillPopScope(
      onWillPop: () async => false,
      child: Scaffold(
        body: Stack(
          children: [
            Center(
                child: Text(
                    "Match score: ${_match?.score}\nIs same persons: ${_match!.score > 0.9 ? "true" : "false"} \nLiveness passed: $_livenessPassed")),
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
              _qaa?.dispose();
              _template?.dispose();
              _liveTemplate?.dispose();
              verification?.dispose();

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
              setIDTemplate, _qaa!);
        } else if (settings.name == livenessRoute) {
          page = VideoProcessing(cameras, _service!, _templateExtractor!, _qaa!, endRoute, setLiveness);
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
