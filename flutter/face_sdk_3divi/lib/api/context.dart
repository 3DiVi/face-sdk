part of face_sdk_3divi;


class Context extends _ComplexObject {
  Context(DynamicLibrary dll_handle, Pointer<Void> impl)
      : super(dll_handle, impl) {
    if (impl is Pointer<Never>) {
      var constructor = dll_handle.lookupFunction<
          _Context_Construct_c,
          _Context_Constructor_dart>(_context_namespace + 'create');
      var exception = _getException();

      final newImpl = constructor(exception);

      tdvCheckException(exception, _dll_handle);
      this._impl = newImpl;
    }
  }

  void dispose() {
    if (_isDisposed)
      return;

    var exception = _getException();
    var destructor = _dll_handle.lookupFunction<
        _Context_Destruct_c,
        _Context_Destructor_dart>(_context_namespace + 'destroy');

    destructor(this._impl, exception);

    tdvCheckException(exception, _dll_handle);

    _isDisposed = true;
  }


  Context operator [](dynamic key) {
    if (key is String) {
      return this._getByKey(key);
    }
    else if (key is int) {
      return this._getByIndex(key);
    }
    return Context(_dll_handle, nullptr);
  }

  void placeValues(data) {
    if (data is Map) {
      this._insertDict(data);
    }
    if ((data is List) & (data is! Uint8List)) {
      this._insertList(data);
    }
    if (data is int) {
      this._setLong(data);
    }
    else {
      if (data is double) {
        this._setDouble(data);
      }
    }
    if (data is String) {
      this._setStr(data);
    }
    if (data is bool) {
      this._setBool(data);
    }
    if (data is Uint8List) {
      this._setDataPtr(data);
    }
  }

  void _insertDict(Map data) {
    data.forEach((k, v) {
      this._getOrInsertByKey(k).placeValues(v);
    });
  }

  void _insertList(List data) {
    for (final value in data) {
      var ctx = Context(this._dll_handle, nullptr);
      ctx.placeValues(value);
      this._pushBack(ctx);
      ctx.dispose();
    }
  }

