import 'dart:io';
import 'package:face_sdk_3divi/face_sdk.dart';

Future<void> main() async {
  FacerecService? service;
  AsyncProcessingBlock? detector;
  AsyncProcessingBlock? fitter;
  AsyncProcessingBlock? ageEst;
  AsyncProcessingBlock? genderEst;
  AsyncProcessingBlock? emotionEst;
  Context? ioData;
  try {
    service = await FaceSdkPlugin.createFacerecService();

    detector = service.createAsyncProcessingBlock(
        {"unit_type": "FACE_DETECTOR", "modification": "ssyv_light"});
    fitter = service.createAsyncProcessingBlock(
        {"unit_type": "FACE_FITTER", "modification": "fda"});
    ageEst = service.createAsyncProcessingBlock(
        {"unit_type": "AGE_ESTIMATOR"});
    genderEst = service.createAsyncProcessingBlock(
        {"unit_type": "GENDER_ESTIMATOR"});
    emotionEst = service.createAsyncProcessingBlock(
        {"unit_type": "EMOTION_ESTIMATOR"});

    ioData = service.createContextFromEncodedImage(
        await File("input.jpg").readAsBytes());
    await detector.process(ioData);
    await fitter.process(ioData);
    await ageEst.process(ioData);
    await genderEst.process(ioData);
    await emotionEst.process(ioData);

    if (ioData["objects"].len() == 0) {
      print("No faces detected");
      return;
    }

    for (int i = 0; i < ioData["objects"].len(); i++) {
      Context obj = ioData["objects"][i];
      print("Face ${i + 1}:");
      if (obj.contains("age"))
        print("  Age: ${obj["age"].get_value()}");
      if (obj.contains("gender"))
        print("  Gender: ${obj["gender"].get_value()}");
      if (obj.contains("emotions"))
        for (int k = 0; k < obj["emotions"].len(); k++) {
          Context em = obj["emotions"][k];
          print("  ${em["emotion"].get_value()}: ${em["confidence"].get_value().toStringAsFixed(3)}");
        }
    }
  } on TDVException catch (e) {
    print("Error: ${e.message}");
  } on Exception catch (e) {
    print("Error: $e");
  } finally {
    ioData?.dispose();
    detector?.dispose();
    fitter?.dispose();
    ageEst?.dispose();
    genderEst?.dispose();
    emotionEst?.dispose();
    service?.dispose();
  }
}
