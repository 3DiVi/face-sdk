part of face_sdk_3divi;

class PointerBuffer<T extends NativeType> {
  late Pointer<Void> ptr;

  late Pointer<Int32> _elementSizePointer;
  late Pointer<Int32> _sizePointer;
  late Pointer<Pointer<T>> _dataPointer;

  PointerBuffer(
      this.ptr,
      this._elementSizePointer,
      this._sizePointer,
      this._dataPointer
      );

  PointerBuffer.empty(int elementSize) {
    _elementSizePointer = malloc.allocate(sizeOf<Int32>());
    _elementSizePointer.value = elementSize;

    _sizePointer = malloc.allocate(sizeOf<Int32>());
    _sizePointer.value = -1;

    _dataPointer = malloc.allocate(sizeOf<Pointer<T>>());

    ptr = malloc.allocate(sizeOf<Int64>() * 3);
    Int64List ptrData = ptr.cast<Int64>().asTypedList(3);

    ptrData[0] = _elementSizePointer.address;
    ptrData[1] = _sizePointer.address;
    ptrData[2] = _dataPointer.address;
  }

  PointerBuffer.fromSize(int elementSize, int length) {
    _elementSizePointer = malloc.allocate(sizeOf<Int32>());
    _elementSizePointer.value = elementSize;

    _sizePointer = malloc.allocate(sizeOf<Int32>());
    _sizePointer.value = length;

    _dataPointer = malloc.allocate(sizeOf<Pointer<T>>());
    _dataPointer.value = malloc.allocate(elementSize * length);

    if(_dataPointer.value.address == nullptr.address)
      throw Exception("RawSample.fromSize: Failed to allocate a ${elementSize * length} bytes");

    ptr = malloc.allocate(sizeOf<Int64>() * 3);
    Int64List ptrData = ptr.cast<Int64>().asTypedList(3);

    ptrData[0] = _elementSizePointer.address;
    ptrData[1] = _sizePointer.address;
    ptrData[2] = _dataPointer.address;
  }

  PointerBuffer.fromPointer(Pointer sourcePtr) {
    ptr = sourcePtr.cast();
    Int64List ptrData = ptr.cast<Int64>().asTypedList(3);

    _elementSizePointer = Pointer.fromAddress(ptrData[0]);
    _sizePointer = Pointer.fromAddress(ptrData[1]);
    _dataPointer = Pointer.fromAddress(ptrData[2]);
  }

  void dispose() {
    if(length != -1)
      malloc.free(_dataPointer.value);

    malloc.free(_dataPointer);

    malloc.free(_elementSizePointer);
    malloc.free(_sizePointer);
    malloc.free(ptr);
  }

  int get length {
    return _sizePointer.address == nullptr.address ? -1 : _sizePointer.value;
  }

  int get elementSize {
    return _elementSizePointer.address == nullptr.address ? -1 : _elementSizePointer.value;
  }

  void setSize(int newLength) {
    if(length != -1)
      malloc.free(_dataPointer.value);

    _sizePointer.value = newLength;
    _dataPointer.value = malloc.allocate(elementSize * newLength);

    if(_dataPointer.value.address == nullptr.address)
      throw Exception("RawSample.setSize: Failed to allocate a ${elementSize * newLength} bytes");
  }

  Pointer<U> dataPtr<U extends NativeType>() {
    return _dataPointer.value.cast<U>();
  }
}

class WrapBytesWrite {
  static void writeByteListFunc(Pointer<Void> stream, Pointer<Uint8> data, int bytesCount) {
    PointerBuffer buffer = PointerBuffer<Float>.fromPointer(stream);
    buffer.setSize(bytesCount);
    Uint8List streamDataPtr = buffer.dataPtr<Uint8>().asTypedList(bytesCount);

    Uint8List dataList = data.asTypedList(bytesCount);

    streamDataPtr.setAll(0, dataList);
  }

  static void writeFloatListFunc(
      Pointer<Void> pointersVector,
      Pointer<Float> elements,
      int elementsCount
      )
  {
    PointerBuffer buffer = PointerBuffer<Float>.fromPointer(pointersVector);
    buffer.setSize(elementsCount);

    Float32List streamDataPtr = buffer.dataPtr<Float>().asTypedList(elementsCount);

    Pointer<Float> castPointer = elements.cast();
    Float32List dataPtr = castPointer.asTypedList(elementsCount);

    streamDataPtr.setAll(0, dataPtr);
  }
}

/// Image formats for saving.
enum ImageFormatCode {
  /// JPEG (lossy compression).
  IMAGE_FORMAT_JPG,

  /// PNG (lossless compression).
  IMAGE_FORMAT_PNG,

