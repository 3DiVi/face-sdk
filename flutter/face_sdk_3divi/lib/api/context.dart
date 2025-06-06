part of face_sdk_3divi;

enum ContextFormat { FORMAT_BGR, FORMAT_RGB, FORMAT_BGRA8888, FORMAT_YUV420, FORMAT_YUV_NV12, FORMAT_NV21 }

class Context extends _ComplexObject {
  Context(DynamicLibrary dll_handle, Pointer<Void> impl) : super(dll_handle, impl) {
    if (_impl.address == Pointer.fromAddress(0).address) {
      var constructor = dll_handle.lookupFunction<_Context_Construct_c, _Context_Constructor_dart>(_context_namespace + 'create');
      var exception = _getException();

      final newImpl = constructor(exception);

      tdvCheckException(exception, _dll_handle);
      this._impl = newImpl;
    }
  }

  Context.fromImage(DynamicLibrary dll_handle, Uint8List data) : super(dll_handle, nullptr) {
    var constructor = dll_handle.lookupFunction<_Context_CreateFromEncodedImage_c, _Context_CreateFromEncodedImage_dart>(_context_namespace + "createFromEncodedImage");

    var exception = _getException();
    Pointer<Uint8> dataPointer = malloc.allocate<Uint8>(data.length);
    dataPointer.asTypedList(data.length).setAll(0, data);

    final newImpl = constructor(dataPointer, data.length, exception);

    tdvCheckException(exception, _dll_handle);

    this._impl = newImpl;

    malloc.free(dataPointer);
  }

  Context.fromFrame(DynamicLibrary dll_handle, Uint8List data, int width, int height, ContextFormat format, int baseAngle) : super(dll_handle, nullptr) {
    var constructor = dll_handle.lookupFunction<_Context_CreateFromFrame_c, _Context_CreateFromFrame_dart>(_context_namespace + "createFromFrame");

    var exception = _getException();
    Pointer<Uint8> dataPointer = malloc.allocate<Uint8>(data.length);
    dataPointer.asTypedList(data.length).setAll(0, data);

    final newImpl = constructor(dataPointer, width, height, format.index, baseAngle, exception);

    tdvCheckException(exception, _dll_handle);

    this._impl = newImpl;

    malloc.free(dataPointer);
  }

  Context.fromJsonFile(DynamicLibrary dll_handle, String path) : super(dll_handle, nullptr) {
    var constructor = dll_handle.lookupFunction<_Context_CreateFromJsonFile_c, _Context_CreateFromJsonFile_dart>(_context_namespace + "createFromJsonFile");

    var exception = _getException();

    final newImpl = constructor(path.toNativeUtf8(), exception);

    tdvCheckException(exception, _dll_handle);

    this._impl = newImpl;
  }

  void dispose() {
    if (_isDisposed) return;

    var exception = _getException();
    var destructor = _dll_handle.lookupFunction<_Context_Destruct_c, _Context_Destructor_dart>(_context_namespace + 'destroy');

    destructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    _isDisposed = true;
  }

  Context at(String key) {
    return _getByKey(key);
  }

  Context operator [](dynamic key) {
    if (key is String) {
      return this._getOrInsertByKey(key);
    } else if (key is int) {
      return this._getByIndex(key);
    }
    return Context(_dll_handle, nullptr);
  }

  void operator []=(dynamic key, data) {
    this._getOrInsertByKey(key).placeValues(data);
  }

  dynamic toMap() {
    if (this.is_array()) {
      List res = [];
      for (int i = 0; i < this.len(); i++) {
        res.add(this[i].toMap());
      }
      return res;
    }
    if (this.is_object()) {
      Map res = {};
      for (final key in this.getKeys()) {
        res[key] = this[key].toMap();
      }
      return res;
    }
    return this.get_value();
  }

  void placeValues(data) {
    if (data is Context) {
      this._setContext(data);
    } else if (data is Map) {
      this.insertMap(data);
    } else if (data is Uint8List) {
      this._setDataPtr(data);
    } else if (data is List) {
      this.insertList(data);
    } else if (data is int) {
      this._setLong(data);
    } else if (data is double) {
      this._setDouble(data);
    } else if (data is String) {
      this._setStr(data);
    } else if (data is bool) {
      this._setBool(data);
    } else if (data is ContextTemplate) {
      this._setContextTemplate(data);
    } else if (data is DynamicTemplateIndex) {
      this._setDynamicTemplateIndex(data);
    }
  }

