part of face_sdk_3divi;

/// Template matching result.
class MatchResult {
  /// Distance between the templates.
  double distance = -1;

  /// False acceptance rate corresponding to the distance value taken as a threshold
  ///	at the extended LFW test (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
  double fa_r = -1;

  /// False rejection rate corresponding to the distance value taken as a threshold
  ///	at the extended LFW test (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
  double fr_r = -1;

  /// Score of templates similarity - real number from 0 to 1.
  double score = -1;

  MatchResult(this.distance, this.fa_r, this.fr_r, this.score);

  String toString() {
    return "Distance: $distance, FAR: $fa_r, FRR: $fr_r, score: $score";
  }
}

///Types of search acceleration.
enum SearchAccelerationType {
  ///No acceleration, a standard line search. The result is identical to N calls of Recognizer::verifyMatch.
  NO_SEARCH_ACCELERATION,

  ///Heuristically accelerated linear search. Heuristics reduces TAR (true acceptance rate) on the 1:N test by no more than 0.1%
  SEARCH_ACCELERATION_1,
}

///Result of searching the Template query in the TemplatesIndex.
class SearchResult {
  ///Index in the TemplatesIndex.
  late int i;

  ///Result of matching the query Template with the i-th Template from the TemplatesIndex.
  late MatchResult match_result;

  SearchResult(this.i, this.match_result);

  String toString() {
    return "index: $i, match_result: $match_result";
  }
}

void readFunc(Pointer<Void> stream, Pointer<Void> data, int bytesCount) {
  Pointer<Pointer<Uint8>> buffer_stream = stream.cast();
  Pointer<Uint8> data_stream = data.cast();
  Pointer<Int32> currentByteCount = Pointer.fromAddress(buffer_stream[0].address);

  if (bytesCount != 0) {
    for (var i = 0; i < bytesCount; i++) {
      data_stream[i] = buffer_stream[1][currentByteCount.value + i];
    }
    currentByteCount.value += bytesCount;
  }
}

/// Interface object for creating and matching templates.
class Recognizer extends _ComplexObject {
  Recognizer(DynamicLibrary dll_handle, Pointer<Void> impl) : super(dll_handle, impl);

  /// Create a template from detected face [sample] (from Capturer of VideoWorker).
  Template processing(final RawSample sample) {
    final process = _dll_handle.lookupFunction<_RecognizerProcessing, _RecognizerProcessing>(_c_namespace + 'Recognizer_processing');
    final exception = _getException();
    final templPointer = process(_impl, sample._impl, exception);
    checkException(exception, _dll_handle);
    return Template(_dll_handle, templPointer);
  }

  /// Compare two templates ([template1], [template2]).<br>
  ///<br>
  /// The order of templates does not matter. Only the templates that were
  /// created with the same method (i.e. with the same ini_file) can be compared.
  MatchResult verifyMatch(Template template1, Template template2) {
    var exception = _getException();
    final get = _dll_handle.lookupFunction<_Recognizer_verifyMatch_v2_c, _Recognizer_verifyMatch_v2_dart>(_c_namespace + 'Recognizer_verifyMatch_v2');

    Pointer<Double> distance = malloc.allocate(sizeOf<Pointer<Double>>());
    Pointer<Double> score = malloc.allocate(sizeOf<Pointer<Double>>());
    Pointer<Double> frr = malloc.allocate(sizeOf<Pointer<Double>>());
    Pointer<Double> far = malloc.allocate(sizeOf<Pointer<Double>>());
    get(_impl, template1._impl, template2._impl, distance, far, frr, score, exception);
    checkException(exception, _dll_handle);

    MatchResult result = MatchResult(distance.value, far.value, frr.value, score.value);
    malloc.free(distance);
    malloc.free(score);
    malloc.free(frr);
    malloc.free(far);
    return result;
  }

  ///Load the template.<br>
  ///The format is platform-independent.<br>
  ///Only the templates that were created with the same method <br>
  ///(i.e. with the same ini_file) can be loaded.<br>
  ///[binary_stream] - Input stream object.	The file stream (std::ifstream) must be opened with the std::ios_base::binary flag.<br>
  ///[return] - Loaded template.
  Template loadTemplate(Uint8List binary_stream) {
    var exception = _getException();
    final loadTemplate = _dll_handle.lookupFunction<_Recognizer_load_template_c, _Recognizer_load_template_dart>(_c_namespace + 'Recognizer_loadTemplate');

    Pointer<Pointer<Uint8>> _templatePointerStructure = malloc.allocate(sizeOf<Pointer<Pointer<Uint8>>>() * 2);
    _templatePointerStructure[0] = malloc.allocate(sizeOf<Pointer<Int32>>());
    _templatePointerStructure[1] = malloc.allocate(binary_stream.length);

    Pointer<Int32> byteCount = Pointer.fromAddress(_templatePointerStructure[0].address);
    byteCount.value = 0;

    for (var i = 0; i < binary_stream.length; i++) {
      _templatePointerStructure[1][i] = binary_stream[i];
    }

    final templPointer = loadTemplate(_impl, _templatePointerStructure.cast(), Pointer.fromFunction(readFunc), exception);
    checkException(exception, _dll_handle);

    malloc.free(_templatePointerStructure[0]);
    malloc.free(_templatePointerStructure[1]);
    malloc.free(_templatePointerStructure);
    // byteCount not free

    return Template(_dll_handle, templPointer);
  }

