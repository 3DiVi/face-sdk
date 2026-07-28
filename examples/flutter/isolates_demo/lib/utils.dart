import 'package:face_sdk_3divi/face_sdk_3divi.dart';


String getMessageForQualityControl(Context quality)
{
  var check_name = quality["failed_checks"][0]["check"].get_value();
  if (check_name == "face_overflow" || check_name == "face_size_on_image" ||
      check_name == "eye_distance")
  {
    return "Face is not properly positioned in the frame";
  }
  else if(check_name == "yaw" || check_name == "pitch")
  {
    return "Face is turned or tilted";
  }
  else if (check_name == "dynamic_range")
  {
    return "Poor lighting conditions";
  }
  else if (check_name == "sharpness" || check_name == "noise")
  {
    return "Low image quality";
  }
  else if (check_name == "glasses")
  {
    return "Glasses are interfering with detection";
  }
  else if (check_name == "gray_scale")
  {
    return "Invalid image";
  }

  return "";
}