  void _setContext(Context data) {
    if (data != this) {
      var exception = _getException();

      final constructor = _dll_handle.lookupFunction<_Context_copy_c, _Context_copy_dart>(_context_namespace + 'copy');

      constructor(data._impl, this._impl, exception);

      tdvCheckException(exception, _dll_handle);
    }
  }

  void clear() {
    var exception = _getException();

    final constructor = _dll_handle.lookupFunction<_Context_clear_c, _Context_clear_dart>(_context_namespace + 'clear');

    constructor(_impl, exception);

    tdvCheckException(exception, _dll_handle);
  }

  void erase(String key) {
    var exception = _getException();

    final constructor = _dll_handle.lookupFunction<_Context_erase_c, _Context_erase_dart>(_context_namespace + 'erase');

    constructor(_impl, key.toNativeUtf8(), exception);

    tdvCheckException(exception, _dll_handle);
  }

  void reserve(int size) {
    var exception = _getException();

    final constructor = _dll_handle.lookupFunction<_Context_reserve_c, _Context_reserve_dart>(_context_namespace + 'reserve');

    constructor(_impl, size, exception);

    tdvCheckException(exception, _dll_handle);
  }

  bool contains(String key) {
    var exception = _getException();

    final constructor = _dll_handle.lookupFunction<_Context_contains_c, _Context_contains_dart>(_context_namespace + 'contains');

    bool result = constructor(_impl, key.toNativeUtf8(), exception);

    tdvCheckException(exception, _dll_handle);

    return result;
  }

