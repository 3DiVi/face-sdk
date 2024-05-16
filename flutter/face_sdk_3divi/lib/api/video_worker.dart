part of face_sdk_3divi;


class VWSearchResult
{

  late int person_id;
  late int element_id;

  late MatchResult match_result;

  VWSearchResult(this.person_id, this.element_id, this.match_result);

  String toString(){
    return "person_id: $person_id, element_id: $element_id, match_result: $match_result";
  }
}

class DataBaseElement{
  late int element_id;
  late int person_id;
  late Template face_template;

  late double distance_threshold;

  DataBaseElement(this.element_id, this.person_id, this.face_template, this.distance_threshold);
}

/// Parameters of the VideoWorker constructor.
class VideoWorkerParams{
  late Config _video_worker_config;
  String _recognizer_ini_file = "";
  int _streams_count = 0;
  int _processing_threads_count = 0;
  int _matching_threads_count = 0;
  int _age_gender_estimation_threads_count =0;
  int _emotions_estimation_threads_count = 0;
  int _short_time_identification_enabled = 0;
  double _short_time_identification_distance_threshold = 0;
  double _short_time_identification_outdate_time_seconds = 0;
  List<ActiveLivenessCheckType> _active_liveness_checks_order = [];

  /// Set the VideoWorker configuration file with optionally overridden parameters.
  VideoWorkerParams video_worker_config(Config value){
    _video_worker_config = value;
    return this;
  }

  /// Set the name of the configuration file for Recognizer that will be used inside VideoWorker.
  VideoWorkerParams recognizer_ini_file(String value){
    _recognizer_ini_file = value;
    return this;
  }

  /// Set the number of video streams.
  VideoWorkerParams streams_count(int value){
    _streams_count = value;
    return this;
  }

  /// Set the number of threads for creating templates.
  VideoWorkerParams processing_threads_count(int value){
    _processing_threads_count = value;
    return this;
  }

  /// Set the number of threads for matching templates with the database.
  VideoWorkerParams matching_threads_count(int value){
    _matching_threads_count = value;
    return this;
  }

  /// Set the number of threads for age_gender estimation.
  VideoWorkerParams age_gender_estimation_threads_count(int value){
    _age_gender_estimation_threads_count = value;
    return this;
  }

  /// Set the number of threads for estimation of emotions.
  VideoWorkerParams emotions_estimation_threads_count(int value){
    _emotions_estimation_threads_count = value;
    return this;
  }

  /// Set the flag enabling "short time identification".
  VideoWorkerParams short_time_identification_enabled(bool value){
    _short_time_identification_enabled = value ? 1: 0;
    return this;
  }

  /// Set the recognition distance threshold for "short time identification".
  VideoWorkerParams short_time_identification_distance_threshold(double value){
    _short_time_identification_distance_threshold = value;
    return this;
  }

  /// Set outdate_time in seconds for "short time identification".
  VideoWorkerParams short_time_identification_outdate_time_seconds(double value){
    _short_time_identification_outdate_time_seconds = value;
    return this;
  }

  /// Set the order for checking the active liveness
  VideoWorkerParams active_liveness_checks_order(List<ActiveLivenessCheckType> value){
    _active_liveness_checks_order = value;
    return this;
  }
}

/// Tracking callback data.
class TrackingCallbackData{
  /// Integer id of the video stream (0 <= stream_id < streams_count).
  int stream_id = -1;

  /// Integer id of the frame (that was returned by [VideoWorker.addVideoFrame])
  int frame_id = 0;

  /// List of face samples found by the tracker.<br>
  ///<br>
  /// Most of the samples are received from the frame_id frame
  ///	but some samples can be received from the previous frames.
  List<RawSample> samples = [];

  /// Vector of face IDs (track_id).<br>
  ///<br>
  /// track_id is equal to [RawSample.getID()] for the sample given in any VideoWorker callback.<br>
  /// (samples_track_id.size() == samples.size())
  List<int> _samples_track_id = [];

  List<bool> _samples_weak = [];

  ///	Quality value for the face sample. The same as from the FaceQualityEstimator
  /// (samples_quality.size() == samples.size())
  List<double> samples_quality = [];

