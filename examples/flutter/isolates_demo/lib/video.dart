import 'dart:io';
import 'dart:typed_data';
import 'dart:ui' as ui;
import 'dart:async';
import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';
import 'package:face_sdk_3divi/utils.dart';
import 'package:vector_math/vector_math_64.dart' hide Colors;
import 'utils.dart';

typedef void setLivenessStatus(bool isPassed, Context template, Image image, double mirror);

class VideoProcessing extends StatefulWidget {
  final List<CameraDescription> cameras;
  final FacerecService _service;
  final AsyncProcessingBlock templateExtractor;
  final AsyncProcessingBlock quality_control;
  final AsyncProcessingBlock face_crop;
  final String nextRoute;
  final setLivenessStatus callback;

  VideoProcessing(this.cameras, this._service, this.templateExtractor,
      this.quality_control, this.face_crop, this.nextRoute, this.callback);

  @override
  _VideoProcessingState createState() => _VideoProcessingState();
}

class _VideoProcessingState extends State<VideoProcessing> {
  late CameraController controller;
  AsyncProcessingBlock? _tracking;
  AsyncProcessingBlock? _fitter;
  AsyncProcessingBlock? liveness;
  GlobalKey _pictureKey = GlobalKey();
  bool _isLivenessSet = false;
  int baseAngle = 0;
  String _livenessResult = "Processing";
  bool isReady = true;
  bool livenessPassed = false;
  Offset? widgetPosition;
  ui.Size? widgetSize;
  Widget? bboxWidget;

  bool _qualityVerdict = false;
  String _qualityMessage = "";
  late Vector3 bboxMirror;

