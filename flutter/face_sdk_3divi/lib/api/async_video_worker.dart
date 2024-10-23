part of face_sdk_3divi;

enum _VideoWorkerEvents { ADD_VIDEO_FRAME, SET_DATA_BASE, RESET_TRACKER_ON_STREAM, POOL_TRACK_RESULTS, CLEAR }

class AsyncVideoWorker {
  Isolate _isolate;
  SendPort _sendPort;
  String _dllPath;
  bool _isDisposed = false;

  AsyncVideoWorker._create(this._isolate, this._sendPort, this._dllPath);

  static Future<AsyncVideoWorker> create(Pointer<Void> implementation, String facerecConfDir, String dllPath, VideoWorkerParams params) async {
    ReceivePort receivePort = ReceivePort();

    Isolate isolate = await Isolate.spawn(_isolateImplementation, {
      "implementation": implementation.address,
      "facerecConfDir": facerecConfDir,
      "dllPath": dllPath,
      "sendPort": receivePort.sendPort,
      "_recognizer_ini_file": params._recognizer_ini_file,
      "_streams_count": params._streams_count,
      "_processing_threads_count": params._processing_threads_count,
      "_matching_threads_count": params._matching_threads_count,
      "_age_gender_estimation_threads_count": params._age_gender_estimation_threads_count,
      "_emotions_estimation_threads_count": params._emotions_estimation_threads_count,
      "_short_time_identification_enabled": params._short_time_identification_enabled,
      "_short_time_identification_distance_threshold": params._short_time_identification_distance_threshold,
      "_short_time_identification_outdate_time_seconds": params._short_time_identification_outdate_time_seconds,
      "_active_liveness_checks_order": params._active_liveness_checks_order,
      "configFilepath": params._video_worker_config._configFilepath,
      "overriddenParams": params._video_worker_config._overriddenParams,
    });

    SendPort sendPort = await receivePort.first;

    return AsyncVideoWorker._create(isolate, sendPort, dllPath);
  }

  Future<int> addVideoFrame(RawImageF image, int timestamp_microsec) async {
    ReceivePort receivePort = ReceivePort();

    _sendPort.send({
      "event": _VideoWorkerEvents.ADD_VIDEO_FRAME,
      "sendPort": receivePort.sendPort,
      "width": image.width,
      "height": image.height,
      "format": image.format,
      "data": image.data.address,
      "with_crop": image.with_crop,
      "crop_info_offset_x": image.crop_info_offset_x,
      "crop_info_offset_y": image.crop_info_offset_y,
      "crop_info_data_image_width": image.crop_info_data_image_width,
      "crop_info_data_image_height": image.crop_info_data_image_height,
      "timestamp_microsec": timestamp_microsec
    });

    return await receivePort.first;
  }

  Future<void> resetTrackerOnStream() async {
    ReceivePort receivePort = ReceivePort();

    _sendPort.send({
      "event": _VideoWorkerEvents.RESET_TRACKER_ON_STREAM,
      "sendPort": receivePort.sendPort,
    });

    await receivePort.first;
  }

  Future<void> setDataBase(List<DataBaseElement> dataBase, {SearchAccelerationType acceleration = SearchAccelerationType.SEARCH_ACCELERATION_1}) async {
    ReceivePort receivePort = ReceivePort();
    List<Map<String, dynamic>> data = [];

    dataBase.forEach((element) {
      Map<String, dynamic> elementData = Map<String, dynamic>();

      elementData["person_id"] = element.person_id;
      elementData["element_id"] = element.element_id;
      elementData["distance_threshold"] = element.distance_threshold;
      elementData["face_template"] = element.face_template._impl.address;

      data.add(elementData);
    });

    _sendPort.send({"event": _VideoWorkerEvents.SET_DATA_BASE, "sendPort": receivePort.sendPort, "data": data, "acceleration": acceleration});

    await receivePort.first;
  }

  Future<TrackingData> poolTrackResults() async {
    ReceivePort receivePort = ReceivePort();
    TrackingData result = TrackingData();

    _sendPort.send({
      "event": _VideoWorkerEvents.POOL_TRACK_RESULTS,
      "sendPort": receivePort.sendPort,
    });

    Map<String, dynamic> data = await receivePort.first;

    result.tracking_callback_data = _parseTrackingCallbackData(data["tracking_callback_data"]);
    result.template_created_callback_data = _parseTemplateCreatedCallbackData(data["template_created_callback_data"]);
    result.tracking_lost_callback_data = _parseTrackingLostCallbackData(data["tracking_lost_callback_data"]);
    result.tracking_match_found_callback_data = _parseTrackingMatchFoundCallbackData(data["tracking_match_found_callback_data"]);
    result.sti_person_outdated_callback_data = _parseStiPersonOutdatedCallbackData(data["sti_person_outdated_callback_data"]);

    return result;
  }

