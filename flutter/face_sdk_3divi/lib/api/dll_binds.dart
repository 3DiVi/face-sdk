part of face_sdk_3divi;


const String _c_namespace = "__4848a76477c449608aa5deb15c5495e4_facerec_v3_";
const String _context_namespace = "TDVContext_";

typedef _facerecConstructor = Pointer<Void> Function(Pointer<Utf8>, Pointer<Utf8>, Pointer<Utf8>, Pointer<Pointer<Void>>);

typedef _exceptionCode_c = Uint32 Function(Pointer<Void>);
typedef _exceptionCode_dart = int Function(Pointer<Void>);

typedef _exceptionWhat = Pointer<Utf8> Function(Pointer<Void>);


//    VideoWorker Interface

typedef _VWConstructor_c = Pointer<Void> Function(
    Pointer<Void>, // service

    Pointer<Void>, // trackingCallback
    Pointer<Void>, // templateCreatedCallback
    Pointer<Void>, // matchFoundCallback
    Pointer<Void>, // trackingLostCallback
    Pointer<Void>, // stiPersonOutdatedCallback

    Pointer<Utf8>, // video_worker_ini_file
    Int32, // vw_overridden_count
    Pointer<Pointer<Utf8>>, // vw_overridden_keys
    Pointer<Double>, // vw_overridden_values

    Pointer<Utf8>, // recognizer_ini_file
    Int32, // rec_overridden_count
    Pointer<Pointer<Utf8>>, // rec_overridden_keys
    Pointer<Double>, // rec_overridden_values

    Int32, // streams_count
    Int32, // processing_threads_count
    Int32, // matching_threads_count

    Uint32, // short_time_identification_enabled
    Float, // short_time_identification_distance_threshold
    Float, // short_time_identification_outdate_time_seconds

    Int32, // age_gender_threads_count
    Int32, // emotions_threads_count

    Pointer<Pointer<Void>> /*out_exception*/);

typedef _VWConstructor_dart = Pointer<Void> Function(
    Pointer<Void>, // service

    Pointer<Void>, // trackingCallback
    Pointer<Void>, // templateCreatedCallback
    Pointer<Void>, // matchFoundCallback
    Pointer<Void>, // trackingLostCallback
    Pointer<Void>, // stiPersonOutdatedCallback

    Pointer<Utf8>, // video_worker_ini_file
    int, // vw_overridden_count
    Pointer<Pointer<Utf8>>, // vw_overridden_keys
    Pointer<Double>, // vw_overridden_values

    Pointer<Utf8>, // recognizer_ini_file
    int, // rec_overridden_count
    Pointer<Pointer<Utf8>>, // rec_overridden_keys
    Pointer<Double>, // rec_overridden_values

    int, // streams_count
    int, // processing_threads_count
    int, // matching_threads_count

    int, // short_time_identification_enabled
    double, // short_time_identification_distance_threshold
    double, // short_time_identification_outdate_time_seconds

    int, // age_gender_threads_count
    int, // emotions_threads_count

    Pointer<Pointer<Void>> /*out_exception*/);

typedef _VWSetThisVW_c = Void Function(Pointer<Void>, Pointer<Void>, Pointer<Pointer<Void>>);
typedef _VWSetThisVW_dart = void Function(Pointer<Void>, Pointer<Void>, Pointer<Pointer<Void>>);

typedef _VWPollResults_c = Pointer<Void> Function(Pointer<Void>, Int32, Pointer<Pointer<Void>>);
typedef _VWPollResults_dart = Pointer<Void> Function(Pointer<Void>, int, Pointer<Pointer<Void>>);

typedef _VWaddVideoFrame_c = Int32 Function(
    Pointer<Void>, // video_worker
    Pointer<Void>, // image_data
    Int32, // image_width
    Int32, // image_height
    Int32, // image_format
    Int32, // image_with_crop
    Int32, // image_crop_info_offset_x
    Int32, // image_crop_info_offset_y
    Int32, // image_crop_info_data_image_width
    Int32, // image_crop_info_data_image_height
    Int32, // stream_id
    Uint64, // timestamp_microsec
    Pointer<Pointer<Void>>); // out_exception

