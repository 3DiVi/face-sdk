import 'dart:ui';
import 'dart:io';
import 'dart:async';
import "dart:typed_data";
import 'dart:math' as math;
import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';
import 'package:face_sdk_3divi/utils.dart';

import 'bndbox.dart';


void logError(String code, String message) {
  if (message != null) {
    print('Error: $code\nError Message: $message');
  } else {
    print('Error: $code');
  }
}

typedef void setTempalte(Template templ, Image photo);


class DetectPicture extends StatefulWidget {
  final List<CameraDescription> cameras;
  final FacerecService _facerecService;
  Recognizer _recognizer;

  final setTempalte callback;
  final String descriprion;
  final String nextRoute;

  DetectPicture(this.cameras, this._facerecService, this._recognizer,
      this.descriprion, this.nextRoute, this.callback);

  @override
  _DetectPictureState createState() => new _DetectPictureState();
}



class _DetectPictureState extends State<DetectPicture> {
  late CameraController controller;
  bool isDetecting = false;
  late Capturer _capturer;
  Image? _lastImage;
  late Image _cropImg;
  late var fornatal;
  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey<ScaffoldState>();
  GlobalKey _pictureKey = GlobalKey();
  Offset widgetPosition = Offset(0, 0);
  double widthPreviewImage = 0;
  double heightPreviewImage = 0;
  Size? widgetSize;
  int currentCameraIndex = -1;
  List<Template> templs = [];
  List<dynamic> _recognitions = [];

