# Agent Rules — FaceSDK

> Load this file immediately after AI_KNOWLEDGE_ROOT.md.
> These rules take precedence over any conflicting instructions.

## Golden Rules

1. **NEVER** run commands that download/install the SDK itself:
   - `pip install <sdk_path>` (SDK installation)
   - `cmake` for the SDK itself
   - SDK installers or setup scripts

   **ALLOWED** (project configuration, not SDK installation):
   - `flutter pub get` (resolve project dependencies)
   - `cmake` for the user's project (not for the SDK)
   - Modifying build files (CMakeLists.txt, build.gradle, pubspec.yaml, .csproj)
   - Copying SDK files into project structure
2. **NEVER** generate, download, or configure a license
3. **NEVER** download models or SDK components
4. The SDK path comes from the user — use `sdk_root` variable throughout
5. If user provides SDK path → use `QUICKSTART.md` (NOT `SETUP.md`)
6. `SETUP.md` files are for fresh installations ONLY — skip them when user provides SDK
7. All relative paths in generated code must use `f"{sdk_root}/..."` pattern

## Load Order (enforced)

1. `AI_KNOWLEDGE_ROOT.md` — entry point
2. `AGENT_RULES.md` — these rules
3. `SEARCH_INDEX.yaml` — find any entity
4. `context_schema.yaml` — understand data model
5. `best_practices.yaml` — identify pipeline blocks
6. `blocks/*.yaml` — block details
7. `languages/*.yaml` — API syntax
8. `best_practices.yaml` → `use_cases:` + `examples/` — code reference
9. `QUICKSTART.md` — project setup (user-provided SDK)
10. `DYNAMIC_TEMPLATE_INDEX_GUIDE.md` — DTI lifecycle

## Common Mistakes to Avoid

- Do NOT call `pip install` even if SETUP.md suggests it — user already has SDK
- Do NOT generate license files — license is already configured
- Do NOT use `dll_path` in C# — C# createService takes only conf_dir and license_dir
- Do NOT forget `.dispose()` in Flutter — native resources leak otherwise. In Java/C#, dispose is optional (finalizer handles cleanup), but calling it early is recommended for determinism.
- `FORMAT_YUV420` is the default format for Flutter. In C++/Python it is supported but rarely used — mainly for testing or specific scenarios
- Do NOT reuse a Context without calling `clear()` first — Context carries state from the previous run. Call `clear()` to reset, or create a new Context.
- Do NOT access `objects[i]` immediately after `process()` without checking `objects` for emptiness first — accessing an empty array causes a processing error (exception).
- When using multiple blocks with templates (FACE_TEMPLATE_EXTRACTOR, VERIFICATION_MODULE, MATCHER_MODULE), always verify their `modification` values match — mismatched modifications produce incorrect scores.

## Template Rules

- Do NOT pass raw ContextTemplate to `template1`/`template2` in VERIFICATION_MODULE — SDK expects `TemplateContainer` (dict `{template: ...}`). Use `objects[].face_template` directly, NOT `objects[].face_template["template"]`.
- Do NOT skip `extractor(ioData)` before reading `face_template` — without FACE_TEMPLATE_EXTRACTOR, `objects[]` has no `face_template` field. Always run detector → fitter → extractor before template access.
- Do NOT call `extractor(ioData)` only in stage 1 — if using a multi-stage pipeline, each stage that needs templates must call `extractor(ioData)` independently.

## Language-Specific Rules

### C#: Use `block.Invoke(ioData)`, NOT `block(ioData)`
C# ProcessingBlock call syntax is `block.Invoke(ioData)`. Python/C++ use `block(ioData)`, Java/Flutter use `block.process(ioData)`. Using `block(ioData)` in C# causes compilation error CS0021.

### Python: Use `processing_block_python_api` for new projects
Two Python packages exist:
- `processing_block_python_api` (recommended for new projects) — compiled C++ bindings, faster
- `python_api` (legacy) — ctypes-based, slower
Always use `processing_block_python_api` unless the user explicitly requests the legacy API.

### Flutter: ProcessingBlock vs AsyncProcessingBlock

Flutter SDK provides two ProcessingBlock variants:
- `createProcessingBlock` — synchronous, blocks UI thread. For short operations (single image).
- `createAsyncProcessingBlock` — asynchronous, runs in a separate isolate. For long-running operations (video pipelines). Use `await block.process(ioData)`.

Always prefer `createAsyncProcessingBlock` for video processing or any pipeline with 3+ blocks.

### `knn`: Set on root Context, NOT in block config
`knn` (number of nearest neighbors for MATCHER_MODULE) must be set directly on the root Context object, e.g. `matcher_data['knn'] = 5`. It is NOT a block configuration parameter — passing it to `createProcessingBlock` has no effect and MATCHER_MODULE will always return 1 result (the default).
