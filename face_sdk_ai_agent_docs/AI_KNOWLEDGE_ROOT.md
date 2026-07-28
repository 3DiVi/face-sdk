# AI Knowledge Root — FaceSDK Knowledge Base

> **SDK version: 3.31**

This file is the **entry point** for AI agents using this knowledge base.
Read this file first, then follow the load order.

---

## 1. Purpose

This knowledge base enables AI agents to:

- Understand FaceSDK ProcessingBlock architecture
- Build correct ProcessingBlock pipelines
- Understand Context evolution at each pipeline stage
- Use ContextTemplate and DynamicTemplateIndex correctly
- Generate working code in C++, Python, Java, C#, and Flutter

## 2. Coverage

**Analyzed:**
- ProcessingBlock API
- Context (hierarchical key-value container)
- ContextTemplate (binary face template)
- DynamicTemplateIndex (1:N template database)

**Ignored:**
- Legacy API
- Deprecated API
- Migration Guides
- Historical Documentation

## 3. Architecture Overview

```
[image] → FACE_DETECTOR → FACE_FITTER → [estimators] → [templates] → [match]
                                  ↓
                          [quality filter]
                                  ↓
                          [face normalization]
```

Every pipeline starts with a **source block** (detector) that adds `objects[]` to the Context.
Attribute estimators and template extractors follow, consuming fields produced by previous blocks.

## 4. Artifact Inventory

### Entry point

| File | Content |
|------|---------|
| `QUICKSTART.md` | Quickstart when user provides SDK path (DO NOT install) |
| `SEARCH_INDEX.yaml` | Entity → file lookup (find any doc by name) |
| `AGENT_RULES.md` | Agent operation rules (load 2nd) |

### Reference docs

| File | Content |
|------|---------|
| `ARCHITECTURE.md` | Pipeline architecture, DAG, thread safety |
| `context_schema.yaml` | Context fields, image format, Context evolution |
| `CONTEXT_LIFECYCLE.md` | Context lifecycle: create, reuse after error, clone, dispose |
| `DYNAMIC_TEMPLATE_INDEX_GUIDE.md` | Full DTI lifecycle: create → populate → search → save/load |

### Core schemas (load with reference docs)

| File | Content |
|------|---------|
| `context_schema.yaml` | Complete Context model: 13 root-level entities, 105+ fields, types, producers/consumers |


### Blocks (load second)

`blocks/*.yaml` — 20 files, one per ProcessingBlock:

| Block type | Files |
|------------|-------|
| Detection | face_detector, face_tracking_module, human_body_detector, object_detector |
| Fitting | face_fitter, human_pose_estimator |
| Estimation | age_estimator, gender_estimator, emotion_estimator, mask_estimator, glasses_estimator, eye_openness_estimator, deepfake_estimator, liveness_estimator |
| Quality | quality_assessment_estimator, quality_control |
| Recognition | face_template_extractor, verification_module, matcher_module |
| Utility | utility_module |

Each block YAML contains: unit_type, description, confidence, gpu_supported, modifications (with versions/parameters), requires, produces, prerequisites, constraints, errors, see_also.

### Best practices (load third)

`best_practices.yaml` — 16 recommended pipelines with notes.
Includes: face detection, tracking, attribute analysis, quality filtering, enrollment, verification, identification, liveness, normalization, pose estimation, face crop.

### Language APIs (load fourth)

`languages/*.yaml` — 5 files:

| File | Language |
|------|----------|
| `languages/cpp_api.yaml` | C++ (FacerecService, Context, ProcessingBlock, ContextTemplate, DynamicTemplateIndex) |
| `languages/python_api.yaml` | Python (same classes, dict-style context access) |
| `languages/java_api.yaml` | Java (same classes, .get("key").setString() pattern, explicit dispose) |
| `languages/csharp_api.yaml` | C# (same classes, Dictionary config, Invoke pattern) |
| `languages/flutter_api.yaml` | Flutter/Dart (same classes, async support) |

### Use cases (load fifth)

`best_practices.yaml` → `use_cases:` section — 16 scenario descriptions:

| Key | Goal |
|-----|------|
| face_detection | Detect faces in images |
| face_tracking | Track faces in video stream |
| face_fitting | Fit facial landmarks |
| age_estimation | Estimate age |
| gender_estimation | Estimate gender |
| emotion_estimation | Recognize emotions |
| liveness_detection | Anti-spoofing liveness |
| template_extraction | Extract biometric template |
| face_verification | 1:1 face comparison |
| face_identification | 1:N face search |
| face_normalization | Face crop to frontal |
| face_attribute_analysis | Composite age+gender+emotion |
| face_quality_filtering | Quality-based filtering |
| face_recognition_enrollment | Enroll into gallery |
| face_crop_with_quality | Quality crop + normalization |
| human_pose_estimation | Body pose estimation |

### Code examples (load last)

`examples/{lang}/` — 4 production examples per language, plus Python-only examples:

