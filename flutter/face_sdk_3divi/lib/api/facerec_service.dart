part of face_sdk_3divi;

class _NativeConfigData {
  late int length;
  late Pointer<Pointer<Utf8>> keys;
  late Pointer<Double> values;

  _NativeConfigData(Map overriddenParams) {
    length = overriddenParams.length;
    final keysDart = overriddenParams.keys.toList();
    keys = malloc.allocate(sizeOf<Pointer<Utf8>>() * length);
    values = malloc.allocate(sizeOf<Double>() * length);
    final valList = overriddenParams.values.toList();
    for (int i = 0; i < length; i++) {
      keys[i] = keysDart[i].toString().toNativeUtf8();
      values[i] = valList[i];
    }
  }
}

/// A class used to override the configuration parameters at runtime.
class Config {
  final String _configFilepath;
  Map _overriddenParams = new Map<String, double>();

  /// A constructor that takes the path to a configuration file.
  Config(this._configFilepath);

  /// Override the parameter value.<br>
  /// [parameter] - parameter name (a tag name from the .xml config file).<br>
  /// [value] - new parameter value.
  Config overrideParameter(final String parameter, final double value) {
    _overriddenParams[parameter] = value;
    return this;
  }

  _NativeConfigData _prepare() {
    return _NativeConfigData(_overriddenParams);
  }
}

/// Interface object for creating other objects.
class FacerecService extends _ComplexObject {
  String _facerecConfDir;
  String _dllPath;

  FacerecService(DynamicLibrary dll_handle, Pointer<Void> impl, String facerecConfDir, String dllPath)
      : _facerecConfDir = facerecConfDir + '/',
        _dllPath = dllPath,
        super(dll_handle, impl);

  /// Initializes the facerec library (can be called only once).<br>
  ///<br>
  /// By default it is recommended to use [FaceSdkPlugin.createFacerecService].
  static Future<FacerecService> createService() async {
    final String dataDirectory = await loadAssets();
    final String libraryDirectory = await getLibraryDirectory();
    final String facerecConfDir = "$dataDirectory/conf/facerec";
    final String licenseDir = "$dataDirectory/license";
    final String dllPath = "$libraryDirectory/${FaceSdkPlugin.nativeLibName}";
    final DynamicLibrary dylib = DynamicLibrary.open(dllPath);

    final createService =
        dylib.lookupFunction<_facerecConstructor, _facerecConstructor>(_c_namespace + 'FacerecService_constructor2');

    final exception = _getException();
    final pointer =
        createService(facerecConfDir.toNativeUtf8(), licenseDir.toNativeUtf8(), dllPath.toNativeUtf8(), exception);
    checkException(exception, dylib);

    return FacerecService(dylib, pointer, facerecConfDir, dllPath);
  }

  /// Creates a [VideoWorker] object<br>
  ///<br>
  /// it is used to:<br>
  ///  - track faces on video stream<br>
  ///  - create templates<br>
  ///  - match templates with the database<br>
  ///  - estimate age, gender, and emotions<br>
  ///  - estimate liveness<br>
  ///  - match the faces detected in a specified period with each other.
  VideoWorker createVideoWorker(VideoWorkerParams params) {
    final vwConstructor = _dll_handle.lookupFunction<_VWConstructor_c, _VWConstructor_dart>(
        _c_namespace + 'FacerecService_createVideoWorker_sti_age_gender_emotions');
    final exception = _getException();

    Pointer<Int32> _emptyPointer = malloc.allocate(1);
    Pointer<Pointer<Utf8>> _emptyPointerStrList = malloc.allocate(1);
    Pointer<Double> _emptyPointerDouble = malloc.allocate(1);

    final vw_config = _facerecConfDir + params._video_worker_config._configFilepath;
    if (params._active_liveness_checks_order.isNotEmpty) {
      if ({...params._active_liveness_checks_order}.length != params._active_liveness_checks_order.length)
        throw ("Error 0x3302330e: Set a unique order of `active_liveness_checks_order` for Active Liveness.");
      for (int i = 0; i < params._active_liveness_checks_order.length; i++) {
        final check = params._active_liveness_checks_order[i];
        var check_str = "active_liveness.check_" + check.toString().split('.').last.toLowerCase();
        params._video_worker_config.overrideParameter(check_str, -(i + 1).toDouble());
      }
    }
    final res_vw = params._video_worker_config._prepare();

    final rec_config = _facerecConfDir + params._recognizer_ini_file;

    final vw_pointer = vwConstructor(
        _impl,
        // service

        _emptyPointer.cast(),
        // trackingCallback
        _emptyPointer.cast(),
        // templateCreatedCallback
        _emptyPointer.cast(),
        // matchFoundCallback
        _emptyPointer.cast(),
        // trackingLostCallback
        _emptyPointer.cast(),
        // stiPersonOutdatedCallback

        vw_config.toNativeUtf8(),
        // video_worker_ini_file
        res_vw.length,
        // vw_overridden_count
        res_vw.keys,
        // vw_overridden_keys
        res_vw.values,
        // vw_overridden_values

        rec_config.toNativeUtf8(),
        // recognizer_ini_file
        0,
        // rec_overridden_count
        _emptyPointerStrList,
        // rec_overridden_keys
        _emptyPointerDouble,
        // rec_overridden_values

        params._streams_count,
        // streams_count
        params._processing_threads_count,
        // processing_threads_count
        params._matching_threads_count,
        // matching_threads_count

        params._short_time_identification_enabled,
        // short_time_identification_enabled
        params._short_time_identification_distance_threshold,
        // short_time_identification_distance_threshold
        params._short_time_identification_outdate_time_seconds,
        // short_time_identification_outdate_time_seconds

        params._age_gender_estimation_threads_count,
        // age_gender_threads_count
        params._emotions_estimation_threads_count,
        // emotions_threads_count

        exception /*out_exception*/);
    checkException(exception, _dll_handle);

    return VideoWorker(_dll_handle, vw_pointer);
  }

