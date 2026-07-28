# FaceSDK Architecture

> **SDK version: 3.31**
>
> **Entry point:** [`AI_KNOWLEDGE_ROOT.md`](AI_KNOWLEDGE_ROOT.md) — read first for load order.
> See [`blocks/*.yaml`](blocks/) for detailed block specifications.
> See [`context_schema.yaml`](context_schema.yaml) for Context field definitions.

## 1. Architecture Overview

FaceSDK ProcessingBlock API is a pipeline-based architecture where each stage is a **ProcessingBlock** that transforms a shared **Context** object. Blocks are chained in a pipeline, with each block adding or modifying fields in the Context.

### Core Entities
- **Context**: Hierarchical key-value container (like JSON) that flows through the pipeline
- **ProcessingBlock**: Computational unit that reads from and writes to Context
- **ContextTemplate**: Binary biometric face template produced by template extraction
- **DynamicTemplateIndex**: Template database for 1:N face search

## 2. Pipeline Graph

### Face Pipeline
```
FACE_DETECTOR (source) ─ or ─ FACE_TRACKING_MODULE (source)
  └── FACE_FITTER
        ├── AGE_ESTIMATOR
        ├── GENDER_ESTIMATOR
        ├── EMOTION_ESTIMATOR
        ├── MASK_ESTIMATOR
        ├── GLASSES_ESTIMATOR
        ├── EYE_OPENNESS_ESTIMATOR
        ├── DEEPFAKE_ESTIMATOR
        ├── LIVENESS_ESTIMATOR (⚠️ requires exactly 1 face)
        ├── QUALITY_ASSESSMENT_ESTIMATOR
        ├── QUALITY_CONTROL
        ├── FACE_TEMPLATE_EXTRACTOR
        │     ├── VERIFICATION_MODULE (1:1, ⚠️ requires template1 + template2)
        │     └── MATCHER_MODULE (1:N, ⚠️ requires exactly 1 query template)
        └── UTILITY_MODULE (face_cut)
```

**⚠️ Modification matching:** FACE_TEMPLATE_EXTRACTOR, VERIFICATION_MODULE, MATCHER_MODULE, and DynamicTemplateIndex must use the **same modification string** (e.g., `"1000"`). Mismatch causes silent accuracy degradation or SDK error.

### Body Pipeline
```
HUMAN_BODY_DETECTOR (source)
  └── HUMAN_POSE_ESTIMATOR

OBJECT_DETECTOR (source)
  └── HUMAN_POSE_ESTIMATOR (if class == body)
```

### Dependency Graph Properties
- **No cycles**: Graph is a DAG
- **All blocks reachable**: Source blocks (DETECTORs) have no dependencies
- **Correct prerequisites**: All required fields are produced by prerequisite blocks

## 3. Thread Safety

- **ProcessingBlock**: A single instance must not be called from multiple threads simultaneously. Create separate instances for parallel processing — they can share the same `FacerecService`.
- **Context**: ⚠️ **Reading Context from another thread WHILE `process()` is running is NOT safe.** Only read/write Context from the thread that calls `process()`. After `process()` returns, Context is safe for concurrent read.
- **ContextTemplate**: Immutable after creation — safe for concurrent read access.
- **DynamicTemplateIndex**: Not thread-safe for concurrent modifications. Use external synchronization if `add()`/`remove()` are called from multiple threads. Concurrent reads (`size()`, `at()`, `get()`) are not safe during modifications.
- **FacerecService**: Safe for creating new blocks and contexts from multiple threads.

## 4. Memory Management Per Language

| Language | `dispose()` required? | Context | ProcessingBlock | ContextTemplate | DynamicTemplateIndex |
|----------|----------------------|---------|----------------|-----------------|---------------------|
| C++ | No | RAII (shared_ptr) | RAII (shared_ptr) | RAII (shared_ptr) | RAII (shared_ptr) |
| Python | No | GC collected | GC collected | GC collected | GC collected |
| Java | **Optional** (has finalizer) | `.dispose()` | `.dispose()` | `.dispose()` | `.dispose()` |
| C# | **Optional** (has finalizer) | `.Dispose()` | `.Dispose()` | `.Dispose()` | `.Dispose()` |
| Flutter | **Yes — required** | `.dispose()` | `.dispose()` | `.dispose()` | `.dispose()` |

**⚠️ Flutter always needs explicit `dispose()` to avoid native resource leaks.
C++/Python never need it (handled by smart pointers / GC).
Java/C# have finalizers but explicit `dispose()` is recommended for deterministic cleanup.**

## 5. See Also

- [`context_schema.yaml`](context_schema.yaml) — Context fields, image format, evolution

- [`DYNAMIC_TEMPLATE_INDEX_GUIDE.md`](DYNAMIC_TEMPLATE_INDEX_GUIDE.md) — DTI lifecycle
- [`SEARCH_INDEX.yaml`](SEARCH_INDEX.yaml) — entity to file lookup