| Language | face_detection | face_verification | attribute_estimation | face_identification | liveness | face_tracking |
|----------|---------------|-------------------|---------------------|-------------------|----------|---------------|
| C++ | ✅ | ✅ | ✅ | ✅ | — | — |
| Python | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Java | ✅ | ✅ | ✅ | ✅ | — | — |
| C# | ⚠️ fixed | ✅ | ✅ | ✅ | — | — |
| Flutter | ✅ (async) | ✅ (async) | ✅ (async) | ✅ | — | — |

## 5. Key Architectural Rules

1. **Source blocks** (FACE_DETECTOR, FACE_TRACKING_MODULE, HUMAN_BODY_DETECTOR, OBJECT_DETECTOR) have no prerequisites
2. **FACE_TRACKING_MODULE** can replace FACE_DETECTOR in any pipeline (produces additional `track_id`)
3. **All face estimators** require FACE_DETECTOR + FACE_FITTER (input: image, bbox, keypoints)
4. **FACE_FITTER** is the central hub — most downstream blocks depend on its keypoints
5. **Modifications must match** between FACE_TEMPLATE_EXTRACTOR, VERIFICATION_MODULE (1:1), and MATCHER_MODULE (1:N)
6. **LIVENESS_ESTIMATOR** requires exactly 1 face per frame (`objects.size() == 1`)
7. **Context** is modified in-place; each block adds specific fields
8. **YAML is the source of truth** — Markdown is derived and may be incomplete

## 6. Integration Guide

If the user has FaceSDK installed — start with [`QUICKSTART.md`](QUICKSTART.md).
For fresh installations, use per-language setup files below.

