part of face_sdk_3divi;

enum _CapturerEvents { CAPTURE, CAPTURE_RAW_IMAGE_F, CLEAR }

class AsyncCapturer {
  Isolate _isolate;
  SendPort _sendPort;
  String _dllPath;
  bool _isDisposed = false;

  AsyncCapturer._create(this._isolate, this._sendPort, this._dllPath);

  static Future<AsyncCapturer> create(
      Pointer<Void> implementation, String facerecConfDir, String dllPath, Config config) async {
    ReceivePort receivePort = ReceivePort();

    Isolate isolate = await Isolate.spawn(_isolateImplementation, {
      "implementation": implementation.address,
      "facerecConfDir": facerecConfDir,
      "dllPath": dllPath,
      "configFilepath": config._configFilepath,
      "overriddenParams": config._overriddenParams,
      "sendPort": receivePort.sendPort
    });
    SendPort sendPort = await receivePort.first;

    return AsyncCapturer._create(isolate, sendPort, dllPath);
  }

  Future<List<RawSample>> capture(Uint8List encodedImage) async {
    ReceivePort receivePort = ReceivePort();
    List<RawSample> result = [];
    DynamicLibrary dylib = DynamicLibrary.open(_dllPath);

    _sendPort.send({"event": _CapturerEvents.CAPTURE, "sendPort": receivePort.sendPort, "encodedImage": encodedImage});

    List<int> pointers = await receivePort.first;

    pointers.forEach((address) => result.add(RawSample(dylib, Pointer<Void>.fromAddress(address))));

    return result;
  }

  Future<List<RawSample>> captureRawImageF(RawImageF image) async {
    ReceivePort receivePort = ReceivePort();
    List<RawSample> result = [];
    DynamicLibrary dylib = DynamicLibrary.open(_dllPath);

    _sendPort.send({
      "event": _CapturerEvents.CAPTURE_RAW_IMAGE_F,
      "sendPort": receivePort.sendPort,
      "width": image.width,
      "height": image.height,
      "format": image.format,
      "data": image.data.address,
      "with_crop": image.with_crop,
      "crop_info_offset_x": image.crop_info_offset_x,
      "crop_info_offset_y": image.crop_info_offset_y,
      "crop_info_data_image_width": image.crop_info_data_image_width,
      "crop_info_data_image_height": image.crop_info_data_image_height
    });

    List<int> pointers = await receivePort.first;

    pointers.forEach((address) => result.add(RawSample(dylib, Pointer<Void>.fromAddress(address))));

    return result;
  }

  Future<void> dispose() async {
    if (_isDisposed) {
      return;
    }

    ReceivePort receivePort = ReceivePort();

    _sendPort.send({
      "event": _CapturerEvents.CLEAR,
      "sendPort": receivePort.sendPort,
    });

    _isDisposed = await receivePort.first;

    _isolate.kill(priority: Isolate.immediate);
  }

  static Future<void> _isolateImplementation(Map<String, dynamic> initialization) async {
    FacerecService service = FacerecService(
        DynamicLibrary.open(initialization["dllPath"]),
        Pointer<Void>.fromAddress(initialization["implementation"]),
        initialization["facerecConfDir"],
        initialization["dllPath"]);
    Config config = Config(initialization["configFilepath"]);
    Map<String, double> overriddenParams = initialization["overriddenParams"];

    overriddenParams.forEach((key, value) => config.overrideParameter(key, value));

    ReceivePort receivePort = ReceivePort();
    Capturer capturer = service.createCapturer(config);

    (initialization["sendPort"] as SendPort).send(receivePort.sendPort);

    await for (dynamic message in receivePort) {
      if (message is! Map) {
        print("Not a map ${message.runtimeType}");

        continue;
      }

      List<RawSample> samples;
      List<int> result = [];

      final _CapturerEvents event = message["event"];
      final SendPort sendPort = message["sendPort"];

      switch (event) {
        case _CapturerEvents.CAPTURE:
          samples = capturer.capture(message["encodedImage"]);

          break;

        case _CapturerEvents.CAPTURE_RAW_IMAGE_F:
          int width = message["width"];
          int height = message["height"];
          Format format = message["format"];
          Pointer<Utf8> data = Pointer<Utf8>.fromAddress(message["data"]);
          int with_crop = message["with_crop"];
          int crop_info_offset_x = message["crop_info_offset_x"];
          int crop_info_offset_y = message["crop_info_offset_y"];
          int crop_info_data_image_width = message["crop_info_data_image_width"];
          int crop_info_data_image_height = message["crop_info_data_image_height"];

          RawImageF image = RawImageF(width, height, format, data);

          image.with_crop = with_crop;
          image.crop_info_offset_x = crop_info_offset_x;
          image.crop_info_offset_y = crop_info_offset_y;
          image.crop_info_data_image_width = crop_info_data_image_width;
          image.crop_info_data_image_height = crop_info_data_image_height;

          samples = capturer.captureRawImageF(image);

          break;

        case _CapturerEvents.CLEAR:
          capturer.dispose();

          sendPort.send(true);

          return;
      }

      samples.forEach((sample) => result.add(sample._impl.address));

      sendPort.send(result);
    }
  }
}
