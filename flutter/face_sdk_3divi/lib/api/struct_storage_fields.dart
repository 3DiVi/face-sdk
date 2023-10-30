part of face_sdk_3divi;


class StructStorageFields
{
  // tracking
  static int video_worker_stream_id_t                              = 0;
  static int video_worker_frame_id_t                               = 1;
  static int video_worker_samples_count_t                          = 2;
  static int video_worker_samples_t                                = 3;
  static int video_worker_weak_samples_t                           = 4;
  static int video_worker_samples_quality_t                        = 5;

  // template created
  static int video_worker_quality_t                                = 6;
  static int video_worker_templ_t                                  = 7;

  // match found ext (+ match found)
  static int video_worker_search_result_size_t                     = 8;
  static int video_worker_search_result_person_ids_t               = 9;
  static int video_worker_search_result_element_ids_t              = 10;
  static int video_worker_search_result_match_result_distance_t    = 11;
  static int video_worker_search_result_match_result_fa_r_t        = 12;
  static int video_worker_search_result_match_result_fr_r_t        = 13;
  static int video_worker_search_result_match_result_score_t       = 36;

  // tracking lost
  static int video_worker_first_frame_id_t                         = 14;
  static int video_worker_last_frame_id_t                          = 15;
  static int video_worker_best_quality_t                           = 16;
  static int video_worker_best_qaulity_frame_id_t                  = 17;
  static int video_worker_best_quality_sample_t                    = 18;
  static int video_worker_best_quality_template_t                  = 19;
  static int video_worker_track_id_t                               = 20;

  // tracking
  static int video_worker_good_light_and_blur_samples_t            = 21;
  static int video_worker_good_angles_samples_t                    = 22;
  static int video_worker_detector_confirmed_samples_t             = 23;
  static int video_worker_depth_liveness_confirmed_samples_t       = 24;
  static int video_worker_good_face_size_samples_t                 = 25;

  // short time identification
  static int video_worker_sti_person_id_t                          = 26;
  static int video_worker_sti_person_id_set_t                      = 27;

  // tracking
  static int video_worker_samples_track_age_gender_set_t           = 28;
  static int video_worker_samples_track_gender_t                   = 29;
  static int video_worker_samples_track_age_t                      = 30;
  static int video_worker_samples_track_age_years_t                = 31;
  static int video_worker_samples_track_emotions_set_t             = 32;
  static int video_worker_samples_track_emotions_count_t           = 33;
  static int video_worker_samples_track_emotions_confidence_t      = 34;
  static int video_worker_samples_track_emotions_emotion_t         = 35;
  static int video_worker_ir_liveness_confirmed_samples_t          = 37;
  static int video_worker_active_liveness_type_samples_t           = 38;
  static int video_worker_active_liveness_confirmed_samples_t      = 39;
  static int video_worker_active_liveness_score_samples_t          = 40;

  // license state
  static int license_state_android_serial_t                        = -4;
  static int license_state_android_app_id_t                        = -3;
  static int license_state_ios_app_id_t                            = -2;
  static int license_state_hardware_reg_t                          = -1;
  static int license_state_online_t                                =  0;
  static int license_state_licenses_count_t                        =  1;
  static int license_state_licenses_names_int16_t                  =  0xbc4f;
  static int license_state_licenses_total_counts_int16_t           =  0x84c6;
  static int license_state_licenses_in_use_counts_int16_t          =  0x2a1c;

  // pool results
  static int video_worker_tracking_callback_t                      = 42;
  static int video_worker_template_created_callback_t              = 43;
  static int video_worker_match_found_callback_t                   = 44;
  static int video_worker_tracking_lost_callback_t                 = 45;
  static int video_worker_tracking_sti_callback_t                  = 46;

  static int max_fields_count                                      = 47;
}
