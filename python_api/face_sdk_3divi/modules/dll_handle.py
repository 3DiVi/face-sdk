from ctypes import CDLL
from ctypes import c_void_p, c_char_p, POINTER
from ctypes import c_uint32, c_int64, c_int32, c_float
from ctypes import c_double, c_bool, c_long, c_ulong


class DllHandle:
    __namespace = '__4848a76477c449608aa5deb15c5495e4_facerec_v3_'
    __contextNamespace = 'TDVContext_'
    __exceptionNamcepsace = 'TDVException_'

    def __init__(self, dll_handle: CDLL):
        self.__dll_handle = dll_handle

    def apiException_what(self, *args, **kwargs):
        func = self.__dll_handle['{}apiException_what'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def apiException_code(self, *args, **kwargs):
        func = self.__dll_handle['{}apiException_code'.format(self.__namespace)]
        func.restype = c_uint32

        return func(*args, **kwargs)

    def apiProcessingBlockException_what(self, *args, **kwargs):
        func = self.__dll_handle['{}getMessage'.format(self.__exceptionNamcepsace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def apiProcessingBlockException_code(self, *args, **kwargs):
        func = self.__dll_handle['{}getErrorCode'.format(self.__exceptionNamcepsace)]
        func.restype = c_uint32

    def apiProcessingBlockObject_destructor(self, *args, **kwargs):
        self.__dll_handle['{}deleteException'.format(self.__exceptionNamcepsace)](*args, **kwargs)

    def apiObject_destructor(self, *args, **kwargs):
        self.__dll_handle['{}apiObject_destructor'.format(self.__namespace)](*args, **kwargs)

    def StructStorage_get_int64(self, *args, **kwargs):
        func = self.__dll_handle['{}StructStorage_get_int64'.format(self.__namespace)]
        func.restype = c_int64

        return func(*args, **kwargs)

    def StructStorage_get_double(self, *args, **kwargs):
        func = self.__dll_handle['{}StructStorage_get_double'.format(self.__namespace)]
        func.restype = c_double

        return func(*args, **kwargs)

    def StructStorage_get_pointer(self, *args, **kwargs):
        func = self.__dll_handle['{}StructStorage_get_pointer'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createCapturerE(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createCapturerE'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createAgeGenderEstimator(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createAgeGenderEstimator'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createEmotionsEstimator(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createEmotionsEstimator'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createFaceQualityEstimator(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createFaceQualityEstimator'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createQualityEstimator(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createQualityEstimator'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createDepthLivenessEstimatorE(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createDepthLivenessEstimatorE'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createIRLivenessEstimatorE(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createIRLivenessEstimatorE'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createLiveness2DEstimatorE(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createLiveness2DEstimatorE'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_ProcessingBlock_createProcessingBlock(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_ProcessingBlock_createProcessingBlock'.format(self.__namespace)]
        func.restype = c_void_p
        return func(*args, **kwargs)

    def TDVFaceAttributesEstimator_createByConfig(self, *args, **kwargs):
        func = self.__dll_handle['TDVFaceAttributesEstimator_createByConfig']
        func.restype = c_void_p

        return func(*args, **kwargs)

    def TDVProcessingBlock_destroy(self, *args, **kwargs):
        self.__dll_handle['TDVProcessingBlock_destroy'](*args, **kwargs)

    def TDVProcessingBlock_destroyBlock(self, *args, **kwargs):
        self.__dll_handle['TDVProcessingBlock_destroyBlock'](*args, **kwargs)

    def TDVProcessingBlock_processSparse(self, *args, **kwargs):
        func = self.__dll_handle['TDVProcessingBlock_processSparse']
        func.restype = c_void_p

        return func(*args, **kwargs)

    def TDVProcessingBlock_processContext(self, *args, **kwargs):
        func = self.__dll_handle['TDVProcessingBlock_processContext']
        func.restype = c_void_p

        return func(*args, **kwargs)


    def TDVProcessingBlock_getException(self, *args, **kwargs):
        func = self.__dll_handle['TDVProcessingBlock_getException']
        func.restype = c_void_p

        return func(*args, **kwargs)

    def tdvFreeStr(self, *args, **kwargs):
        func = self.__dll_handle['tdvFreeStr']
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createRecognizer2(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createRecognizer2'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_loadRawSample(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_loadRawSample'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createVideoWorker(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createVideoWorker'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_createVideoWorkerStiAgeGenderEmotions(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_createVideoWorker_sti_age_gender_emotions'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def FacerecService_loadRawSampleWithoutImage_encoded_image(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_loadRawSampleWithoutImage_encoded_image'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def Capturer_capture_encoded_image(self, *args, **kwargs):
        self.__dll_handle['{}Capturer_capture_encoded_image'.format(self.__namespace)](*args, **kwargs)

    def Capturer_manualCapture_encoded_image_eyes_points(self, *args, **kwargs):
        func = self.__dll_handle['{}Capturer_manualCapture_encoded_image_eyes_points'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def Capturer_getType(self, *args, **kwargs):
        func = self.__dll_handle['{}Capturer_getType'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def Capturer_resetHistory(self, *args, **kwargs):
        self.__dll_handle['{}Capturer_resetHistory'.format(self.__namespace)](*args, **kwargs)

    def Capturer_setParameter(self, *args, **kwargs):
        self.__dll_handle['{}Capturer_setParameter'.format(self.__namespace)](*args, **kwargs)

    def EmotionsEstimator_estimateEmotions(self, *args, **kwargs):
        func = self.__dll_handle['{}EmotionsEstimator_estimateEmotions'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def AgeGenderEstimator_estimateAgeGender(self, *args, **kwargs):
        self.__dll_handle['{}AgeGenderEstimator_estimateAgeGender'.format(self.__namespace)](*args, **kwargs)

    def FaceQualityEstimator_estimateQuality(self, *args, **kwargs):
        func = self.__dll_handle['{}FaceQualityEstimator_estimateQuality'.format(self.__namespace)]
        func.restype = c_float

        return func(*args, **kwargs)

    def QualityEstimator_estimateQuality(self, *args, **kwargs):
        self.__dll_handle['{}QualityEstimator_estimateQuality'.format(self.__namespace)](*args, **kwargs)

    def DepthLivenessEstimator_estimateLiveness(self, *args, **kwargs):
        func = self.__dll_handle['{}DepthLivenessEstimator_estimateLiveness'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def IRLivenessEstimator_estimateLiveness(self, *args, **kwargs):
        func = self.__dll_handle['{}IRLivenessEstimator_estimateLiveness'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def Liveness2DEstimator_estimateLiveness(self, *args, **kwargs):
        func = self.__dll_handle['{}Liveness2DEstimator_estimateLiveness'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def RawSample_getRectangle(self, *args, **kwargs):
        self.__dll_handle['{}RawSample_getRectangle'.format(self.__namespace)](*args, **kwargs)

    def RawSample_getLandmarks(self, *args, **kwargs):
        self.__dll_handle['{}RawSample_getLandmarks'.format(self.__namespace)](*args, **kwargs)

    def RawSample_getIrisLandmarks(self, *args, **kwargs):
        self.__dll_handle['{}RawSample_getIrisLandmarks'.format(self.__namespace)](*args, **kwargs)

    def RawSample_getLeftEye(self, *args, **kwargs):
        self.__dll_handle['{}RawSample_getLeftEye'.format(self.__namespace)](*args, **kwargs)

    def RawSample_getRightEye(self, *args, **kwargs):
        self.__dll_handle['{}RawSample_getRightEye'.format(self.__namespace)](*args, **kwargs)

    def RawSample_getAngles(self, *args, **kwargs):
        self.__dll_handle['{}RawSample_getAngles'.format(self.__namespace)](*args, **kwargs)

    def RawSample_getID(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_getID'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def RawSample_getType(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_getType'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def RawSample_downscaleToPreferredSize(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_downscaleToPreferredSize'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def RawSample_getFaceCutRectangle(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_getFaceCutRectangle'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def RawSample_save(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_save'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def RawSample_save_without_image(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_save_without_image'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def RawSample_getFrameID(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_getFrameID'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def RawSample_hasOriginalImage(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_hasOriginalImage'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def RawSample_getScore(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_getScore'.format(self.__namespace)]
        func.restype = c_float

        return func(*args, **kwargs)

    def RawSample_getFaceVisibilityScore(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_getFaceVisibilityScore'.format(self.__namespace)]
        func.restype = c_float

        return func(*args, **kwargs)

    def RawSample_getOriginalImage(self, *args, **kwargs):
        self.__dll_handle['{}RawSample_getOriginalImage'.format(self.__namespace)](*args, **kwargs)

    def RawSample_cutFaceImage(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_cutFaceImage'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def RawSample_cutFaceImage_original_raw_image(self, *args, **kwargs):
        func = self.__dll_handle['{}RawSample_cutFaceImage_original_raw_image'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    # Recognizer methods
    def Recognizer_processing(self, *args, **kwargs):
        func = self.__dll_handle['{}Recognizer_processing'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def Recognizer_verifyMatch(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_verifyMatch'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_verifyMatch_v2(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_verifyMatch_v2'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_getROCCurvePointByDistanceThreshold_v2(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_getROCCurvePointByDistanceThreshold_v2'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_getROCCurvePointByFAR_v2(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_getROCCurvePointByFAR_v2'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_getROCCurvePointByFRR_v2(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_getROCCurvePointByFRR_v2'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_getROCCurvePointByScoreThreshold(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_getROCCurvePointByScoreThreshold'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_createIndex(self, *args, **kwargs):
        func = self.__dll_handle['{}Recognizer_createIndex'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def Recognizer_getMethodName(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_getMethodName'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_search(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_search'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_search_v2(self, *args, **kwargs):
        self.__dll_handle['{}Recognizer_search_v2'.format(self.__namespace)](*args, **kwargs)

    def Recognizer_loadTemplate(self, *args, **kwargs):
        func = self.__dll_handle['{}Recognizer_loadTemplate'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def Recognizer_chooseRepresentativeTemplatesSet(self, *args, **kwargs):
        func = self.__dll_handle['{}Recognizer_chooseRepresentativeTemplatesSet'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    # Template method
    def Template_getMethodName(self, *args, **kwargs):
        self.__dll_handle['{}Template_getMethodName'.format(self.__namespace)](*args, **kwargs)

    def Template_save(self, *args, **kwargs):
        self.__dll_handle['{}Template_save'.format(self.__namespace)](*args, **kwargs)

    # TemplatesIndex method
    def TemplatesIndex_getMethodName(self, *args, **kwargs):
        self.__dll_handle['{}TemplatesIndex_getMethodName'.format(self.__namespace)](*args, **kwargs)

    def TemplatesIndex_size(self, *args, **kwargs):
        func = self.__dll_handle['{}TemplatesIndex_size'.format(self.__namespace)]
        func.restype = c_int64

        return func(*args, **kwargs)

    def TemplatesIndex_reserveSearchMemory(self, *args, **kwargs):
        self.__dll_handle['{}TemplatesIndex_reserveSearchMemory'.format(self.__namespace)](*args, **kwargs)

    def TemplatesIndex_at(self, *args, **kwargs):
        func = self.__dll_handle['{}TemplatesIndex_at'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    # VideoWorker methods
    def VideoWorker_setThisVW(self, *args, **kwargs):
        self.__dll_handle['{}VideoWorker_setThisVW'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_addVideoFrameWithTimestamp(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addVideoFrameWithTimestamp'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_addVideoFrameWithTimestamp_with_crop(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addVideoFrameWithTimestamp_with_crop'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_addDepthFrame(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addDepthFrame'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def VideoWorker_addIRFrame(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addIRFrame'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def VideoWorker_checkExceptions(self, *args, **kwargs):
        self.__dll_handle['{}VideoWorker_checkExceptions'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_resetTrackerOnStream(self, *args, **kwargs):
        self.__dll_handle['{}VideoWorker_resetTrackerOnStream'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_resetStream(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_resetStream'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_addTrackingCallbackU(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addTrackingCallbackU'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_addTemplateCreatedCallbackU(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addTemplateCreatedCallbackU'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_addMatchFoundCallbackU(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addMatchFoundCallbackU'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_addMatchFoundCallbackExtU(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addMatchFoundCallbackExt'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_addTrackingLostCallbackU(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addTrackingLostCallbackU'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_addStiPersonOutdatedCallbackU(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_addStiPersonOutdatedCallbackU'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_removeTrackingCallback(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_removeTrackingCallback'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_removeTrackingLostCallback(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_removeTrackingLostCallback'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_removeTemplateCreatedCallback(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_removeTemplateCreatedCallback'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_removeMatchFoundCallback(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_removeMatchFoundCallback'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_removeStiPersonOutdatedCallback(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_removeStiPersonOutdatedCallback'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_setDatabase(self, *args, **kwargs):
        self.__dll_handle['{}VideoWorker_setDatabase'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_getTrackingConveyorSize(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_getTrackingConveyorSize'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_getMethodName(self, *args, **kwargs):
        self.__dll_handle['{}VideoWorker_getMethodName'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_getStreamsCount(self, *args, **kwargs):
        func = self.__dll_handle['{}VideoWorker_getStreamsCount'.format(self.__namespace)]
        func.restype = c_int32

        return func(*args, **kwargs)

    def VideoWorker_errStreamWriteFunc(self, *args, **kwargs):
        self.__dll_handle['{}VideoWorker_errStreamWriteFunc'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_toggleSomething(self, *args, **kwargs):
        self.__dll_handle['{}VideoWorker_toggleSomething'.format(self.__namespace)](*args, **kwargs)

    def VideoWorker_setParameter(self, *args, **kwargs):
        self.__dll_handle['{}VideoWorker_setParameter'.format(self.__namespace)](*args, **kwargs)

    def get_version(self, *args, **kwargs):
        self.__dll_handle['{}get_version'.format(self.__namespace)](*args, **kwargs)

    def FacerecService_constructor2(self, *args, **kwargs):
        func = self.__dll_handle['{}FacerecService_constructor2'.format(self.__namespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)


    def create(self, *args, **kwargs):
        func = self.__dll_handle['{}create'.format(self.__contextNamespace)]
        func.restype = c_void_p

        return func(*args, **kwargs)

    def destroy(self, *args, **kwargs):
        self.__dll_handle['{}destroy'.format(self.__contextNamespace)](*args, **kwargs)

    def freePtr(self, *args, **kwargs):
        self.__dll_handle['{}freePtr'.format(self.__contextNamespace)](*args, **kwargs)


    def copy(self, *args, **kwargs):
        self.__dll_handle['{}copy'.format(self.__contextNamespace)](*args, **kwargs)

    def clone(self, *args, **kwargs):
        func = self.__dll_handle['{}clone'.format(self.__contextNamespace)]
        func.restype = c_void_p
        return func(*args, **kwargs)


    def putStr(self, *args, **kwargs):
        self.__dll_handle['{}putStr'.format(self.__contextNamespace)](*args, **kwargs)

    def putDouble(self, *args, **kwargs):
        self.__dll_handle['{}putDouble'.format(self.__contextNamespace)](*args, **kwargs)

    def putLong(self, *args, **kwargs):
        self.__dll_handle['{}putLong'.format(self.__contextNamespace)](*args, **kwargs)

    def putBool(self, *args, **kwargs):
        self.__dll_handle['{}putBool'.format(self.__contextNamespace)](*args, **kwargs)

    def putDataPtr(self, *args, **kwargs):
        func = self.__dll_handle['{}putDataPtr'.format(self.__contextNamespace)]
        func.restype = c_char_p
        return func(*args, **kwargs)

    def pushBack(self, *args, **kwargs):
        self.__dll_handle['{}pushBack'.format(self.__contextNamespace)](*args, **kwargs)



    def getDouble(self, *args, **kwargs):
        func = self.__dll_handle['{}getDouble'.format(self.__contextNamespace)]
        func.restype = c_double
        return func(*args, **kwargs)

    def getLength(self, *args, **kwargs):
        func = self.__dll_handle['{}getLength'.format(self.__contextNamespace)]
        func.restype = c_ulong
        return func(*args, **kwargs)

    def getKeys(self, *args, **kwargs):
        func = self.__dll_handle['{}getKeys'.format(self.__contextNamespace)]
        func.restype = POINTER(c_char_p)
        return func(*args, **kwargs)

    def getStr(self, *args, **kwargs):
        func = self.__dll_handle['{}getStr'.format(self.__contextNamespace)]
        func.restype = c_char_p
        return func(*args, **kwargs)

    def getStrSize(self, *args, **kwargs):
        func = self.__dll_handle['{}getStrSize'.format(self.__contextNamespace)]
        func.restype = c_ulong
        return func(*args, **kwargs)

    def getLong(self, *args, **kwargs):
        func = self.__dll_handle['{}getLong'.format(self.__contextNamespace)]
        func.restype = c_long
        return func(*args, **kwargs)

    def getBool(self, *args, **kwargs):
        func = self.__dll_handle['{}getBool'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)

    def getDataPtr(self, *args, **kwargs):
        func = self.__dll_handle['{}getDataPtr'.format(self.__contextNamespace)]
        func.restype = c_char_p
        return func(*args, **kwargs)


    def getByIndex(self, *args, **kwargs):
        func = self.__dll_handle['{}getByIndex'.format(self.__contextNamespace)]
        func.restype = c_void_p
        return func(*args, **kwargs)

    def getByKey(self, *args, **kwargs):
        func = self.__dll_handle['{}getByKey'.format(self.__contextNamespace)]
        func.restype = c_void_p
        return func(*args, **kwargs)

    def getOrInsertByKey(self, *args, **kwargs):
        func = self.__dll_handle['{}getOrInsertByKey'.format(self.__contextNamespace)]
        func.restype = c_void_p
        return func(*args, **kwargs)

    def isNone(self, *args, **kwargs):
        func = self.__dll_handle['{}isNone'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)

    def isArray(self, *args, **kwargs):
        func = self.__dll_handle['{}isArray'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)

    def isObject(self, *args, **kwargs):
        func = self.__dll_handle['{}isObject'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)

    def isBool(self, *args, **kwargs):
        func = self.__dll_handle['{}isBool'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)

    def isLong(self, *args, **kwargs):
        func = self.__dll_handle['{}isLong'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)

    def isDouble(self, *args, **kwargs):
        func = self.__dll_handle['{}isDouble'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)

    def isString(self, *args, **kwargs):
        func = self.__dll_handle['{}isString'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)

    def isDataPtr(self, *args, **kwargs):
        func = self.__dll_handle['{}isDataPtr'.format(self.__contextNamespace)]
        func.restype = c_bool
        return func(*args, **kwargs)
