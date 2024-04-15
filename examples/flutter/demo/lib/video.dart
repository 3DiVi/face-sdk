import 'dart:ui';
import 'dart:async';
import 'dart:math' as math;
import 'dart:io' show Platform;
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:percent_indicator/linear_percent_indicator.dart';
import 'package:face_sdk_3divi/face_sdk_3divi.dart';
import 'package:face_sdk_3divi/utils.dart';

import 'bndbox.dart';


typedef void setLivenssStatus(bool isPassed, Template templ, Image? img, double mirror);


class VideoProcessing extends StatefulWidget {
  final List<CameraDescription> cameras;
  final FacerecService _facerecService;
  final Recognizer _recognizer;
  final String nextRoute;
  final setLivenssStatus callback;

  VideoProcessing(this.cameras, this._facerecService, this._recognizer, this.nextRoute, this.callback);

  @override
  _VideoProcessingState createState() => new _VideoProcessingState();
}



class _VideoProcessingState extends State<VideoProcessing> {
  late CameraController controller;
  NativeDataStruct _data = new NativeDataStruct();
  RawImageF? _ri;

  late VideoWorker _videoWorker;
  Offset? widgetPosition;
  Size? widgetSize;
  GlobalKey _pictureKey = GlobalKey();
  int _lastImgTimestamp = 0;
  CameraImage? _lastImg;
  CameraImage? bestImage;
  int livenessProgress = 0;
  String activeLivenessAction = " ";
  List<dynamic> _recognitions = [];
  Template? templ;
  bool _isLivenessSet = false;
  int baseAngle = 0;
  late Rectangle rect;
  late Rectangle bestRect;
  double mirror = 0;

  double best_quality = -100000000;
  bool livenessPassed = false;
  bool livenessFailed = false;


  void _processStream(CameraImage img) async {
    if(!mounted)
      return;
    final RenderBox renderBox = _pictureKey.currentContext?.findRenderObject() as RenderBox;
    widgetPosition = renderBox.localToGlobal(Offset.zero);
    widgetSize = renderBox.size;

    int startTime = new DateTime.now().millisecondsSinceEpoch;
    setState(() {
      _lastImgTimestamp = startTime;
      _lastImg = img;
    });
  }

  @override
  void initState() {
    super.initState();
    if (widget.cameras == null || widget.cameras.length < 1) {
      print('No camera is found');
    } else {
      final camera = widget.cameras[1];
      controller = new CameraController(
        camera,
        ResolutionPreset.high,
      );
      controller.initialize().then((_) {
        if (!mounted) {
          return;
        }
        setState(() {});
        controller.startImageStream(_processStream);
      });
    }
    if (controller.description.sensorOrientation == 90) {
      baseAngle = 1;
      mirror = 1;
    }
    else if (controller.description.sensorOrientation == 270) {
      baseAngle = 2;
    }

    double apply_horizontal_flip = 0;
    if (Platform.isIOS){
      if (controller.description.lensDirection == CameraLensDirection.front)
        apply_horizontal_flip = 1;
      baseAngle = 0;
    }

    List<ActiveLivenessCheckType> checks = [
      ActiveLivenessCheckType.TURN_LEFT,
      ActiveLivenessCheckType.SMILE,
      ActiveLivenessCheckType.TURN_DOWN
    ];

    _videoWorker = widget._facerecService.createVideoWorker(
        VideoWorkerParams()
            .recognizer_ini_file("method12v30_recognizer.xml")
            .video_worker_config(
                Config("video_worker_fdatracker_blf_fda_front.xml")
                  .overrideParameter("enable_active_liveness", 1)
                  .overrideParameter("base_angle", baseAngle.toDouble())
                  .overrideParameter("active_liveness.apply_horizontal_flip", apply_horizontal_flip)
                )
            .streams_count(1)
            .processing_threads_count(0)
            .matching_threads_count(0)
            .emotions_estimation_threads_count(1)
            .active_liveness_checks_order(checks)
    );
  }