  ///Create the TemplatesIndex for quick search.<br>
  ///The total size of all indexes is limited by the license.<br>
  ///[templates] - Vector of templates for creating an index.<br>
  ///[search_threads_count] - Count of threads that will be used while searching in this index.<br>
  ///[reserve_queries_count] - Integer passed to TemplatesIndex::reserveSearchMemory, which is called with the created index.<br>
  ///[return] - Created index.
  TemplateIndex createIndex(List<Template> templates, int search_threads_count, int reserve_queries_count) {
    var exception = _getException();
    final _createIndex = _dll_handle.lookupFunction<_Recognizer_create_index_c, _Recognizer_create_index_dart>(_c_namespace + 'Recognizer_createIndex');

    Pointer<Pointer<Void>> templatesImpl = malloc.allocate(sizeOf<Pointer<Pointer<Void>>>() * templates.length);
    for (var i = 0; i < templates.length; i++) {
      templatesImpl[i] = templates[i]._impl;
    }

    final templIndexPointer = _createIndex(_impl, templates.length, templatesImpl, search_threads_count, exception);
    checkException(exception, _dll_handle);

    malloc.free(templatesImpl);
    return TemplateIndex(_dll_handle, templIndexPointer);
  }

  List<SearchResult> singleSearch(Template template, TemplateIndex templatesIndex, int k,
      {SearchAccelerationType acceleration = SearchAccelerationType.SEARCH_ACCELERATION_1}) {
    final _search = _dll_handle.lookupFunction<_Recognizer_search_v2_c, _Recognizer_search_v2_dart>(_c_namespace + 'Recognizer_search_v2');
    final exception = _getException();

    Pointer<Pointer<Void>> templatesImpl = malloc.allocate(sizeOf<Pointer<Pointer<Void>>>());
    templatesImpl[0] = template._impl;

    Pointer<Int64> result_i = malloc.allocate(k * sizeOf<Int64>());
    Pointer<Float> result_distance = malloc.allocate(k * sizeOf<Float>());
    Pointer<Float> result_far = malloc.allocate(k * sizeOf<Float>());
    Pointer<Float> result_frr = malloc.allocate(k * sizeOf<Float>());
    Pointer<Float> result_score = malloc.allocate(k * sizeOf<Float>());

    _search(_impl, acceleration.index, 1, templatesImpl, templatesIndex._impl, k, result_i, result_distance, result_far, result_frr, result_score, exception);
    checkException(exception, _dll_handle);

    List<SearchResult> result = [];

    for (var i = 0; i < k; i++) {
      result.add(SearchResult(result_i[i], MatchResult(result_distance[i], result_far[i], result_frr[i], result_score[i])));
    }

    malloc.free(result_distance);
    malloc.free(result_far);
    malloc.free(result_frr);
    malloc.free(result_score);
    malloc.free(templatesImpl);

    return result;
  }

