part of face_sdk_3divi;

class EyesOpennessResult {
  bool leftEyeVerdict;
  bool rightEyeVerdict;

  double leftEyeScore;
  double rightEyeScore;

  EyesOpennessResult(this.leftEyeVerdict, this.rightEyeVerdict, this.leftEyeScore, this.rightEyeScore);
}

class FaceAttributesEstimator extends _ComplexObject {
  FaceAttributesEstimator(DynamicLibrary dll_handle, Pointer<Void> impl) :
        super(dll_handle, impl);

  /// Create a template from detected face [sample] (from Capturer of VideoWorker).
  EyesOpennessResult estimateEyesOpenness(final RawSample sample){
    final process = _dll_handle.lookupFunction<_FaceAttributesEstimator_estimateEyesOpenness, _FaceAttributesEstimator_estimateEyesOpenness>
      (_c_namespace + 'FaceAttributesEstimator_estimateEyesOpenness');

    Pointer<Int32> leftVerdictPtr = malloc.allocate<Int32>(sizeOf<Pointer<Int32>>());
    Pointer<Int32> rightVerdictPtr = malloc.allocate<Int32>(sizeOf<Pointer<Int32>>());
    Pointer<Float> leftScorePtr = malloc.allocate<Float>(sizeOf<Pointer<Float>>());
    Pointer<Float> rightScorePtr = malloc.allocate<Float>(sizeOf<Pointer<Float>>());

    final exception = _getException();
    process(
        _impl,
        sample._impl,
        leftVerdictPtr,
        rightVerdictPtr,
        leftScorePtr,
        rightScorePtr,
        exception);

    checkException(exception, _dll_handle);

    int leftVerdictValue = leftVerdictPtr.value;
    int rightVerdictValue = rightVerdictPtr.value;

    double leftScoreValue = leftScorePtr.value;
    double rightScoreValue = rightScorePtr.value;

    malloc.free(leftVerdictPtr);
    malloc.free(rightVerdictPtr);
    malloc.free(leftScorePtr);
    malloc.free(rightScorePtr);

    return EyesOpennessResult(leftVerdictValue == 1, rightVerdictValue == 1, leftScoreValue, rightScoreValue);
  }
}