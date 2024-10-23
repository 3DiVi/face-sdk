part of face_sdk_3divi;

enum _RecognizerEvents { PROCESSING, VERIFY_MATCH, LOAD_TEMPLATE, CREATE_INDEX, SINGLE_SEARCH, SEARCH, CLEAR }

class AsyncRecognizer {
  Isolate _isolate;
  SendPort _sendPort;
  String _dllPath;
  bool _isDisposed = false;

  AsyncRecognizer._create(this._isolate, this._sendPort, this._dllPath);

  Future<Template> processing(RawSample sample) async {
    ReceivePort receivePort = ReceivePort();
    int result = 0;

    _sendPort.send({"event": _RecognizerEvents.PROCESSING, "sendPort": receivePort.sendPort, "rawSample": sample._impl.address});

    result = await receivePort.first;

    return Template(DynamicLibrary.open(_dllPath), Pointer<Void>.fromAddress(result));
  }

  Future<MatchResult> verifyMatch(Template template1, Template template2) async {
    ReceivePort receivePort = ReceivePort();
    Map<String, double> result = Map<String, double>();

    _sendPort.send({
      "event": _RecognizerEvents.VERIFY_MATCH,
      "sendPort": receivePort.sendPort,
      "template1": template1._impl.address,
      "template2": template2._impl.address
    });

    result = await receivePort.first;

    return MatchResult(result["distance"]!, result["fa_r"]!, result["fr_r"]!, result["score"]!);
  }

  Future<Template> loadTemplate(Uint8List binary_stream) async {
    ReceivePort receivePort = ReceivePort();

    _sendPort.send({"event": _RecognizerEvents.LOAD_TEMPLATE, "sendPort": receivePort.sendPort, "binary_stream": binary_stream});

    int result = await receivePort.first;

    return Template(DynamicLibrary.open(_dllPath), Pointer<Void>.fromAddress(result));
  }

  Future<TemplateIndex> createIndex(List<Template> templates, int search_threads_count, int reserve_queries_count) async {
    ReceivePort receivePort = ReceivePort();

    List<int> pointers = [];

    templates.forEach((template) => pointers.add(template._impl.address));

    _sendPort.send({
      "event": _RecognizerEvents.CREATE_INDEX,
      "sendPort": receivePort.sendPort,
      "pointers": pointers,
      "search_threads_count": search_threads_count,
      "reserve_queries_count": reserve_queries_count
    });

    int result = await receivePort.first;

    return TemplateIndex(DynamicLibrary.open(_dllPath), Pointer<Void>.fromAddress(result));
  }

  Future<List<SearchResult>> singleSearch(Template template, TemplateIndex templatesIndex, int k,
      {SearchAccelerationType acceleration = SearchAccelerationType.SEARCH_ACCELERATION_1}) async {
    ReceivePort receivePort = ReceivePort();
    List<SearchResult> result = [];

    _sendPort.send({
      "event": _RecognizerEvents.SINGLE_SEARCH,
      "sendPort": receivePort.sendPort,
      "template": template._impl.address,
      "templateIndex": templatesIndex._impl.address,
      "k": k,
      "acceleration": acceleration
    });

    List<Map<String, dynamic>> data = await receivePort.first;

    for (Map<String, dynamic> mapData in data) {
      result.add(SearchResult(mapData["i"], MatchResult(mapData["distance"]!, mapData["fa_r"]!, mapData["fr_r"]!, mapData["score"]!)));
    }

    return result;
  }

  Future<List<List<SearchResult>>> search(List<Template> queries_templates, TemplateIndex templates_index, int k,
      {SearchAccelerationType acceleration = SearchAccelerationType.SEARCH_ACCELERATION_1}) async {
    ReceivePort receivePort = ReceivePort();
    List<List<SearchResult>> result = [];

    List<int> pointers = [];

    queries_templates.forEach((template) => pointers.add(template._impl.address));

    _sendPort.send({
      "event": _RecognizerEvents.SEARCH,
      "sendPort": receivePort.sendPort,
      "queries_templates": pointers,
      "templateIndex": templates_index._impl.address,
      "k": k,
      "acceleration": acceleration
    });

    List<List<Map<String, dynamic>>> data = await receivePort.first;

    for (List<Map<String, dynamic>> listMapData in data) {
      List<SearchResult> searchResults = [];

      for (Map<String, dynamic> mapData in listMapData) {
        searchResults.add(SearchResult(mapData["i"], MatchResult(mapData["distance"]!, mapData["fa_r"]!, mapData["fr_r"]!, mapData["score"]!)));
      }

      result.add(searchResults);
    }

    return result;
  }

  static Future<AsyncRecognizer> create(Pointer<Void> implementation, String facerecConfDir, String dllPath, String ini_file, bool processing, bool matching,
      bool processing_less_memory_consumption) async {
    ReceivePort receivePort = ReceivePort();

    Isolate isolate = await Isolate.spawn(_isolateImplementation, {
      "implementation": implementation.address,
      "facerecConfDir": facerecConfDir,
      "dllPath": dllPath,
      "ini_file": ini_file,
      "processing": processing,
      "matching": matching,
      "processing_less_memory_consumption": processing_less_memory_consumption,
      "sendPort": receivePort.sendPort
    });
    SendPort sendPort = await receivePort.first;

    return AsyncRecognizer._create(isolate, sendPort, dllPath);
  }