  Future<void> dispose() async {
    if (_isDisposed) {
      return;
    }

    ReceivePort receivePort = ReceivePort();

    _sendPort.send({
      "event": _VideoWorkerEvents.CLEAR,
      "sendPort": receivePort.sendPort,
    });

    _isDisposed = await receivePort.first;

    _isolate.kill(priority: Isolate.immediate);
  }

  static Future<void> _isolateImplementation(Map<String, dynamic> initialization) async {
    DynamicLibrary dylib = DynamicLibrary.open(initialization["dllPath"]);
    FacerecService service =
        FacerecService(dylib, Pointer<Void>.fromAddress(initialization["implementation"]), initialization["facerecConfDir"], initialization["dllPath"]);
    VideoWorkerParams params = VideoWorkerParams();
    Config config = Config(initialization["configFilepath"]);
    Map<String, double> overriddenParams = initialization["overriddenParams"];

    overriddenParams.forEach((key, value) => config.overrideParameter(key, value));

    params._recognizer_ini_file = initialization["_recognizer_ini_file"];
    params._streams_count = initialization["_streams_count"];
    params._processing_threads_count = initialization["_processing_threads_count"];
    params._matching_threads_count = initialization["_matching_threads_count"];
    params._age_gender_estimation_threads_count = initialization["_age_gender_estimation_threads_count"];
    params._emotions_estimation_threads_count = initialization["_emotions_estimation_threads_count"];
    params._short_time_identification_enabled = initialization["_short_time_identification_enabled"];
    params._short_time_identification_distance_threshold = initialization["_short_time_identification_distance_threshold"];
    params._short_time_identification_outdate_time_seconds = initialization["_short_time_identification_outdate_time_seconds"];
    params._active_liveness_checks_order = initialization["_active_liveness_checks_order"];
    params._video_worker_config = config;

    ReceivePort receivePort = ReceivePort();
    VideoWorker videoWorker = service.createVideoWorker(params);

    (initialization["sendPort"] as SendPort).send(receivePort.sendPort);

    await for (dynamic message in receivePort) {
      if (message is! Map) {
        print("Not a map ${message.runtimeType}");

        continue;
      }

      final _VideoWorkerEvents event = message["event"];
      final SendPort sendPort = message["sendPort"];

      switch (event) {
        case _VideoWorkerEvents.ADD_VIDEO_FRAME:
          int width = message["width"];
          int height = message["height"];
          Format format = message["format"];
          Pointer<Utf8> data = Pointer<Utf8>.fromAddress(message["data"]);
          int with_crop = message["with_crop"];
          int crop_info_offset_x = message["crop_info_offset_x"];
          int crop_info_offset_y = message["crop_info_offset_y"];
          int crop_info_data_image_width = message["crop_info_data_image_width"];
          int crop_info_data_image_height = message["crop_info_data_image_height"];
          int timestamp_microsec = message["timestamp_microsec"];

          RawImageF image = RawImageF(width, height, format, data);

          image.with_crop = with_crop;
          image.crop_info_offset_x = crop_info_offset_x;
          image.crop_info_offset_y = crop_info_offset_y;
          image.crop_info_data_image_width = crop_info_data_image_width;
          image.crop_info_data_image_height = crop_info_data_image_height;

          sendPort.send(videoWorker.addVideoFrame(image, timestamp_microsec));

          break;

        case _VideoWorkerEvents.SET_DATA_BASE:
          List<Map<String, dynamic>> data = message["data"];
          SearchAccelerationType acceleration = message["acceleration"];

          List<DataBaseElement> dataBase = [];

          data.forEach((element) {
            dataBase.add(DataBaseElement(element["element_id"], element["person_id"], Template(dylib, Pointer<Void>.fromAddress(element["face_template"])),
                element["distance_threshold"]));
          });

          videoWorker.setDataBase(dataBase, acceleration: acceleration);

          sendPort.send(true);

          break;

        case _VideoWorkerEvents.RESET_TRACKER_ON_STREAM:
          videoWorker.resetTrackerOnStream();

          sendPort.send(true);

          break;

        case _VideoWorkerEvents.POOL_TRACK_RESULTS:
          TrackingData data = videoWorker.poolTrackResults();
          Map<String, dynamic> result = Map<String, dynamic>();
          List<int> samplePointers = [];

          data.tracking_callback_data.samples.forEach((element) => samplePointers.add(element._impl.address));
          List<ActiveLivenessStatus> status = data.tracking_callback_data.samples_active_liveness_status;
          List<ActiveLiveness> verdict = [];
          List<ActiveLivenessCheckType> check_type = [];
          List<double> progress_level = [];
          RawSample? bestQualitySample;
          Template? bestQualityTemplate;
          RawSample? matchFoundSample;
          Template? matchFoundTemplate;

          if (data.tracking_lost_callback_data.stream_id != -1) {
            bestQualitySample = data.tracking_lost_callback_data.best_quality_sample;
            bestQualityTemplate = data.tracking_lost_callback_data.best_quality_templ;
          }

          if (data.tracking_match_found_callback_data.stream_id != -1) {
            matchFoundSample = data.tracking_match_found_callback_data.sample;
            matchFoundTemplate = data.tracking_match_found_callback_data.templ;
          }

          status.forEach((element) {
            verdict.add(element.verdict);
            check_type.add(element.check_type);
            progress_level.add(element.progress_level);
          });

          result["tracking_callback_data"] = {
            "stream_id": data.tracking_callback_data.stream_id,
            "frame_id": data.tracking_callback_data.frame_id,
            "_samples_track_id": data.tracking_callback_data._samples_track_id,
            "_samples_weak": data.tracking_callback_data._samples_weak,
            "samples_quality": data.tracking_callback_data.samples_quality,
            "samples": samplePointers,
            "samples_active_liveness_status": {"verdict": verdict, "check_type": check_type, "progress_level": progress_level}
          };

          result["template_created_callback_data"] = {
            "stream_id": data.template_created_callback_data.stream_id,
            "frame_id": data.template_created_callback_data.frame_id,
            "quality": data.template_created_callback_data.quality
          };

          if (data.template_created_callback_data.stream_id != -1) {
            result["template_created_callback_data"]["sample"] = data.template_created_callback_data.sample._impl.address;
            result["template_created_callback_data"]["template"] = data.template_created_callback_data.templ._impl.address;
          }

          result["tracking_lost_callback_data"] = {
            "stream_id": data.tracking_lost_callback_data.stream_id,
            "first_frame_id": data.tracking_lost_callback_data.first_frame_id,
            "last_frame_id": data.tracking_lost_callback_data.last_frame_id,
            "best_quality": data.tracking_lost_callback_data.best_quality,
            "best_quality_frame_id": data.tracking_lost_callback_data.best_quality_frame_id,
            "track_id": data.tracking_lost_callback_data.track_id,
            "sti_person_id_set": data.tracking_lost_callback_data.sti_person_id_set,
            "sti_person_id": data.tracking_lost_callback_data.sti_person_id
          };

          if (bestQualitySample != null) {
            result["tracking_lost_callback_data"]["best_quality_sample"] = bestQualitySample._impl.address;
          }

          if (bestQualityTemplate != null) {
            result["tracking_lost_callback_data"]["best_quality_template"] = bestQualityTemplate._impl.address;
          }

          List<Map<String, dynamic>> searchResults = [];

          for (VWSearchResult searchResult in data.tracking_match_found_callback_data.search_results) {
            Map<String, dynamic> temp = <String, dynamic>{};

            temp["person_id"] = searchResult.person_id;
            temp["element_id"] = searchResult.element_id;
            temp["distance"] = searchResult.match_result.distance;
            temp["fa_r"] = searchResult.match_result.fa_r;
            temp["fr_r"] = searchResult.match_result.fr_r;
            temp["score"] = searchResult.match_result.score;

            searchResults.add(temp);
          }

          result["tracking_match_found_callback_data"] = {
            "stream_id": data.tracking_match_found_callback_data.stream_id,
            "frame_id": data.tracking_match_found_callback_data.frame_id,
            "quality": data.tracking_match_found_callback_data.quality,
            "search_results": searchResults
          };

          if (matchFoundSample != null) {
            result["tracking_match_found_callback_data"]["sample"] = matchFoundSample._impl.address;
          }

          if (matchFoundTemplate != null) {
            result["tracking_match_found_callback_data"]["template"] = matchFoundTemplate._impl.address;
          }

          result["sti_person_outdated_callback_data"] = {
            "stream_id": data.sti_person_outdated_callback_data.stream_id,
            "sti_person_id": data.sti_person_outdated_callback_data.sti_person_id
          };

          sendPort.send(result);

          break;

        case _VideoWorkerEvents.CLEAR:
          videoWorker.dispose();

          sendPort.send(true);

          return;
      }
    }
  }