  /// Face active liveness check status. See [ActiveLivenessStatus] for details.<br>
  ///	(samples_active_liveness_status.size() == samples.size())
  List<ActiveLivenessStatus> samples_active_liveness_status = [];
  //   List[Verdict] samples_good_light_and_blur;<br>
  //   samples_good_angles: List[Verdict]<br>
  //   samples_good_face_size: List[Verdict]<br>
  //   samples_detector_confirmed: List[Verdict]<br>
  //   samples_depth_liveness_confirmed: List[depth_liveness_estimator.Liveness]<br>
  //   samples_ir_liveness_confirmed: List[ir_liveness_estimator.Liveness]<br>
  //   samples_track_age_gender_set: List[bool]<br>
  //   samples_track_age_gender: List[AgeGender]<br>
  //   samples_track_emotions_set: List[bool]<br>
  //   samples_track_emotions: List[EmotionConfidence]<br>
}

/// TemplateCreated callback data.
class TemplateCreatedCallbackData{
  /// Integer id of the video stream (0 <= stream_id < streams_count).
  int stream_id = -1;

  /// Integer id of the frame (that was returned by [VideoWorker.addVideoFrame])
  int frame_id = 0;

  /// Face sample quality. The same as from the FaceQualityEstimator.
  double quality = 0;

  /// Face sample.
  late RawSample sample;

  /// Template created for this sample.
  late Template templ;
}


/// TrackingLost callback data.
class TrackingLostCallbackData{
  /// Integer id of the video stream (0 <= stream_id < streams_count).
  int stream_id = -1;

  /// Integer id of the frame on which the face was first detected.
  int first_frame_id = 0;

  /// Integer id of the frame after which the face tracking was lost.
  int last_frame_id = 0;

  /// Best sample quality over all frames.
  double best_quality = 0;

  /// Integer id of the frame of the best quality.
  int best_quality_frame_id = 0;

  /// The best quality sample from the best_quality_frame_id frame.<br>
  ///<br>
  /// Will be null if "weak_tracks_in_tracking_callback" is enabled and all samples with that track_id
  /// are flagged as "weak=true".
  RawSample? best_quality_sample;

  /// Template created from best_quality_sample.<br>
  /// Will be null if processing_threads_count is zero or best_quality_sample is NULL.
  Template? best_quality_templ;

  /// ID of the lost face (track_id).<br>
  /// track_id is equal to sample.getID() for a sample given in any VideoWorker callback.
  int track_id = 0;

  /// Flag indicating that sti_person_id is set. sti_person_id is not set, if short time identification is disabled
  /// or if no templates were generated for this track.
  bool sti_person_id_set = false;

  /// ID of "sti_person", which is a set of tracks formed by short time identification.<br>
  /// sti_person_id is equal to track_id of the first mebmer of this "sti_person" set of tracks.
  int sti_person_id = -1;
}


/// TrackingMatchFound callback data.
class TrackingMatchFoundCallbackData{
  int stream_id = -1;
  int frame_id = 0;

  double quality = 0;
  RawSample? sample;
  Template? templ;
  List<VWSearchResult> search_results = [];
}

/// StiPersonOutdated callback data.
class StiPersonOutdatedCallbackData{
  int stream_id = -1;
  int sti_person_id = -1;
}


/// Class containing tracking data ([TrackingCallbackData],
/// [TemplateCreatedCallbackData], [TrackingLostCallbackData]).<br>
///<br>
/// Returned when calling the [VideoWorker.poolTrackResults] method.
class TrackingData{
  TrackingCallbackData tracking_callback_data = new TrackingCallbackData();
  TemplateCreatedCallbackData template_created_callback_data = new TemplateCreatedCallbackData();
  TrackingLostCallbackData tracking_lost_callback_data = new TrackingLostCallbackData();
  TrackingMatchFoundCallbackData tracking_match_found_callback_data = new TrackingMatchFoundCallbackData();
  StiPersonOutdatedCallbackData sti_person_outdated_callback_data = new StiPersonOutdatedCallbackData();
}