typedef _VWaddVideoFrame_dart = int Function(
    Pointer<Void>, // video_worker
    Pointer<Void>, // image_data
    int, // image_width
    int, // image_height
    int, // image_format
    int, // image_with_crop
    int, // image_crop_info_offset_x
    int, // image_crop_info_offset_y
    int, // image_crop_info_data_image_width
    int, // image_crop_info_data_image_height
    int, // stream_id
    int, // timestamp_microsec
    Pointer<Pointer<Void>>); // out_exception

typedef _VWresetTrackerOnStream_c = Void Function(Pointer<Void>, Int32, Pointer<Pointer<Void>>);
typedef _VWresetTrackerOnStream_dart = void Function(Pointer<Void>, int, Pointer<Pointer<Void>>);

typedef _VWsetDataBase = Void Function(
    Pointer<Void>, // video_worker
    Int32, // acceleration
    Int32, // elements_count
    Pointer<Uint64>, // elements_ids
    Pointer<Uint64>, // persons_ids
    Pointer<Pointer<Void>>, // elements_templates
    Pointer<Float>, // elements_thresholds
    Pointer<Pointer<Void>>); // out_exception

typedef _VWsetDataBase_dart = void Function(
    Pointer<Void>, // video_worker
    int, // acceleration
    int, // elements_count
    Pointer<Uint64>, // elements_ids
    Pointer<Uint64>, // persons_ids
    Pointer<Pointer<Void>>, // elements_templates
    Pointer<Float>, // elements_thresholds
    Pointer<Pointer<Void>>);


//    Capturer Interface

typedef _CapConstr_c = Pointer<Void> Function(
    Pointer<Void>, // service
    Pointer<Utf8>, // ini_file
    Int32, // overridden_count
    Pointer<Pointer<Utf8>>, // overridden_keys
    Pointer<Double>, // overridden_values
    Pointer<Pointer<Void>>); // out_exception

typedef _CapConstr_dart = Pointer<Void> Function(
    Pointer<Void>, // service
    Pointer<Utf8>, // ini_file
    int, // overridden_count
    Pointer<Pointer<Utf8>>, // overridden_keys
    Pointer<Double>, // overridden_values
    Pointer<Pointer<Void>>); // out_exception

typedef _assign_pointer_vect_fu = Void Function(
    Pointer<Void>, // pointers_vector
    Pointer<Pointer<Void>>, // elements
    Int32); // elements_count

typedef _assign_float_vect_fu = Void Function(
    Pointer<Void>,
    Pointer<Float>,
    Int32);

typedef _CapCap_c = Void Function(
    Pointer<Void>, // capturer
    Pointer<Void>, // image_data
    Int32, // image_width
    Int32, // image_height
    Int32, // image_format
    Int32, // image_with_crop
    Int32, // image_crop_info_offset_x
    Int32, // image_crop_info_offset_y
    Int32, // image_crop_info_data_image_width
    Int32, // image_crop_info_data_image_height
    Pointer<Void>, // result_pointers_vector
    Pointer<NativeFunction<_assign_pointer_vect_fu>>, // assign_pointers_vector_func
    Pointer<Pointer<Void>>); // out_exception

typedef _CapCap_dart = void Function(
    Pointer<Void>, // capturer
    Pointer<Void>, // image_data
    int, // image_width
    int, // image_height
    int, // image_format
    int, // image_with_crop
    int, // image_crop_info_offset_x
    int, // image_crop_info_offset_y
    int, // image_crop_info_data_image_width
    int, // image_crop_info_data_image_height
    Pointer<Void>, // result_pointers_vector
    Pointer<NativeFunction<_assign_pointer_vect_fu>>, // assign_pointers_vector_func
    Pointer<Pointer<Void>>); // out_exception

typedef _CapturerCapBytes_c = Void Function(
    Pointer<Void>, // capturer
    Pointer<Utf8>, // data
    Int32, // data_size
    Pointer<Void>, // result_pointers_vector
    Pointer<NativeFunction<_assign_pointer_vect_fu>>, // assign_pointers_vector_func
    Pointer<Pointer<Void>>); // out_exception

