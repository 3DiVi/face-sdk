# FaceSDK Troubleshooting

## Exception Types

| Language | Exception | Inheritance | When Raised |
|----------|-----------|-------------|-------------|
| C++ | `pbio::Error` | inherits `std::exception` | All errors — single exception class |
| Python | `Error` | inherits `Exception` | All errors — single exception class |
| Java | `SDKException` | inherits `RuntimeException` | All errors — single exception class |
| C# | `ProcessingBlockError` | inherits `Exception` | All errors — single exception class |
| Flutter | `TDVException` | implements `Exception` | Block processing errors |
| Flutter | `Exception` (built-in) | — | Invalid config, license errors, I/O failures |

**Tip:** Each language has a single custom exception class. Catch it to handle all FaceSDK errors uniformly. Use language-specific exception names in catch clauses.

## Error Codes Reference

### Context Type Mismatch

Errors from `Context.getValue()` / type-access methods when called with the wrong value type.

| Error Code | Description | Solution |
|------------|-------------|----------|
| `0x2353ead7` | `Context type mismatch: long int was requested` | Call `get_string()` instead, or ensure value is `int` |
| `0x9097d24a` | `Context type mismatch: std::basic_string<char> was requested` | Call `get_long()` instead, or ensure value is `str` |
| `0xfe8d07c6` | `Context type mismatch: bool was requested` | Ensure value is `bool` type |
| `0xd4fbd56c` | `Context type mismatch: double was requested` | Ensure value is `float` type |
| `0xc3febc72` | `Context type mismatch: tdv::data::BlobDataPtr was requested` | Pass `bytes` (e.g., raw image data) |

### Context Index / Creation

| Error Code | Description | Solution |
|------------|-------------|----------|
| `0x6ba98764` | `no index: N` — index out of bounds | Use valid index `0 <= i < len(ctx)` |
| `0x5470a78e` | Assertion failed — file not found or invalid JSON | Verify file path exists and contains valid JSON |
| `0x56234802` | OpenCV error — invalid or empty image bytes | Pass valid JPEG/PNG/BMP bytes (non-empty) |

**Note:** Error `0x5470a78e` is a generic wrapper code for file-related errors. The inner error code or message text distinguishes the specific cause.

### Block Configuration

| Error Code | Description | Solution |
|------------|-------------|----------|
| `0x836504a0` | `Context has no item unit_type` or wrong type | Include `"unit_type"` key with a string value in config |
| `0xdb764015` | Unknown unit type | Use valid unit type from block definitions |
| `0x18ba1f8e` | Unknown modification | Specify correct modification for the unit type |
| `0xc0bd450f` | Unknown version | Use version number within valid range (e.g., `1` to `2`) |

**Note:** Error `0x836504a0` is a catch-all wrapper code for config validation failures.

### Block Pipeline

| Error Code | Description | Solution |
|------------|-------------|----------|
| `0x9398017a` | `Context has no item mouth_right` | Run `FACE_FITTER` before `AGE_ESTIMATOR` |
| `0x9398017a` | `map::at` — missing image data in context | Create context from frame, not plain dict |
| `0x9398017a` | `AgeEstimationInference not found objects` | Ensure faces are detected first with `FACE_DETECTOR` |
| `0x9398017a` | `The number of objects to process should be one` | Filter to exactly 1 face before running `LIVENESS_ESTIMATOR` |

**Note:** Code `0x9398017a` is a generic wrapper used by multiple blocks. The inner assertion or key name distinguishes the root cause.

### License

| Error Code | Description | Solution |
|------------|-------------|----------|
| `0x2fd1cbdd` | License file not found | Place license file in `license/` directory |
| `0x76a10672` | Invalid component or license count exceeded | Reduce concurrent blocks or upgrade license. Each component type (`face_detector`, `age_estimator`, etc.) has its own limit in the `.lic` file |

### DynamicTemplateIndex

| Error Code | Description | Solution |
|------------|-------------|----------|
| `0xa76a1481` | `Assertion 'validIndex' failed` — invalid index on empty DTI | Add templates before calling `get()` |
| `0xa58ccf7b` | `key not found` — UUID lookup failed | Use UUID returned by `add()` |
| `0x52a44b87` | Non-unique UUIDs found | Use unique UUIDs for each template |

### GPU / Runtime

