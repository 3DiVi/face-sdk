import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:camera/camera.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';

typedef Future<void> setServiceCallback(FacerecService service);

class HomePage extends StatefulWidget {
  final List<CameraDescription> cameras;
  final String _dataDir;
  final setServiceCallback _setService;
  final String nextRoute;

  HomePage(this.cameras, this._dataDir, this.nextRoute, this._setService);

  @override
  _HomePageState createState() => new _HomePageState();
}

class _HomePageState extends State<HomePage> {
  String _libDir = "";
  static const platform = const MethodChannel('samples.flutter.dev/facesdk');
  late FacerecService _service;
  bool _loading = true;

  Future<void> getLibDir() async {
    String libDir = "None";
    try {
      final String res = await platform.invokeMethod('getNativeLibDir');
      libDir = res;
    } on PlatformException catch (e) {}
    setState(() {
      _libDir = libDir;
    });
  }

  Future<void> createService() async {
    if (widget._dataDir == '' || _libDir == '') {
      return;
    }
    _service = FaceSdkPlugin.createFacerecService(widget._dataDir + "/conf/facerec", widget._dataDir + "/license",
        libPath: _libDir + "/" + FaceSdkPlugin.nativeLibName);

    await widget._setService(_service);

    setState(() {
      _loading = false;
    });
  }

  @override
  void initState() {
    super.initState();
    getLibDir().whenComplete(() async {
      await createService();
    });
  }

  @override
  Widget build(BuildContext context) {
    return _loading
        ? CircularProgressIndicator()
        : Scaffold(
            body: Center(
                child: Column(mainAxisAlignment: MainAxisAlignment.center, children: <Widget>[
            ElevatedButton(
                child: const Text("Start"),
                onPressed: () {
                  Navigator.of(context).pushNamed(widget.nextRoute);
                })
          ])));
  }

  @override
  void dispose() {
    super.dispose();
  }
}
