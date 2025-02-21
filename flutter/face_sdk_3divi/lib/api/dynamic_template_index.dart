part of face_sdk_3divi;

class DynamicTemplateIndex {
  DynamicLibrary _dll_handle;

  Pointer<Void> _impl;
  var _destructor;
  bool _isDisposed = false;
  bool _weak;

  DynamicTemplateIndex(DynamicLibrary dll_handle, Pointer<Void> impl, {bool weak = false})
      : _dll_handle = dll_handle,
        _impl = impl,
        _destructor = dll_handle
            .lookupFunction<_DynamicTemplateIndexDestructor_c, _DynamicTemplateIndexDestructor_dart>(_c_namespace + "DynamicTemplateIndex_destructor"),
        _weak = weak;

  String getMethodName() {
    Pointer<Pointer<Uint8>> _emptyPointerList = malloc.allocate(sizeOf<Pointer<Pointer<Uint8>>>() * 2);
    _emptyPointerList[0] = malloc.allocate(sizeOf<Pointer<Int32>>());
    _emptyPointerList[1] = malloc.allocate(1000);

    Pointer<Int32> byteCount = Pointer.fromAddress(_emptyPointerList[0].address);
    byteCount.value = 0;

    final templateSave = _dll_handle.lookupFunction<_DynamicTemplateIndex_get_method_name_c, _DynamicTemplateIndex_get_method_name_dart>(
        _c_namespace + "DynamicTemplateIndex_getMethodName");

    var exception = _getException();
    templateSave(_impl, _emptyPointerList.cast(), Pointer.fromFunction(writeFunc), exception);
    checkException(exception, _dll_handle);

    List<int> result = [];
    for (var i = 0; i < byteCount.value; i++) {
      result.add(_emptyPointerList[1][i]);
    }
    malloc.free(_emptyPointerList[0]);
    malloc.free(_emptyPointerList[1]);
    malloc.free(_emptyPointerList);

    return utf8.decode(result);
  }

  int size() {
    var exception = _getException();

    int result = _dll_handle.lookupFunction<_DynamicTemplateIndex_size_c, _DynamicTemplateIndex_size_dart>(_c_namespace + "DynamicTemplateIndex_size")(
        _impl, exception);

    checkException(exception, _dll_handle);

    return result;
  }

  int capacity() {
    var exception = _getException();

    int result = _dll_handle.lookupFunction<_DynamicTemplateIndex_capacity_c, _DynamicTemplateIndex_capacity_dart>(
        _c_namespace + "DynamicTemplateIndex_capacity")(_impl, exception);

    checkException(exception, _dll_handle);

    return result;
  }

  String getUUID(int index) {
    Pointer<Pointer<Uint8>> _emptyPointerList = malloc.allocate(sizeOf<Pointer<Pointer<Uint8>>>() * 2);
    _emptyPointerList[0] = malloc.allocate(sizeOf<Pointer<Int32>>());
    _emptyPointerList[1] = malloc.allocate(1000);

    Pointer<Int32> byteCount = Pointer.fromAddress(_emptyPointerList[0].address);
    byteCount.value = 0;

    final atByIndex = _dll_handle
        .lookupFunction<_DynamicTemplateIndex_at_by_index_c, _DynamicTemplateIndex_at_by_index_dart>(_c_namespace + "DynamicTemplateIndex_at_by_index");

    var exception = _getException();
    atByIndex(_impl, index, _emptyPointerList.cast(), Pointer.fromFunction(writeFunc), exception);
    checkException(exception, _dll_handle);

    List<int> result = [];
    for (var i = 0; i < byteCount.value; i++) {
      result.add(_emptyPointerList[1][i]);
    }
    malloc.free(_emptyPointerList[0]);
    malloc.free(_emptyPointerList[1]);
    malloc.free(_emptyPointerList);

    return utf8.decode(result);
  }

  ContextTemplate at(String uuid) {
    final atByUuid = _dll_handle
        .lookupFunction<_DynamicTemplateIndex_at_by_uuid_c, _DynamicTemplateIndex_at_by_uuid_dart>(_c_namespace + "DynamicTemplateIndex_at_by_uuid");

    var exception = _getException();
    ContextTemplate result = ContextTemplate(_dll_handle, atByUuid(_impl, uuid.toNativeUtf8(), exception));

    checkException(exception, _dll_handle);

    return result;
  }

  ContextTemplate get(int index) {
    final getByIndex = _dll_handle.lookupFunction<_DynamicTemplateIndex_get_c, _DynamicTemplateIndex_get_dart>(_c_namespace + "DynamicTemplateIndex_get");

    var exception = _getException();

    ContextTemplate result = ContextTemplate(_dll_handle, getByIndex(_impl, index, exception));

    checkException(exception, _dll_handle);

    return result;
  }

