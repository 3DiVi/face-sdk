# FaceSDK Setup

> ⚠️ **FOR FRESH INSTALLATIONS ONLY.**
> If the user already has FaceSDK installed and provides the SDK path, use [`QUICKSTART.md`](QUICKSTART.md) instead.
> Do **NOT** run any install commands — the SDK is already set up.

## SDK Directory Structure

After installation, the FaceSDK root directory contains:

```
face_sdk/
├── bin/                          # facerec.dll (Windows), utilities
├── lib/                          # libfacerec.so (Linux), JNI, C# wrapper, Java JAR
├── include/                      # C++ headers (facerec/)
├── conf/facerec/                 # Configuration files (XML/JSON)
├── license/                      # 3divi_face_sdk.lic
├── share/                        # CV models (faceanalysis/, processing_block/)
├── python_api/                   # Legacy Python package (ctypes)
├── processing_block_python_api/  # Recommended Python package (compiled)
├── flutter/face_sdk_3divi/       # Flutter plugin
├── examples/                     # Demo projects (all languages)
└── api/                          # Wrapper source code
```

## License Setup

Two license types:

### Offline license
Place `3divi_face_sdk.lic` in `license/` directory. Generate via:
```
bin/get_license --get-hwid
# submit HWID to licensing portal, place received file in license/
```

### Online license
The `license/` directory must contain `log_params.xml` and `net_params.xml`.
Both files are created automatically on first run with online license.