  /// TIFF (lossless compression).
  IMAGE_FORMAT_TIF,

  /// BMP (no compression).
  IMAGE_FORMAT_BMP
}
///Face cropping types.
enum FaceCutType{
  /// Unspecified cropping (any sample type).
  FACE_CUT_BASE,

  /// ISO/IEC 19794-5 Full Frontal (for ID, travel documents) (only frontal sample type).
  FACE_CUT_FULL_FRONTAL,

  /// ISO/IEC 19794-5 Token Frontal (fixed eye positions) (only frontal sample type).
  FACE_CUT_TOKEN_FRONTAL
}

/// 3D point.
class Point {
  double _x;
  double _y;
  double _z;

  Point(this._x, this._y, this._z);

  /// X coordinate.
  double get x { return _x;}

  /// Y coordinate.
  double get y { return _y;}

  /// Z coordinate.
  double get z { return _z;}

  String toString(){
    return "Point(x: $x, y: $y, z: $z)";
  }
}


/// Face orientation angles.
class Angles {
  ///Yaw angle in degrees.
  double _yaw = 0;
  ///Pitch angle in degrees.
  double _pitch = 0;
  ///Roll angle in degrees.
  double _roll = 0;

  Angles(this._yaw, this._pitch, this._roll);

  /// Yaw angle in degrees.
  double get yaw { return _yaw;}

  /// Pitch angle in degrees.
  double get pitch { return _pitch;}

  /// Roll angle in degrees.
  double get roll { return _roll;}

  String toString(){
    return "Angle(yaw: $yaw, pitch: $pitch, roll: $roll)";
  }
}


/// Interface object that stores a captured face sample.
class RawSample extends _ComplexObject {

  RawSample(DynamicLibrary dll_handle, Pointer<Void> impl):
        super(dll_handle, impl);

  /// Get a face bounding rectangle.
  Rectangle getRectangle(){
    var exception = _getException();
    final get_rect = _dll_handle.lookupFunction<_RSgetRectangle_c, _RSgetRectangle_dart>
      (_c_namespace + 'RawSample_getRectangle');

    Pointer<Int32> w = malloc.allocate(sizeOf<Pointer<Int32>>());
    Pointer<Int32> h = malloc.allocate(sizeOf<Pointer<Int32>>());
    Pointer<Int32> x = malloc.allocate(sizeOf<Pointer<Int32>>());
    Pointer<Int32> y = malloc.allocate(sizeOf<Pointer<Int32>>());

    
    get_rect(_impl, x, y, w, h, exception);

    checkException(exception, _dll_handle);

    Rectangle rect = Rectangle(x.value, y.value, w.value, h.value);

    malloc.free(w);
    malloc.free(h);
    malloc.free(x);
    malloc.free(y);

    return rect;
  }

  /// Get a face orientation.
  Angles getAngles() {
    var exception = _getException();
    final get = _dll_handle.lookupFunction<_RSgetAngles_c, _RSgetAngles_dart>
      (_c_namespace + 'RawSample_getAngles');

    Pointer<Float> y = malloc.allocate(sizeOf<Pointer<Float>>());
    Pointer<Float> p = malloc.allocate(sizeOf<Pointer<Float>>());
    Pointer<Float> r = malloc.allocate(sizeOf<Pointer<Float>>());

    get(_impl, y, p, r, exception);

    checkException(exception, _dll_handle);

    final res = Angles(y.value, p.value, r.value);

    malloc.free(y);
    malloc.free(p);
    malloc.free(r);

    return res;
  }
  ///Get a face ID.
  int getID(){
    var exception = _getException();
    final get = _dll_handle.lookupFunction<_RSgetID_c, _RSgetID_dart>
      (_c_namespace + 'RawSample_getID');

    final int id = get(_impl, exception);

    checkException(exception, _dll_handle);

    return id;
  }

