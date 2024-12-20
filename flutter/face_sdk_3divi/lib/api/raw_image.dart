part of face_sdk_3divi;

/// Image data format.
enum Format {
  /// Grayscale, 8 bit per pixel.
  FORMAT_GRAY,

  /// RGB, 24 bit per pixel, 8 bit per channel.
  FORMAT_RGB,

  /// BGR, 24 bit per pixel, 8 bit per channel.
  FORMAT_BGR,

  /// NV21 format in the YUV color coding system, the standard image format used on the Android camera preview.
  FORMAT_YUV_NV21,

  /// NV12 format in the YUV color coding system.
  FORMAT_YUV_NV12
}

/// Struct that provides raw image data.
class RawImageF {
  final width;

  final height;

  final Pointer<Utf8> data;

  /// Format of image data.
  final Format format;

  int with_crop = 0;
  int crop_info_offset_x = -1;
  int crop_info_offset_y = -1;
  int crop_info_data_image_width = -1;
  int crop_info_data_image_height = -1;
  bool freed = false;

  /// Constructor for an image without cropping.
  RawImageF(this.width, this.height, this.format, this.data);

  bool get isValid {
    return !freed && data.address != nullptr.address;
  }

  void dispose() {
    if (isValid) {
      malloc.free(data);

      freed = true;
    }
  }

  static RawImageF emptyBuffer(int width, int height, {Format format = Format.FORMAT_RGB}) {
    if (!_formatMap.containsKey(format)) throw Exception("Unsuport image format");

    int size = width * height * (format == Format.FORMAT_GRAY ? 1 : 3);
    Pointer<Uint8> dataPtr = malloc.allocate(size);

    if (dataPtr.address == nullptr.address) throw Exception("RawImageF.emptyBuffer: Failed to allocate a $size bytes");

    return RawImageF(width, height, format, dataPtr.cast());
  }

  static RawImageF fromImage(imglib.Image image, {Format format = Format.FORMAT_RGB}) {
    if (!_formatMap.containsKey(format)) throw Exception("Unsuport image format");

    Uint8List data = image.getBytes(order: _formatMap[format]!);

    Pointer<Uint8> dataPtr = malloc.allocate(data.length);

    if (dataPtr.address == nullptr.address)
      throw Exception("RawImageF.fromImage: Failed to allocate a ${data.length} bytes");

    Uint8List rawImgData = dataPtr.cast<Uint8>().asTypedList(data.length);

    rawImgData.setAll(0, data);

    return RawImageF(image.width, image.height, format, dataPtr.cast());
  }

  static Map<Format, imglib.ChannelOrder> _formatMap = {
    Format.FORMAT_GRAY: imglib.ChannelOrder.grayAlpha,
    Format.FORMAT_RGB: imglib.ChannelOrder.rgb,
    Format.FORMAT_BGR: imglib.ChannelOrder.bgr
  };

  Uint8List? rawFrame() {
    if (!_formatMap.containsKey(format) || !isValid) return null;

    int size = width * height * (format == Format.FORMAT_GRAY ? 1 : 3);
    return data.cast<Uint8>().asTypedList(size);
  }

  imglib.Image? getImage() {
    Uint8List? rawFrameData = rawFrame();

    if (rawFrameData == null) {
      return null;
    }

    return imglib.Image.fromBytes(width: width, height: height, bytes: rawFrameData.buffer, order: _formatMap[format]!);
  }
}
