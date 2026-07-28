# Quickstart — SDK Provided by User

> Use this guide when the user provides the FaceSDK installation path.
> Do **NOT** download, install, run installers, or configure licensing.
> Do **NOT** call `pip install` or run SDK installers.

## Python

> **Option 1 (recommended for new projects):** `processing_block_python_api` — compiled C++ bindings, faster.
> **Option 2 (legacy):** `python_api` — ctypes-based, slower.

```python
import sys
# Recommended for new projects:
sys.path.insert(0, f"{sdk_root}/processing_block_python_api")
# OR (legacy):
# sys.path.insert(0, f"{sdk_root}/python_api")

from face_sdk_3divi import FacerecService, Error

service = FacerecService.create_service(
    f"{sdk_root}/lib/libfacerec.so",
    f"{sdk_root}/conf/facerec",
    f"{sdk_root}/license")           # license already configured by user
```

## C++

```cpp
#include <facerec/libfacerec.h>

std::string sdk_root = "/path/from/user";
auto service = pbio::FacerecService::createService(
    sdk_root + "/lib/libfacerec.so",
    sdk_root + "/conf/facerec",
    sdk_root + "/license");
```

## Java

```java
FacerecService service = FacerecService.createService(
    sdk_root + "/lib/libfacerec.so",
    sdk_root + "/conf/facerec",
    sdk_root + "/license");
```

## C#

```csharp
// ⚠️ C# does NOT need dll_path — only conf_dir and license_dir
string sdk_root = "/path/from/user";
FacerecService service = FacerecService.createService(
    sdk_root + "/conf/facerec/",
    sdk_root + "/license/");
```

## Flutter/Dart

> ⚠️ Flutter requires `flutter pub get` and asset declaration in `pubspec.yaml`.
> If the user provides SDK path, they need to set up Flutter project manually
> (see [`SETUP.md`](SETUP.md#flutter) for reference, SKIP install steps).

```dart
// For user-provided SDK: Flutter project must already have the SDK plugin
// declared in pubspec.yaml and assets configured.
FacerecService service = await FaceSdkPlugin.createFacerecService();
```

> ⚠️ **Flutter `createProcessingBlock` takes `Map`, not `Context`.** Unlike other languages where `createProcessingBlock(Context)` is the convention, Flutter's `service.createProcessingBlock(Map config)` accepts a plain `Map`. Passing a `Context` object will cause a compilation error. Use `createAsyncProcessingBlock` for background processing.

## Rules

1. Never call `pip install <sdk_path>` or run SDK installers
2. `flutter pub get` and `cmake` for the user's project are **allowed** (project configuration, not SDK installation)
3. Never generate or configure a license
4. The path to SDK comes from the user — use `f"{sdk_root}/..."` in all paths
5. All relative paths in examples should use the `sdk_root` variable

## createService Signature Reference

| Language | Method | Param 1 | Param 2 | Param 3 |
|----------|--------|---------|---------|---------|
| C++ | `FacerecService::createService` | `dll_path` | `config_dir` | `license_dir` |
| Python | `FacerecService.create_service` | `dll_path` | `facerec_conf_dir` | `license_dir` (optional) |
| Java | `FacerecService.createService` | `dll_path` | `config_dir` | `license_dir` |
| C# | `FacerecService.createService` | `facerec_conf_dir` | `license_dir` or `License` | *(no dll_path)* |
| Flutter | `FaceSdkPlugin.createFacerecService` | async, auto-loads assets | — | — |

## Per-Language Project Setup

When SDK is provided by user, each language needs project configuration in addition to the `createService` call. These steps are **configuration**, not installation — AI agents can perform them.

| Language | Files to configure | Environment |
|----------|-------------------|-------------|
| C++ | CMakeLists.txt: `include_directories(${sdk_root}/include)` | None |
| Java | build.gradle: `implementation files("${sdk_root}/lib/facerec.jar")` | `-Djava.library.path=${sdk_root}/lib` |
| C# | .csproj: Reference to `FacerecCSharpWrapper.dll` | Copy `facerec.dll`/`libfacerec.so` to output |
| Python | None (`sys.path.insert` in code) | None |
| Flutter | pubspec.yaml (path dep + assets), MainActivity.kt, build.gradle | Copy SDK files to `assets/` |

See [`SETUP.md`](SETUP.md) for full details.

## See Also

- [`SETUP.md`](SETUP.md) — per-language setup (for fresh installations)
- [`GPU_ACCELERATION.md`](GPU_ACCELERATION.md) — GPU setup
- [`SEARCH_INDEX.yaml`](SEARCH_INDEX.yaml) — entity to file lookup
