# GPU Acceleration

GPU inference is available for most blocks.

**⚠️ Blocks that do NOT support GPU (CPU only):** UTILITY_MODULE, VERIFICATION_MODULE, MATCHER_MODULE, QUALITY_ASSESSMENT_ESTIMATOR. Setting `use_cuda: true` on these will throw an exception.

## Configuration

**Python (recommended — via Context):**
```python
config = service.create_context({
    "unit_type": "FACE_DETECTOR",
    "use_cuda": True,
    "ONNXRuntime": {
        "library_path": "/path/to/onnxruntime"
    }
})
```
This creates a Context with GPU config. The Context can then be passed to `create_processing_block(config)` or properties can be modified before block creation.

**C++:**
```cpp
pbio::Context config = service->createContext();
config["unit_type"] = "FACE_DETECTOR";
config["use_cuda"] = true;
config["ONNXRuntime"]["library_path"] = "/path/to/onnxruntime";
```

**Java:**
```java
Context config = service.createContext();
config.get("unit_type").setString("FACE_DETECTOR");
config.get("use_cuda").setBool(true);
config.get("ONNXRuntime").get("library_path").setString("/path/to/onnxruntime");
```

**C#:**
```csharp
var config = service.CreateContext();
config["unit_type"] = "FACE_DETECTOR";
config["use_cuda"] = true;
config["ONNXRuntime"]["library_path"] = "/path/to/onnxruntime";
```

**Flutter:**
```dart
// ⚠️ Flutter createProcessingBlock takes a Map, NOT a Context
final config = {
  "unit_type": "FACE_DETECTOR",
  "use_cuda": true,
  "ONNXRuntime": {"library_path": "/path/to/onnxruntime"},
};
```

## Requirements

- CUDA Toolkit 11.8
- cuDNN 8.9.0
- TensorRT 8.6.0

## Fallback behavior

> **If GPU is unavailable** (no CUDA-capable device, missing libraries, or build without GPU support), block creation with `use_cuda: true` will throw an exception. There is **no automatic CPU fallback**. You must catch the exception and create the block without `use_cuda` as a manual fallback.

### Python fallback example
```python
try:
    config = service.create_context({
        "unit_type": "FACE_DETECTOR",
        "modification": "ssyv_light",
        "use_cuda": True,
        "ONNXRuntime": {"library_path": "/path/to/onnxruntime"}
    })
    detector = service.create_processing_block(config)
except Error:
    print("GPU not available, falling back to CPU")
    config = service.create_context({"unit_type": "FACE_DETECTOR", "modification": "ssyv_light"})
    detector = service.create_processing_block(config)
```

### C++ fallback example
```cpp
pbio::ProcessingBlock detector;
try {
    pbio::Context config = service->createContext();
    config["unit_type"] = "FACE_DETECTOR";
    config["modification"] = "ssyv_light";
    config["use_cuda"] = true;
    config["ONNXRuntime"]["library_path"] = "/path/to/onnxruntime";
    detector = service->createProcessingBlock(config);
} catch (const pbio::Error& e) {
    std::cerr << "GPU not available, falling back to CPU" << std::endl;
    pbio::Context config = service->createContext();
    config["unit_type"] = "FACE_DETECTOR";
    config["modification"] = "ssyv_light";
    config["use_cuda"] = false;
    detector = service->createProcessingBlock(config);
}
```

## See Also

- [`SETUP.md`](SETUP.md) — per-language setup
- [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md) — error recovery