  void showInSnackBar(String message) {
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      content: Text(message),
      behavior: SnackBarBehavior.floating,
    ));
  }

  Future<void> _processStream(CameraImage image) async {
    if (!isReady || _tracking == null || !mounted) {
      return;
    }

    isReady = false;

    try {
      final RenderBox? renderBox = _pictureKey.currentContext?.findRenderObject() as RenderBox;

      if (renderBox != null) {
        widgetPosition = renderBox.localToGlobal(Offset.zero);
        widgetSize = renderBox.size;
      }

      Context ioData = widget._service.createContextFromCameraImage(image, baseAngle);

      await _tracking!.process(ioData);

      if (ioData["objects"].len() > 0) {
        if (_isLivenessSet) {
          int imgW = ioData["image"]["shape"][1].get_value();
          int imgH = ioData["image"]["shape"][0].get_value();
          Context bbox = ioData["objects"][0]["bbox"];
          bboxWidget = buildBboxWidget(imgW, imgH, bbox);

          if (mounted) {
            setState(() {});
          }
          ioData.dispose();
          return;
        }

        if (_fitter != null) {
          await _fitter!.process(ioData);
          await widget.quality_control.process(ioData);

          Context obj = ioData["objects"][0];
          _qualityVerdict = obj["quality"]["value"].get_value();

          if (_qualityVerdict && liveness != null) {
            if (ioData["objects"].len() != 1) {
              ioData.dispose();
              return;
            }

            _qualityMessage = "";
            await liveness!.process(ioData);

            _isLivenessSet = true;

            if (obj["liveness"]["value"].get_value() == "REAL") {
              _livenessResult = "Real";
              livenessPassed = true;
            } else {
              _livenessResult = "Fake";
              livenessPassed = false;
            }

            await widget.templateExtractor.process(ioData);
            await widget.face_crop.process(ioData);

            Context template = widget._service.createContext(obj["face_template"]);

            final cropImg = await getFaceCutImageFromContext(obj["face_crop"]);

            widget.callback(livenessPassed, template, cropImg, 0.0);
          } else if (!_qualityVerdict) {
            final newMessage = getMessageForQualityControl(obj["quality"]);
            if (_qualityMessage != newMessage) {
              _qualityMessage = newMessage;
              showInSnackBar(_qualityMessage);
            }
          }
        }

        int imgW = ioData["image"]["shape"][1].get_value();
        int imgH = ioData["image"]["shape"][0].get_value();
        Context bbox = ioData["objects"][0]["bbox"];
        bboxWidget = buildBboxWidget(imgW, imgH, bbox);

        if (mounted) {
          setState(() {});
        }
      } else {
        bboxWidget = null;
      }

      ioData.dispose();
    } catch (e) {
      print("_processStream error: $e");
    } finally {
      isReady = true;
    }
  }

  @override
  void initState() {
    super.initState();
    if (widget.cameras.isEmpty) {
      print("No camera is found");
    } else {
      final CameraDescription camera = widget.cameras[1];

      controller = CameraController(camera, ResolutionPreset.high, enableAudio: false);
      controller.initialize().then((_) {
        if (!mounted) {
          return;
        }
        setState(() {});

        controller.startImageStream(_processStream);
      });
    }

    baseAngle = getBaseAngle(controller);

    bboxMirror = Platform.isIOS ? Vector3(1.0, 1.0, 1.0) : Vector3(-1.0, 1.0, 1.0);

    widget._service
        .createAsyncProcessingBlock({"unit_type": "LIVENESS_ESTIMATOR", "modification": "2d_light", "version": 1}).then(
            (value) => liveness = value);

    widget._service
        .createAsyncProcessingBlock({"unit_type": "FACE_TRACKING_MODULE", "modification": "fda"}).then(
            (value) => _tracking = value);

    widget._service
        .createAsyncProcessingBlock({"unit_type": "FACE_FITTER", "modification": "fda"}).then(
            (value) => _fitter = value);
  }

  Widget? buildBboxWidget(int imgW, int imgH, Context bbox) {
    if (widgetSize == null) {
      return null;
    }

    double scaleX = widgetSize!.width / imgW;
    double scaleY = widgetSize!.height / imgH;

    double x1 = bbox[0].get_value() * imgW;
    double y1 = bbox[1].get_value() * imgH;
    double x2 = bbox[2].get_value() * imgW;
    double y2 = bbox[3].get_value() * imgH;

    double widgetWidth = (x2 - x1) * scaleX;
    double widgetHeight = (y2 - y1) * scaleY;

    return Positioned(
        left: widgetPosition!.dx + x1 * scaleX,
        top: widgetPosition!.dy + y1 * scaleY,
        width: widgetWidth,
        height: widgetHeight,
        child: Stack(children: [
          Container(
            padding: EdgeInsets.only(top: 5.0, left: 5.0),
            decoration: BoxDecoration(
              border: Border.all(
                color: Color.fromRGBO(37, 213, 253, 1.0),
                width: 3.0,
              ),
            ),
          ),
          Transform.translate(
              key: const ValueKey<String>("Quality"),
              offset: Offset(0, widgetHeight),
              child: Text("Quality verdict: $_qualityVerdict",
                  style: TextStyle(fontSize: 20, backgroundColor: Colors.black))),
          Transform.translate(
              key: const ValueKey<String>("Liveness"),
              offset: Offset(0, widgetHeight + 25),
              child: Text(_livenessResult, style: TextStyle(fontSize: 20, backgroundColor: Colors.black)))
        ]));
  }

  Widget? bboxDrawer() {
    if (bboxWidget != null) {
      Stack stack = (bboxWidget as Positioned).child as Stack;

      Transform liveness =
      stack.children.firstWhere((element) => element.key == ValueKey<String>("Liveness")) as Transform;
      Transform quality =
      stack.children.firstWhere((element) => element.key == ValueKey<String>("Quality")) as Transform;

      (liveness.child as Text).data?.replaceRange(0, null, _livenessResult);
      (quality.child as Text).data?.replaceRange(0, null, "Quality score: $_qualityVerdict");
    }

    return bboxWidget;
  }

  @override
  Widget build(BuildContext context) {
    if (!controller.value.isInitialized) {
      return Container();
    }

    return PopScope(
        child: Scaffold(
          body: Stack(
            children: [
              Center(
                child: Padding(
                  key: _pictureKey,
                  padding: const EdgeInsets.all(1.0),
                  child: Transform(
                    alignment: Alignment.center,
                    transform: Matrix4.identity()..scaleByVector3(bboxMirror),
                    child: CameraPreview(controller),
                  ),
                ),
              ),
              Container(child: bboxDrawer())
            ],
          ),
          floatingActionButton: Visibility(
            visible: _isLivenessSet,
            child: FloatingActionButton(
              heroTag: "btn5",
              child: Icon(Icons.navigate_next),
              onPressed: () {
                setState(() {
                  Navigator.of(context).pop();
                  Navigator.of(context).pushNamed(widget.nextRoute);
                });
              },
            ),
          ),
          floatingActionButtonLocation: FloatingActionButtonLocation.endFloat,
        ));
  }

  @override
  void dispose() {
    controller.dispose();
    _tracking?.dispose();
    _fitter?.dispose();
    liveness?.dispose();

    super.dispose();
  }
}