  TrackingCallbackData _parseTrackingCallbackData(Map<String, dynamic> data) {
    DynamicLibrary dylib = DynamicLibrary.open(_dllPath);
    TrackingCallbackData result = TrackingCallbackData();

    result.stream_id = data["stream_id"];
    result.frame_id = data["frame_id"];
    result._samples_track_id = data["_samples_track_id"];
    result._samples_weak = data["_samples_weak"];
    result.samples_quality = data["samples_quality"];

    List<int> pointers = data["samples"];

    List<ActiveLiveness> verdict = data["samples_active_liveness_status"]["verdict"];
    List<ActiveLivenessCheckType> check_type = data["samples_active_liveness_status"]["check_type"];
    List<double> progress_level = data["samples_active_liveness_status"]["progress_level"];

    pointers.forEach((pointer) => result.samples.add(RawSample(dylib, Pointer<Void>.fromAddress(pointer))));

    for (int i = 0; i < verdict.length; i++) {
      ActiveLivenessStatus status = ActiveLivenessStatus();

      status.verdict = verdict[i];
      status.check_type = check_type[i];
      status.progress_level = progress_level[i];

      result.samples_active_liveness_status.add(status);
    }

    return result;
  }

  TemplateCreatedCallbackData _parseTemplateCreatedCallbackData(Map<String, dynamic> data) {
    DynamicLibrary dylib = DynamicLibrary.open(_dllPath);
    TemplateCreatedCallbackData result = TemplateCreatedCallbackData();

    result.stream_id = data["stream_id"];
    result.frame_id = data["frame_id"];
    result.quality = data["quality"];

    if (result.stream_id != -1) {
      result.sample = RawSample(dylib, Pointer<Void>.fromAddress(data["sample"]));
      result.templ = Template(dylib, Pointer<Void>.fromAddress(data["template"]));
    }

    return result;
  }

