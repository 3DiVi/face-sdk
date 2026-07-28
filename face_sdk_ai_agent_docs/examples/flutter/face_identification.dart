import 'dart:io';
import 'package:face_sdk_3divi/face_sdk.dart';

Future<void> main() async {
  FacerecService? service;
  ProcessingBlock? detector;
  ProcessingBlock? fitter;
  ProcessingBlock? extractor;
  ProcessingBlock? matcher;
  DynamicTemplateIndex? index;
  try {
    // sdk_root is provided by the user
    service = await FaceSdkPlugin.createFacerecService();

    detector = service.createProcessingBlock(
        {"unit_type": "FACE_DETECTOR", "modification": "ssyv_light"});
    fitter = service.createProcessingBlock(
        {"unit_type": "FACE_FITTER", "modification": "fda"});
    extractor = service.createProcessingBlock({
      "unit_type": "FACE_TEMPLATE_EXTRACTOR",
      "modification": "1000"
    });
    matcher = service.createProcessingBlock({
      "unit_type": "MATCHER_MODULE",
      "modification": "1000"
    });

    // Create DynamicTemplateIndex and enroll faces
    index = service.createDynamicTemplateIndex({
      "modification": "1000",
      "version": 1,
      "max_license_count": 10000
    });

    for (String imgPath in ["face1.jpg", "face2.jpg", "face3.jpg"]) {
      Context ioData = service.createContextFromEncodedImage(
          await File(imgPath).readAsBytes());
      detector.process(ioData);
      fitter.process(ioData);
      extractor.process(ioData);
      for (int i = 0; i < ioData["objects"].len(); i++) {
        Context obj = ioData["objects"][i];
        if (obj["class"].get_value() != "face") continue;
        ContextTemplate templ = obj["face_template"]["template"].get_value();
        String uuid = obj["face_template"]["uuid"].get_value();
        index.add(templ, uuid);
        print("Enrolled: $uuid");
      }
      ioData.dispose();
    }

    // Search with query face
    Context queryData = service.createContextFromEncodedImage(
        await File("query.jpg").readAsBytes());
    detector.process(queryData);
    fitter.process(queryData);
    extractor.process(queryData);

    if (queryData["objects"].len() == 0) {
        print("No faces detected");
        return;
    }

    Context matcherData = service.createContext({
      "knn": 5,
      "template_index": index,
      "queries": queryData["objects"][0]["face_template"]["template"]
    });
    matcher.process(matcherData);

    for (int i = 0; i < matcherData["results"].len(); i++) {
      Context result = matcherData["results"][i];
      int idx = result["index"].get_value();
      double score = result["score"].get_value();
      double distance = result["distance"].get_value();
      print("Match index: $idx Score: ${score.toStringAsFixed(4)} "
          "Distance: ${distance.toStringAsFixed(4)}");
    }

    queryData.dispose();
    matcherData.dispose();
  } on TDVException catch (e) {
    print("Error: ${e.message}");
  } on Exception catch (e) {
    print("Error: $e");
  } finally {
    index?.dispose();
    service?.dispose();
    detector?.dispose();
    fitter?.dispose();
    extractor?.dispose();
    matcher?.dispose();
  }
}
