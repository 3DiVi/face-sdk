part of face_sdk_3divi;

///Estimated liveness.
enum LivenessVerdict {
  ///Not enough information.
  NotEnoughData,

  ///The observed face belongs to a real person.
  Real,

  ///The observed face is taken from a photo.
  Fake,

  ///The observed face was not checked.
  NotComputed
}

class LivenessResult {
  /// Liveness verdict
  int verdict = -1;

  /// Liveness (?) score
  double score = -1;

  LivenessResult(this.verdict, this.score);

  LivenessVerdict get livenessVerdict {
    if (verdict == -1)
      return LivenessVerdict.NotEnoughData;
    else
      return LivenessVerdict.values[verdict];
  }

  String toString() {
    String intrepretable_verdict = '';
    if (verdict == 0)
      intrepretable_verdict = "Not enough data";
    else if (verdict == 1)
      intrepretable_verdict = "Real";
    else if (verdict == 2)
      intrepretable_verdict = "Fake";
    else if (verdict == 3) intrepretable_verdict = "Not computed";

    return "$intrepretable_verdict, Score: ${score.toStringAsFixed(2)}";
  }
}

/// Interface object for liveness evaluation.
class Liveness2DEstimator extends _ComplexObject {
  Liveness2DEstimator(DynamicLibrary dll_handle, Pointer<Void> impl) : super(dll_handle, impl);

  /// Create a template from detected face [sample] (from Capturer of VideoWorker).
  LivenessResult estimateLiveness(final RawSample sample) {
    final process = _dll_handle
        .lookupFunction<_Liveness2DEstimator_estimateLiveness, _Liveness2DEstimator_estimateLiveness>(_c_namespace + 'Liveness2DEstimator_estimateLiveness');
    Pointer<Int32> verdictPtr = malloc.allocate<Int32>(sizeOf<Pointer<Int32>>());
    Pointer<Float> scorePtr = malloc.allocate<Float>(sizeOf<Pointer<Float>>());

    final exception = _getException();
    process(_impl, sample._impl, verdictPtr, scorePtr, exception);

    checkException(exception, _dll_handle);

    int verdictVal = verdictPtr.value;
    double scoreVal = scorePtr.value;

    malloc.free(verdictPtr);
    malloc.free(scorePtr);

    return LivenessResult(verdictVal, scoreVal);
  }
}