  bool compare(Context ctx) {
    var exception = _getException();

    final constructor = _dll_handle.lookupFunction<_Context_compare_c, _Context_compare_dart>(_context_namespace + 'compare');

    bool result = constructor(_impl, ctx._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return result;
  }

  void saveToJsonFile(String path) {
    var exception = _getException();

    final constructor = _dll_handle.lookupFunction<_Context_saveToJsonFile_c, _Context_saveToJsonFile_dart>(_context_namespace + 'saveToJsonFile');

    constructor(_impl, path.toNativeUtf8(), exception);

    tdvCheckException(exception, _dll_handle);
  }

  String serializeToJson() {
    var exception = _getException();

    final serializeConstructor = _dll_handle.lookupFunction<_Context_serializeToJson_c, _Context_serializeToJson_dart>(_context_namespace + 'serializeToJson');

    Pointer<Utf8> data = serializeConstructor(_impl, exception);

    tdvCheckException(exception, _dll_handle);

    String result = data.toDartString();

    malloc.free(data);

    return result;
  }

  void pushBack(data) {
    if (data is Context) {
      this._pushBack(data);
    } else {
      var ctx = Context(this._dll_handle, nullptr);
      ctx.placeValues(data);
      this._pushBack(ctx);
      ctx.dispose();
    }
  }

  void insertMap(Map data) {
    data.forEach((k, v) {
      this._getOrInsertByKey(k).placeValues(v);
    });
  }

  void insertList(List data) {
    for (final value in data) {
      var ctx = Context(this._dll_handle, nullptr);
      ctx.placeValues(value);
      this._pushBack(ctx);
      ctx.dispose();
    }
  }

  Context _getOrInsertByKey(String key) {
    final constructor = _dll_handle.lookupFunction<_Context_getOrInsertByKey_c, _Context_getOrInsertByKey_dart>(_context_namespace + 'getOrInsertByKey');
    final exception = _getException();
    final pointer = constructor(this._impl, key.toNativeUtf8(), exception);

    tdvCheckException(exception, _dll_handle);

    return Context(_dll_handle, pointer);
  }

  Context _getByIndex(int index) {
    final constructor = _dll_handle.lookupFunction<_Context_getByIndex_c, _Context_getByIndex_dart>(_context_namespace + 'getByIndex');
    final exception = _getException();
    final pointer = constructor(this._impl, index, exception);

    tdvCheckException(exception, _dll_handle);

    return Context(_dll_handle, pointer);
  }

  Context _getByKey(String key) {
    final constructor = _dll_handle.lookupFunction<_Context_getByKey_c, _Context_getByKey_dart>(_context_namespace + 'getByKey');
    final exception = _getException();
    final pointer = constructor(this._impl, key.toNativeUtf8(), exception);

    tdvCheckException(exception, _dll_handle);

    return Context(_dll_handle, pointer);
  }

  void _pushBack(Context data) {
    final constructor = _dll_handle.lookupFunction<_Context_pushBack_c, _Context_pushBack_dart>(_context_namespace + 'pushBack');
    final exception = _getException();

    constructor(this._impl, data._impl, true, exception);

    tdvCheckException(exception, _dll_handle);
  }

  void _setStr(String str) {
    final constructor = _dll_handle.lookupFunction<_Context_putStr_c, _Context_putStr_dart>(_context_namespace + 'putStr');
    final exception = _getException();
    constructor(this._impl, str.toNativeUtf8(), exception);

    tdvCheckException(exception, _dll_handle);
  }

  void _setLong(int val) {
    final constructor = _dll_handle.lookupFunction<_Context_putLong_c, _Context_putLong_dart>(_context_namespace + 'putLong');
    final exception = _getException();
    constructor(this._impl, val, exception);

    tdvCheckException(exception, _dll_handle);
  }

  void _setDouble(double val) {
    final constructor = _dll_handle.lookupFunction<_Context_putDouble_c, _Context_putDouble_dart>(_context_namespace + 'putDouble');
    final exception = _getException();
    constructor(this._impl, val, exception);

    tdvCheckException(exception, _dll_handle);
  }

  void _setBool(bool val) {
    final constructor = _dll_handle.lookupFunction<_Context_putBool_c, _Context_putBool_dart>(_context_namespace + 'putBool');
    final exception = _getException();
    constructor(this._impl, val, exception);

    tdvCheckException(exception, _dll_handle);
  }

  void _setContextTemplate(ContextTemplate val) {
    final constructor = _dll_handle.lookupFunction<_Context_putContextTemplate_c, _Context_putContextTemplate_dart>(_context_namespace + 'putContextTemplate');
    final exception = _getException();
    constructor(this._impl, val._impl, exception);

    tdvCheckException(exception, _dll_handle);
  }

  void _setDynamicTemplateIndex(DynamicTemplateIndex val) {
    final constructor = _dll_handle.lookupFunction<_Context_putDynamicTemplateIndex_c, _Context_putDynamicTemplateIndex_dart>(_context_namespace + 'putDynamicTemplateIndex');
    final exception = _getException();
    constructor(this._impl, val._impl, exception);

    tdvCheckException(exception, _dll_handle);
  }

  void _setDataPtr(Uint8List data) {
    final constructor = _dll_handle.lookupFunction<_Context_putDataPtr_c, _Context_putDataPtr_dart>(_context_namespace + 'putDataPtr');
    final exception = _getException();

    final Pointer<Uint8> frameData = malloc.allocate<Uint8>(data.length);
    final pointerList = frameData.asTypedList(data.length);
    pointerList.setAll(0, data);

    constructor(this._impl, frameData, data.length, exception);

    tdvCheckException(exception, _dll_handle);

    malloc.free(frameData);
  }

  bool _getBool() {
    final constructor = _dll_handle.lookupFunction<_Context_getBool_c, _Context_getBool_dart>(_context_namespace + 'getBool');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  String _getStr() {
    final strLengthFunction = _dll_handle.lookupFunction<_Context_getStrSize_c, _Context_getStrSize_dart>(_context_namespace + 'getStrSize');
    final exceptionStrSize = _getException();
    int strSize = strLengthFunction(this._impl, exceptionStrSize);
    tdvCheckException(exceptionStrSize, _dll_handle);

    final getStrFun = _dll_handle.lookupFunction<_Context_getStr_c, _Context_getStr_dart>(_context_namespace + 'getStr');
    final exception = _getException();

    Pointer<Utf8> result = getStrFun(this._impl, exception);
    tdvCheckException(exception, _dll_handle);

    return result.toDartString(length: strSize);
  }

  int _getLong() {
    final constructor = _dll_handle.lookupFunction<_Context_getLong_c, _Context_getLong_dart>(_context_namespace + 'getLong');
    final exception = _getException();
    var res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  double _getDouble() {
    final constructor = _dll_handle.lookupFunction<_Context_getDouble_c, _Context_getDouble_dart>(_context_namespace + 'getDouble');
    final exception = _getException();
    var res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  Pointer<Uint8> _getDataPtr() {
    final constructor = _dll_handle.lookupFunction<_Context_getDataPtr_c, _Context_getDataPtr_dart>(_context_namespace + 'getDataPtr');
    final exception = _getException();
    var res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  ContextTemplate _getContextTemplate() {
    final constructor = _dll_handle.lookupFunction<_Context_getContextTemplate_c, _Context_getContextTemplate_dart>(_context_namespace + 'getContextTemplate');
    final exception = _getException();
    var res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return ContextTemplate(_dll_handle, res, weak: true);
  }

  DynamicTemplateIndex _getDynamicTemplateIndex() {
    final constructor = _dll_handle.lookupFunction<_Context_getDynamicTemplateIndex_c, _Context_getDynamicTemplateIndex_dart>(_context_namespace + 'getDynamicTemplateIndex');
    final exception = _getException();
    var res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return DynamicTemplateIndex(_dll_handle, res, weak: true);
  }

  Uint8List getBytes(int size) {
    return Context.getBytesFromPointer(_getDataPtr(), size);
  }

  int len() {
    final constructor = _dll_handle.lookupFunction<_Context_getLength_c, _Context_getLength_dart>(_context_namespace + 'getLength');
    final exception = _getException();
    var res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);
    return res;
  }

  List<String> getKeys() {
    final constructor = _dll_handle.lookupFunction<_Context_getKeys_c, _Context_getKeys_dart>(_context_namespace + 'getKeys');
    final exception = _getException();

    final length = this.len();

    var p_value_array = constructor(this._impl, length, exception);

    tdvCheckException(exception, _dll_handle);

    List<String> res = [];

    for (int i = 0; i < length; i++) {
      res.add(p_value_array[i].toDartString());
    }

    return res;
  }

  bool is_none() {
    final constructor = _dll_handle.lookupFunction<_Context_isNone_c, _Context_isNone_dart>(_context_namespace + 'isNone');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_array() {
    final constructor = _dll_handle.lookupFunction<_Context_isArray_c, _Context_isArray_dart>(_context_namespace + 'isArray');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_object() {
    final constructor = _dll_handle.lookupFunction<_Context_isObject_c, _Context_isObject_dart>(_context_namespace + 'isObject');
    final exception = _getException();

    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_bool() {
    final constructor = _dll_handle.lookupFunction<_Context_isBool_c, _Context_isBool_dart>(_context_namespace + 'isBool');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_long() {
    final constructor = _dll_handle.lookupFunction<_Context_isLong_c, _Context_isLong_dart>(_context_namespace + 'isLong');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_double() {
    final constructor = _dll_handle.lookupFunction<_Context_isDouble_c, _Context_isDouble_dart>(_context_namespace + 'isDouble');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_string() {
    final constructor = _dll_handle.lookupFunction<_Context_isString_c, _Context_isString_dart>(_context_namespace + 'isString');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_data_ptr() {
    final constructor = _dll_handle.lookupFunction<_Context_isDataPtr_c, _Context_isDataPtr_dart>(_context_namespace + 'isDataPtr');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_context_template() {
    final constructor = _dll_handle.lookupFunction<_Context_isContextTemplate_c, _Context_isContextTemplate_dart>(_context_namespace + 'isContextTemplate');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  bool is_dynamic_template_index() {
    final constructor = _dll_handle.lookupFunction<_Context_isDynamicTemplateIndex_c, _Context_isDynamicTemplateIndex_dart>(_context_namespace + 'isDynamicTemplateIndex');
    final exception = _getException();
    bool res = constructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  dynamic get_value() {
    if (this.is_none()) {
      return null;
    }
    if (this.is_bool()) {
      return this._getBool();
    }
    if (this.is_string()) {
      return this._getStr();
    }
    if (this.is_long()) {
      return this._getLong();
    }
    if (this.is_double()) {
      return this._getDouble();
    }
    if (this.is_data_ptr()) {
      return this._getDataPtr();
    }
    if (this.is_context_template()) {
      return this._getContextTemplate();
    }
    if (this.is_dynamic_template_index()) {
      return this._getDynamicTemplateIndex();
    }
  }

  static Uint8List getBytesFromPointer(Pointer<Uint8> pointer, int size) {
    return Uint8List.fromList(pointer.asTypedList(size));
  }
}
