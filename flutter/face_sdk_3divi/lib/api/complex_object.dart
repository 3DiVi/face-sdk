part of face_sdk_3divi;


class _ComplexObject {
  /// Private variable
  DynamicLibrary _dll_handle;
  /// Private variable
  Pointer<Void> _impl;
  var _destructor;
  bool _isDisposed = false;

  _ComplexObject(DynamicLibrary dll_handle, Pointer<Void> impl):
        _dll_handle = dll_handle,
        _impl = impl,
        _destructor = dll_handle.lookupFunction<_objDestructor_c, _objDestructor_dart>(_c_namespace + 'apiObject_destructor');

  /// Removing an object from memory. Must be called manually.
  void dispose(){
    if (_isDisposed)
      return;
    _destructor(_impl);
    _isDisposed = true;
  }
}
