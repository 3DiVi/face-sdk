part of utils;

Future<String> loadAssets() async {
  final assetManifest = await AssetManifest.loadFromAssetBundle(rootBundle);
  final assets = assetManifest.listAssets();

  Directory documentsDirectory = await getApplicationDocumentsDirectory();

  for (String key in assets) {
    String assetPath = key;

    if (assetPath.contains("packages/face_sdk_3divi_models/")) {
      assetPath = assetPath.replaceFirst("packages/face_sdk_3divi_models/", "");
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