  TrackingLostCallbackData _parseTrackingLostCallbackData(Map<String, dynamic> data) {
    DynamicLibrary dylib = DynamicLibrary.open(_dllPath);
    TrackingLostCallbackData result = TrackingLostCallbackData();

    result.stream_id = data["stream_id"];
    result.first_frame_id = data["first_frame_id"];
    result.last_frame_id = data["last_frame_id"];
    result.best_quality = data["best_quality"];
    result.best_quality_frame_id = data["best_quality_frame_id"];
    result.track_id = data["track_id"];
    result.sti_person_id_set = data["sti_person_id_set"];
    result.sti_person_id = data["sti_person_id"];

    if (data.containsKey("best_quality_sample")) {
      result.best_quality_sample = RawSample(dylib, Pointer<Void>.fromAddress(data["best_quality_sample"]));
    }

    if (data.containsKey("best_quality_template")) {
      result.best_quality_templ = Template(dylib, Pointer<Void>.fromAddress(data["best_quality_template"]));
    }

    return result;
  }

  TrackingMatchFoundCallbackData _parseTrackingMatchFoundCallbackData(Map<String, dynamic> data) {
    DynamicLibrary dylib = DynamicLibrary.open(_dllPath);
    TrackingMatchFoundCallbackData result = TrackingMatchFoundCallbackData();

    result.stream_id = data["stream_id"];
    result.frame_id = data["frame_id"];
    result.quality = data["quality"];

    if (data.containsKey("sample")) {
      result.sample = RawSample(dylib, Pointer<Void>.fromAddress(data["sample"]));
    }

    if (data.containsKey("template")) {
      result.templ = Template(dylib, Pointer<Void>.fromAddress(data["template"]));
    }

    List<Map<String, dynamic>> results = data["search_results"];

    results.forEach((element) {
      int person_id = element["person_id"];
      int element_id = element["element_id"];
      double distance = element["distance"];
      double fa_r = element["fa_r"];
      double fr_r = element["fr_r"];
      double score = element["score"];

      MatchResult matchResult = MatchResult(distance, fa_r, fr_r, score);

      result.search_results.add(VWSearchResult(person_id, element_id, matchResult));
    });

    return result;
  }

  StiPersonOutdatedCallbackData _parseStiPersonOutdatedCallbackData(Map<String, dynamic> data) {
    StiPersonOutdatedCallbackData result = StiPersonOutdatedCallbackData();

    result.stream_id = data["stream_id"];
    result.sti_person_id = data["sti_person_id"];

    return result;
  }
}