  Point _getEye(String sideEye) {
    assert(sideEye == "Right" || sideEye == "Left", "Available values: Right or Left");

    var exception = _getException();
    final get = _dll_handle.lookupFunction<_RSgetEye_c, _RSgetEye_dart>
      (_c_namespace + 'RawSample_get${sideEye}Eye');

    Pointer<Float> x = malloc.allocate(sizeOf<Pointer<Float>>());
    Pointer<Float> y = malloc.allocate(sizeOf<Pointer<Float>>());

    get(_impl, x, y, exception);

    checkException(exception, _dll_handle);

    final Point point = Point(x.value, y.value, 0);

    malloc.free(x);
    malloc.free(y);

    return point;
  }
///Get a point of the right eye. Only frontal samples.
  Point getRightEye() {
    return _getEye("Right");
  }
///Get a point of the left eye. Only frontal samples.
  Point getLeftEye() {
    return _getEye("Left");
  }
///Get the score of the detected face (for samples made with supported single-shot Capturers).<br>
///[return] - One if this RawSample was made with an unsupported detector,otherwise - a number in the range (0 ... 1).
  double getScore() {
    var exception = _getException();
    final get = _dll_handle.lookupFunction<_RSgetScore_c, _RSgetScore_dart>(
        _c_namespace + 'RawSample_getScore');

    final double res = get(this._impl, exception);
    checkException(exception, _dll_handle);

    return res;
  }
///Get anthropometric landmarks. Only frontal samples.<br>
///[return] - The vector of anthropometric points in the original image.
  List<double> getLandmarks(){
    final exception = _getException();
    final get = _dll_handle.lookupFunction<_RSgetLandmarks_c, _RSgetLandmarks_dart>
      (_c_namespace + 'RawSample_getLandmarks');

    PointerBuffer<Float> buffer = PointerBuffer<Float>.empty(sizeOf<Float>());

    get(
        this._impl,
        buffer.ptr.cast(),
        Pointer.fromFunction(WrapBytesWrite.writeFloatListFunc),
        exception);

    checkException(exception, _dll_handle);

    Float32List list = buffer.dataPtr<Float>().asTypedList(buffer.length);
    List<double> returnVal = List<double>.from(list);

    buffer.dispose();

    return returnVal;
  }
///Crop the face and save in the specified format.<br>
///[imageFormat] - Image encoding format.<br>
///[cutType] - Face cropping types.
  Uint8List cutFaceImage(ImageFormatCode imageFormat, FaceCutType cutType) {
    final exception = _getException();
    Pointer<Int32> widthPtr = malloc.allocate(sizeOf<Pointer<Int32>>());
    Pointer<Int32> heightPtr = malloc.allocate(sizeOf<Pointer<Int32>>());

    PointerBuffer<Uint8> buffer = PointerBuffer<Uint8>.empty(sizeOf<Uint8>());

    if(widthPtr.address == nullptr.address || heightPtr.address == nullptr.address)
      throw Exception("RawSample.cutFaceImage: Failed to allocate a ${sizeOf<Pointer<Int32>>()} bytes");

    final get = _dll_handle.lookupFunction<_RScutFaceImage_c, _RScutFaceImage_dart>
      (_c_namespace + 'RawSample_cutFaceImage');

    get(
        this._impl,
        buffer.ptr.cast(),
        Pointer.fromFunction(WrapBytesWrite.writeByteListFunc),
        widthPtr,
        heightPtr,
        imageFormat.index,
        -1,
        cutType.index,
        exception);

    checkException(exception, _dll_handle);

    int width = widthPtr.value;
    int height = heightPtr.value;

    Uint8List list = buffer.dataPtr<Uint8>().asTypedList(buffer.length);
    Uint8List outData = Uint8List.fromList(list);

    buffer.dispose();

    malloc.free(widthPtr);
    malloc.free(heightPtr);

    return outData;
  }
///Crop face in RawImage format (with raw pixels).<br>
///[colorFormat] - Image color model.<br>
///[cutType] - Face cropping types.
  RawImageF cutFaceRawImage(Format colorFormat, FaceCutType cutType) {
    final exception = _getException();
    Pointer<Int32> widthPtr = malloc.allocate(sizeOf<Pointer<Int32>>());
    Pointer<Int32> heightPtr = malloc.allocate(sizeOf<Pointer<Int32>>());
    Pointer<Pointer<Void>> bufer = malloc.allocate(sizeOf<Pointer<Pointer<Uint8>>>());

    final get = _dll_handle.lookupFunction<_RScutFaceImage_c, _RScutFaceImage_dart>
      (_c_namespace + 'RawSample_cutFaceImage');

    get(
        this._impl,
        bufer.cast(),
        Pointer.fromFunction(WrapBytesWrite.writeByteListFunc),
        widthPtr,
        heightPtr,
        -1,
        colorFormat.index,
        cutType.index,
        exception);

    checkException(exception, _dll_handle);

    int width = widthPtr.value;
    int height = heightPtr.value;

    Pointer<Uint8> bufferData = bufer.value.cast<Uint8>();

    malloc.free(widthPtr);
    malloc.free(heightPtr);
    malloc.free(bufer);

    return RawImageF(width, height, colorFormat, bufferData.cast());
  }

  Context toContext(){
    var exception = _getException();
    final func = _dll_handle.lookupFunction<_RStoContext, _RStoContext_dart>
      (_c_namespace + 'RawSample_toContext');

    final ctxPointer = func(this._impl, exception);

    checkException(exception, _dll_handle);

    return Context(_dll_handle, ctxPointer);
  }

}