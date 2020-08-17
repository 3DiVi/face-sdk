#ifndef __PBIO_API__PBIO__STRUCT_STORAGE_FIELDS_H_
#define __PBIO_API__PBIO__STRUCT_STORAGE_FIELDS_H_

namespace pbio
{

struct StructStorageFields
{
	enum Type
	{
		// tracking
		video_worker_stream_id_t                              = 0,
		video_worker_frame_id_t                               = 1,
		video_worker_samples_count_t                          = 2,
		video_worker_samples_t                                = 3,
		video_worker_weak_samples_t                           = 4,
		video_worker_samples_quality_t                        = 5,

		// template created
		video_worker_quality_t                                = 6,
		video_worker_templ_t                                  = 7,

		// match found ext (+ match found)
		video_worker_search_result_size_t                     = 8,
		video_worker_search_result_person_ids_t               = 9,
		video_worker_search_result_element_ids_t              = 10,
		video_worker_search_result_match_result_distance_t    = 11,
		video_worker_search_result_match_result_fa_r_t        = 12,
		video_worker_search_result_match_result_fr_r_t        = 13,
		video_worker_search_result_match_result_score_t       = 36,

		// tracking lost
		video_worker_first_frame_id_t                         = 14,
		video_worker_last_frame_id_t                          = 15,
		video_worker_best_quality_t                           = 16,
		video_worker_best_qaulity_frame_id_t                  = 17,
		video_worker_best_quality_sample_t                    = 18,
		video_worker_best_quality_template_t                  = 19,
		video_worker_track_id_t                               = 20,

		// tracking
		video_worker_good_light_and_blur_samples_t            = 21,
		video_worker_good_angles_samples_t                    = 22,
		video_worker_detector_confirmed_samples_t             = 23,
		video_worker_depth_liveness_confirmed_samples_t       = 24,
		video_worker_good_face_size_samples_t                 = 25,

		// short time identification
		video_worker_sti_person_id_t                          = 26,
		video_worker_sti_person_id_set_t                      = 27,

		// tracking
		video_worker_samples_track_age_gender_set_t           = 28,
		video_worker_samples_track_gender_t                   = 29,
		video_worker_samples_track_age_t                      = 30,
		video_worker_samples_track_age_years_t                = 31,
		video_worker_samples_track_emotions_set_t             = 32,
		video_worker_samples_track_emotions_count_t           = 33,
		video_worker_samples_track_emotions_confidence_t      = 34,
		video_worker_samples_track_emotions_emotion_t         = 35,
		video_worker_ir_liveness_confirmed_samples_t          = 36,

		// license state
		license_state_android_app_id_t                        = -3,
		license_state_ios_app_id_t                            = -2,
		license_state_hardware_reg_t                          = -1,
		license_state_online_t                                =  0,
		license_state_licenses_count_t                        =  1,
		license_state_licenses_names_int16_t                  =  0xbc4f,
		license_state_licenses_total_counts_int16_t           =  0x84c6,
		license_state_licenses_in_use_counts_int16_t          =  0x2a1c,

		max_fields_count                                      = 37
	};
};

} // pbio namespace

#endif // __PBIO_API__PBIO__STRUCT_STORAGE_FIELDS_H_