  Context _getOrInsertByKey(String key) {
    final constructor = _dll_handle.lookupFunction<
        _Context_getOrInsertByKey_c,
        _Context_getOrInsertByKey_dart>
      (_context_namespace + 'getOrInsertByKey');
    final exception = _getException();
    final pointer = constructor(
        this._impl,
        key.toNativeUtf8(),
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return Context(_dll_handle, pointer);
  }

  Context _getByIndex(int index) {
    final constructor = _dll_handle.lookupFunction<
        _Context_getByIndex_c,
        _Context_getByIndex_dart>
      (_context_namespace + 'getByIndex');
    final exception = _getException();
    final pointer = constructor(
        this._impl,
        index,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return Context(_dll_handle, pointer);
  }

  Context _getByKey(String key) {
    final constructor = _dll_handle.lookupFunction<
        _Context_getByKey_c,
        _Context_getByKey_dart>
      (_context_namespace + 'getByKey');
    final exception = _getException();
    final pointer = constructor(
        this._impl,
        key.toNativeUtf8(),
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return Context(_dll_handle, pointer);
  }

  void _pushBack(Context data) {
    final constructor = _dll_handle.lookupFunction<
        _Context_pushBack_c,
        _Context_pushBack_dart>
      (_context_namespace + 'pushBack');
    final exception = _getException();

    constructor(
        this._impl,
        data._impl,
        1,
        exception
    );

    tdvCheckException(exception, _dll_handle);
  }

  void _setStr(String str) {
    final constructor = _dll_handle.lookupFunction<
        _Context_putStr_c,
        _Context_putStr_dart>
      (_context_namespace + 'putStr');
    final exception = _getException();
    constructor(
        this._impl,
        str.toNativeUtf8(),
        exception);

    tdvCheckException(exception, _dll_handle);
  }

  void _setLong(int val) {
    final constructor = _dll_handle.lookupFunction<
        _Context_putLong_c,
        _Context_putLong_dart>
      (_context_namespace + 'putLong');
    final exception = _getException();
    constructor(
        this._impl,
        val,
        exception
    );

    tdvCheckException(exception, _dll_handle);
  }

  void _setDouble(double val) {
    final constructor = _dll_handle.lookupFunction<
        _Context_putDouble_c,
        _Context_putDouble_dart>
      (_context_namespace + 'putDouble');
    final exception = _getException();
    constructor(
        this._impl,
        val,
        exception
    );

    tdvCheckException(exception, _dll_handle);
  }

  void _setBool(bool val) {
    final constructor = _dll_handle.lookupFunction<
        _Context_putBool_c,
        _Context_putBool_dart>
      (_context_namespace + 'putBool');
    final exception = _getException();
    constructor(
        this._impl,
        val ? 1 : 0,
        exception
    );

    tdvCheckException(exception, _dll_handle);
  }

  void _setDataPtr(Uint8List data) {
    final constructor = _dll_handle.lookupFunction<
        _Context_putDataPtr_c,
        _Context_putDataPtr_dart>
      (_context_namespace + 'putDataPtr');
    final exception = _getException();

    final Pointer<Uint8> frameData = malloc.allocate<Uint8>(data.length);
    final pointerList = frameData.asTypedList(data.length);
    pointerList.setAll(0, data);

    constructor(
        this._impl,
        frameData,
        data.length,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    malloc.free(frameData);
  }

  bool _getBool() {
    final constructor = _dll_handle.lookupFunction<
        _Context_getBool_c,
        _Context_getBool_dart>
      (_context_namespace + 'getBool');
    final exception = _getException();
    int res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
  }

  String _getStr() {
    final strLengthFunction = _dll_handle.lookupFunction<
        _Context_getStrSize_c,
        _Context_getStrSize_dart>
      (_context_namespace + 'getStrSize');
    final exceptionStrSize = _getException();
    int strSize = strLengthFunction(this._impl, exceptionStrSize);
    tdvCheckException(exceptionStrSize, _dll_handle);

    final getStrFun = _dll_handle.lookupFunction<
        _Context_getStr_c,
        _Context_getStr_dart>
      (_context_namespace + 'getStr');

    final exception = _getException();

    Pointer<Utf8> strPointer = malloc.allocate(sizeOf<Pointer<Utf8>>() * strSize);
    getStrFun(this._impl, strPointer, exception);
    tdvCheckException(exception, _dll_handle);

    String strValue = strPointer.toDartString(length: strSize);

    malloc.free(strPointer);

    return strValue;
  }

  int _getLong() {
    final constructor = _dll_handle.lookupFunction<
        _Context_getLong_c,
        _Context_getLong_dart>
      (_context_namespace + 'getLong');
    final exception = _getException();
    var res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  double _getDouble() {
    final constructor = _dll_handle.lookupFunction<
        _Context_getDouble_c,
        _Context_getDouble_dart>
      (_context_namespace + 'getDouble');
    final exception = _getException();
    var res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  Pointer<Uint8> getDataPtr() {
    final constructor = _dll_handle.lookupFunction<
        _Context_getDataPtr_c,
        _Context_getDataPtr_dart>
      (_context_namespace + 'getDataPtr');
    final exception = _getException();
    var res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res;
  }

  int len() {
    final constructor = _dll_handle.lookupFunction<
        _Context_getLength_c,
        _Context_getLength_dart>
      (_context_namespace + 'getLength');
    final exception = _getException();
    var res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);
    return res;
  }

  List<String> getKeys() {
    final constructor = _dll_handle.lookupFunction<
        _Context_getKeys_c,
        _Context_getKeys_dart>
      (_context_namespace + 'getKeys');
    final exception = _getException();

    final length = this.len();

    var p_value_array = constructor(
        this._impl,
        length,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    List<String> res = [];

    for (int i = 0; i < length; i++) {
      res.add(p_value_array[i].toDartString());
    }

    return res;
  }

  bool is_none() {
    final constructor = _dll_handle.lookupFunction<
        _Context_isNone_c,
        _Context_isNone_dart>
      (_context_namespace + 'isNone');
    final exception = _getException();
    int res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
  }

  bool is_array() {
    final constructor = _dll_handle.lookupFunction<
        _Context_isArray_c,
        _Context_isArray_dart>
      (_context_namespace + 'isArray');
    final exception = _getException();
    int res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
  }

  bool is_object() {
    final constructor = _dll_handle.lookupFunction<
        _Context_isObject_c,
        _Context_isObject_dart>
      (_context_namespace + 'isObject');
    final exception = _getException();

    int res = constructor(
        this._impl,
        exception);

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
  }

  bool is_bool() {
    final constructor = _dll_handle.lookupFunction<
        _Context_isBool_c,
        _Context_isBool_dart>
      (_context_namespace + 'isBool');
    final exception = _getException();
    int res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
  }

  bool is_long() {
    final constructor = _dll_handle.lookupFunction<
        _Context_isLong_c,
        _Context_isLong_dart>
      (_context_namespace + 'isLong');
    final exception = _getException();
    int res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
  }

  bool is_double() {
    final constructor = _dll_handle.lookupFunction<
        _Context_isDouble_c,
        _Context_isDouble_dart>
      (_context_namespace + 'isDouble');
    final exception = _getException();
    int res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
  }

  bool is_string() {
    final constructor = _dll_handle.lookupFunction<
        _Context_isString_c,
        _Context_isString_dart>
      (_context_namespace + 'isString');
    final exception = _getException();
    int res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
  }

  bool is_data_ptr() {
    final constructor = _dll_handle.lookupFunction<
        _Context_isDataPtr_c,
        _Context_isDataPtr_dart>
      (_context_namespace + 'isDataPtr');
    final exception = _getException();
    int res = constructor(
        this._impl,
        exception
    );

    tdvCheckException(exception, _dll_handle);

    return res == 1 ? true : false;
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
      return this.getDataPtr();
    }
  }
}