  Stream<List<dynamic>> addVF(int prev_time) async* {
    final time = _lastImgTimestamp;
    var img = _lastImg;
    if (!mounted || img == null){
      await Future.delayed(const Duration(milliseconds: 50));
      yield* addVF(time);
    }
    img = img!;
    if (prev_time != _lastImgTimestamp) {
      Format format = Format.FORMAT_RGB;
      if (img.format.group == ImageFormatGroup.yuv420) {
        format = Format.FORMAT_YUV_NV21;
        convertRAW(img.planes, _data);
      }
      else if (img.format.group == ImageFormatGroup.bgra8888) {
        format = Format.FORMAT_BGR;
        convertBGRA8888(img.planes, _data);
      }
      else {
        print("Unsupported image format");
        convertRAW(img.planes, _data);
      }
      _ri = RawImageF(img.width, img.height, format, _data.pointer!.cast());
      _videoWorker.addVideoFrame(_ri!, time);
    }

    await Future.delayed(const Duration(milliseconds: 50));
    yield* addVF(time);
  }

  Stream<String> pool() async* {
    if (!mounted){
      await Future.delayed(const Duration(milliseconds: 50));
      yield* pool();
    }
    final callbackData = _videoWorker.poolTrackResults();
    final rawSamples = callbackData.tracking_callback_data.samples;
    List<dynamic> detections = [];
    var angles;
    if (callbackData.tracking_callback_data.samples.length > 0) {
      for(var i = 0; i < rawSamples.length; i+=1) {
        rect = rawSamples[i].getRectangle();
        angles = rawSamples[i].getAngles();
        detections.add({
          "rect": {"x": rect.x, "y": rect.y, "w": rect.width, "h": rect.height},
          "widget": {"w": widgetSize!.height, "h": widgetSize!.width},
          "picture": {"w": _ri!.width, "h": _ri!.height},
          "offset": {"x": widgetPosition!.dx, "y": widgetPosition!.dy}
        });
      }

      if (best_quality < callbackData.tracking_callback_data.samples_quality[0]) {
        best_quality = callbackData.tracking_callback_data.samples_quality[0];
        bestImage = _lastImg;
        bestRect = rect;
      }
    }
    int progress = livenessProgress;
    if (!livenessFailed && !livenessPassed) {
      if (callbackData.tracking_callback_data.samples.length == 1) {
        var status = callbackData.tracking_callback_data
            .samples_active_liveness_status[0];
        if (status.verdict == ActiveLiveness.WAITING_FACE_ALIGN) {
          activeLivenessAction = 'Please, look at the camera';
          if (angles.yaw > 10)
            activeLivenessAction += ' (turn face →)';
          else if (angles.yaw < -10)
            activeLivenessAction += ' (turn face ←)';
          else if (angles.pitch > 10)
            activeLivenessAction += ' (turn face ↓)';
          else if (angles.pitch < -10)
            activeLivenessAction += ' (turn face ↑)';
        }
        else if (status.verdict == ActiveLiveness.CHECK_FAIL) {
          activeLivenessAction = 'Active liveness check FAILED';
          livenessFailed = true;
          _videoWorker.resetTrackerOnStream();
        }
        else if (status.verdict == ActiveLiveness.ALL_CHECKS_PASSED) {
          activeLivenessAction = 'Active liveness check PASSED';
          livenessPassed = true;
          _videoWorker.resetTrackerOnStream();
        }
        else if (status.verdict == ActiveLiveness.IN_PROGRESS) {
          if (status.check_type == ActiveLivenessCheckType.BLINK)
            activeLivenessAction = 'Blink';
          else if (status.check_type == ActiveLivenessCheckType.SMILE)
            activeLivenessAction = 'Smile';
          else if (status.check_type == ActiveLivenessCheckType.TURN_DOWN)
            activeLivenessAction = 'Turn face down';
          else if (status.check_type == ActiveLivenessCheckType.TURN_LEFT) {
            activeLivenessAction = 'Turn face left';
          } else if (status.check_type == ActiveLivenessCheckType.TURN_RIGHT) {
            activeLivenessAction = 'Turn face right';
          } else if (status.check_type == ActiveLivenessCheckType.TURN_UP)
            activeLivenessAction = 'Turn face up';
        }
        else if (status.verdict == ActiveLiveness.NOT_COMPUTED)
          activeLivenessAction = 'Active liveness disabled';

        progress = (status.progress_level * 100).toInt();
      }
      else if (callbackData.tracking_callback_data.samples.length > 1) {
        progress = 0;
        activeLivenessAction = "Leave one face in the frame ";
      }
      else {
        progress = 0;
        activeLivenessAction = "";
      }
    }

    if(templ == null && callbackData.tracking_lost_callback_data.best_quality_templ != null){
      templ = callbackData.tracking_lost_callback_data.best_quality_templ!;
      if (livenessFailed || livenessPassed)
        _isLivenessSet = true;
    }
    else if (templ == null && callbackData.tracking_lost_callback_data.best_quality_sample != null){
      templ = widget._recognizer.processing(callbackData.tracking_lost_callback_data.best_quality_sample!);
    }

    rawSamples.forEach((element) => element.dispose());
    setState(() {
      _recognitions = detections;
      livenessProgress = progress;
      if (templ != null && !_isLivenessSet) {
        if (livenessPassed)
          widget.callback(true, templ!, cutFaceFromCameraImage(bestImage!, bestRect), mirror);
        if (livenessFailed)
          widget.callback(false, templ!, cutFaceFromCameraImage(bestImage!, bestRect), mirror);

        if (livenessFailed || livenessPassed)
          _isLivenessSet = true;
      }
    });


    yield activeLivenessAction;
    await Future.delayed(const Duration(milliseconds: 50));
    yield* pool();
  }

