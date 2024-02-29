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
late String dataDir;

Future<String> loadAsset() async {
  final manifestContent = await rootBundle.loadString('AssetManifest.json');
  final Map<String, dynamic> manifestMap = jsonDecode(manifestContent);
  Directory doc_directory = await getApplicationDocumentsDirectory();
  for (String key in manifestMap.keys) {
    var dbPath = doc_directory.path + '/' + key;
    if (FileSystemEntity.typeSync(dbPath) == FileSystemEntityType.notFound ||
        dbPath.contains('conf/facerec') || dbPath.contains('license')) {
      ByteData data = await rootBundle.load(key);
      List<int> bytes = data.buffer.asUint8List(
          data.offsetInBytes, data.lengthInBytes);
      File file = File(dbPath);
      file.createSync(recursive: true);
      await file.writeAsBytes(bytes);
    }
  }
  return doc_directory.path + '/assets';
}

Future<Null> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  try {
    cameras = await availableCameras();
  } on CameraException catch (e) {
    print('Error: $e.code\nError Message: $e.message');
  }
  dataDir = await loadAsset();
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
  FacerecService? _facerecService;
  Template? _idTempl;
  Image? _idPhoto;
  Template? _liveTempl;
  Image? _livePhoto;
  MatchResult? _match;
  bool? _livenessPassed;
  Recognizer? _recognizer;
  double _liveMirror = 0;
  late Matrix4 rotate;

  setService(service){
    _facerecService = service;
    _recognizer = _facerecService!.createRecognizer("method10v30_recognizer.xml");
  }

  setIDTemplate(idTempl, idPhoto){
    _idTempl = idTempl;
    _idPhoto = idPhoto;
  }

  setLiveTemaplte(liveTempl, livePhoto){
    _liveTempl = liveTempl;
    _livePhoto = livePhoto;
  }

  setMatchScore(matchRes){
    _match = matchRes;
  }

  setLivenss(liveness, template, img, mirror){
    _livenessPassed = liveness;
    _liveTempl = template;
    _livePhoto = img;
    _liveMirror = mirror;
    if (Platform.isIOS){
      rotate = Matrix4.rotationY(math.pi);
    }else{
      rotate = Matrix4.rotationX(_liveMirror * math.pi);
    }

    setState(() {
      if(_idTempl != null && _liveTempl != null) {
        _match = _recognizer?.verifyMatch(_idTempl!, _liveTempl!);
      }
      else
        print("Templates not computed");
    });
  }

  Widget getFinalStatus(BuildContext context){
    return new WillPopScope(
        onWillPop: () async => false,
        child: Scaffold(
          body: Stack(
            children: [
              Center(child: Text("Match score: ${_match?.score}\nIs same persons: ${_match!.score > 0.9 ? "true" : "false" } \nLiveness passed: $_livenessPassed")),
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
              //Center(child: Center(child: _livePhoto)),
            ],
          ),
          floatingActionButton: FloatingActionButton(
            heroTag: "btn6",
            child: Icon(Icons.navigate_next),
            onPressed: (){setState(() {
              _recognizer!.dispose();
              Phoenix.rebirth(context);
            });},
          ),
          floatingActionButtonLocation: FloatingActionButtonLocation.endFloat,
        ),


    );
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Face SDK example',
      onGenerateRoute: (settings){
        Widget page;
        if (settings.name == routeHome) {
          page = HomePage(cameras, dataDir, detectIDRoute, setService);
        }
        else if (settings.name == detectIDRoute){
          page = DetectPicture(cameras, _facerecService!, _recognizer!, 'Take a photo of the document', livenessRoute, setIDTemplate);
        }
        else if (settings.name == livenessRoute){
          page = VideoProcessing(cameras, _facerecService!, _recognizer!, endRoute, setLivenss);
        }
        else if (settings.name == endRoute){
          page = getFinalStatus(context);
        }
        else
          page = Text('Wrong route');

        return MaterialPageRoute<dynamic>(
          builder: (context) {
            return Material(child: page);
          },
          settings: settings,
        );
      },
      // debugShowCheckedModeBanner: false,
      initialRoute: '/',
      theme: ThemeData(
        hintColor: Colors.blue, // hintColor => accentColor
        brightness: Brightness.dark,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
    );
  }
}
