import 'dart:io';
import 'package:face_sdk_3divi/face_sdk.dart';

Future<void> main() async {
  FacerecService? service;
  AsyncProcessingBlock? detector;
  AsyncProcessingBlock? fitter;
  AsyncProcessingBlock? extractor;
  AsyncProcessingBlock? verifier;
  Context? ioData1;
  Context? ioData2;
  Context? verData;
  try {
    service = await FaceSdkPlugin.createFacerecService();

    detector = service.createAsyncProcessingBlock(
        {"unit_type": "FACE_DETECTOR", "modification": "ssyv_light"});
    fitter = service.createAsyncProcessingBlock(
        {"unit_type": "FACE_FITTER", "modification": "fda"});
    extractor = service.createAsyncProcessingBlock({
      "unit_type": "FACE_TEMPLATE_EXTRACTOR",
      "modification": "1000"
    });
    verifier = service.createAsyncProcessingBlock({
      "unit_type": "VERIFICATION_MODULE",
      "modification": "1000"
    });

    // Process first face
    ioData1 = service.createContextFromEncodedImage(
        await File("face1.jpg").readAsBytes());
    await detector.process(ioData1);
    await fitter.process(ioData1);
    await extractor.process(ioData1);
    if (ioData1["objects"].len() == 0) {
      print("No face in face1.jpg");
      return;
    }

    // Process second face
    ioData2 = service.createContextFromEncodedImage(
        await File("face2.jpg").readAsBytes());
    await detector.process(ioData2);
    await fitter.process(ioData2);
    await extractor.process(ioData2);
    if (ioData2["objects"].len() == 0) {
      print("No face in face2.jpg");
      return;
    }

    // Compare templates
    verData = service.createContext({
      "template1": ioData1["objects"][0]["face_template"],
      "template2": ioData2["objects"][0]["face_template"],
    });
    await verifier.process(verData);

    double score = verData["result"]["score"].get_value();
    double distance = verData["result"]["distance"].get_value();
    print("Score: ${score.toStringAsFixed(4)} Distance: ${distance.toStringAsFixed(4)}");
    print("Verdict: ${score >= 0.85 ? "match" : "no match"}");
  } on TDVException catch (e) {
    print("Error: ${e.message}");
  } on Exception catch (e) {
    print("Error: $e");
  } finally {
    verData?.dispose();
    ioData1?.dispose();
    ioData2?.dispose();
    detector?.dispose();
    fitter?.dispose();
    extractor?.dispose();
    verifier?.dispose();
    service?.dispose();
  }
}