| Error Code | Description | Solution |
|------------|-------------|----------|
| `0x54faacfe` | GPU device not available but `use_cuda` enabled | Install CUDA or disable `use_cuda` |
| `0x78121b36` | Build does not support GPU | Disable `use_cuda` or use GPU-enabled build |
| `0x032ad038` | `libonnxruntime.so` / `onnxruntime.dll` not found | Enable `use_legacy` parameter or ensure onnxruntime library is in the libs directory |
| `0x0616033c`, `0xbe61f5ed` | Error while reading file | Re-download or reinstall distribution |

### Python Wrapper (No Hex Code)

| Exception | Description | Solution |
|-----------|-------------|----------|
| `AssertionError` | `the object does not belong to the base types or Context` | Use only `int`, `float`, `str`, `bool`, `bytes`, `list`, `dict`, or `Context` |

### Error Code Patterns

Several wrapper codes are reused across different error conditions:

| Wrapper Code | Used By | Distinguishing Factor |
|-------------|---------|----------------------|
| `0x836504a0` | Empty config, None unit_type, wrong block type | Inner message text |
| `0x9398017a` | Missing face landmarks, missing image, missing objects, multiple faces | Inner assertion/key name |
| `0x5470a78e` | File not found, invalid JSON | Inner error code or parse_error text |
| `0x56234802` | Invalid image bytes, empty image bytes | OpenCV function name in message |

### Import Errors: Which Package to Use (Python)

**Problem:** Import `FacerecService` fails or wrong package is installed.

| Package (pip install) | Import as | Status | When to Use |
|----------------------|-----------|--------|-------------|
| `processing_block_python_api` | `face_sdk_3divi` | **Recommended** | New projects. Compiled, faster method calls. Pure ProcessingBlock API. |
| `face_sdk_3divi` | `face_sdk_3divi` | Legacy | Existing projects. Includes legacy modules. ctypes-based, slower. |

```python
# Recommended for new projects (compiled PB API, installed via pip install processing_block_python_api):
from face_sdk_3divi import FacerecService, Error

# For backward compatibility (legacy ctypes-based, installed via pip install face_sdk_3divi):
# from face_sdk_3divi import FacerecService
```

Both packages exist in the SDK distribution at `processing_block_python_api/` and `python_api/`.

## Common Problems

### License Errors

**Symptom:** Exception with license-related message on `createService`.

**Recovery:**
- Verify `license/` directory contains valid license file (`3divi_face_sdk.lic`)
- For online licenses, ensure `log_params.xml` and `net_params.xml` exist
- Check HWID matches the license (run `bin/get_license --get-hwid`)
- Ensure `max_license_count` in `DynamicTemplateIndex` config does not exceed licensed capacity
- Each component type (`face_detector`, `age_estimator`, etc.) has its own limit — creating more concurrent blocks of one type than allowed triggers `0x76a10672`

### Context Type Mismatch

**Symptom:** Exception with `Context type mismatch: <type> was requested` (codes `0x2353ead7`, `0x9097d24a`, `0xfe8d07c6`, `0xd4fbd56c`, `0xc3febc72`).

**Recovery:**
- The inner message specifies which type was expected — use the matching accessor:
  - `long int was requested` → use `get_long()` and pass `int`
  - `string was requested` → use `get_string()` and pass `str`
  - `bool was requested` → use `get_bool()` and pass `bool`
  - `double was requested` → use `get_double()` and pass `float`
  - `BlobDataPtr was requested` → use `get_bytes()` and pass `bytes`

### Invalid Paths

**Symptom:** Exception on `createService` — "file not found" or "cannot load library".

**Recovery:**
- Verify `lib/libfacerec.so` (Linux) or `bin/facerec.dll` (Windows) exists
- Verify `conf/facerec/` directory contains configuration files
- Use absolute paths during development to isolate path issues
- In C#, `dll_path` is not passed — the wrapper DLL must be in the executable directory
- In Flutter, verify assets are correctly declared in `pubspec.yaml`

### GPU Setup

**Symptom:** Block creation fails or throws `SDKException` with code `0x54faacfe` / `0x78121b36`.

**Recovery:**
- Verify CUDA Toolkit 11.8 is installed
- Verify cuDNN 8.9.0 is installed and in `LD_LIBRARY_PATH`
- Verify TensorRT 8.6.0 is installed
- Check that `onnxruntime` library path in config points to correct location
- Verify GPU is CUDA-capable with compute capability >= 6.0
- There is **no automatic CPU fallback** — catch the exception and create the block without `use_cuda`