/// VideoWorker is an interface object for tracking, processing and matching faces.<br>
///<br>
/// We recommend you to use VideoWorker instead of Capturer for face tracking on video streams. When VideoWorker is created with
/// <i>matching_thread=0</i> and <i>processing_thread=0</i>, then the standard Capturer license is used.
class VideoWorker extends _ComplexObject{
  late _StructStorage_get_int64_dart _getInt64;
  late _StructStorage_get_double_dart _getDouble;
  late _StructStorage_get_pointer_dart _getPointer;
  late _VWPollResults_dart _poll;
  List<ActiveLivenessStatus> _last_al = [];

  VideoWorker(DynamicLibrary dll_handle, Pointer<Void> impl):
        super(dll_handle, impl){
    final setVW = dll_handle.lookupFunction<_VWSetThisVW_c, _VWSetThisVW_dart>
      (_c_namespace + 'VideoWorker_setThisVW');
    Pointer<Pointer<Void>> exception = _getException();
    Pointer<Pointer<Int32>> _emptyPointer = malloc.allocate(10);
    setVW(impl, _emptyPointer.cast(), exception);
    checkException(exception, dll_handle);
    _getInt64 = dll_handle.lookupFunction<_StructStorage_get_int64_c, _StructStorage_get_int64_dart>
      (_c_namespace + 'StructStorage_get_int64');
    _getDouble = dll_handle.lookupFunction<_StructStorage_get_double_c, _StructStorage_get_double_dart>
      (_c_namespace + 'StructStorage_get_double');
    _getPointer = dll_handle.lookupFunction<_StructStorage_get_pointer_c, _StructStorage_get_pointer_dart>
      (_c_namespace + 'StructStorage_get_pointer');
    _poll = _dll_handle.lookupFunction<_VWPollResults_c, _VWPollResults_dart>
      (_c_namespace + 'VideoWorker_poolTrackResults');
  }

  int getInt64(Pointer<Void> pointer, int field){
    Pointer<Pointer<Void>> exception = _getException();
    final result = _getInt64(pointer, field, exception);
    checkException(exception, _dll_handle);
    return result;
  }

  double getDouble(Pointer<Void> pointer, int field){
    Pointer<Pointer<Void>> exception = _getException();
    final result = _getDouble(pointer, field, exception);
    checkException(exception, _dll_handle);
    return result;
  }

  Pointer<Void> getPointer(Pointer<Void> pointer, int field){
    Pointer<Pointer<Void>> exception = _getException();
    final result = _getPointer(pointer, field, exception);
    checkException(exception, _dll_handle);
    return result;
  }

  Pointer<Void> poll(Pointer<Void> pointer, int value){
    Pointer<Pointer<Void>> exception = _getException();
    final result = _poll(pointer, value, exception);
    checkException(exception, _dll_handle);
    return result;
  }


  // TODO: reafactor this
  TrackingCallbackData _parseTrackingCallback(Pointer<Void> trackingCallbackPointer){

    final stream_id = getInt64(trackingCallbackPointer, StructStorageFields.video_worker_stream_id_t);

    final frame_id = getInt64(trackingCallbackPointer, StructStorageFields.video_worker_frame_id_t);

    final samples_count = getInt64(trackingCallbackPointer, StructStorageFields.video_worker_samples_count_t);

    Pointer<Pointer<Void>> samples = getPointer(trackingCallbackPointer, StructStorageFields.video_worker_samples_t).cast();

    List<RawSample> rss = List<RawSample>.generate(samples_count, (i) => RawSample(_dll_handle, samples[i]));

    Pointer<Float> sample_quality = getPointer(trackingCallbackPointer, StructStorageFields.video_worker_samples_quality_t).cast();

    List<double> qss = [];
    for (int i = 0; i < samples_count; i++){
      double buf = Pointer<Float>.fromAddress(sample_quality.address + i * sizeOf<Pointer<Float>>()).value;
      qss.add(buf);
    }

    Pointer<Float> ALScore = getPointer(trackingCallbackPointer, StructStorageFields.video_worker_active_liveness_score_samples_t).cast();
    Pointer<Int32> ALType = getPointer(trackingCallbackPointer, StructStorageFields.video_worker_active_liveness_type_samples_t).cast();
    Pointer<Int32> ALConf = getPointer(trackingCallbackPointer, StructStorageFields.video_worker_active_liveness_confirmed_samples_t).cast();

    List<ActiveLivenessStatus> al = [];

    for (int i = 0; i < samples_count; ++i) {
      ActiveLivenessStatus status = new ActiveLivenessStatus();
      status.check_type = ActiveLivenessCheckType.values[Pointer<Int32>.fromAddress(ALType.address + i * sizeOf<Pointer<Int32>>()).value];
      status.progress_level = Pointer<Float>.fromAddress(ALScore.address + i * sizeOf<Pointer<Float>>()).value;
      status.verdict = ActiveLiveness.values[Pointer<Int32>.fromAddress(
          ALConf.address + i * sizeOf<Pointer<Int32>>()).value];

      if(status.progress_level > 1 || status.progress_level < 0 || (status.progress_level > 0 && status.progress_level < 1e-20) ||
          (status.verdict == ActiveLiveness.ALL_CHECKS_PASSED && status.progress_level != 1)) {
        if (_last_al.length != 0)
          status = _last_al[i];
      }

      al.add(status);
    }

    TrackingCallbackData td = new TrackingCallbackData();
    td.stream_id = stream_id;
    td.frame_id = frame_id;
    td.samples = rss;
    td.samples_active_liveness_status = al;
    td.samples_quality = qss;
    _last_al = al;

    return td;
  }

