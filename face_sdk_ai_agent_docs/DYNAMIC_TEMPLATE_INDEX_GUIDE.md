# DynamicTemplateIndex Lifecycle Guide

See `context_schema.yaml` `DynamicTemplateIndex` entity for method signatures.
See `languages/*.yaml` for per-language API details.

## 1. Creation

> ⚠️ **`max_license_count` default is 100** — set higher for production use.
> When exceeded, `add()` throws an exception.

```python
index = service.create_dynamic_template_index(
    service.create_context({
        "modification": "1000",        # Must match FACE_TEMPLATE_EXTRACTOR
        "version": 1,                  # Modification version
        "max_license_count": 10000,    # ⚠️ Default=100! Increase for production
        "capacity": 10000,             # Optional, defaults to max_license_count
        "force_unique_uuids": True,    # Default: True
        "async": False,                # Beta feature
    }))
```

## 2. Populate (Enroll)

> ⚠️ `add()` throws an exception on duplicate UUID or capacity exceeded. Wrap in try-catch.

```python
for each face:
    ioData = service.create_context_from_encoded_image(image_bytes)
    detector(ioData)
    fitter(ioData)
    extractor(ioData)
    templ = ioData["objects"][i]["face_template"]["template"].get_value()
    uuid  = ioData["objects"][i]["face_template"]["uuid"].get_value()
    try:
        index.add(templ, uuid)
    except Error as e:
        print(f"Enroll failed for {uuid}: {e}")
```

## 3. Set on Context for MATCHER_MODULE

`DynamicTemplateIndex` can be passed directly into `create_context(dict)` or assigned via `context["key"] = index` (Python).

```python
matcher_data = service.create_context({
    "template_index": index,           # ← DynamicTemplateIndex
    "queries": query_template,         # ← ContextTemplate
})
matcher_data["knn"] = 5               # Return top-5 matches (default: 1)
matcher(matcher_data)
```

### Cross-language Context assignment

| Language | Method |
|----------|--------|
| C++ | `context.setDynamicTemplateIndex(index)` |
| Python | `context["template_index"] = index` or `create_context({"template_index": index})` |
| Java | `context.setDynamicTemplateIndex(index)` |
| C# | `context.SetDynamicTemplateIndex(index)` |
| Flutter | `context["template_index"] = index` |

## 4. Read Results

```python
for r in matcher_data["results"]:
    print(r["uuid"].get_value(), r["score"].get_value(), r["distance"].get_value())
```

## 5. Single Template Save / Load

### Save single template
```python
template = ioData["objects"][0]["face_template"]["template"].get_value()  # ContextTemplate
with open("face_template.bin", "wb") as f:
    template.save(f)
```

### Load single template
```python
with open("face_template.bin", "rb") as f:
    template = service.load_context_template(f)
```

## 6. Save / Load

### Save
> ⚠️ `save()` throws an exception on I/O errors (disk full, permission denied). Wrap in try-catch.

```python
try:
    index.save("gallery.bin", allow_overwrite=True)
except Error as e:
    print(f"Save failed: {e}")
```

### Load (via templates_path in config)
```python
index = service.create_dynamic_template_index(
    service.create_context({
        "modification": "1000",
        "version": 1,
        "max_license_count": 10000,
        "templates_path": "gallery.bin"   # ← loads from file
    }))
```

## 7. Manage

> ⚠️ `remove()` throws an exception if UUID not found. `concat()` invalidates the source index.

```python
index.size()                           # Current count
index.capacity()                       # Max capacity
index.get_uuid(i)                      # UUID by position
index.at(uuid)                         # Template by UUID
try:
    index.remove(uuid)                 # Remove by UUID
except Error as e:
    print(f"Remove failed: {e}")
index.concat(other_index)              # Merge (⚠️ invalidates source)
index.clear()                          # Reset
```

## 8. Thread Safety

> DynamicTemplateIndex is **NOT thread-safe** for concurrent modifications.
> Use external locking if `add()`/`remove()` are called from multiple threads.

## 9. Cross-Language Method Mapping

| Operation | C++ | Python | Java | C# | Flutter |
|-----------|-----|--------|------|----|---------|
| Add 1 | `add(templ, uuid)` | `add(templ, uuid)` | `addContextTemplate(templ, uuid)` | `Add(templ, uuid)` | `add(templ, uuid)` |
| Add N | `add(templates[], uuids[])` | `add(templates, uuids)` | `addContextTemplate(templates[], uuids[])` | `AddRange(templates[], uuids[])` | `add(templates, uuids)` |
| Remove 1 | `remove(uuid)` | `remove(uuid)` | `remove(String)` | `Remove(string)` | `remove(String)` |
| Remove N | `remove(uuid)` (per-item) | `remove(uuids)` | `remove(Vector)` | `Remove(string)` (per-item) | `remove(String)` (per-item) |
| Get by UUID | `at(uuid)` | `at(uuid)` | `at(String)` | `At(string)` | `at(String)` |
| Get by index | `get(index)` | `get(index)` | `get(long)` | `Get(long)` | `get(int)` |
| UUID by index | `getUUID(index)` | `get_uuid(index)` | `getUUID(long)` | `GetUUID(long)` | `getUUID(int)` |

## See Also

- [`context_schema.yaml`](context_schema.yaml) — DynamicTemplateIndex entity
- `languages/*.yaml` — per-language API
- [`SEARCH_INDEX.yaml`](SEARCH_INDEX.yaml) — entity to file lookup
