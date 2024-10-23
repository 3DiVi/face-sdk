part of utils;

int getBaseAngle(CameraController controller) {
  if (Platform.isIOS) {
    return 0;
  }

  switch (controller.description.sensorOrientation) {
    case 0:
      return 0;

    case 90:
      return 1;

    case 270:
      return 2;

    case 180:
      return 3;

    default:
      throw Exception(
          "Wrong sensor orientation: ${controller.description.sensorOrientation}");
  }
}