  Future<AsyncVideoWorker> createAsyncVideoWorker(VideoWorkerParams params) async {
    return AsyncVideoWorker.create(_impl, _facerecConfDir, _dllPath, params);
  }

  Context createContext(Object ctx) {
    var meta_ctx = Context(_dll_handle, nullptr);
    meta_ctx.placeValues(ctx);
    return meta_ctx;
  }

  Context createContextFromEncodedImage(Uint8List data) {
    return Context.fromImage(_dll_handle, data);
  }

  Context createContextFromFrame(Uint8List data, int width, int height,
      {ContextFormat format = ContextFormat.FORMAT_YUV420, int baseAngle = 0}) {
    return Context.fromFrame(_dll_handle, data, width, height, format, baseAngle);
  }

  Context createContextFromJsonFile(String path) {
    return Context.fromJsonFile(_dll_handle, path);
  }

  Context createContextFromCameraImage(CameraImage image, int baseAngle) {
    int width = image.width;
    int height = image.height;
    ContextFormat format;
    Uint8List data = getRawData(image.planes);

    switch (image.format.group) {
      case ImageFormatGroup.unknown:
        throw Exception("Unknown image format");

      case ImageFormatGroup.yuv420:
        format = ContextFormat.FORMAT_YUV420;

        if (width != image.planes.first.bytesPerRow) {
          data = _removePaddingFromYUV420_888(data, width, height, image.planes.first.bytesPerRow);
        }

        break;

      case ImageFormatGroup.bgra8888:
        format = ContextFormat.FORMAT_BGRA8888;

        break;

      case ImageFormatGroup.jpeg:
        throw Exception("Unsupported image format jpeg");

      case ImageFormatGroup.nv21:
        format = ContextFormat.FORMAT_NV21;
        break;
    }

    return createContextFromFrame(data, width, height, format: format, baseAngle: baseAngle);
  }

  ProcessingBlock createProcessingBlock(Map ctx) {
    final exception = _getException();

    Context meta_ctx = this.createContext(ctx);

    final constructor = this
        ._dll_handle
        .lookupFunction<_ProcessingBlock_createProcessingBlock_c, _ProcessingBlock_createProcessingBlock_dart>(
            _c_namespace + 'FacerecService_ProcessingBlock_createProcessingBlock');

    final impl = constructor(_impl, meta_ctx._impl, exception);

    checkException(exception, _dll_handle);

    return ProcessingBlock(this._dll_handle, impl);
  }

  Future<AsyncProcessingBlock> createAsyncProcessingBlock(Map context) {
    return AsyncProcessingBlock.create(_impl, _facerecConfDir, _dllPath, context);
  }

