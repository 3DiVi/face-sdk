part of face_sdk_3divi;

/// Interface object for saving the face template.
class ContextTemplate {
  DynamicLibrary _dll_handle;

  Pointer<Void> _impl;
  var _destructor;
  bool _isDisposed = false;

  ContextTemplate(DynamicLibrary dll_handle, Pointer<Void> impl, {bool weak = false})
      : _dll_handle = dll_handle,
        _impl = impl,
        _destructor = dll_handle.lookupFunction<_contextTemplateDestructor_c, _contextTemplateDestructor_dart>(_c_namespace + 'ContextTemplate_destructor');

  ///Get a method name.<br>
  ///[return] - The name of the method that created this template.
  String getMethodName() {
    Pointer<Pointer<Uint8>> _emptyPointerList = malloc.allocate(sizeOf<Pointer<Pointer<Uint8>>>() * 2);
    _emptyPointerList[0] = malloc.allocate(sizeOf<Pointer<Int32>>());
    _emptyPointerList[1] = malloc.allocate(1000);

    Pointer<Int32> byteCount = Pointer.fromAddress(_emptyPointerList[0].address);
    byteCount.value = 0;

    final templateSave =
        _dll_handle.lookupFunction<_ContextTemplate_get_method_name_c, _ContextTemplate_get_method_name_dart>(_c_namespace + 'ContextTemplate_getMethodName');

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

  ///Save the template<br>
  ///The format is platform-independent.
  Uint8List save() {
    Pointer<Pointer<Uint8>> _emptyPointerList = malloc.allocate(sizeOf<Pointer<Pointer<Uint8>>>() * 2);
    _emptyPointerList[0] = malloc.allocate(sizeOf<Pointer<Int32>>());
    _emptyPointerList[1] = malloc.allocate(1000);

    Pointer<Int32> byteCount = Pointer.fromAddress(_emptyPointerList[0].address);
    byteCount.value = 0;

    final templateSave = _dll_handle.lookupFunction<_ContextTemplate_save_c, _ContextTemplate_save_dart>(_c_namespace + 'ContextTemplate_save');

    var exception = _getException();
    templateSave(_impl, _emptyPointerList.cast(), Pointer.fromFunction(writeFunc), exception);
    checkException(exception, _dll_handle);

    Uint8List result = Uint8List(byteCount.value);
    for (var i = 0; i < byteCount.value; i++) {
      result[i] = _emptyPointerList[1][i];
    }
    malloc.free(_emptyPointerList[0]);
    malloc.free(_emptyPointerList[1]);
    malloc.free(_emptyPointerList);

    return result;
  }

  int size() {
    var exception = _getException();

    int result = _dll_handle.lookupFunction<_contextTemplate_size_c, _contextTemplate_size_dart>(_c_namespace + 'ContextTemplate_size')(_impl, exception);

    checkException(exception, _dll_handle);

    return result;
  }

  void dispose() {
    if (_isDisposed) {
      return;
    }

    _destructor(_impl);
    _isDisposed = true;
  }
}