  Widget bboxDrawer(){
    return Container();
  }

  @override
  Widget build(BuildContext context) {
    if (controller == null || !controller.value.isInitialized) {
      return Container();
    }
    return new WillPopScope(
      onWillPop: () async => false,
      child: Scaffold(
        body: Stack(
          children: [
            Center(
              child: Padding(
                key: _pictureKey,
                child: CameraPreview(controller, child: bboxDrawer()),
                padding: const EdgeInsets.all(1.0)
              ),
            ),
            StreamBuilder(
              stream: pool(),
              builder: (context, snapshot){
                return Transform.translate(
                    offset: Offset(0, 100),
                    child: Text(activeLivenessAction,
                        style: new TextStyle(fontSize: 20, backgroundColor: Colors.black))
                );
              }
            ),
            StreamBuilder(
                stream: addVF(0),
                builder: (context, snapshot){return Text("");},
            ),
            Container(
              margin: EdgeInsets.only(left:30,right:30),
              alignment:Alignment.bottomCenter,
              child: LinearPercentIndicator( //leaner progress bar
                // animation: false,
                // animationDuration: 30,
                lineHeight: 20.0,
                percent:livenessProgress/100,
                restartAnimation: true,
                center: Text(
                  livenessProgress.toString() + "%",
                  style: TextStyle(
                      fontSize: 12.0,
                      fontWeight: FontWeight.w600,
                      color: Colors.black),
                ),
                linearStrokeCap: LinearStrokeCap.roundAll,
                progressColor: Colors.blue[400],
                backgroundColor: Colors.grey[300],
              ),
            ),
          ],
        ),
        floatingActionButton: new Visibility(
          visible: _isLivenessSet,
          child: FloatingActionButton(
            heroTag: "btn5",
            child:  Icon(Icons.navigate_next),
            onPressed: (){setState(() {
                Navigator.of(context).pop();
                Navigator.of(context).pushNamed(widget.nextRoute);
            });},
          ),
        ),
        floatingActionButtonLocation: FloatingActionButtonLocation.endFloat,
      )
    );
  }

  @override
  void dispose() {
    controller.dispose();
    _videoWorker.dispose();
    super.dispose();
  }
}
