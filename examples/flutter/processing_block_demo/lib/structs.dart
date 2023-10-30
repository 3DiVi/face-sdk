import 'package:face_sdk_3divi/face_sdk_3divi.dart';

class BBox {
  final double x1;
  final double y1;
  final double x2;
  final double y2;

  const BBox({required this.x1, required this.y1, required this.x2, required this.y2});
}

class TextInformation {
  final double _totalQualityScore;
  final List<String> _qualityAssessmentResults = [];
  static const Map<String, String> qualityAssessmentFields = {
    "is_rotation_acceptable": "Face frontal",
    "not_masked": "No mask",
    "is_neutral_emotion": "Neutral emotion"
  };

  TextInformation(Context quality, this._totalQualityScore) {
    bool value = quality["is_left_eye_opened"].get_value() && quality["is_right_eye_opened"].get_value();

    _qualityAssessmentResults.add("Eye opens: ${_convertBoolToString(value)}");

    qualityAssessmentFields.forEach((field, name) {
      bool value = quality[field].get_value();

      _qualityAssessmentResults.add("$name: ${_convertBoolToString(value)}");
    });
  }

  String getTotalQualityScoreWithPrecision({int precision = 3}) {
    return _totalQualityScore.toStringAsPrecision(precision);
  }

  String getQualityAssessmentResults() {
    StringBuffer result = StringBuffer();

    for (String element in _qualityAssessmentResults) {
      result.write("$element\n");
    }

    return result.toString();
  }

  String _convertBoolToString(bool value) {
    return value ? "yes" : "no";
  }
}
