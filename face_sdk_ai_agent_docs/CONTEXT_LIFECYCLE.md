# FaceSDK Context Lifecycle

> **Entry point:** [`AI_KNOWLEDGE_ROOT.md`](AI_KNOWLEDGE_ROOT.md) — read first for load order.
> See [`context_schema.yaml`](context_schema.yaml) for Context fields.
> See [`blocks/*.yaml`](blocks/) for blocks that read/write Context.

## 1. When to Create a New Context

Create **one Context per input** (frame or image). Use the dedicated API methods — **DO NOT construct the `image` field manually** (error-prone, language-specific):

```python
# ✅ PREFERRED: createContextFromFrame() for raw pixels
io_data = service.create_context_from_frame(frame_ptr, width, height, FORMAT_BGR)

# ✅ PREFERRED: createContextFromEncodedImage() for file bytes
io_data = service.create_context_from_encoded_image(file_bytes)

# ❌ AVOID: manual construction (wrong blob type, dtype, or shape per language)
# io_data["image"] = {"format": "NDARRAY", "blob": frame_ptr, "dtype": "uint8_t", "shape": [h, w, 3]}
```

## 2. Can Context Be Reused?

**Yes.** Two ways:

- **After successful `process()`** — you can overwrite fields for the next input.
- **Via `clear()`** — resets Context to initial empty state. Use when you want a fresh start without creating a new object.

```python
io_data.clear()
io_data = service.create_context_from_frame(frame_ptr, width, height, FORMAT_BGR)
```

## 3. Context After `process()` Error

If an exception is thrown during `process()`:
- Context **remains valid** — data is not corrupted
- Processing simply did not complete for the failing block
- You can inspect the Context, recover fields from earlier blocks, or call `clear()` for retry

```python
try:
    detector(io_data)
    fitter(io_data)  # fails here
except Error:
    # Context is valid — keypoints from earlier blocks are still readable
    # Call clear() and retry with new image
    io_data.clear()
io_data = service.create_context_from_frame(frame_ptr, width, height, FORMAT_BGR)
```

## 4. `clone()` — Independent Copy

Use `clone()` when you need a snapshot of Context for parallel or deferred processing:

```python
snapshot = io_data.clone()
# snapshot is independent — modifying original does not affect snapshot
```

Use cases:
- Fork pipeline: run two different block chains from the same detection result
- Deferred processing: save Context state for later analysis
- Thread handoff: pass cloned Context to another thread

## 5. `dispose()` — When Required

| Language | `dispose()` required? | Pattern |
|----------|----------------------|---------|
| C++ | Not required | `shared_ptr` auto-cleanup |
| Python | Not required | GC collected |
| Java | Optional (has finalizer) | Call `.dispose()` for early cleanup |
| C# | Optional (has finalizer) | Call `.Dispose()` or `using()` block |
| Flutter | **Required** | Always call `.dispose()` |

```dart
// Flutter — always dispose
var ioData = service.createContext({});
try {
  detector.process(ioData);
} finally {
  ioData.dispose();
}
```

## 6. Partial Initialization — Cleanup on `createProcessingBlock` Error

If `createProcessingBlock()` throws after some blocks were successfully created, you must **dispose the already-created blocks** to avoid resource leaks:

```java
// Java example — partial init cleanup
FacerecService service = FacerecService.createService(dll, conf, license);
ProcessingBlock detector = null;
ProcessingBlock fitter = null;
ProcessingBlock extractor = null;
try {
    detector = service.createProcessingBlock(config1);
    fitter = service.createProcessingBlock(config2);
    // ⚠️ If next line throws, detector and fitter must be disposed
    extractor = service.createProcessingBlock(config3);
} catch (SDKException e) {
    if (extractor != null) extractor.dispose();
    if (fitter != null) fitter.dispose();
    if (detector != null) detector.dispose();
    throw e;
}
```

```python
# Python — use try/finally to ensure cleanup
blocks = []
try:
    blocks.append(service.create_processing_block(config1))
    blocks.append(service.create_processing_block(config2))
    blocks.append(service.create_processing_block(config3))
except Error:
    # Blocks created before the failure are valid — must dispose explicitly
    for b in reversed(blocks):
        b.dispose()
    raise
```

**Rule:** Always track created blocks in a list/array. In the error handler, iterate and dispose in reverse order. Do NOT rely on GC for native resource cleanup.

## 7. Correct Lifecycle Pattern

```python
# 1. Create
io_data = service.create_context_from_frame(frame_ptr, width, height, FORMAT_BGR)

# 2. Process
try:
    detector(io_data)
    fitter(io_data)
except Error:
    # Context is valid — can clear() and retry with a new frame
    io_data.clear()
    # Option A: retry with new frame data (reprocess)
    io_data = service.create_context_from_frame(frame_ptr, width, height, FORMAT_BGR)
    detector(io_data)
    fitter(io_data)
    # Option B: skip reading and continue to next frame (return / continue)

# 3. Read
for obj in io_data["objects"]:
    print(obj["age"])

# 4. Dispose (Java/C#/Flutter)
# io_data.dispose()
```

## 8. Context Reuse in Video Pipelines

For video streams, creating a new Context per frame is acceptable. Use `clear()` to reset between frames:

```python
while True:
    frame = capture()
    io_data = service.create_context_from_frame(frame.data, frame.cols, frame.rows, FORMAT_BGR)
    try:
        tracker(io_data)
    except Error:
        pass  # io_data will be garbage collected; no explicit dispose needed in Python
```
