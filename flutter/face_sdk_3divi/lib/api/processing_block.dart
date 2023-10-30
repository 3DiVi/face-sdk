part of face_sdk_3divi;

///Interface object used to work with estimators from Processing Block API.
class ProcessingBlock extends _ComplexObject {
  ProcessingBlock(DynamicLibrary dll_handle, Pointer<Void> impl) :
        super(dll_handle, impl);

  void dispose() {
    final exception = _getException();
    final destructor = _dll_handle.lookupFunction<
        _ProcessingBlock_destroyProcessingBlock_c,
        _ProcessingBlock_destroyProcessingBlock_dart>
      ('TDVProcessingBlock_destroy');

    destructor(_impl, exception);

    tdvCheckException(exception, _dll_handle);
  }

  dynamic process(ctx) {
    if (ctx is Context) {
      return this._call_context(ctx);
    }
    else if (ctx is Map) {
      return this._call_dict(ctx);
    }
  }

  Map _call_dict(Map ctx) {
    Context meta_ctx = Context(this._dll_handle, nullptr);
    meta_ctx.placeValues(ctx);

    Context outContext = _call_context(meta_ctx);
    dynamic outMap = get_output_data(outContext);

    outContext.dispose();

    return outMap;
  }

  Context _call_context(Context ctx) {
    final exception = _getException();
    final constructor = _dll_handle.lookupFunction<
        _ProcessingBlock_processContext_c,
        _ProcessingBlock_processContext_dart>
      ('TDVProcessingBlock_processContext');
    constructor(this._impl, ctx._impl, exception);

    tdvCheckException(exception, _dll_handle);

    return ctx;
  }

  dynamic get_output_data(Context ctx) {
    if (ctx.is_array()) {
      List res = [];
      for (int i = 0; i < ctx.len(); i++) {
        res.add(this.get_output_data(ctx[i]));
      }
      return res;
    }
    if (ctx.is_object()) {
      Map res = {};
      for (final key in ctx.getKeys()) {
        res[key] = this.get_output_data(ctx[key]);
      }
      return res;
    }
    return ctx.get_value();
  }
}