  TemplateCreatedCallbackData _parseTemplateCreatedCallback(Pointer<Void> templateCallbackPointer) {
    final stream_id = getInt64(templateCallbackPointer, StructStorageFields.video_worker_stream_id_t);

    final frame_id = getInt64(templateCallbackPointer, StructStorageFields.video_worker_frame_id_t);

    final quality = getDouble(templateCallbackPointer, StructStorageFields.video_worker_quality_t);

    Pointer<Void> sample = getPointer(templateCallbackPointer, StructStorageFields.video_worker_samples_t);

    Pointer<Void> templ = getPointer(templateCallbackPointer, StructStorageFields.video_worker_templ_t);

    TemplateCreatedCallbackData tccd = new TemplateCreatedCallbackData();
    tccd.stream_id = stream_id;
    tccd.frame_id = frame_id;
    tccd.sample = RawSample(_dll_handle, sample);
    tccd.templ = Template(_dll_handle, templ);
    tccd.quality = quality;
    return tccd;
  }

  TrackingLostCallbackData _parseTrackingLostCallback(Pointer<Void> trackingLostCallbackPointer) {
    final stream_id = getInt64(trackingLostCallbackPointer, StructStorageFields.video_worker_stream_id_t);

    final first_frame_id = getInt64(trackingLostCallbackPointer, StructStorageFields.video_worker_first_frame_id_t);

    final last_frame_id = getInt64(trackingLostCallbackPointer, StructStorageFields.video_worker_last_frame_id_t);

    final best_quality = getDouble(trackingLostCallbackPointer, StructStorageFields.video_worker_best_quality_t);

    final best_quality_frame_id = getInt64(trackingLostCallbackPointer, StructStorageFields.video_worker_best_qaulity_frame_id_t);

    Pointer<Void> best_quality_sample_impl = getPointer(trackingLostCallbackPointer, StructStorageFields.video_worker_samples_t);

    Pointer<Void> best_quality_template_impl = getPointer(trackingLostCallbackPointer, StructStorageFields.video_worker_templ_t);

    final track_id = getInt64(trackingLostCallbackPointer, StructStorageFields.video_worker_track_id_t);

    final sti_person_id_set = getInt64(trackingLostCallbackPointer, StructStorageFields.video_worker_sti_person_id_set_t);

    final sti_person_id = getInt64(trackingLostCallbackPointer, StructStorageFields.video_worker_sti_person_id_t);

    TrackingLostCallbackData data = new TrackingLostCallbackData();
    data.stream_id = stream_id;
    data.first_frame_id = first_frame_id;
    data.last_frame_id = last_frame_id;
    data.best_quality = best_quality;
    data.best_quality_frame_id = best_quality_frame_id;
    if (best_quality_sample_impl != nullptr)
      data.best_quality_sample = RawSample(_dll_handle, best_quality_sample_impl);

    if (best_quality_template_impl != nullptr)
      data.best_quality_templ = Template(_dll_handle, best_quality_template_impl);

    data.track_id = track_id;
    data.sti_person_id = sti_person_id;
    data.sti_person_id_set = sti_person_id_set == 1;
    return data;
  }

