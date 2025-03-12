part of face_sdk_3divi;

enum _ProcessingBlockEvents { PROCESS, CLEAR }

class AsyncProcessingBlock {
  Isolate _isolate;
  SendPort _sendPort;
  bool _isDisposed = false;

  AsyncProcessingBlock._create(this._isolate, this._sendPort);

  static Future<AsyncProcessingBlock> create(Pointer<Void> implementation, String facerecConfDir, String dllPath, Map context) async {
    ReceivePort receivePort = ReceivePort();

    Isolate isolate = await Isolate.spawn(_isolateImplementation,
        {"implementation": implementation.address, "facerecConfDir": facerecConfDir, "dllPath": dllPath, "sendPort": receivePort.sendPort, "context": context});

    dynamic isolateResult = await receivePort.first;

    _checkException(isolateResult, isolate: isolate);

    SendPort sendPort = isolateResult;

    return AsyncProcessingBlock._create(isolate, sendPort);
  }

  Future<void> process(Context context) async {
    ReceivePort receivePort = ReceivePort();

    _sendPort.send({
      "event": _ProcessingBlockEvents.PROCESS,
      "sendPort": receivePort.sendPort,
      "context": context._impl.address,
    });

    dynamic isolateResult = await receivePort.first;

    _checkException(isolateResult);
  }

  Future<void> dispose() async {
    if (_isDisposed) {
      return;
    }

    ReceivePort receivePort = ReceivePort();

    _sendPort.send({
      "event": _ProcessingBlockEvents.CLEAR,
      "sendPort": receivePort.sendPort,
    });

    dynamic isolateResult = await receivePort.first;
    _isDisposed = true;

    _checkException(isolateResult, isolate: _isolate);

    _isolate.kill(priority: Isolate.immediate);
  }

  static Future<void> _isolateImplementation(Map<String, dynamic> initialization) async {
    DynamicLibrary dylib = DynamicLibrary.open(initialization["dllPath"]);
    FacerecService service =
        FacerecService(dylib, Pointer<Void>.fromAddress(initialization["implementation"]), initialization["facerecConfDir"], initialization["dllPath"]);

    ReceivePort receivePort = ReceivePort();
    ProcessingBlock block;

    try {
      block = service.createProcessingBlock(initialization["context"]);
    } on TDVException catch (exception) {
      (initialization["sendPort"] as SendPort).send(exception.message);

      return;
    }

    (initialization["sendPort"] as SendPort).send(receivePort.sendPort);

    await for (dynamic message in receivePort) {
      if (message is! Map) {
        print("Not a map ${message.runtimeType}");

        continue;
      }

      final _ProcessingBlockEvents event = message["event"];
      final SendPort sendPort = message["sendPort"];

      try {
        switch (event) {
          case _ProcessingBlockEvents.PROCESS:
            int context = message["context"];

            block.process(Context(dylib, Pointer<Void>.fromAddress(context)));

            sendPort.send(true);

            break;

          case _ProcessingBlockEvents.CLEAR:
            block.dispose();

            sendPort.send(true);

            return;
        }
      } on TDVException catch (exception) {
        sendPort.send(exception.message);
      }
    }
  }

  static void _checkException(dynamic isolateResult, {Isolate? isolate}) {
    if (isolateResult is String) {
      isolate?.kill(priority: Isolate.immediate);

      throw TDVException(isolateResult);
    }
  }
}