  void addRecognizerTemplate(Template template, String uuid) {
    var exception = _getException();

    final addConstructor =
        _dll_handle.lookupFunction<_DynamicTemplateIndex_add_1_c, _DynamicTemplateIndex_add_1_dart>(_c_namespace + "DynamicTemplateIndex_add_1_");

    addConstructor(_impl, template._impl, uuid.toNativeUtf8(), exception);

    checkException(exception, _dll_handle);
  }

  void addRecognizerTemplates(List<Template> templates, List<String> uuids) {
    var exception = _getException();

    final addConstructor =
        _dll_handle.lookupFunction<_DynamicTemplateIndex_add_2_c, _DynamicTemplateIndex_add_2_dart>(_c_namespace + "DynamicTemplateIndex_add_2");

    Pointer<Pointer<Void>> tempTemplates = malloc.allocate(sizeOf<Pointer<Pointer<Void>>>() * templates.length);
    Pointer<Pointer<Utf8>> tempUuids = malloc.allocate(sizeOf<Pointer<Pointer<Utf8>>>() * uuids.length);

    for (int i = 0; i < templates.length; i++) {
      tempTemplates[i] = templates[i]._impl;
    }

    for (int i = 0; i < uuids.length; i++) {
      tempUuids[i] = uuids[i].toNativeUtf8();
    }

    addConstructor(_impl, tempTemplates, tempUuids, templates.length, exception);

    malloc.free(tempTemplates);
    malloc.free(tempUuids);

    checkException(exception, _dll_handle);
  }

  void addContextTemplate(ContextTemplate template, String uuid) {
    var exception = _getException();

    final addConstructor =
        _dll_handle.lookupFunction<_DynamicTemplateIndex_add_3_c, _DynamicTemplateIndex_add_3_dart>(_c_namespace + "DynamicTemplateIndex_add_3");

    addConstructor(_impl, template._impl, uuid.toNativeUtf8(), exception);

    checkException(exception, _dll_handle);
  }

  void addContextTemplates(List<ContextTemplate> templates, List<String> uuids) {
    var exception = _getException();

    final addConstructor =
        _dll_handle.lookupFunction<_DynamicTemplateIndex_add_4_c, _DynamicTemplateIndex_add_4_dart>(_c_namespace + "DynamicTemplateIndex_add_4");

    Pointer<Pointer<Void>> tempTemplates = malloc.allocate(sizeOf<Pointer<Pointer<Void>>>() * templates.length);
    Pointer<Pointer<Utf8>> tempUuids = malloc.allocate(sizeOf<Pointer<Pointer<Utf8>>>() * uuids.length);

    for (int i = 0; i < templates.length; i++) {
      tempTemplates[i] = templates[i]._impl;
    }

    for (int i = 0; i < uuids.length; i++) {
      tempUuids[i] = uuids[i].toNativeUtf8();
    }

    addConstructor(_impl, tempTemplates, tempUuids, templates.length, exception);

    malloc.free(tempTemplates);
    malloc.free(tempUuids);

    checkException(exception, _dll_handle);
  }

  void removeByUuid(String uuid) {
    var exception = _getException();

    final removeConstructor =
        _dll_handle.lookupFunction<_DynamicTemplateIndex_remove_1_c, _DynamicTemplateIndex_remove_1_dart>(_c_namespace + "DynamicTemplateIndex_remove_1");

    removeConstructor(_impl, uuid.toNativeUtf8(), exception);

    checkException(exception, _dll_handle);
  }

  void removeByUuids(List<String> uuids) {
    var exception = _getException();

    final removeConstructor =
        _dll_handle.lookupFunction<_DynamicTemplateIndex_remove_2_c, _DynamicTemplateIndex_remove_2_dart>(_c_namespace + "DynamicTemplateIndex_remove_2");

    Pointer<Pointer<Utf8>> tempUuids = malloc.allocate(sizeOf<Pointer<Pointer<Utf8>>>() * uuids.length);

    for (int i = 0; i < uuids.length; i++) {
      tempUuids[i] = uuids[i].toNativeUtf8();
    }

    removeConstructor(_impl, tempUuids, uuids.length, exception);

    malloc.free(tempUuids);

    checkException(exception, _dll_handle);
  }

  void concat(DynamicTemplateIndex otherIndex) {
    var exception = _getException();

    final concatConstructor =
        _dll_handle.lookupFunction<_DynamicTemplateIndex_concat_c, _DynamicTemplateIndex_concat_dart>(_c_namespace + "DynamicTemplateIndex_concatenate");

    concatConstructor(_impl, otherIndex._impl, exception);

    checkException(exception, _dll_handle);
  }

  void dispose() {
    if (_weak || _isDisposed) {
      return;
    }

    _destructor(_impl);
    _isDisposed = true;
  }
}
