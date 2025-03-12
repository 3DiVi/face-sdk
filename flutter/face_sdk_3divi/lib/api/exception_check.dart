part of face_sdk_3divi;


Pointer<Pointer<Void>> _getException(){
  Pointer<Pointer<Void>> exception = malloc.allocate(1);

  if(exception.address == nullptr.address)
    throw Exception("_getException: Failed to allocate a 1 byte");

  exception.value = nullptr;
  return exception;
}

class TDVException implements Exception {
  String message;
  TDVException(this.message);
}

void checkException(Pointer<Pointer<Void>> exception, DynamicLibrary dll_handle){
  if (exception.value.address != nullptr.address) {
    final check_exception_code = dll_handle.lookupFunction<_exceptionCode_c, _exceptionCode_dart>
      (_c_namespace + 'apiException_code');

    final check_exception_what = dll_handle.lookupFunction<_exceptionWhat, _exceptionWhat>
      (_c_namespace + 'apiException_what');

    final res_ex = check_exception_code(exception.value);
    developer.log("Check ex " + res_ex.toRadixString(16), name: 'my.app.category');

    final res_ex_content = check_exception_what(exception.value).toDartString();
    developer.log("Check ex2 " + res_ex_content, name: 'my.app.category');
    if (res_ex != 0){
      throw TDVException("Error 0x$res_ex. Text: $res_ex_content");
    }
  }

  if(exception.address != nullptr.address)
    malloc.free(exception);
}


void tdvCheckException(Pointer<Pointer<Void>> exception, DynamicLibrary dll_handle) {
  if (exception.value.address != nullptr.address) {
    final check_exception_code = dll_handle.lookupFunction<_exceptionCode_c, _exceptionCode_dart>
      ("TDVException_" + 'getErrorCode');

    final check_exception_message = dll_handle.lookupFunction<_exceptionWhat, _exceptionWhat>
      ("TDVException_" + 'getMessage');

    final res_ex = check_exception_code(exception.value);
    developer.log("Check context ex " + res_ex.toRadixString(16), name: 'my.app.category');

    final res_ex_content = check_exception_message(exception.value).toDartString();
    developer.log("Check context ex2 " + res_ex_content, name: 'my.app.category');
    if (res_ex != 0){
      throw TDVException("Error 0x$res_ex. Text: $res_ex_content");
    }
  }

  if(exception.address != nullptr.address)
    malloc.free(exception);
}