  /// Creates a [Capturer] object (it's used to detect or track faces in images or video sequences).
  Capturer createCapturer(Config config) {
    final capConstructor =
        _dll_handle.lookupFunction<_CapConstr_c, _CapConstr_dart>(_c_namespace + 'FacerecService_createCapturerE');
    final exception = _getException();
    final res = config._prepare();

    final cap_pointer = capConstructor(
        _impl, (_facerecConfDir + config._configFilepath).toNativeUtf8(), res.length, res.keys, res.values, exception);

    checkException(exception, _dll_handle);

    return Capturer(_dll_handle, cap_pointer);
  }

  Future<AsyncCapturer> createAsyncCapturer(Config config) async {
    return AsyncCapturer.create(_impl, _facerecConfDir, _dllPath, config);
  }

  /// Similar to the [FacerecService.createCapturer] method.
  Capturer createCapturer2(String ini_file) {
    final capConstructor =
        _dll_handle.lookupFunction<_CapConstr_c, _CapConstr_dart>(_c_namespace + 'FacerecService_createCapturerE');
    Pointer<Pointer<Utf8>> _emptyPointerStrList = malloc.allocate(1);
    Pointer<Double> _emptyPointerDouble = malloc.allocate(1);
    final exception = _getException();

    final cap_pointer = capConstructor(
        _impl, (_facerecConfDir + ini_file).toNativeUtf8(), 0, _emptyPointerStrList, _emptyPointerDouble, exception);

    checkException(exception, _dll_handle);

    return Capturer(_dll_handle, cap_pointer);
  }

  /// Creates a [Recognizer] object (it's used to create face templates and compare them).
  Recognizer createRecognizer(final String ini_file,
      {final bool processing = true,
      final bool matching = true,
      final bool processing_less_memory_consumption = false}) {
    final recConstructor = _dll_handle
        .lookupFunction<_RecognizerConstr_c, _RecognizerConstr_dart>(_c_namespace + 'FacerecService_createRecognizer2');
    Pointer<Pointer<Utf8>> _emptyPointerStrList = malloc.allocate(1);
    Pointer<Double> _emptyPointerDouble = malloc.allocate(1);
    final exception = _getException();

    final recPointer = recConstructor(
        _impl,
        (_facerecConfDir + ini_file).toNativeUtf8(),
        0,
        _emptyPointerStrList,
        _emptyPointerDouble,
        processing ? 1 : 0,
        matching ? 1 : 0,
        processing_less_memory_consumption ? 1 : 0,
        exception);

    checkException(exception, _dll_handle);

    return Recognizer(_dll_handle, recPointer);
  }

  Future<AsyncRecognizer> createAsyncRecognizer(final String ini_file,
      {final bool processing = true,
      final bool matching = true,
      final bool processing_less_memory_consumption = false}) async {
    return AsyncRecognizer.create(
        _impl, _facerecConfDir, _dllPath, ini_file, processing, matching, processing_less_memory_consumption);
  }

  /// Creates a [Liveness2DEstimator] object (it's used evalute liveness of face).
  Liveness2DEstimator createLiveness2DEstimator(final String ini_file) {
    final livenessEstimatorConstructor =
        _dll_handle.lookupFunction<_Liveness2DEstimatorConstr_c, _Liveness2DEstimatorConstr_dart>(
            _c_namespace + 'FacerecService_createLiveness2DEstimatorE');
    Pointer<Pointer<Utf8>> _emptyPointerStrList = malloc.allocate(1);
    Pointer<Double> _emptyPointerDouble = malloc.allocate(1);
    final exception = _getException();

    final livenessEstimatorPointer = livenessEstimatorConstructor(
        _impl, (_facerecConfDir + ini_file).toNativeUtf8(), 0, _emptyPointerStrList, _emptyPointerDouble, exception);

    checkException(exception, _dll_handle);

    return Liveness2DEstimator(_dll_handle, livenessEstimatorPointer);
  }

  FaceAttributesEstimator createFaceAttributesEstimator(final String ini_file) {
    final faceAttributesEstimatorConstructor =
        _dll_handle.lookupFunction<_FaceAttributesEstimatorConstr_c, _FaceAttributesEstimatorConstr_dart>(
            _c_namespace + 'FacerecService_createFaceAttributesEstimator');
    Pointer<Pointer<Utf8>> _emptyPointerStrList = malloc.allocate(1);
    Pointer<Double> _emptyPointerDouble = malloc.allocate(1);
    final exception = _getException();

    final faceAttributesEstimatorPointer = faceAttributesEstimatorConstructor(
        _impl, (_facerecConfDir + ini_file).toNativeUtf8(), 0, _emptyPointerStrList, _emptyPointerDouble, exception);

    checkException(exception, _dll_handle);

    return FaceAttributesEstimator(_dll_handle, faceAttributesEstimatorPointer);
  }