  void showInSnackBar(String message) {
    // ignore: deprecated_member_use
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(message), behavior: SnackBarBehavior.floating,));
  }

  Future<XFile?> takePicture() async {
    final CameraController cameraController = controller;
    if (cameraController == null || !cameraController.value.isInitialized) {
      showInSnackBar('Error: select a camera first.');
      return null;
    }

    if (cameraController.value.isTakingPicture) {
      // A capture is already pending, do nothing.
      return null;
    }

    try {
      XFile file = await cameraController.takePicture();
      return file;
    } on CameraException catch (e) {
      _showCameraException(e);
      return null;
    }
  }

  void onTakePictureButtonPressed() {
    final RenderBox renderBox = _pictureKey.currentContext?.findRenderObject() as RenderBox;
    widgetPosition = renderBox.localToGlobal(Offset.zero);
    widgetSize = renderBox.size;

    takePicture().then((XFile? file) async {

      if (mounted) {
        if (file != null && _capturer != null) {
          final Uint8List img_bytes = File(file.path).readAsBytesSync();
          _lastImage = Image.memory(img_bytes);
          var img = await decodeImageFromList(img_bytes);

          List<RawSample> rss = _capturer.capture(img_bytes);

          List<dynamic>  dets = [];
          if (rss != null && rss.length > 0) {
            for(var i = 0; i < rss.length; i+=1) {
              final rect = rss[i].getRectangle();

              widthPreviewImage = widgetSize!.width < img.width ? widgetSize!.width : img.width.toDouble();
              heightPreviewImage = widgetSize!.height < img.height ? widgetSize!.height : img.height.toDouble();

              dets.add({
                "rect": {"x": rect.x, "y": rect.y, "w": rect.width, "h": rect.height},
                "widget": {"w": widthPreviewImage, "h": heightPreviewImage},
                "picture": {"w": img.width, "h": img.height}
              });
              _cropImg = await cutFaceFromImageBytes(img_bytes, rect);
              if (rss.length ==  1) {
                templs.add(widget._recognizer.processing(rss[i]));
                widget.callback(templs[0], _cropImg);
                controller.dispose();
              }
              rss[i].dispose();
            }
            if (rss.length > 1){
              showInSnackBar("Photo will be skipped (for verification), because multiple faces detected");
              _lastImage = null;
            }
            setState(() {
              _recognitions = dets;
            });
          }
          else
            showInSnackBar("No faces found in the image");
        }
        if (file != null) File(file.path).delete();
      }
    });
  }

  void _showCameraException(CameraException e) {
    logError(e.code, e.description!);
    showInSnackBar('Error: ${e.code}\n${e.description}');
  }

  void changeCamera(){
    currentCameraIndex += 1;
    currentCameraIndex %= math.min(2, widget.cameras.length);
    controller = new CameraController(
      widget.cameras[currentCameraIndex],
      ResolutionPreset.high,
    );

    controller.initialize().then((_) {
      if (!mounted) {
        return;
      }
      setState(() {});
    });

  }

  @override
  void initState() {
    super.initState();

    _capturer = widget._facerecService.createCapturer(Config("common_capturer_blf_fda_front.xml"));

    if (widget.cameras == null || widget.cameras.length < 1) {
      print('No camera is found');
    } else {
      changeCamera();
    }
  }

  Widget livePreview(BuildContext context){
    return Scaffold(
        key: _scaffoldKey,
        appBar: AppBar(
          title: Text(widget.descriprion),
        ),
        body: Column(
          children: <Widget>[
            Expanded(
              child: Center(
                child: Padding(
                    child: CameraPreview(controller, child: Text(" ",key: _pictureKey, ),),
                    padding: const EdgeInsets.all(1.0)
                ),
              ),
            ),
            Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                mainAxisSize: MainAxisSize.max,
                children: <Widget>[
                  FloatingActionButton(
                    heroTag: "btn1",
                    child: Icon(Icons.camera_alt),
                    // color: Colors.blue,
                    onPressed: controller != null && controller.value.isInitialized
                        ? onTakePictureButtonPressed
                        : null,
                  ),
                  FloatingActionButton(
                    heroTag: "btn2",
                    child: const Icon(Icons.flip_camera_android),
                    // color: Colors.blue,
                    onPressed: controller != null && controller.value.isInitialized
                        ? (){changeCamera();}
                        : null,
                  ),
                ]
            )
          ],
        )
    );
  }


  Widget imagePreview(BuildContext context){
    return Scaffold(
        key: _scaffoldKey,
        appBar: AppBar(
          title: const Text('Detected faces'),
        ),
        body: Column(
          children: <Widget>[
            Expanded(
              child: Padding(
                child: Stack(
                  children: [
                    Center(
                      child: Container(
                        width: widthPreviewImage,
                        height: heightPreviewImage,
                        margin: const EdgeInsets.only(top: 0),
                        child: Center(child: _lastImage)
                      ),

                    ),
                    Center(
                      child: Container(
                        width: widthPreviewImage,
                        height: heightPreviewImage,
                        margin: const EdgeInsets.only(top: 0),
                        child: BndBox(
                          _recognitions,
                          widgetPosition.dx == null ? 0 : widgetPosition.dx,
                          widgetPosition.dy == null ? 0 : widgetPosition.dy,
                        ),
                      ),
                    ),
                  ],
                ),
                padding: const EdgeInsets.all(1.0)
              ),
            ),
          ],
        ),
        floatingActionButton: FloatingActionButton(
          heroTag: "btn3",
          child: templs.length == 0 ? Icon(Icons.settings_backup_restore) : Icon(Icons.navigate_next),
          // color: Colors.blue,
          onPressed: (){setState(() {
            _recognitions = [];
            if (templs.length == 1){
              Navigator.of(context).pop();
              Navigator.of(context).pushNamed(widget.nextRoute);
            }
          });},
        ),
      floatingActionButtonLocation: FloatingActionButtonLocation.endFloat,
    );
  }

  @override
  Widget build(BuildContext context) {
    if (controller == null || !controller.value.isInitialized) {
      return Container();
    }
    return _lastImage == null ? livePreview(context): imagePreview(context);
  }

  @override
  void dispose() {
    controller.dispose();
    _capturer.dispose();
    super.dispose();
  }
}
