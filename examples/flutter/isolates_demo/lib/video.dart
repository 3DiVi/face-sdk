import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';
import 'dart:ui' as ui;
import 'dart:async';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:flutter/services.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';
import 'package:face_sdk_3divi/utils.dart';
import 'package:image/image.dart' as image_lib;

typedef void setLivenessStatus(bool isPassed, Context template, Image image, double mirror);

class VideoProcessing extends StatefulWidget {
  final List<CameraDescription> cameras;
  final FacerecService _service;
  final AsyncProcessingBlock templateExtractor;
  final AsyncProcessingBlock qaa;
  final String nextRoute;
  final setLivenessStatus callback;

  VideoProcessing(this.cameras, this._service, this.templateExtractor, this.qaa, this.nextRoute, this.callback);

  @override
  _VideoProcessingState createState() => _VideoProcessingState();
}

class _VideoProcessingState extends State<VideoProcessing> {
  late CameraController controller;
  image_lib.PngEncoder pngEncoder = image_lib.PngEncoder(level: 0);
  AsyncVideoWorker? _videoWorker;
  AsyncProcessingBlock? liveness;
  GlobalKey _pictureKey = GlobalKey();
  bool _isLivenessSet = false;
  int baseAngle = 0;
  double mirror = 0;
  String _livenessResult = "Processing";
  bool isReady = true;
  bool isProcessFrameReady = true;
  bool livenessPassed = false;
  Offset? widgetPosition;
  ui.Size? widgetSize;
  NativeDataStruct data = NativeDataStruct();
  NativeDataStruct reusableData = NativeDataStruct();
  List<RawImageF> _targets = [];
  Widget? bboxWidget;
  Image? bboxImage;
  bool flipX = true;

  double _qaaTotalScore = 0.0;

  Future<void> _processStream(CameraImage image) async {
    if (!isReady || _videoWorker == null || !mounted) {
      return;
    }

    isReady = false;

    final RenderBox? renderBox = _pictureKey.currentContext?.findRenderObject() as RenderBox;

    if (renderBox != null) {
      widgetPosition = renderBox.localToGlobal(Offset.zero);
      widgetSize = renderBox.size;
    }

    int width;
    int height;

    switch (baseAngle) {
      case 1:
      case 2:
        width = image.height;
        height = image.width;

        break;

      default:
        width = image.width;
        height = image.height;

        break;
    }

    Uint8List? bytes = await addVideoFrame(image);

    if (bytes == null) {
      bboxWidget = null;

      isReady = true;

      return;
    }

    RawSample? sample = await pool();

    if (sample == null) {
      bboxWidget = null;

      isReady = true;

      return;
    }

    bboxWidget = buildBboxWidget(width, height, sample.getRectangle());

    if (mounted) {
      setState(() {
        isReady = true;
      });
    }

    processFrame(width, height, bytes, sample);
  }

  @override
  void initState() {
    super.initState();
    if (widget.cameras.isEmpty) {
      print("No camera is found");
    } else {
      final CameraDescription camera = widget.cameras[1];

      controller = CameraController(camera, ResolutionPreset.high);
      controller.initialize().then((_) {
        if (!mounted) {
          return;
        }
        setState(() {});

        controller.startImageStream(_processStream);
      });
    }

    baseAngle = getBaseAngle(controller);

    if (Platform.isIOS) {
      flipX = false;
    }

    widget._service
        .createAsyncProcessingBlock({"unit_type": "LIVENESS_ESTIMATOR", "modification": "2d_light", "version": 1}).then(
            (value) => liveness = value);

    widget._service
        .createAsyncVideoWorker(VideoWorkerParams()
            .video_worker_config(Config("video_worker_fdatracker_pb_blf_fda_front.xml"))
            .streams_count(1))
        .then((value) {
      _videoWorker = value;
    });
  }

