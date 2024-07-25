part of utils;

Future<String> loadAssets() async {
  final manifestContent = await rootBundle.loadString("AssetManifest.json");
  final Map<String, dynamic> manifestMap = jsonDecode(manifestContent);
  Directory documentsDirectory = await getApplicationDocumentsDirectory();

  for (String key in manifestMap.keys) {
    String assetPath = key;

    if (assetPath.contains("packages/face_sdk_3divi/")) {
      assetPath = assetPath.replaceFirst("packages/face_sdk_3divi/", "");
    }

    String dbPath = "${documentsDirectory.path}/$assetPath";

    if (FileSystemEntity.typeSync(dbPath) == FileSystemEntityType.notFound ||
        dbPath.contains('conf/facerec') ||
        dbPath.contains('license')) {
      ByteData data = await rootBundle.load(key);
      List<int> bytes = data.buffer.asUint8List(data.offsetInBytes, data.lengthInBytes);
      File file = File(dbPath);
      file.createSync(recursive: true);

      await file.writeAsBytes(bytes);
    }
  }

  return "${documentsDirectory.path}/assets";
}

Future<String> getLibraryDirectory() async {
  const platform = MethodChannel('samples.flutter.dev/facesdk');
  String libraryDirectory = "None";
  libraryDirectory = await platform.invokeMethod('getNativeLibDir');

  return libraryDirectory;
}