### Missing Pipeline Prerequisites

**Symptom:** Exception `0x9398017a` with messages like `mouth_right`, `map::at`, or `AgeEstimationInference not found objects`.

**Recovery:**
- **Missing landmarks (`mouth_right`):** Run `FACE_FITTER` before `AGE_ESTIMATOR` — the estimator requires fitted face landmarks
- **Missing image data (`map::at`):** Ensure context is created from frame data via `create_context_from_frame()` or `create_context_from_encoded_image()`, not from a plain dict
- **No detected objects (`AgeEstimationInference not found objects`):** Run `FACE_DETECTOR` first to populate `objects[]`
- **Multiple faces (`objects to process should be one`):** `LIVENESS_ESTIMATOR` requires exactly 1 face — filter with `FACE_DETECTOR` and select the highest-confidence face

### Multiple Faces in Liveness

**Symptom:** `ProcessingBlockError` / `TDVException` when running `LIVENESS_ESTIMATOR`.

**Recovery:**
- `LIVENESS_ESTIMATOR` requires exactly one face per frame
- Ensure pre-filtering with `FACE_DETECTOR` and select only the largest/highest-confidence face
- Use `QUALITY_CONTROL` to filter out low-quality detections before liveness

### Template Modification Mismatch

**Symptom:** Verification or matching returns low scores or fails.

**Recovery:**
- `VERIFICATION_MODULE` and `MATCHER_MODULE` modifications must match `FACE_TEMPLATE_EXTRACTOR` modification
- Gallery templates must be extracted with the same modification as probe templates
- Use `ContextTemplate.getMethodName()` to check template modification

### DynamicTemplateIndex Errors

**Symptom:** Exception from DTI operations (codes `0xa76a1481`, `0xa58ccf7b`, `0x52a44b87`).

**Recovery:**
- **`validIndex` failed:** Call `get()` only on indices `0 <= i < len(dti)` — add templates first
- **`key not found`:** Use the UUID string returned by `dti.add()`, not an arbitrary string
- **Non-unique UUIDs:** Each template must have a unique UUID — do not reuse UUIDs from other templates

### onnxruntime Not Found

**Symptom:** Error code `0x032ad038` — `libonnxruntime.so` / `onnxruntime.dll` could not be loaded.

**Recovery:**
- Ensure the onnxruntime library is in the SDK `lib/` or `bin/` directory
- Alternatively, set `"use_legacy": true` in the ProcessingBlock config to use the legacy CPU path

## Silent Acceptance (No Error Raised)

These edge cases do NOT raise errors — the SDK silently accepts invalid input:

| Input | Result | Risk |
|-------|--------|------|
| Empty frame `b""` with `width=0, height=0` | Context created, `is_none()=False` | Passing to any block will likely crash or produce wrong results |
| Buffer smaller than `width*height*3` | Context created, `is_none()=False` | Memory corruption possible in block processing |
| Buffer larger than needed | Context created, `is_none()=False` | Extra data silently ignored |
| `get_uuid(0)` on empty DTI | Returns without error | May return invalid/undefined UUID |
| `dti.concat(dti2)` with same-method DTIs | Merges templates silently | Templates from `dti2` are moved into `dti1` |

## Platform-Specific Issues

### Linux

- `libusb-0.1.so.4` is required for hardware key (license) access
- `LD_LIBRARY_PATH` must include the SDK `lib/` directory
- GPU: set `LD_LIBRARY_PATH` to include CUDA/cuDNN/TensorRT library paths

### Windows

- Microsoft Visual C++ Redistributable for VS 2019 is required
- `PATH` must include the SDK `bin/` directory
- JNI: `java.library.path` must include the directory with `facerec_jni.dll`

### Flutter (Android)

- `getNativeLibDir` MethodChannel must be implemented in `MainActivity` to locate native `.so` files
- Assets must be declared explicitly in `pubspec.yaml` (no recursive directory copy)
- `TDVException` wraps native processing errors — use `try/catch` and check `e.message` for details

### Java (JNI)

- `java.library.path` must include `libfacerec_jni.so` (Linux) or `facerec_jni.dll` (Windows)
- Native objects must be explicitly disposed to avoid memory leaks
