import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:camera/camera.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';

typedef Future<void> setServiceCallback(FacerecService service);

class HomePage extends StatefulWidget {
  final List<CameraDescription> cameras;
  final setServiceCallback _setService;
  final String nextRoute;

  HomePage(this.cameras, this.nextRoute, this._setService);

  @override
  _HomePageState createState() => new _HomePageState();
}

class _HomePageState extends State<HomePage> {
  late FacerecService _service;
  bool _loading = true;

  Future<void> createService() async {
    _service = await FaceSdkPlugin.createFacerecService();

    await widget._setService(_service);

    setState(() {
      _loading = false;
    });
  }

  @override
  void initState() {
    super.initState();
    
    createService();
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