  TrackingMatchFoundCallbackData _parseTrackingMatchFoundCallback(Pointer<Void> trackingMatchFoundCallbackPointer) {

    final stream_id = getInt64(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_stream_id_t);
    final frame_id = getInt64(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_frame_id_t);
    final quality = getDouble(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_samples_quality_t);

    Pointer<Void> sample_impl = getPointer(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_samples_t);
    Pointer<Void> template_impl = getPointer(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_templ_t);

    final search_result_count = getInt64(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_search_result_size_t);
    Pointer<Int32> all_person_id = getPointer(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_search_result_person_ids_t).cast();
    Pointer<Int32> all_element_id = getPointer(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_search_result_element_ids_t).cast();
    Pointer<Float> all_distance = getPointer(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_search_result_match_result_distance_t).cast();
    Pointer<Float> all_far = getPointer(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_search_result_match_result_fa_r_t).cast();
    Pointer<Float> all_frr = getPointer(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_search_result_match_result_fr_r_t).cast();
    Pointer<Float> all_score = getPointer(trackingMatchFoundCallbackPointer, StructStorageFields.video_worker_search_result_match_result_score_t).cast();

    List<VWSearchResult> search_results = [];

    for (int i = 0; i < search_result_count; ++i) {
      final person_id = Pointer<Int32>.fromAddress(all_person_id.address + i * sizeOf<Pointer<Int32>>()).value;
      final element_id = Pointer<Int32>.fromAddress(all_element_id.address + i * sizeOf<Pointer<Int32>>()).value;

      MatchResult match_result = new MatchResult(
          Pointer<Float>.fromAddress(all_distance.address + i * sizeOf<Pointer<Float>>()).value,
          Pointer<Float>.fromAddress(all_far.address + i * sizeOf<Pointer<Float>>()).value,
          Pointer<Float>.fromAddress(all_frr.address + i * sizeOf<Pointer<Float>>()).value,
          Pointer<Float>.fromAddress(all_score.address + i * sizeOf<Pointer<Float>>()).value,
      );

      search_results.add(VWSearchResult(person_id, element_id, match_result));
    }

    TrackingMatchFoundCallbackData data = new TrackingMatchFoundCallbackData();
    data.stream_id = stream_id;
    data.frame_id = frame_id;
    data.quality = quality;

    if (sample_impl != nullptr)
      data.sample = RawSample(_dll_handle, sample_impl);

    if (template_impl != nullptr)
      data.templ = Template(_dll_handle, template_impl);
    data.search_results = search_results;


    return data;
  }

  StiPersonOutdatedCallbackData _parseStiPersonOutdatedCallbackData(Pointer<Void> stiPersonOutdatedCallbackPointer){
    final stream_id = getInt64(stiPersonOutdatedCallbackPointer, StructStorageFields.video_worker_stream_id_t);
    final sti_person_id = getInt64(stiPersonOutdatedCallbackPointer, StructStorageFields.video_worker_sti_person_id_t);

    StiPersonOutdatedCallbackData data = new StiPersonOutdatedCallbackData();
    data.stream_id = stream_id;
    data.sti_person_id = sti_person_id;

    return data;
  }