| Language | Setup file | Service creation | Key difference |
|----------|-----------|-----------------|----------------|
| C++ | [`SETUP.md`](SETUP.md#c) | `FacerecService::createService(dll, conf, license)` | dll_path required |
| Python | [`SETUP.md`](SETUP.md#python) | `FacerecService.create_service(dll, conf, license?)` | license optional; two package options |
| Java | [`SETUP.md`](SETUP.md#java) | `FacerecService.createService(dll, conf, license)` | explicit .dispose() |
| C# | [`SETUP.md`](SETUP.md#c-1) | `FacerecService.createService(conf, license)` | NO dll_path param ⚠️ |
| Flutter | [`SETUP.md`](SETUP.md#flutter) | `FaceSdkPlugin.createFacerecService()` | async, auto-loads assets; most complex |

Additional: [`GPU_ACCELERATION.md`](GPU_ACCELERATION.md).

## 7. Recommended Agent Workflow

1. Check `QUICKSTART.md` or `SEARCH_INDEX.yaml` for quick navigation
2. Load `context_schema.yaml` to understand available Context fields
3. Load `blocks/*.yaml` to understand block ordering and parameter details
4. Identify required pipeline blocks from `best_practices.yaml`
5. Load relevant `blocks/*.yaml` for parameter details and constraints
6. Load appropriate `languages/*.yaml` for API syntax
7. Use `best_practices.yaml` → `use_cases:` and `examples/` as reference for code generation
8. Consult `CONTEXT_LIFECYCLE.md` for Context lifecycle rules (reuse, clone, dispose)
9. Consult `DYNAMIC_TEMPLATE_INDEX_GUIDE.md` for DTI lifecycle
10. (optional) Consult `SETUP.md` or `QUICKSTART.md` for project setup

## 8. See Also

- [`ARCHITECTURE.md`](ARCHITECTURE.md) — pipeline architecture and thread safety
- [`context_schema.yaml`](context_schema.yaml) — Context fields and evolution
- [`CONTEXT_LIFECYCLE.md`](CONTEXT_LIFECYCLE.md) — Context lifecycle: create, reuse, clone, dispose
- [`DYNAMIC_TEMPLATE_INDEX_GUIDE.md`](DYNAMIC_TEMPLATE_INDEX_GUIDE.md) — full DTI lifecycle
- [`QUICKSTART.md`](QUICKSTART.md) — SDK provided by user (no installation)

- [`best_practices.yaml`](best_practices.yaml) — recommended pipelines
- [`TROUBLESHOOTING.md`](TROUBLESHOOTING.md) — error codes and recovery strategies
- [`GPU_ACCELERATION.md`](GPU_ACCELERATION.md) — CUDA setup and fallback
- [`SEARCH_INDEX.yaml`](SEARCH_INDEX.yaml) — entity to file lookup

## 9. Glossary

| Term | Definition |
|------|------------|
| Context | Hierarchical key-value container flowing through pipeline |
| ProcessingBlock | Computational unit reading/writing Context |
| ContextTemplate | Binary biometric face template |
| DynamicTemplateIndex | Searchable template database for 1:N matching |
| Modification | Algorithm variant for a block (trade speed/accuracy) |
| unit_type | Block identifier string (e.g., "FACE_DETECTOR") |
| confidence: explicit | Indicates that a block returns confidence scores explicitly in the output Context (e.g. `objects[].confidence`). Documents that confidence is a first-class output, not implicit or derived. Present on blocks that produce confidence scores and on Context entities that contain them. |
| Prerequisites | Blocks that must execute before a given block (produces required fields) |
| ContextFormat | Enum defining pixel data formats (FORMAT_BGR, FORMAT_RGB, etc.) |
| `pbio::Error` (C++) / `Error` (Python) / `SDKException` (Java) / `ProcessingBlockError` (C#) / `TDVException` (Flutter) | Exception class — name varies by language; see TROUBLESHOOTING.md |
| processing_block_python_api | Compiled ProcessingBlock Python package (pip package name). Both legacy and compiled packages import as `face_sdk_3divi`. |
| Pipeline | Ordered sequence of ProcessingBlocks transforming Context |
| Source block | Block with no prerequisites (e.g., FACE_DETECTOR) |
| dispose() | Method to explicitly free native resources (Java, C#, Flutter) |

## 10. DynamicTemplateIndex — See Lifecycle Guide

The DTI cross-language method mapping has been moved to [`DYNAMIC_TEMPLATE_INDEX_GUIDE.md`](DYNAMIC_TEMPLATE_INDEX_GUIDE.md) (§8. Cross-Language Method Mapping).

## 11. ContextFormat — Cross-Language Enum Mapping

| Value | Format | C++ | Python | Java | C# | Flutter |
|-------|--------|-----|--------|------|----|---------|
| 0 | BGR | `FORMAT_BGR` | `FORMAT_BGR` | `FORMAT_BGR(0)` | `FORMAT_BGR = 0` | `FORMAT_BGR` |
| 1 | RGB | `FORMAT_RGB` | `FORMAT_RGB` | `FORMAT_RGB(1)` | `FORMAT_RGB = 1` | `FORMAT_RGB` |
| 2 | BGRA | `FORMAT_BGRA8888` | `FORMAT_BGRA8888` | `FORMAT_BGRA8888(2)` | `FORMAT_BGRA8888 = 2` | `FORMAT_BGRA8888` |
| 3 | YUV420 | `FORMAT_YUV420` | `FORMAT_YUV420` | `FORMAT_YUV420(3)` | `FORMAT_YUV420 = 3` | `FORMAT_YUV420` |
| 4 | NV12 | `FORMAT_YUV_NV12` | `FORMAT_YUV_NV12` | `FORMAT_YUV_NV12(4)` | `FORMAT_YUV_NV12 = 4` | `FORMAT_YUV_NV12` |
| 5 | NV21 | `FORMAT_NV21` | `FORMAT_NV21` | `FORMAT_NV21(5)` | `FORMAT_NV21 = 5` | `FORMAT_NV21` |

## 12. FacerecService — Cross-Language Method Mapping

| Operation | C++ | Python | Java | C# | Flutter |
|-----------|-----|--------|------|----|---------|
| Create service | `FacerecService::createService` | `FacerecService.create_service` | `FacerecService.createService` | `FacerecService.createService` | `FaceSdkPlugin.createFacerecService` |
| Create block | `service.createProcessingBlock` | `service.create_processing_block` | `service.createProcessingBlock` | `service.CreateProcessingBlock` | `service.createProcessingBlock(Map)` / `service.createAsyncProcessingBlock` |
| Create Context | `service.createContext` | `service.create_context` | `service.createContext` | `service.CreateContext` | `service.createContext` |
| Context from image | `service.createContextFromEncodedImage` | `service.create_context_from_encoded_image` | `service.createContextFromEncodedImage` | `service.CreateContextFromEncodedImage` | `service.createContextFromEncodedImage` |
| Create DTI | `service.createDynamicTemplateIndex` | `service.create_dynamic_template_index` | `service.createDynamicTemplateIndex` | `service.createDynamicTemplateIndex` | `service.createDynamicTemplateIndex` |
| Load template | `service.loadContextTemplate` | `service.load_context_template` | `service.loadContextTemplate` | `service.LoadContextTemplate` | `service.loadContextTemplate` |
| Clear Context | `ioData.clear()` | `ioData.clear()` | `ioData.clear()` | `ioData.Clear()` | `ioData.clear()` |
| Array length | `.size()` | `len(ioData["objects"])` | `.size()` | `.Length()` | `.len()` |
| Get string | `.getString()` | `["key"].get_string()` | `.getString()` | `.GetString()` | `.getString()` |
| Get double | `.getDouble()` | `["key"].get_double()` | `.getDouble()` | `.GetDouble()` | `.getDouble()` |
| Get long | `.getLong()` | `["key"].get_long()` | `.getLong()` | `.GetLong()` | `.getLong()` |
| Block invoke | `block(ioData)` | `block(ioData)` | `block.process(ioData)` | `block.Invoke(ioData)` | `block.process(ioData)` / `await block.process(ioData)` |
| Dispose | `—` (scope-managed) | `—` (GC) | `service.dispose()` | `service.Dispose()` | `service.dispose()` |