typedef _CapturerCapBytes_dart = void Function(
    Pointer<Void>, // capturer
    Pointer<Utf8>, // data
    int, // data_size
    Pointer<Void>, // result_pointers_vector
    Pointer<NativeFunction<_assign_pointer_vect_fu>>, // assign_pointers_vector_func
    Pointer<Pointer<Void>>); // out_exception

//    Recognizer Interface

typedef _RecognizerConstr_c = Pointer<Void> Function(
    Pointer<Void>, // service
    Pointer<Utf8>, // ini_file
    Int32, // overridden_count
    Pointer<Pointer<Utf8>>, // overridden_keys
    Pointer<Double>, // overridden_values
    Int32, // processing
    Int32, // matching
    Int32, // processing_less_memory_consumption
    Pointer<Pointer<Void>>); // out_exception

typedef _RecognizerConstr_dart = Pointer<Void> Function(
    Pointer<Void>, // service
    Pointer<Utf8>, // ini_file
    int, // overridden_count
    Pointer<Pointer<Utf8>>, // overridden_keys
    Pointer<Double>, // overridden_values
    int, // processing
    int, // matching
    int, // processing_less_memory_consumption
    Pointer<Pointer<Void>>); // out_exception

typedef _RecognizerProcessing = Pointer<Void> Function(
    Pointer<Void>, // recognizer
    Pointer<Void>, // raw_sample
    Pointer<Pointer<Void>>); // out_exception

typedef _Recognizer_verifyMatch_v2_c = Void Function(
    Pointer<Void>, // recognizer
    Pointer<Void>, // template1
    Pointer<Void>, // template2
    Pointer<Double>, // result_distance
    Pointer<Double>, // result_fa_r
    Pointer<Double>, // result_fr_r
    Pointer<Double>, // result_score
    Pointer<Pointer<Void>>); // out_exception

typedef _Recognizer_verifyMatch_v2_dart = void Function(
    Pointer<Void>, // recognizer
    Pointer<Void>, // template1
    Pointer<Void>, // template2
    Pointer<Double>, // result_distance
    Pointer<Double>, // result_fa_r
    Pointer<Double>, // result_fr_r
    Pointer<Double>, // result_score
    Pointer<Pointer<Void>>); // out_exception

//begin context

typedef _Context_Construct_c = Pointer<Void> Function(
    Pointer<Pointer<Void>>);

typedef _Context_Constructor_dart = Pointer<Void> Function(
    Pointer<Pointer<Void>>);

typedef _Context_Destruct_c = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_Destructor_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_CreateFromEncodedImage_c = Pointer<Void> Function(
    Pointer<Uint8>,
    Uint64,
    Pointer<Pointer<Void>>);

typedef _Context_CreateFromEncodedImage_dart = Pointer<Void> Function(
    Pointer<Uint8>,
    int,
    Pointer<Pointer<Void>>);

typedef _Context_CreateFromFrame_c = Pointer<Void> Function(
    Pointer<Uint8>,
    Int32,
    Int32,
    Int32,
    Int32,
    Pointer<Pointer<Void>>);

typedef _Context_CreateFromFrame_dart = Pointer<Void> Function(
    Pointer<Uint8>,
    int,
    int,
    int,
    int,
    Pointer<Pointer<Void>>);