  /// Synchronous method for obtaining tracking results.<br>
  ///<br>
  /// Returns the last [TrackingCallbackData], [TemplateCreatedCallbackData],
  /// [TrackingLostCallbackData] structs.
  TrackingData poolTrackResults(){
    TrackingData trackData = new TrackingData();

    final storage = poll(_impl, 1);

    if(storage.address == nullptr.address) {
      // developer.log("Storage nullpointer", name: 'my.app.category');
      return trackData;
    }

    final track_callback = getPointer(storage, StructStorageFields.video_worker_tracking_callback_t);

    if(track_callback.address != nullptr.address) {
      final td = _parseTrackingCallback(track_callback);
      trackData.tracking_callback_data = td;
    }

    final templ_create_callback = getPointer(storage, StructStorageFields.video_worker_template_created_callback_t);

    if(templ_create_callback.address != nullptr.address){
      trackData.template_created_callback_data = _parseTemplateCreatedCallback(templ_create_callback);
    }

    final lost_callback = getPointer(storage, StructStorageFields.video_worker_tracking_lost_callback_t);

    if(lost_callback.address != nullptr.address){
      trackData.tracking_lost_callback_data = _parseTrackingLostCallback(lost_callback);
    }

    final match_found_callback = getPointer(storage, StructStorageFields.video_worker_match_found_callback_t);
    if(match_found_callback.address != nullptr.address){
      trackData.tracking_match_found_callback_data = _parseTrackingMatchFoundCallback(match_found_callback);
    }

    final sti_person_callback = getPointer(storage, StructStorageFields.video_worker_tracking_sti_callback_t);
    if(sti_person_callback.address != nullptr.address){
      trackData.sti_person_outdated_callback_data = _parseStiPersonOutdatedCallbackData(sti_person_callback);
    }


    return trackData;
  }

  /// Add a new video frame for a specific video stream.<br>
  ///<br>
  /// Tracking results can be obtained via [VideoWorker.poolTrackResults].<br>
  /// Return integer id for this frame, unique for this video stream.<br>
  /// This id will be used to identify this frame in the callbacks.
  int addVideoFrame(RawImageF image, int timestamp_microsec){
    final addVF = _dll_handle.lookupFunction<_VWaddVideoFrame_c, _VWaddVideoFrame_dart>
      (_c_namespace + 'VideoWorker_addVideoFrameWithTimestamp_with_crop');
    Pointer<Pointer<Void>> exception = _getException();

    int res = addVF(
      _impl,
      image.data.cast(),
      image.width,
      image.height,
      image.format.index,
      image.with_crop,
      image.crop_info_offset_x,
      image.crop_info_offset_y,
      image.crop_info_data_image_width,
      image.crop_info_data_image_height,
      0,
      timestamp_microsec,
      exception);
    checkException(exception, _dll_handle);

    return res;
  }

  /// Reset tracker state.
  void resetTrackerOnStream(){
    final reset = _dll_handle.lookupFunction<_VWresetTrackerOnStream_c, _VWresetTrackerOnStream_dart>
      (_c_namespace + 'VideoWorker_resetTrackerOnStream');
    Pointer<Pointer<Void>> exception = _getException();
    reset(_impl, 0, exception);
    checkException(exception, _dll_handle);
  }

  void setDataBase(
      List<DataBaseElement> dataBase,
      {SearchAccelerationType acceleration = SearchAccelerationType.SEARCH_ACCELERATION_1}){

    final set_db = _dll_handle.lookupFunction<_VWsetDataBase, _VWsetDataBase_dart>
      (_c_namespace + 'VideoWorker_setDatabase');

    final data_base_size = dataBase.length;
    Pointer<Uint64> element_ids_v = malloc.allocate(data_base_size * sizeOf<Uint64>());
    Pointer<Uint64> person_ids_v = malloc.allocate(data_base_size * sizeOf<Uint64>());
    Pointer<Float> thresholds_v = malloc.allocate(data_base_size * sizeOf<Float>());
    Pointer<Pointer<Void>> templates_v = malloc.allocate(data_base_size * sizeOf<Pointer<Pointer<Void>>>());


    for (var i = 0; i < data_base_size; i++){
      element_ids_v[i] = dataBase[i].element_id;
      person_ids_v[i] = dataBase[i].person_id;
      thresholds_v[i] = dataBase[i].distance_threshold;
      templates_v[i] = dataBase[i].face_template._impl;
    }

    Pointer<Pointer<Void>> exception = _getException();
    set_db(
        _impl,
        acceleration.index,
        data_base_size,
        element_ids_v,
        person_ids_v,
        templates_v,
        thresholds_v,
        exception);
    checkException(exception, _dll_handle);

    malloc.free(element_ids_v);
    malloc.free(person_ids_v);
    malloc.free(thresholds_v);
    malloc.free(templates_v);

  }

}
