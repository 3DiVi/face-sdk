import 'dart:io';
import 'package:yaml/yaml.dart';

void main(List<String> args) {
  if (args.isEmpty) {
    stderr.writeln('Usage: dart run prepare_assets.dart <sdk_root>');
    exit(1);
  }

  final sdkRoot = args.first;
  final pubspec = File('pubspec.yaml');

  if (!pubspec.existsSync()) {
    stderr.writeln('pubspec.yaml not found');
    exit(1);
  }

  final yaml = loadYaml(pubspec.readAsStringSync());
  final flutterSection = yaml['flutter'] as YamlMap?;
  final assets = flutterSection?['assets'] as YamlList?;

  if (assets == null || assets.isEmpty) {
    print('No assets defined in pubspec.yaml');
    return;
  }

  for (final assetPath in assets) {
    final target = assetPath.toString();
    final relative = target.startsWith('assets/') ? target.substring(7) : target;
    final source = '$sdkRoot/$relative';

    final srcEntity = FileSystemEntity.typeSync(source);

    switch (srcEntity) {
      case FileSystemEntityType.directory:
        copyRecursive(source, target);
        break;
      case FileSystemEntityType.file:
        copyFile(source, target);
        break;
      default:
        print('Skip missing or unknown: $source');
    }
  }
  copyRecursive('$sdkRoot/flutter/lib', 'assets/lib');

  print('Assets prepared successfully');
}

void copyFile(String from, String to) {
  final src = File(from);
  final dest = File(to);
  dest.parent.createSync(recursive: true);

  bool needCopy = true;
  if (dest.existsSync()) {
    final srcStat = src.statSync();
    final dstStat = dest.statSync();
    if (srcStat.size == dstStat.size &&
        srcStat.modified.isBefore(dstStat.modified.add(const Duration(seconds: 2)))) {
      needCopy = false;
    }
  }

  if (needCopy) {
    src.copySync(dest.path);
    print('Copied file: $from → $to');
  } else {
    print('Skipped (up-to-date): $to');
  }
}

void copyRecursive(String from, String to) {
  final src = Directory(from);
  if (!src.existsSync()) {
    print('Skip missing dir: $from');
    return;
  }

  int copied = 0;
  int skipped = 0;

  for (final entity in src.listSync(recursive: true)) {
    if (entity is File) {
      final relPath = entity.path.substring(src.path.length).replaceAll('\\', '/');
      final destFile = File('$to$relPath');
      destFile.parent.createSync(recursive: true);

      bool needCopy = true;
      if (destFile.existsSync()) {
        final srcStat = entity.statSync();
        final dstStat = destFile.statSync();
        if (srcStat.size == dstStat.size &&
            srcStat.modified.isBefore(dstStat.modified.add(const Duration(seconds: 2)))) {
          needCopy = false;
        }
      }

      if (needCopy) {
        entity.copySync(destFile.path);
        copied++;
      } else {
        skipped++;
      }
    }
  }

  print('Copied $copied, skipped $skipped — from $from → $to');
}