typedef _Context_CreateFromJsonFile_c = Pointer<Void> Function(
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_CreateFromJsonFile_dart = Pointer<Void> Function(
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_saveToJsonFile_c = Void Function(
    Pointer<Void>, // context
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_saveToJsonFile_dart = void Function(
    Pointer<Void>, // context
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_serializeToJson_c = Pointer<Utf8> Function(
    Pointer<Void>, // context
    Pointer<Pointer<Void>>);

typedef _Context_serializeToJson_dart = Pointer<Utf8> Function(
    Pointer<Void>, // context
    Pointer<Pointer<Void>>);

typedef _Context_getOrInsertByKey_c = Pointer<Void> Function(
    Pointer<Void>, // context
    Pointer<Utf8>, //
    Pointer<Pointer<Void>>);

typedef _Context_getOrInsertByKey_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_pushBack_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Bool,
    Pointer<Pointer<Void>>);

typedef _Context_pushBack_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    bool,
    Pointer<Pointer<Void>>);

typedef _Context_copy_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_copy_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_clear_c = Void Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_clear_dart = void Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_erase_c = Void Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_erase_dart = void Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_reserve_c = Void Function(
    Pointer<Void>,
    Int64,
    Pointer<Pointer<Void>>);

typedef _Context_reserve_dart = void Function(
    Pointer<Void>,
    int,
    Pointer<Pointer<Void>>);

typedef _Context_contains_c = Bool Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_contains_dart = bool Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_compare_c = Bool Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_compare_dart = bool Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);


typedef _Context_putStr_c = Void Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_putStr_dart = void Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_putLong_c = Void Function(
    Pointer<Void>,
    Int64,
    Pointer<Pointer<Void>>);

typedef _Context_putLong_dart = void Function(
    Pointer<Void>,
    int,
    Pointer<Pointer<Void>>);

typedef _Context_putDouble_c = Void Function(
    Pointer<Void>,
    Double,
    Pointer<Pointer<Void>>);

typedef _Context_putDouble_dart = void Function(
  Pointer<Void>,
  double,
  Pointer<Pointer<Void>>);

typedef _Context_putBool_c = Void Function(
    Pointer<Void>,
    Bool,
    Pointer<Pointer<Void>>);

typedef _Context_putBool_dart = void Function(
    Pointer<Void>,
    bool,
    Pointer<Pointer<Void>>);

typedef _Context_putContextTemplate_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_putContextTemplate_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_putDynamicTemplateIndex_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_putDynamicTemplateIndex_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_putDataPtr_c = Pointer<Utf8> Function(
    Pointer<Void>,
    Pointer<Uint8>,
    Uint64,
    Pointer<Pointer<Void>>);

typedef _Context_putDataPtr_dart = Pointer<Utf8> Function(
    Pointer<Void>,
    Pointer<Uint8>,
    int,
    Pointer<Pointer<Void>>);

typedef _Context_getBool_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getBool_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getStrSize_c = Uint64 Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getStrSize_dart = int Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getStr_c = Pointer<Utf8> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getStr_dart = Pointer<Utf8> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getLong_c = Int64 Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getLong_dart = int Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getDouble_c = Double Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getDouble_dart = double Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getDataPtr_c = Pointer<Uint8> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getDataPtr_dart = Pointer<Uint8> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getContextTemplate_c = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getContextTemplate_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getDynamicTemplateIndex_c = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getDynamicTemplateIndex_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isNone_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isNone_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isArray_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isArray_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isObject_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isObject_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isBool_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isBool_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isLong_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isLong_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isDouble_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isDouble_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isString_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isString_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isDataPtr_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isDataPtr_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isContextTemplate_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isContextTemplate_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isDynamicTemplateIndex_c = Bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_isDynamicTemplateIndex_dart = bool Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getLength_c = Uint64 Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getLength_dart = int Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _Context_getByIndex_c = Pointer<Void> Function(
    Pointer<Void>,
    Int32,
    Pointer<Pointer<Void>>);

typedef _Context_getByIndex_dart = Pointer<Void> Function(
    Pointer<Void>,
    int,
    Pointer<Pointer<Void>>);

typedef _Context_getKeys_c = Pointer<Pointer<Utf8>> Function(
    Pointer<Void>,
    Uint64,
    Pointer<Pointer<Void>>);

typedef _Context_getKeys_dart = Pointer<Pointer<Utf8>> Function(
    Pointer<Void>,
    int,
    Pointer<Pointer<Void>>);

typedef _Context_getByKey_c = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _Context_getByKey_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

//end context

//Processing block

typedef _ProcessingBlock_createProcessingBlock_c = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _ProcessingBlock_createProcessingBlock_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _ProcessingBlock_destroyProcessingBlock_c = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _ProcessingBlock_destroyProcessingBlock_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _ProcessingBlock_processContext_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _ProcessingBlock_processContext_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

//end PB

// LivenessEstimator

typedef _Liveness2DEstimatorConstr_c = Pointer<Void> Function(
    Pointer<Void>, // service
    Pointer<Utf8>, // ini_file
    Int32, // overridden_count
    Pointer<Pointer<Utf8>>, // overridden_keys
    Pointer<Double>, // overridden_values
    Pointer<Pointer<Void>>); // out_exception

typedef _FaceAttributesEstimatorConstr_c = Pointer<Void> Function(
    Pointer<Void>, // service
    Pointer<Utf8>, // ini_file
    Int32, // overridden_count
    Pointer<Pointer<Utf8>>, // overridden_keys
    Pointer<Double>, // overridden_values
    Pointer<Pointer<Void>>); // out_exception

typedef _Liveness2DEstimatorConstr_dart = Pointer<Void> Function(
    Pointer<Void>, // service
    Pointer<Utf8>, // ini_file
    int, // overridden_count
    Pointer<Pointer<Utf8>>, // overridden_keys
    Pointer<Double>, // overridden_values
    Pointer<Pointer<Void>>); // out_exception

typedef _FaceAttributesEstimatorConstr_dart = Pointer<Void> Function(
    Pointer<Void>, // service
    Pointer<Utf8>, // ini_file
    int, // overridden_count
    Pointer<Pointer<Utf8>>, // overridden_keys
    Pointer<Double>, // overridden_values
    Pointer<Pointer<Void>>); // out_exception

typedef _Liveness2DEstimator_estimateLiveness = Pointer<Void> Function(
    Pointer<Void>, // liveness_2d_estimator
    Pointer<Void>, // raw_sample
    Pointer<Int32>, // verdict
    Pointer<Float>, // score
    Pointer<Pointer<Void>>); // out_exception

typedef _FaceAttributesEstimator_estimateEyesOpenness = Pointer<Void> Function(
    Pointer<Void>, // face_attributes_estimator
    Pointer<Void>, // raw_sample
    Pointer<Int32>, // left_verdict
    Pointer<Int32>, // right_verdict
    Pointer<Float>, // left_score
    Pointer<Float>, // right_score
    Pointer<Pointer<Void>>); // out_exception

typedef binary_stream_read_func = Void Function(
    Pointer<Void>, // stream
    Pointer<Void>, // data
    Uint64); // out_exception

typedef _Recognizer_load_template_c = Pointer<Void> Function(
    Pointer<Void>, // recognizer
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_read_func>>, // binary_stream_read_func
    Pointer<Pointer<Void>>); // out_exception

typedef _Recognizer_load_template_dart = Pointer<Void> Function(
    Pointer<Void>, // recognizer
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_read_func>>, // binary_stream_read_func
    Pointer<Pointer<Void>>); // out_exception

typedef _Recognizer_create_index_c = Pointer<Void> Function(
    Pointer<Void>, // recognizer
    Int64, //  templates_count
    Pointer<Pointer<Void>>, // templates
    Int32, // search_threads_count
    Pointer<Pointer<Void>>); // out_exception

typedef _Recognizer_create_index_dart = Pointer<Void> Function(
    Pointer<Void>, // recognizer
    int, //  templates_count
    Pointer<Pointer<Void>>, // templates
    int, // search_threads_count
    Pointer<Pointer<Void>>); // out_exception

typedef _Recognizer_search_v2_c = Void Function(
    Pointer<Void>, // recognizer
    Int32, // acceleration
    Int32, // queries_count
    Pointer<Pointer<Void>>, // query_templates
    Pointer<Void>, // templates_index
    Int64, // k
    Pointer<Int64>, // result_i_ptr
    Pointer<Float>, // result_distance_ptr
    Pointer<Float>, // result_far_ptr
    Pointer<Float>, // result_frr_ptr
    Pointer<Float>, // result_score_ptr
    Pointer<Pointer<Void>>); // out_exception

typedef _Recognizer_search_v2_dart = void Function(
    Pointer<Void>, // recognizer
    int, // acceleration
    int, // queries_count
    Pointer<Pointer<Void>>, // query_templates
    Pointer<Void>, // templates_index
    int, // k
    Pointer<Int64>, // result_i_ptr
    Pointer<Float>, // result_distance_ptr
    Pointer<Float>, // result_far_ptr
    Pointer<Float>, // result_frr_ptr
    Pointer<Float>, // result_score_ptr
    Pointer<Pointer<Void>>); // out_exception


typedef _Recognizer_get_roc_curve_c = Void Function(
    Pointer<Void>, // recognizer
    Double, // distance_threshold, desired_far, desired_frr, score_threshold
    Pointer<Double>, // result_distance_ptr
    Pointer<Double>, // result_far_ptr
    Pointer<Double>, // result_frr_ptr
    Pointer<Double>, // result_score_ptr
    Pointer<Pointer<Void>>); // out_exception

typedef _Recognizer_get_roc_curve_dart = void Function(
    Pointer<Void>, // recognizer
    double, // distance_threshold, desired_far, desired_frr, score_threshold
    Pointer<Double>, // result_distance_ptr
    Pointer<Double>, // result_far_ptr
    Pointer<Double>, // result_frr_ptr
    Pointer<Double>, // result_score_ptr
    Pointer<Pointer<Void>>); // out_exception


// Template Inference

typedef binary_stream_write_func = Void Function(
    Pointer<Void>, // stream
    Pointer<Void>, // data
    Uint64); // out_exception

typedef _Template_save_c = Void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _Template_save_dart = void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _Template_get_method_name_c = Void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _Template_get_method_name_dart = void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _ContextTemplate_save_c = Void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _ContextTemplate_save_dart = void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _ContextTemplate_loadTemplate_c = Pointer<Void> Function(
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_read_func>>, // binary_stream_read_func
    Pointer<Pointer<Void>>); // out_exception

typedef _ContextTemplate_loadTemplate_dart = Pointer<Void> Function(
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_read_func>>, // binary_stream_read_func
    Pointer<Pointer<Void>>); // out_exception

typedef _ContextTemplate_get_method_name_c = Void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _ContextTemplate_get_method_name_dart = void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

// TemplateIndex Inference

typedef _Templates_index_size_c = Int64 Function(
    Pointer<Void>, // templates_index
    Pointer<Pointer<Void>>); // out_exception

typedef _Templates_index_size_dart = int Function(
    Pointer<Void>, // templates_index
    Pointer<Pointer<Void>>); // out_exception

typedef _Templates_index_at_c = Pointer<Void> Function(
    Pointer<Void>, // templates_index
    Int64, // index
    Pointer<Pointer<Void>>); // out_exception

typedef _Templates_index_at_dart = Pointer<Void> Function(
    Pointer<Void>, // templates_index
    int, // index
    Pointer<Pointer<Void>>); // out_exception

typedef _Templates_index_reserve_search_memory_c = Void Function(
    Pointer<Void>, // templates_index
    Int64, // queries_count
    Pointer<Pointer<Void>>); // out_exception

typedef _Templates_index_reserve_search_memory_dart = void Function(
    Pointer<Void>, // templates_index
    int, // queries_count

    Pointer<Pointer<Void>>); // out_exception

// DynamicTemplateIndex
typedef _DynamicTemplateIndex_size_c = Int64 Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_size_dart = int Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_capacity_c = Int64 Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_capacity_dart = int Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_get_method_name_c = Void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _DynamicTemplateIndex_get_method_name_dart = void Function(
    Pointer<Void>, // templ
    Pointer<Void>, // binary_stream
    Pointer<NativeFunction<binary_stream_write_func>>, // binary_stream_write_func
    Pointer<Pointer<Void>>); // out_exception

typedef _DynamicTemplateIndex_at_by_index_c = Void Function(
    Pointer<Void>,
    Int64,
    Pointer<Void>,
    Pointer<NativeFunction<binary_stream_write_func>>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_at_by_index_dart = void Function(
    Pointer<Void>,
    int,
    Pointer<Void>,
    Pointer<NativeFunction<binary_stream_write_func>>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_at_by_uuid_c = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_at_by_uuid_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_get_c = Pointer<Void> Function(
    Pointer<Void>,
    Int64,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_get_dart = Pointer<Void> Function(
    Pointer<Void>,
    int,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_add_1_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_add_1_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_add_2_c = Void Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>,
    Pointer<Pointer<Utf8>>,
    Uint64,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_add_2_dart = void Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>,
    Pointer<Pointer<Utf8>>,
    int,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_add_3_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_add_3_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_add_4_c = Void Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>,
    Pointer<Pointer<Utf8>>,
    Uint64,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_add_4_dart = void Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>,
    Pointer<Pointer<Utf8>>,
    int,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_remove_1_c = Void Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_remove_1_dart = void Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_remove_2_c = Void Function(
    Pointer<Void>,
    Pointer<Pointer<Utf8>>,
    Uint64,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_remove_2_dart = void Function(
    Pointer<Void>,
    Pointer<Pointer<Utf8>>,
    int,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_concat_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_concat_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_clear_c = Void Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_clear_dart = void Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_save_c = Void Function(
    Pointer<Void>,
    Pointer<Utf8>,
    Bool,
    Pointer<Pointer<Void>>);

typedef _DynamicTemplateIndex_save_dart = void Function(
    Pointer<Void>,
    Pointer<Utf8>,
    bool,
    Pointer<Pointer<Void>>);

// Others

typedef _StructStorage_get_int64_c = Int64 Function(Pointer<Void>, Int32, Pointer<Pointer<Void>>);
typedef _StructStorage_get_int64_dart = int Function(Pointer<Void>, int, Pointer<Pointer<Void>>);

typedef _StructStorage_get_double_c = Double Function(Pointer<Void>, Int32, Pointer<Pointer<Void>>);
typedef _StructStorage_get_double_dart = double Function(Pointer<Void>, int, Pointer<Pointer<Void>>);

typedef _StructStorage_get_pointer_c = Pointer<Void> Function(Pointer<Void>, Int32, Pointer<Pointer<Void>>);
typedef _StructStorage_get_pointer_dart = Pointer<Void> Function(Pointer<Void>, int, Pointer<Pointer<Void>>);


typedef binary_stream_write_func_type = Void Function(
    Pointer<Void>,
    Pointer<Uint8>,
    Int32);

typedef _RSgetRectangle_c = Void Function(
    Pointer<Void>,
    Pointer<Int32>,
    Pointer<Int32>,
    Pointer<Int32>,
    Pointer<Int32>,
    Pointer<Pointer<Void>>);

typedef _RSgetRectangle_dart = void Function(
    Pointer<Void>,
    Pointer<Int32>,
    Pointer<Int32>,
    Pointer<Int32>,
    Pointer<Int32>,
    Pointer<Pointer<Void>>);

typedef _RSgetAngles_c = Void Function(
    Pointer<Void>,
    Pointer<Float>,
    Pointer<Float>,
    Pointer<Float>,
    Pointer<Pointer<Void>>);

typedef _RSgetAngles_dart = void Function(
    Pointer<Void>,
    Pointer<Float>,
    Pointer<Float>,
    Pointer<Float>,
    Pointer<Pointer<Void>>);

typedef _RSgetLandmarks_c = Void Function(
    Pointer<Void>,
    Pointer<Float>,
    Pointer<NativeFunction<_assign_float_vect_fu>>,
    Pointer<Pointer<Void>>);

typedef _RSgetLandmarks_dart = void Function(
    Pointer<Void>,
    Pointer<Float>,
    Pointer<NativeFunction<_assign_float_vect_fu>>,
    Pointer<Pointer<Void>>);

typedef _RSgetID_c = Int32 Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RSgetID_dart = int Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RSgetEye_c = Void Function(
    Pointer<Void>,
    Pointer<Float>,
    Pointer<Float>,
    Pointer<Pointer<Void>>);

typedef _RSgetEye_dart = void Function(
    Pointer<Void>,
    Pointer<Float>,
    Pointer<Float>,
    Pointer<Pointer<Void>>);

typedef _RSgetScore_c = Float Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RSgetScore_dart = double Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RScutFaceImage_c = Void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<NativeFunction<binary_stream_write_func_type>>,
    Pointer<Int32>,
    Pointer<Int32>,
    Int32,
    Int32,
    Int32,
    Pointer<Pointer<Void>>);

typedef _RScutFaceImage_dart = void Function(
    Pointer<Void>,
    Pointer<Void>,
    Pointer<NativeFunction<binary_stream_write_func_type>>,
    Pointer<Int32>,
    Pointer<Int32>,
    int,
    int,
    int,
    Pointer<Pointer<Void>>);

typedef _RStoContext = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RStoContext_dart = Pointer<Void> Function(
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RawImage_convertYUV2ARGB_c = Void Function(
    Pointer<Void>,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RawImage_convertYUV2ARGB_dart = void Function(
    Pointer<Void>,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RawImage_convertYUV2RGB_c = Void Function(
    Pointer<Void>,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Int32,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RawImage_convertYUV2RGB_dart = void Function(
    Pointer<Void>,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RawImage_convertBGRA88882RGB_c = Void Function(
    Pointer<Void>,
    Int32,
    Int32,
    Int32,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _RawImage_convertBGRA88882RGB_dart = void Function(
    Pointer<Void>,
    int,
    int,
    int,
    Pointer<Void>,
    Pointer<Pointer<Void>>);

typedef _ContextTemplate_convert_c = Pointer<Void> Function(Pointer<Void>, Pointer<Pointer<Void>>);
typedef _ContextTemplate_convert_dart = Pointer<Void> Function(Pointer<Void>, Pointer<Pointer<Void>>);

typedef _objDestructor_c = Void Function(Pointer<Void>);
typedef _objDestructor_dart = void Function(Pointer<Void>);

typedef _contextTemplateDestructor_c = Void Function(Pointer<Void>);
typedef _contextTemplateDestructor_dart = void Function(Pointer<Void>);

typedef _DynamicTemplateIndexDestructor_c = Void Function(Pointer<Void>);
typedef _DynamicTemplateIndexDestructor_dart = void Function(Pointer<Void>);

typedef _convertYUV420_888ToNV21_c = Void Function(Pointer<Uint8>, Int32, Int32, Int32, Pointer<Uint8>, Uint64, Pointer<Pointer<Void>>);
typedef _convertYUV420_888ToNV21_dart = void Function(Pointer<Uint8>, int, int, int, Pointer<Uint8>, int, Pointer<Pointer<Void>>);

typedef _createDynamicTemplateIndex_1_c = Pointer<Void> Function(Pointer<Void>, Pointer<Pointer<Void>>, Pointer<Pointer<Utf8>>, Int64, Pointer<Void>, Pointer<Pointer<Void>>);
typedef _createDynamicTemplateIndex_1_dart = Pointer<Void> Function(Pointer<Void>, Pointer<Pointer<Void>>, Pointer<Pointer<Utf8>>, int, Pointer<Void>, Pointer<Pointer<Void>>);

typedef _createDynamicTemplateIndex_2_c = Pointer<Void> Function(Pointer<Void>, Pointer<Void>, Pointer<Pointer<Void>>);
typedef _createDynamicTemplateIndex_2_dart = Pointer<Void> Function(Pointer<Void>, Pointer<Void>, Pointer<Pointer<Void>>);

typedef _contextTemplate_size_c = Int32 Function(Pointer<Void>, Pointer<Pointer<Void>>);
typedef _contextTemplate_size_dart = int Function(Pointer<Void>, Pointer<Pointer<Void>>);

typedef _get_version_c = Void Function(
    Pointer<Void>, // stream
    Pointer<NativeFunction<binary_stream_write_func>>, // stream_write_func
    Pointer<Pointer<Void>>); // out_exception
typedef _get_version_dart = void Function(
    Pointer<Void>, // stream
    Pointer<NativeFunction<binary_stream_write_func>>, // stream_write_func
    Pointer<Pointer<Void>>);