  ///Search for the k nearest Templates in the TemplatesIndex.<br>
  ///The search time depends on k and the size of the TemplatesIndex.<br>
  ///[queries_templates] - 	The Template query.<br>
  ///[templates_index] - TemplatesIndex for search.<br>
  ///[k] - Count of the nearest templates for search.<br>
  ///[acceleration] - Acceleration type.<br>
  ///[return] - Vector with min(k, templates_index.size()) elements
  /// in ascending order of distance to the query_template.
  List<List<SearchResult>> search(List<Template> queries_templates, TemplateIndex templates_index, int k,
      {SearchAccelerationType acceleration = SearchAccelerationType.SEARCH_ACCELERATION_1}) {
    final _search = _dll_handle.lookupFunction<_Recognizer_search_v2_c, _Recognizer_search_v2_dart>(_c_namespace + 'Recognizer_search_v2');
    final exception = _getException();

    final queries_count = queries_templates.length;

    Pointer<Pointer<Void>> queriesTemplatesImpl = malloc.allocate(sizeOf<Pointer<Pointer<Void>>>() * queries_templates.length);
    for (var i = 0; i < queries_templates.length; i++) {
      queriesTemplatesImpl[i] = queries_templates[i]._impl;
    }

    Pointer<Int64> result_i = malloc.allocate(k * queries_count * sizeOf<Int64>());
    Pointer<Float> result_distance = malloc.allocate(k * queries_count * sizeOf<Float>());
    Pointer<Float> result_far = malloc.allocate(k * queries_count * sizeOf<Float>());
    Pointer<Float> result_frr = malloc.allocate(k * queries_count * sizeOf<Float>());
    Pointer<Float> result_score = malloc.allocate(k * queries_count * sizeOf<Float>());

    _search(_impl, acceleration.index, queries_count, queriesTemplatesImpl, templates_index._impl, k, result_i, result_distance, result_far, result_frr,
        result_score, exception);
    checkException(exception, _dll_handle);

    List<List<SearchResult>> result = [];

    for (var i = 0; i < queries_count; i++) {
      result.add([]);
      for (var j = 0; j < k; j++) {
        var h = i * k + j;
        result[i].add(SearchResult(result_i[h], MatchResult(result_distance[h], result_far[h], result_frr[h], result_score[h])));
      }
    }

    malloc.free(result_distance);
    malloc.free(result_far);
    malloc.free(result_frr);
    malloc.free(result_score);
    malloc.free(queriesTemplatesImpl);

    return result;
  }

  MatchResult getRocCurvePointByFar(double far_threshold) {
    final get_roc_curve =
        _dll_handle.lookupFunction<_Recognizer_get_roc_curve_c, _Recognizer_get_roc_curve_dart>(_c_namespace + 'Recognizer_getROCCurvePointByFAR_v2');

    Pointer<Double> result_distance = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_far = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_frr = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_score = malloc.allocate(sizeOf<Double>());

    final exception = _getException();
    get_roc_curve(_impl, far_threshold, result_distance, result_far, result_frr, result_score, exception);
    checkException(exception, _dll_handle);

    final result = MatchResult(result_distance.value, result_far.value, result_frr.value, result_score.value);

    malloc.free(result_distance);
    malloc.free(result_far);
    malloc.free(result_frr);
    malloc.free(result_score);

    return result;
  }

  MatchResult getRocCurvePointByFrr(double frr_threshold) {
    final get_roc_curve =
        _dll_handle.lookupFunction<_Recognizer_get_roc_curve_c, _Recognizer_get_roc_curve_dart>(_c_namespace + 'Recognizer_getROCCurvePointByFRR_v2');

    Pointer<Double> result_distance = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_far = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_frr = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_score = malloc.allocate(sizeOf<Double>());

    final exception = _getException();
    get_roc_curve(_impl, frr_threshold, result_distance, result_far, result_frr, result_score, exception);
    checkException(exception, _dll_handle);

    final result = MatchResult(result_distance.value, result_far.value, result_frr.value, result_score.value);

    malloc.free(result_distance);
    malloc.free(result_far);
    malloc.free(result_frr);
    malloc.free(result_score);

    return result;
  }

  MatchResult getRocCurvePointByScore(double score_threshold) {
    final get_roc_curve =
        _dll_handle.lookupFunction<_Recognizer_get_roc_curve_c, _Recognizer_get_roc_curve_dart>(_c_namespace + 'Recognizer_getROCCurvePointByScoreThreshold');

    Pointer<Double> result_distance = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_far = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_frr = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_score = malloc.allocate(sizeOf<Double>());

    final exception = _getException();
    get_roc_curve(_impl, score_threshold, result_distance, result_far, result_frr, result_score, exception);
    checkException(exception, _dll_handle);

    final result = MatchResult(result_distance.value, result_far.value, result_frr.value, result_score.value);

    malloc.free(result_distance);
    malloc.free(result_far);
    malloc.free(result_frr);
    malloc.free(result_score);

    return result;
  }

  MatchResult getRocCurvePointByDistance(double distance_threshold) {
    final get_roc_curve = _dll_handle
        .lookupFunction<_Recognizer_get_roc_curve_c, _Recognizer_get_roc_curve_dart>(_c_namespace + 'Recognizer_getROCCurvePointByDistanceThreshold_v2');

    Pointer<Double> result_distance = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_far = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_frr = malloc.allocate(sizeOf<Double>());
    Pointer<Double> result_score = malloc.allocate(sizeOf<Double>());

    final exception = _getException();
    get_roc_curve(_impl, distance_threshold, result_distance, result_far, result_frr, result_score, exception);
    checkException(exception, _dll_handle);

    final result = MatchResult(result_distance.value, result_far.value, result_frr.value, result_score.value);

    malloc.free(result_distance);
    malloc.free(result_far);
    malloc.free(result_frr);
    malloc.free(result_score);

    return result;
  }
}
