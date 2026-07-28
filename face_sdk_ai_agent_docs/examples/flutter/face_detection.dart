import 'dart:io';
import 'package:face_sdk_3divi/face_sdk.dart';

Future<void> main() async {
  FacerecService? service;
  AsyncProcessingBlock? detector;
  Context? ioData;
  try {
    service = await FaceSdkPlugin.createFacerecService();

    detector = service.createAsyncProcessingBlock(
        {"unit_type": "FACE_DETECTOR", "modification": "ssyv_light"});

    ioData = service.createContextFromEncodedImage(
        await File("input.jpg").readAsBytes());
    await detector.process(ioData);

    for (int i = 0; i < ioData["objects"].len(); i++) {
      Context obj = ioData["objects"][i];
      Context bbox = obj["bbox"];
      int w = ioData["image"]["shape"][1].get_value();
      int h = ioData["image"]["shape"][0].get_value();
      double conf = obj["confidence"].get_value();
      print("Face: (${(bbox[0].get_value() * w).toInt()}, "
          "${(bbox[1].get_value() * h).toInt()})-"
          "(${(bbox[2].get_value() * w).toInt()}, "
          "${(bbox[3].get_value() * h).toInt()})"
          " confidence: ${conf.toStringAsFixed(3)}");
    }

    if (ioData["objects"].len() == 0) {
      print("No faces detected");
    }
  } on TDVException catch (e) {
    print("Error: ${e.message}");
  } on Exception catch (e) {
    print("Error: $e");
  } finally {
    ioData?.dispose();
    detector?.dispose();
    service?.dispose();
  }
}
