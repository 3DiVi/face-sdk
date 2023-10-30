part of face_sdk_3divi;

void writeFunc(Pointer<Void> stream, Pointer<Void> data, int bytesCount){
  Pointer<Pointer<Uint8>> buffer_stream = stream.cast();
  Pointer<Int32> currentByteCount = Pointer.fromAddress(buffer_stream[0].address);

  if (bytesCount != 0) {
    for (var i = 0; i < bytesCount; i++) {
      buffer_stream[1][currentByteCount.value + i] = Pointer<Uint8>.fromAddress(data.address + i).value;
    }
    currentByteCount.value += bytesCount;
  }
}



/// Interface object for saving the face template.
class Template extends _ComplexObject {
  Template(DynamicLibrary dll_handle, Pointer<Void> impl) :
        super(dll_handle, impl);

///Get a method name.<br>
///[return] - The name of the method that created this template.
  String getMethodName(){
    Pointer<Pointer<Uint8>> _emptyPointerList = malloc.allocate(sizeOf<Pointer<Pointer<Uint8>>>() * 2);
    _emptyPointerList[0] = malloc.allocate(sizeOf<Pointer<Int32>>());
    _emptyPointerList[1] = malloc.allocate(1000);

    Pointer<Int32> byteCount = Pointer.fromAddress(_emptyPointerList[0].address);
    byteCount.value = 0;

    final templateSave = _dll_handle.lookupFunction<_Template_get_method_name_c, _Template_get_method_name_dart>
      (_c_namespace + 'Template_getMethodName');

    var exception = _getException();
    templateSave(
        _impl,
        _emptyPointerList.cast(),
        Pointer.fromFunction(writeFunc),
        exception);
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
  Uint8List save(){
    Pointer<Pointer<Uint8>> _emptyPointerList = malloc.allocate(sizeOf<Pointer<Pointer<Uint8>>>() * 2);
    _emptyPointerList[0] = malloc.allocate(sizeOf<Pointer<Int32>>());
    _emptyPointerList[1] = malloc.allocate(1000);

    Pointer<Int32> byteCount = Pointer.fromAddress(_emptyPointerList[0].address);
    byteCount.value = 0;

    final templateSave = _dll_handle.lookupFunction<_Template_save_c, _Template_save_dart>
      (_c_namespace + 'Template_save');

    var exception = _getException();
    templateSave(
        _impl,
        _emptyPointerList.cast(),
        Pointer.fromFunction(writeFunc),
        exception);
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

}