  ///Convert input image to android.graphics.Bitmap.Config.ARGB_8888 format.<br>
  ///	Input must be in YUV_NV21 of YUV_NV12 format.<br>
  ///	Note: actual bytes order is BGRA, it looks like ARGB_8888<br>
  ///	name use little-endian 32-bit integer notation.<br>
  /// [image] - Image in YUV_NV21 or YUV_NV12 format.<br>
  /// [result_buffer] - Data buffer to store converted result, with size image.width * image.height bytes if downscale_x2, or 4 * image.width * image.height bytes otherwise.<br>
  /// [downscale_x2] - Downscale image during conversion, so the result image size will be image.width/2 x image.height/2.
  void convertYUV2ARGB(RawImageF image, Pointer<Void> result_buffer, {bool downscale_x2 = false}) {
    final exception = _getException();

    final convertYUV2ARGBConstructor =
        _dll_handle.lookupFunction<_RawImage_convertYUV2ARGB_c, _RawImage_convertYUV2ARGB_dart>(
            _c_namespace + 'RawImage_convertYUV2ARGB');

    convertYUV2ARGBConstructor(
        image.data.cast(),
        image.width,
        image.height,
        image.format.index,
        image.with_crop,
        image.crop_info_offset_x,
        image.crop_info_offset_y,
        image.crop_info_data_image_width,
        image.crop_info_data_image_height,
        downscale_x2 ? 1 : 0,
        result_buffer,
        exception);

    checkException(exception, _dll_handle);
  }

  RawImageF convertYUV2RGB(RawImageF image, {required int baseAngle, NativeDataStruct? reusableData}) {
    final exception = _getException();
    final convertYUV2RGBConstructor =
        _dll_handle.lookupFunction<_RawImage_convertYUV2RGB_c, _RawImage_convertYUV2RGB_dart>(
            _c_namespace + 'RawImage_convertYUV2RGB');
    int width;
    int height;

    switch (baseAngle) {
      case 1:
      case 2:
        width = image.height;
        height = image.width;

        break;

      default:
        width = image.width;
        height = image.height;

        break;
    }

    NativeDataStruct data = NativeDataStruct();
    final int totalBytes = width * height * 3;

    if (reusableData != null) {
      if (reusableData.size != totalBytes) {
        reusableData.resize(totalBytes);
      }
    } else {
      data.resize(totalBytes);
    }

    RawImageF result =
        RawImageF(width, height, Format.FORMAT_RGB, (reusableData == null ? data : reusableData).pointer!.cast());

    convertYUV2RGBConstructor(
        image.data.cast(),
        image.width,
        image.height,
        image.format.index,
        image.with_crop,
        image.crop_info_offset_x,
        image.crop_info_offset_y,
        image.crop_info_data_image_width,
        image.crop_info_data_image_height,
        0,
        baseAngle,
        result.data.cast(),
        exception);

    checkException(exception, _dll_handle);

    return result;
  }

  RawImageF _convertYUV2RGB(Pointer<Uint8> imageData, int imageWidth, int imageHeight, Format format,
      {required int baseAngle, NativeDataStruct? reusableData}) {
    final exception = _getException();
    final convertYUV2RGBConstructor =
        _dll_handle.lookupFunction<_RawImage_convertYUV2RGB_c, _RawImage_convertYUV2RGB_dart>(
            _c_namespace + 'RawImage_convertYUV2RGB');
    int width;
    int height;

    switch (baseAngle) {
      case 1:
      case 2:
        width = imageHeight;
        height = imageWidth;

        break;

      default:
        width = imageWidth;
        height = imageHeight;

        break;
    }

    NativeDataStruct data = NativeDataStruct();
    final int totalBytes = width * height * 3;

    if (reusableData != null) {
      if (reusableData.size != totalBytes) {
        reusableData.resize(totalBytes);
      }
    } else {
      data.resize(totalBytes);
    }

    RawImageF result =
        RawImageF(width, height, Format.FORMAT_RGB, (reusableData == null ? data : reusableData).pointer!.cast());

    convertYUV2RGBConstructor(imageData.cast(), imageWidth, imageHeight, format.index, 0, -1, -1, -1, -1, 0, baseAngle,
        result.data.cast(), exception);

    checkException(exception, _dll_handle);

    return result;
  }