  Future<Uint8List?> addVideoFrame(CameraImage cameraImage) async {
    if (!mounted) {
      await Future.delayed(const Duration(milliseconds: 10));

      return null;
    }

    if (!data.isValid) {
      reusableData.resize(cameraImage.width * cameraImage.height * 3);
    }

    RawImageF target = widget._service.createRawImageFromCameraImage(cameraImage, baseAngle);

    if (_targets.length > 300) {
      for (int i = 0; i < 150; i++) {
        _targets[i].dispose();
      }

      _targets.removeRange(0, 150);
    }

    await _videoWorker!.addVideoFrame(target, DateTime.now().microsecondsSinceEpoch);

    await Future.delayed(const Duration(milliseconds: 10));

    _targets.add(target);

    return target.data.cast<Uint8>().asTypedList(cameraImage.width * cameraImage.height * 3);
  }

  Future<RawSample?> pool() async {
    if (_videoWorker == null || liveness == null || !mounted) {
      await Future.delayed(const Duration(milliseconds: 50));

      return null;
    }

    final TrackingData callbackData = await _videoWorker!.poolTrackResults();
    final List<RawSample> rawSamples = callbackData.tracking_callback_data.samples;

    if (rawSamples.length != 1) {
      return null;
    }

    return rawSamples.first;
  }

  Future<void> processFrame(int width, int height, Uint8List bytes, RawSample sample) async {
    if (!isProcessFrameReady) {
      sample.dispose();

      return;
    }

    isProcessFrameReady = false;

    Context data = widget._service.createContext({
      "objects": [sample.toContext()],
      "image": {
        "blob": bytes,
        "dtype": "uint8_t",
        "format": "NDARRAY",
        "shape": [height, width, 3]
      }
    });

    await widget.qaa.process(data);

    Context object = data["objects"][0];

    _qaaTotalScore = object["quality"]["total_score"].get_value();

    if (mounted) {
      setState(() {});
    }

    if (_isLivenessSet) {
      data.dispose();
      sample.dispose();

      isProcessFrameReady = true;

      return;
    }

    if (object["quality"]["total_score"].get_value() >= 0.6) {
      await liveness!.process(data);

      _isLivenessSet = true;

      if (object["liveness"]["value"].get_value() == "REAL") {
        _livenessResult = "Real";
        livenessPassed = true;
      } else {
        _livenessResult = "Fake";
        livenessPassed = false;
      }

      await widget.templateExtractor.process(data);

      Context template = widget._service.createContext(data["objects"][0]["template"]);
      Rectangle bbox = sample.getRectangle();
      final image_lib.Image image = image_lib.Image.fromBytes(
          width: width, height: height, bytes: bytes.buffer, numChannels: 3, order: image_lib.ChannelOrder.rgb);

      List<int> png =
          pngEncoder.encode(image_lib.copyCrop(image, x: bbox.x, y: bbox.y, width: bbox.width, height: bbox.height));

      widget.callback(livenessPassed, template, Image.memory(png as Uint8List), mirror);
    }

    data.dispose();
    sample.dispose();

    isProcessFrameReady = true;
  }

  Widget? buildBboxWidget(int width, int height, Rectangle bbox) {
    if (widgetSize == null) {
      return null;
    }

    double xCoefficient = widgetSize!.width / width;
    double yCoefficient = widgetSize!.height / height;

    double widgetWidth = bbox.width * xCoefficient;
    double widgetHeight = bbox.height * yCoefficient;

    return Positioned(
        left: widgetPosition!.dx + bbox.x * xCoefficient,
        top: widgetPosition!.dy + bbox.y * yCoefficient,
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
              child: Text("Quality score: $_qaaTotalScore",
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
      (quality.child as Text).data?.replaceRange(0, null, "Quality score: $_qaaTotalScore");
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
              child: CameraPreview(controller)),
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
    _videoWorker?.dispose().whenComplete(() {
      for (RawImageF target in _targets) {
        target.dispose();
      }

      _targets.clear();
    });
    liveness?.dispose();

    super.dispose();
  }
}