  Future<void> dispose() async {
    if (_isDisposed) {
      return;
    }

    ReceivePort receivePort = ReceivePort();

    _sendPort.send({
      "event": _RecognizerEvents.CLEAR,
      "sendPort": receivePort.sendPort,
    });

    _isDisposed = await receivePort.first;

    _isolate.kill(priority: Isolate.immediate);
  }

  static Future<void> _isolateImplementation(Map<String, dynamic> initialization) async {
    DynamicLibrary dylib = DynamicLibrary.open(initialization["dllPath"]);
    FacerecService service = FacerecService(DynamicLibrary.open(initialization["dllPath"]), Pointer<Void>.fromAddress(initialization["implementation"]),
        initialization["facerecConfDir"], initialization["dllPath"]);

    ReceivePort receivePort = ReceivePort();
    bool processing = initialization["processing"];
    bool matching = initialization["matching"];
    bool processing_less_memory_consumption = initialization["processing_less_memory_consumption"];
    Recognizer recognizer = service.createRecognizer(initialization["ini_file"],
        processing: processing, matching: matching, processing_less_memory_consumption: processing_less_memory_consumption);

    (initialization["sendPort"] as SendPort).send(receivePort.sendPort);

    await for (dynamic message in receivePort) {
      if (message is! Map) {
        print("Not a map ${message.runtimeType}");

        continue;
      }

      final _RecognizerEvents event = message["event"];
      final SendPort sendPort = message["sendPort"];

      switch (event) {
        case _RecognizerEvents.PROCESSING:
          Template result = recognizer.processing(RawSample(dylib, Pointer<Void>.fromAddress(message["rawSample"])));

          sendPort.send(result._impl.address);

          break;

        case _RecognizerEvents.VERIFY_MATCH:
          Template template1 = Template(dylib, Pointer<Void>.fromAddress(message["template1"]));
          Template template2 = Template(dylib, Pointer<Void>.fromAddress(message["template2"]));

          MatchResult result = recognizer.verifyMatch(template1, template2);

          sendPort.send({"distance": result.distance, "fa_r": result.fa_r, "fr_r": result.fr_r, "score": result.score});

          break;

        case _RecognizerEvents.LOAD_TEMPLATE:
          Template result = recognizer.loadTemplate(message["binary_stream"]);

          sendPort.send(result._impl.address);

          break;

        case _RecognizerEvents.CREATE_INDEX:
          List<int> pointers = message["pointers"];
          int search_threads_count = message["search_threads_count"];
          int reserve_queries_count = message["reserve_queries_count"];

          List<Template> templates = [];

          pointers.forEach((pointer) => templates.add(Template(dylib, Pointer<Void>.fromAddress(pointer))));

          TemplateIndex templateIndex = recognizer.createIndex(templates, search_threads_count, reserve_queries_count);

          sendPort.send(templateIndex._impl.address);

          break;

        case _RecognizerEvents.SINGLE_SEARCH:
          Template template = Template(dylib, Pointer<Void>.fromAddress(message["template"]));
          TemplateIndex templateIndex = TemplateIndex(dylib, Pointer<Void>.fromAddress(message["templateIndex"]));
          int k = message["k"];
          SearchAccelerationType acceleration = message["acceleration"];
          List<Map<String, dynamic>> result = [];

          List<SearchResult> data = recognizer.singleSearch(template, templateIndex, k, acceleration: acceleration);

          for (SearchResult searchResult in data) {
            MatchResult matchResult = searchResult.match_result;

            result.add({"i": searchResult.i, "distance": matchResult.distance, "fa_r": matchResult.fa_r, "fr_r": matchResult.fr_r, "score": matchResult.score});
          }

          sendPort.send(result);

          break;

        case _RecognizerEvents.SEARCH:
          List<Template> queries_templates = [];
          TemplateIndex templateIndex = TemplateIndex(dylib, Pointer<Void>.fromAddress(message["templateIndex"]));
          int k = message["k"];
          SearchAccelerationType acceleration = message["acceleration"];
          List<List<Map<String, dynamic>>> result = [];

          List<int> pointers = message["queries_templates"];

          pointers.forEach((pointer) => queries_templates.add(Template(dylib, Pointer<Void>.fromAddress(pointer))));

          List<List<SearchResult>> data = recognizer.search(queries_templates, templateIndex, k, acceleration: acceleration);

          for (List<SearchResult> searchResults in data) {
            List<Map<String, dynamic>> temp = [];

            for (SearchResult searchResult in searchResults) {
              MatchResult matchResult = searchResult.match_result;

              temp.add({"i": searchResult.i, "distance": matchResult.distance, "fa_r": matchResult.fa_r, "fr_r": matchResult.fr_r, "score": matchResult.score});
            }

            result.add(temp);
          }

          sendPort.send(result);

          break;

        case _RecognizerEvents.CLEAR:
          recognizer.dispose();

          sendPort.send(true);

          return;
      }
    }
  }
}
