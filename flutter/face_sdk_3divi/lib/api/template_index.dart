part of face_sdk_3divi;

/// Interface object for saving the face template.
class TemplateIndex extends _ComplexObject {
  TemplateIndex(DynamicLibrary dll_handle, Pointer<Void> impl) : super(dll_handle, impl);

  ///Get an t-th template.<br>
  ///[t] - Integer t: 0 <= t < size.<br>
  ///[return] - The t-th template.
  Template at(int t) {
    var exception = _getException();
    final getAt = _dll_handle.lookupFunction<_Templates_index_at_c, _Templates_index_at_dart>(_c_namespace + 'TemplatesIndex_at');

    var result = getAt(_impl, t, exception);
    checkException(exception, _dll_handle);

    return Template(_dll_handle, result);
  }

  ///Get a number of templates in the index.<br>
  ///[return] - Number of templates in the index.
  int size() {
    var exception = _getException();
    final getSize = _dll_handle.lookupFunction<_Templates_index_size_c, _Templates_index_size_dart>(_c_namespace + 'TemplatesIndex_size');

    var result = getSize(_impl, exception);
    checkException(exception, _dll_handle);

    return result;
  }

  ///Reserve memory for temporary buffers used while searching.<br>
  ///These temporary buffers require 8 * size() * queries_count bytes.<br>
  ///[queries_count] - Integer queries_count >= 0 - max size of the queries_templates vector passed
  /// to Recognizer::search at once to prepare the buffers for.
  void reserveSearchMemory(int queries_count) {
    var exception = _getException();
    final _reserveSearchMemory = _dll_handle.lookupFunction<_Templates_index_reserve_search_memory_c, _Templates_index_reserve_search_memory_dart>(
        _c_namespace + 'TemplatesIndex_reserveSearchMemory');

    _reserveSearchMemory(_impl, queries_count, exception);
    checkException(exception, _dll_handle);
  }
}
