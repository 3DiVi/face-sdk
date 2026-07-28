import 'dart:io';
import 'dart:async';
import "dart:typed_data";
import 'dart:math' as math;
import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';
import 'package:face_sdk_3divi/utils.dart';
import 'package:image/image.dart' as image_lib;

import 'bndbox.dart';
import 'utils.dart';

void logError(String code, String message) {
    print('Error: $code\nError Message: $message');
}

typedef void SetTemplate(Context template, Image photo);

class DetectPicture extends StatefulWidget {
  final List<CameraDescription> cameras;
  final FacerecService _service;
  final AsyncProcessingBlock templateExtractor;
  final AsyncProcessingBlock quality_control;
  final AsyncProcessingBlock face_crop;

  final SetTemplate callback;
  final String description;
  final String nextRoute;

  DetectPicture(
      this.cameras, this._service, this.templateExtractor, this.description,
      this.nextRoute, this.callback, this.quality_control, this.face_crop);

  @override
  _DetectPictureState createState() => new _DetectPictureState();
}

class _DetectPictureState extends State<DetectPicture> {
  late CameraController controller;
  bool isDetecting = false;
  AsyncProcessingBlock? _face_detector;
  AsyncProcessingBlock? _face_fitter;
  Image? _lastImage;
  late Image _cropImg;
  late var frontal;
  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey<ScaffoldState>();
  GlobalKey _pictureKey = GlobalKey();
  Offset widgetPosition = Offset(0, 0);
  double widthPreviewImage = 0;
  double heightPreviewImage = 0;
  Size? widgetSize;
  int currentCameraIndex = -1;
  bool hasTemplate = false;
  List<dynamic> _recognitions = [];
  final image_lib.JpegDecoder decoder = image_lib.JpegDecoder();

  void showInSnackBar(String message) {
    // ignore: deprecated_member_use
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      content: Text(message),
      behavior: SnackBarBehavior.floating,
    ));
  }

  Future<XFile?> takePicture() async {
    final CameraController cameraController = controller;
    if (!cameraController.value.isInitialized) {
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
        if (file != null && _face_detector != null) {
          final Uint8List imageBytes = File(file.path).readAsBytesSync();
          _lastImage = Image.memory(imageBytes);
          var img = await decodeImageFromList(imageBytes);
          Context ioData =
            widget._service.createContextFromEncodedImage(imageBytes);

          await _face_detector?.process(ioData);

          var ctxObjects = ioData["objects"];

          if (ctxObjects.len() == 0)
          {
            showInSnackBar("No faces found in the image");
            ioData.dispose();
            _lastImage = null;
            return;
          }

          List<dynamic> dets = [];
          for (var i = 0; i < ctxObjects.len(); i += 1)
          {
            var bbox = ctxObjects[i]["bbox"];
            var x1 = bbox[0].get_value() * img.width;
            var y1 = bbox[1].get_value() * img.height;
            var x2 = bbox[2].get_value() * img.width;
            var y2 = bbox[3].get_value() * img.height;

            widthPreviewImage = math.min(widgetSize!.width, img.width.toDouble());
            heightPreviewImage = math.min(widgetSize!.height, img.height.toDouble());

            dets.add({
              "rect": {"x": x1, "y": y1, "w": x2 - x1, "h": y2 - y1},
              "widget": {"w": widthPreviewImage, "h": heightPreviewImage},
              "picture": {"w": img.width, "h": img.height}
            });
          }

          if (ctxObjects.len() == 1)
          {
            await _face_fitter?.process(ioData);
            await widget.quality_control.process(ioData);
            Context object = ctxObjects[0];

            if (!object["quality"]["value"].get_value())
            {

              showInSnackBar(getMessageForQualityControl(object["quality"]));
              ioData.dispose();
              return;
            }
            await widget.face_crop.process(ioData);
            _cropImg = await getFaceCutImageFromContext(object["face_crop"]);

            await widget.templateExtractor.process(ioData);

            widget.callback(widget._service.createContext(object["face_template"]), _cropImg);

            hasTemplate = true;

            controller.dispose();

          }
          else
          {
            showInSnackBar("Photo will be skipped (for verification), because multiple faces detected");
            _lastImage = null;
          }

          setState(() {
            _recognitions = dets;
          });
          ioData.dispose();
        }
        if (file != null) File(file.path).delete();
      }
    });
  }

  void _showCameraException(CameraException e) {
    logError(e.code, e.description!);
    showInSnackBar('Error: ${e.code}\n${e.description}');
  }

  void changeCamera() {
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

    widget._service.createAsyncProcessingBlock({
      "unit_type": "FACE_DETECTOR", "modification": "ssyv_light"
    }).then((value) => _face_detector = value);

    widget._service.createAsyncProcessingBlock({
      "unit_type": "FACE_FITTER", "modification": "fda",
    }).then((value) => _face_fitter = value);

    if (widget.cameras.length < 1) {
      print('No camera is found');
    } else {
      changeCamera();
    }
  }

  Widget livePreview(BuildContext context) {
    return Scaffold(
        key: _scaffoldKey,
        appBar: AppBar(
          title: Text(widget.description),
        ),
        body: Column(
          children: <Widget>[
            Expanded(
              child: Center(
                child: Padding(
                    child: CameraPreview(
                      controller,
                      child: Text(
                        " ",
                        key: _pictureKey,
                      ),
                    ),
                    padding: const EdgeInsets.all(1.0)),
              ),
            ),
            Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, mainAxisSize: MainAxisSize.max, children: <Widget>[
              FloatingActionButton(
                heroTag: "btn1",
                child: Icon(Icons.camera_alt),
                // color: Colors.blue,
                onPressed: controller.value.isInitialized ? onTakePictureButtonPressed : null,
              ),
              FloatingActionButton(
                heroTag: "btn2",
                child: const Icon(Icons.flip_camera_android),
                // color: Colors.blue,
                onPressed: controller.value.isInitialized
                    ? () {
                        changeCamera();
                      }
                    : null,
              ),
            ])
          ],
        ));
  }

  Widget imagePreview(BuildContext context) {
    return Scaffold(
      key: _scaffoldKey,
      appBar: AppBar(
        title: const Text("Detected faces"),
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
                          child: Center(child: _lastImage)),
                    ),
                    Center(
                      child: Container(
                        width: widthPreviewImage,
                        height: heightPreviewImage,
                        margin: const EdgeInsets.only(top: 0),
                        child: BndBox(
                          _recognitions,
                          widgetPosition.dx,
                          widgetPosition.dy,
                        ),
                      ),
                    ),
                  ],
                ),
                padding: const EdgeInsets.all(1.0)),
          ),
        ],
      ),
      floatingActionButton: FloatingActionButton(
        heroTag: "btn3",
        child: hasTemplate ? Icon(Icons.navigate_next) : Icon(Icons.settings_backup_restore),
        // color: Colors.blue,
        onPressed: () {
          setState(() {
            _recognitions = [];
            if (hasTemplate) {
              Navigator.of(context).pop();
              Navigator.of(context).pushNamed(widget.nextRoute);
            }
          });
        },
      ),
      floatingActionButtonLocation: FloatingActionButtonLocation.endFloat,
    );
  }

  @override
  Widget build(BuildContext context) {
    if (!controller.value.isInitialized) {
      return Container();
    }
    return _lastImage == null ? livePreview(context) : imagePreview(context);
  }

  @override
  void dispose() {
    controller.dispose();
    _face_detector?.dispose();
    _face_fitter?.dispose();
    super.dispose();
  }
}