  RawImageF convertBGRA88882RGB(RawImageF image, {required int baseAngle, NativeDataStruct? reusableData}) {
    final exception = _getException();
    final convertBGRA88882RGBConstructor =
        _dll_handle.lookupFunction<_RawImage_convertBGRA88882RGB_c, _RawImage_convertBGRA88882RGB_dart>(
            _c_namespace + 'RawImage_convertBGRA88882RGB');
    int width;
    int height;

    switch (baseAngle) {
      case 1:
      case 2:
        width = image.height;
        height = image.width;

        break;

      default:
        width = image.width;
        height = image.height;

        break;
    }

    NativeDataStruct data = NativeDataStruct();
    final int totalBytes = width * height * 3;

    if (reusableData != null) {
      if (reusableData.size != totalBytes) {
        reusableData.resize(totalBytes);
      }
    } else {
      data.resize(totalBytes);
    }

    RawImageF result =
        RawImageF(width, height, Format.FORMAT_RGB, (reusableData == null ? data : reusableData).pointer!.cast());

    convertBGRA88882RGBConstructor(
        image.data.cast(), image.width, image.height, baseAngle, result.data.cast(), exception);

    checkException(exception, _dll_handle);

    return result;
  }

  RawImageF createRawImageFromCameraImage(CameraImage image, int baseAngle, {NativeDataStruct? reusableData}) {
    const YUV_420_888 = 0x00000023;

    int width = image.width;
    int height = image.height;
    RawImageF result;

    if (reusableData != null) {
      int totalBytes = width * height * 3;

      if (reusableData.size != totalBytes) {
        reusableData.resize(totalBytes);
      }
    }

    switch (image.format.group) {
      case ImageFormatGroup.unknown:
        throw Exception("Unknown image format");

      case ImageFormatGroup.yuv420:
        Pointer<Uint8> yuv420Data = getRawDataPointer(image.planes);

        if (image.format.raw == YUV_420_888 && width != image.planes.first.bytesPerRow) {
          Pointer<Uint8> temp =
              _removePaddingFromYUV420_888Pointer(yuv420Data, width, height, image.planes.first.bytesPerRow);

          malloc.free(yuv420Data);

          yuv420Data = temp;
        }

        result = _convertYUV2RGB(yuv420Data, width, height, Format.FORMAT_YUV_NV21,
            baseAngle: baseAngle, reusableData: reusableData);

        malloc.free(yuv420Data);

        break;

      case ImageFormatGroup.bgra8888:
        NativeDataStruct data = NativeDataStruct();

        convertRAW(image.planes, data);

        RawImageF temp = RawImageF(image.width, image.height, Format.FORMAT_BGR, data.pointer!.cast());

        result = convertBGRA88882RGB(temp, baseAngle: baseAngle, reusableData: reusableData);

        break;

      case ImageFormatGroup.jpeg:
        throw Exception("Unsupported image format jpeg");

      case ImageFormatGroup.nv21:
        throw Exception("Unsupported image format nv21");
    }

    return result;
  }

  Uint8List _removePaddingFromYUV420_888(Uint8List data, int width, int height, int bytesPerRow) {
    final exception = _getException();
    int resultSize = 2 * width * height - 2;
    Pointer<Uint8> result = malloc.allocate<Uint8>(resultSize);
    Pointer<Uint8> dataPointer = malloc.allocate<Uint8>(data.length);

    dataPointer.asTypedList(data.length).setAll(0, data);

    final convert = _dll_handle.lookupFunction<_convertYUV420_888ToNV21_c, _convertYUV420_888ToNV21_dart>(
        "${_c_namespace}TDV_convertYUV420_888ToNV21");

    convert(dataPointer, width, height, bytesPerRow, result, resultSize, exception);

    data = Uint8List.fromList(result.asTypedList(resultSize));

    malloc.free(result);
    malloc.free(dataPointer);

    checkException(exception, _dll_handle);

    return data;
  }

  Pointer<Uint8> _removePaddingFromYUV420_888Pointer(Pointer<Uint8> data, int width, int height, int bytesPerRow) {
    final exception = _getException();
    int resultSize = 2 * width * height - 2;
    Pointer<Uint8> result = malloc.allocate<Uint8>(resultSize);

    final convert = _dll_handle.lookupFunction<_convertYUV420_888ToNV21_c, _convertYUV420_888ToNV21_dart>(
        "${_c_namespace}TDV_convertYUV420_888ToNV21");

    convert(data, width, height, bytesPerRow, result, resultSize, exception);

    checkException(exception, _dll_handle);

    return result;
  }
}
