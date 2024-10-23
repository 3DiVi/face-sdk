part of face_sdk_3divi;

void _assign_pointers_vector_func(Pointer<Void> pointers_vector, Pointer<Pointer<Void>> elements, int elements_count) {
  Pointer<Pointer<Void>> vec = pointers_vector.cast();
  if (elements_count != 0) {
    for (var i = 0; i < elements_count; i++) {
      vec[i] = Pointer.fromAddress(elements[i].address);
    }
  }
}

/// Interface object for detecting or tracking of faces in the images or video sequences
class Capturer extends _ComplexObject {
  /// Can only be created via FacerecService.createCapturer
  Capturer(DynamicLibrary dll_handle, Pointer<Void> impl) : super(dll_handle, impl);

  /// Capture faces in a given encoded image or video frame.
  List<RawSample> capture(Uint8List encodedImage) {
    final cap = _dll_handle.lookupFunction<_CapturerCapBytes_c, _CapturerCapBytes_dart>(_c_namespace + 'Capturer_capture_encoded_image');

    final Pointer<Uint8> frameData = malloc.allocate<Uint8>(encodedImage.length);
    final pointerList = frameData.asTypedList(encodedImage.length);
    Pointer<Pointer<Void>> _emptyPointerList = malloc.allocate(sizeOf<Pointer<Pointer<Void>>>() * 100);
    Pointer<Pointer<Void>> exception = _getException();
    List<int> addressBefore = new List<int>.generate(100, (i) => _emptyPointerList[i].address);

    pointerList.setAll(0, encodedImage);
    cap(_impl, frameData.cast(), encodedImage.length, _emptyPointerList.cast(), Pointer.fromFunction(_assign_pointers_vector_func), exception);
    checkException(exception, _dll_handle);

    List<RawSample> rss = [];

    for (var i = 0; i < 100; i++) {
      if (addressBefore[i] != _emptyPointerList[i].address)
        rss.add(RawSample(_dll_handle, _emptyPointerList[i]));
      else
        break;
    }

    malloc.free(_emptyPointerList);
    malloc.free(frameData);
    return rss;
  }

  // TODO: refactoring this
  List<RawSample> captureRawImageF(RawImageF image) {
    final cap = _dll_handle.lookupFunction<_CapCap_c, _CapCap_dart>(_c_namespace + 'Capturer_capture_raw_image_with_crop');
    Pointer<Pointer<Void>> _emptyPointerList = malloc.allocate(sizeOf<Pointer<Pointer<Void>>>() * 100);
    Pointer<Pointer<Void>> exception = _getException();

    List<int> addressBefore = new List<int>.generate(100, (i) => _emptyPointerList[i].address);

    cap(
        _impl,
        image.data.cast(),
        image.width,
        image.height,
        image.format.index,
        image.with_crop,
        image.crop_info_offset_x,
        image.crop_info_offset_y,
        image.crop_info_data_image_width,
        image.crop_info_data_image_height,
        _emptyPointerList.cast(),
        Pointer.fromFunction(_assign_pointers_vector_func),
        exception);
    checkException(exception, _dll_handle);

    List<RawSample> rss = [];

    for (var i = 0; i < 100; i++) {
      if (addressBefore[i] != _emptyPointerList[i].address)
        rss.add(RawSample(_dll_handle, _emptyPointerList[i]));
      else
        break;
    }

    malloc.free(_emptyPointerList);

    return rss;
  }
}
