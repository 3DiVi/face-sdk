import 'package:face_sdk_3divi/face_sdk_3divi.dart';

class BBox {
  final double x1;
  final double y1;
  final double x2;
  final double y2;

  const BBox({required this.x1, required this.y1, required this.x2, required this.y2});
}

class TextInformation {
  final List<String> _objectInfo = [];

  TextInformation(Context obj) {
    final age = obj['age'].get_value();
    final gender = obj['gender'].get_value();
    final emotion = obj['emotions'][0]['emotion'].get_value();
    final hasMask = obj['has_medical_mask']['value'].get_value() ? "yes" : "no";
    final hasGlasses = obj['glasses']['value'].get_value() ? "yes" : "no";
    final eyeOpened = (obj["is_left_eye_open"]["value"].get_value() &&
        obj["is_right_eye_open"]["value"].get_value()) ? "yes" : "no";
    final isDeepfake = obj["deepfake"]["value"].get_value() ? "yes" : "no";

    _objectInfo.add("age: ${age}");
    _objectInfo.add("gender: ${gender}");
    _objectInfo.add("emotion: ${emotion}");
    _objectInfo.add("mask: ${hasMask}");
    _objectInfo.add("sunglasses: ${hasGlasses}");
    _objectInfo.add("eye_opened: ${eyeOpened}");
    _objectInfo.add("deepfake: ${isDeepfake}");

    if (obj.contains("liveness")) {
      final liveness = obj["liveness"]["value"].get_value();
      _objectInfo.add("liveness: ${liveness}");
    }
    final quality = obj["quality"]["value"].get_value();
    final qualityInfo = quality ? "passed" : "failed";
    _objectInfo.add("quality: ${qualityInfo}");
    if (!quality){
      final failedCheck = obj["quality"]["failed_checks"][0]["check"].get_value();
      _objectInfo.add("\tfailed_check: ${failedCheck}");
    }
  }

  String getInfo() {
    StringBuffer result = StringBuffer();

    for (String element in _objectInfo) {
      result.write("$element\n");
    }

    return result.toString();
  }
}
