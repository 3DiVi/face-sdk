import 'package:flutter/services.dart';
import 'package:path_provider/path_provider.dart';
import 'dart:io';
import 'dart:convert';

Future<String> _loadAssets() async {
  final manifestContent = await rootBundle.loadString('AssetManifest.json');
  final Map<String, dynamic> manifestMap = jsonDecode(manifestContent);
  Directory docDirectory = await getApplicationDocumentsDirectory();

  for (String key in manifestMap.keys) {
    var dbPath = '${docDirectory.path}/$key';
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

  return '${docDirectory.path}/assets';
}

Future<String> _getLibDir() async {
  String libDir = "None";
  const platform = MethodChannel('samples.flutter.dev/facesdk');

  try {
    final String res = await platform.invokeMethod('getNativeLibDir');
    libDir = res;
  } on PlatformException catch (e) {}

  return libDir;
}

Future<List<String>> getDirs() async {
  List<String> dirs = [];

  dirs.add(await _loadAssets());
  dirs.add(await _getLibDir());

  return dirs;
}