/// FaceSDK plugin. Main module.<br>
///<br>
/// To get started with FaceSDK, you need to call the [FaceSdkPlugin.createFacerecService]
/// (or [FacerecService.createService]) method to create an interface object
/// for interacting with the library.
library face_sdk_3divi;

import 'dart:io';
import 'dart:ffi';
import "dart:typed_data";
import 'dart:developer' as developer;
import 'dart:ui';
import 'dart:convert' show utf8;
import 'package:ffi/ffi.dart';
import 'package:xml/xml.dart';
import 'package:image/image.dart' as imglib;

part 'api/active_liveness.dart';
part 'api/capturer.dart';
part 'api/complex_object.dart';
part 'api/dll_binds.dart';
part 'api/exception_check.dart';
part 'api/facerec_service.dart';
part 'api/raw_image.dart';
part 'api/raw_sample.dart';
part 'api/recognizer.dart';
part 'api/Rectangle.dart';
part 'api/struct_storage_fields.dart';
part 'api/template.dart';
part 'api/template_index.dart';
part 'api/video_worker.dart';
part 'api/context.dart';
part 'api/processing_block.dart';
part 'api/liveness2d_estimator.dart';
part 'api/face_attributes_estimator.dart';



/// Base class of FaceSDK flutter plugin for creating FacerecService.
class FaceSdkPlugin {
  /// Get default library name.
  static String get nativeLibName {
    String dllPath = '';
    if (Platform.isAndroid)
      dllPath = "libfacerec.so";
    else if (Platform.isIOS)
      dllPath = "facerec.framework/libfacerec.dylib";
    return dllPath;
  }

  /// Create a FacerecService using default library paths: `libfacerec.so`
  /// on Andorid and `facerec.framework/libfacerec.dylib` on iOS.<br>
  /// To use a custom library path, use the [FacerecService.createService] method.
  static FacerecService createFacerecService(final String facerecConfDir,
      final String licenseDir,
      {String? libPath}) {
    return FacerecService.createService(
        facerecConfDir, licenseDir, libPath ?? FaceSdkPlugin.nativeLibName);
  }
}