### Common path pattern
All languages (except C#) receive the license directory path as a parameter:
```
license_dir = "<sdk_root>/license/"
```

## Per-Language Setup

Choose your language:

- [C++](#c)
- [Python](#python)
- [Java](#java)
- [C#](#c-1)
- [Flutter](#flutter)

---

### C++

**Include paths** — add `<sdk_root>/include` to header search directories:
```cmake
include_directories(${SDK_ROOT}/include)
```

One `.cpp` file must include:
```cpp
#include <facerec/import.h>    // static imports (once)
```

Every file using the SDK:
```cpp
#include <facerec/libfacerec.h>
```

No linker flags needed — the library is loaded dynamically at runtime.

**Service creation:**
```cpp
auto service = pbio::FacerecService::createService(
    sdk_root + "/lib/libfacerec.so",   // Linux
    // sdk_root + "/bin/facerec.dll",  // Windows
    sdk_root + "/conf/facerec",
    sdk_root + "/license");
```

Parameters: `dll_path`, `config_dir`, `license_dir`.

**CMake example:**
```cmake
cmake_minimum_required(VERSION 3.10)
project(face_app)
include_directories(/path/to/face_sdk/include)
find_package(OpenCV REQUIRED)
add_executable(face_app main.cpp)
target_link_libraries(face_app ${OpenCV_LIBS})
```

**Runtime dependencies:**
- Linux: `libusb-0.1.so.4`
- Windows: Microsoft Visual C++ Redistributable for VS 2019
- GPU: CUDA Toolkit 11.8, cuDNN 8.9.0, TensorRT 8.6.0 (optional)
- OpenCV: required only for image I/O / camera capture in examples, not by the SDK itself

---

### Python

**Installation:**

Option A (recommended for new projects — compiled C++ bindings, faster):
```bash
pip3 install /path/to/face_sdk/processing_block_python_api
```

Option B (legacy — ctypes-based, slower):
```bash
pip3 install /path/to/face_sdk/python_api
```

**Dependencies:** Python >= 3.8, `numpy >= 1.16.2`, `opencv-python`, `multipledispatch`.

**Import (both packages import as `face_sdk_3divi`):**
```python
from face_sdk_3divi import FacerecService, Error
```

**Service creation:**
```python
service = FacerecService.create_service(
    "../lib/libfacerec.so",    # dll_path
    "../conf/facerec",         # facerec_conf_dir
    "../license")              # license_dir (optional)
```

Parameters: `dll_path`, `facerec_conf_dir`, `license_dir` (optional).

**Distribution:** The native library `face_sdk_3divi.*.so` / `face_sdk_3divi.*.pyd` must be in site-packages at runtime.

---

### Java

**Dependencies:**

Gradle:
```gradle
dependencies {
    implementation files("${sdk_root}/lib/facerec.jar")
}
```

JVM argument: `-Djava.library.path=${sdk_root}/lib`

Native libraries required at runtime:
- Linux: `libfacerec_jni.so` + `libfacerec.so`
- Windows: `facerec_jni.dll` + `facerec.dll`

**Import:**
```java
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ProcessingBlock;
import com.vdt.face_recognition.sdk.SDKException;
```

**Service creation:**
```java
FacerecService service = FacerecService.createService(
    "../lib/libfacerec.so",   // dll_path
    "../conf/facerec",        // config_dir
    "../license");            // license_dir
```

Android example:
```java
FacerecService service = FacerecService.createService(
    getApplicationInfo().nativeLibraryDir + "/libfacerec.so",
    getApplicationInfo().dataDir + "/fsdk/conf/facerec",
    getApplicationInfo().dataDir + "/fsdk/license");
```

**Resource management:** All native objects must be explicitly disposed:
```java
ioData.dispose();
block.dispose();
service.dispose();
```

**Error handling:**
```java
try {
    block.process(ioData);
} catch (SDKException e) {
    System.out.println("Error code: 0x" + Long.toHexString(e.code()));
    System.out.println("Message: " + e.getMessage());
}
```

Parameters: `dll_path`, `config_dir`, `license_dir`.

---

### C#

> ⚠️ C# is the only language that does **NOT** take a `dll_path` parameter.

**Dependencies:** Add direct reference to `FacerecCSharpWrapper.dll`:
```xml
<Reference Include="FacerecCSharpWrapper">
    <HintPath>${sdk_root}/lib/FacerecCSharpWrapper.dll</HintPath>
</Reference>
```

Copy to executable's output directory:
- `facerec.dll` (Windows) / `libfacerec.so` (Linux)
- `FacerecCSharpWrapper.dll`

No NuGet package — use direct file reference.

**Import:**
```csharp
using VDT.FaceRecognition.SDK;
```

**Service creation:**
```csharp
// With directory paths
FacerecService service = FacerecService.createService(
    "../conf/facerec/",       // facerec_conf_dir
    "../license/");           // license_dir

// With license as string
FacerecService service = FacerecService.createService(
    "../conf/facerec/",
    new FacerecService.License(license_body));
```

**Error handling:**
```csharp
try {
    block.Invoke(ioData);
} catch (ProcessingBlockError e) {
    Console.WriteLine(e.Message);
}
```

Parameters: `facerec_conf_dir`, `license_dir_or_License`.

---

### Flutter

> Flutter setup is the most complex of all supported platforms.

**pubspec.yaml:**
```yaml
dependencies:
  face_sdk_3divi:
    path: <sdk_root>/flutter/face_sdk_3divi

flutter:
  assets:
    - assets/conf/facerec/
    - assets/license/3divi_face_sdk.lic
    - assets/lib/
    - assets/share/faceanalysis/
```

Plugin dependencies: `camera: ^0.11.1`, `ffi: ^2.1.3`, `image: ^4.2.0`, `path: ^1.9.0`, `path_provider: ^2.1.4`.

**Assets setup:**
1. Create `assets/` in your project root.
2. Copy from SDK: `flutter/lib/` → `assets/lib/`, `conf/facerec/` → `assets/conf/facerec/`, `share/` → `assets/share/`, `license/3divi_face_sdk.lic` → `assets/license/3divi_face_sdk.lic`.

Flutter does **not** copy directories recursively — each directory with files must be listed explicitly.

**Android: jniLibs + getNativeLibDir (required):**

In `android/app/build.gradle`:
```gradle
android {
    sourceSets {
        main {
            jniLibs.srcDirs = ["${projectDir}/../../assets/lib"]
        }
    }
}
```

In `MainActivity.kt`:
```kotlin
class MainActivity : FlutterActivity() {
    companion object {
        init { System.loadLibrary("facerec") }
        private const val CHANNEL = "samples.flutter.dev/facesdk"
    }
    private fun getNativeLibDir(): String = applicationInfo.nativeLibraryDir

    override fun configureFlutterEngine(flutterEngine: FlutterEngine) {
        super.configureFlutterEngine(flutterEngine)
        MethodChannel(flutterEngine.dartExecutor.binaryMessenger, CHANNEL)
            .setMethodCallHandler { call, result ->
                when (call.method) {
                    "getNativeLibDir" -> result.success(getNativeLibDir())
                    else -> result.notImplemented()
                }
            }
    }
}
```

**Without `getNativeLibDir` MethodChannel the plugin cannot locate native `.so` libraries on Android.**

**iOS: framework setup (CLI — no Xcode GUI required):**
```bash
FRAMEWORK_SRC="<sdk_root>/flutter/face_sdk_3divi/ios/assets/lib/facerec.framework"
FRAMEWORK_DST="ios/Runner/facerec.framework"
cp -R "$FRAMEWORK_SRC" "$FRAMEWORK_DST"
```

Add to `ios/Podfile`:
```ruby
target 'Runner' do
  pod 'facerec', :path => '../Runner/facerec.framework'
end
```

**Import:**
```dart
import 'package:face_sdk_3divi/face_sdk.dart';
```

**Service creation (async):**
```dart
FacerecService service = await FaceSdkPlugin.createFacerecService();
```

**Running:**
```bash
flutter pub get
flutter pub upgrade
flutter run
```

## System Requirements

| Platform | Architecture |
|---|---|
| Windows 10/11 | x86_64 |
| Ubuntu 20.04 / 22.04 | x86_64 |
| Android | arm64-v8a |
| iOS | arm64 (via Flutter) |

- **Memory**: 4 GB minimum, 8 GB recommended
- **Disk**: ~2 GB full SDK, ~500 MB minimal
- **GPU (optional)**: CUDA Toolkit 11.8, cuDNN 8.9.0, TensorRT 8.6.0

See [`GPU_ACCELERATION.md`](GPU_ACCELERATION.md) for GPU setup and fallback patterns.

## createService Signature Reference

| Language | Method | Param 1 | Param 2 | Param 3 |
|---|---|---|---|---|
| C++ | `FacerecService::createService` | `dll_path` | `config_dir` | `license_dir` |
| Python | `FacerecService.create_service` | `dll_path` | `facerec_conf_dir` | `license_dir` (optional) |
| Java | `FacerecService.createService` | `dll_path` | `config_dir` | `license_dir` |
| C# | `FacerecService.createService` | `facerec_conf_dir` | `license_dir` or `License` | *(no dll_path)* |
| Flutter | `FaceSdkPlugin.createFacerecService` | async, auto-loads assets | — | — |

## See Also

- [`QUICKSTART.md`](QUICKSTART.md) — when SDK path is provided by user
- [`GPU_ACCELERATION.md`](GPU_ACCELERATION.md) — CUDA setup
- [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md) — error recovery
