#ifndef __pbio__imp_exp_macro_1bcdd890e87a41b786dfa83f9c4236ad
#define __pbio__imp_exp_macro_1bcdd890e87a41b786dfa83f9c4236ad


#include <stdint.h>


namespace pbio {
namespace facerec {

class AgeGenderEstimatorImpl;
class CameraCalibratorImpl;
class CapturerImpl;
class DepthLivenessEstimatorImpl;
class EmotionsEstimatorImpl;
class FaceQualityEstimatorImpl;
class LivenessEstimatorImpl;
class QualityEstimatorImpl;
class RawSampleImpl;
class RecognizerImpl;
class TemplateImpl;
class TemplatesIndexImpl;
class VideoWorkerImpl;
class StructStorageImpl;

namespace capi {

	enum
	{
		VideoWorker_something_Processing_Enable   = 1,
		VideoWorker_something_Processing_Disable  = 2,
		VideoWorker_something_AgeGender_Enable    = 3,
		VideoWorker_something_AgeGender_Disable   = 4,
		VideoWorker_something_Emotions_Enable     = 5,
		VideoWorker_something_Emotions_Disable    = 6,
	};

	typedef void (*assign_floats_vector_func_type)(
		void* pointers_vector,
		float const* elements,
		int32_t elements_count);

	typedef void (*assign_pointers_vector_func_type)(
		void* pointers_vector,
		void* const* elements,
		int32_t elements_count);


	typedef void (*binary_stream_write_func_type)(
		void* stream,
		const void* data,
		uint64_t bytes_count);

	typedef void (*binary_stream_read_func_type)(
		void* stream,
		void* data,
		uint64_t bytes_count);


	typedef
		void (*VideoWorker_TrackingCallbackFunc)(
			void* err_stream,
			void* this_vw,
			void const* callback_data,

			const int32_t callbacks_count,
			void* const* const callbacks_func,
			void* const* const callbacks_userdata,
			const int32_t u_callbacks_count,
			void* const* const u_callbacks_func,
			void* const* const u_callbacks_userdata);

	typedef
		void (*VideoWorker_TemplateCreatedCallbackFunc)(
			void* err_stream,
			void* this_vw,
			void const* callback_data,

			const int32_t callbacks_count,
			void* const* const callbacks_func,
			void* const* const callbacks_userdata,
			const int32_t u_callbacks_count,
			void* const* const u_callbacks_func,
			void* const* const u_callbacks_userdata);

	typedef
		void (*VideoWorker_MatchFoundCallbackFunc)(
			void* err_stream,
			void* this_vw,
			void const* callback_data,

			const int32_t callbacks_count,
			void* const* const callbacks_func,
			void* const* const callbacks_userdata,
			const int32_t ext_callbacks_count,
			void* const* const ext_callbacks_func,
			void* const* const ext_callbacks_userdata,
			const int32_t u_callbacks_count,
			void* const* const u_callbacks_func,
			void* const* const u_callbacks_userdata);

	typedef
		void (*VideoWorker_TrackingLostCallbackFunc)(
			void* err_stream,
			void* this_vw,
			void const* callback_data,

			const int32_t callbacks_count,
			void* const* const callbacks_func,
			void* const* const callbacks_userdata,
			const int32_t u_callbacks_count,
			void* const* const u_callbacks_func,
			void* const* const u_callbacks_userdata);

	typedef
		void (*VideoWorker_StiPersonOutdatedCallbackFunc)(
			void* err_stream,
			void* this_vw,
			void const* callback_data,

			const int32_t callbacks_count,
			void* const* const callbacks_func,
			void* const* const callbacks_userdata);


}  // capi namespace
}  // facerec namespace
}  // pbio namespace


#define _583e_ADD_NAMESPACE(a) __4848a76477c449608aa5deb15c5495e4_facerec_v3_##a

#define _583e_STR_ADD_NAMESPACE(a) _583e_STRINGISE2( _583e_ADD_NAMESPACE(a) )

#define _583e_STRINGISE(a) #a

#define _583e_STRINGISE2(a) _583e_STRINGISE( a )



// decl suppose to be a macro: decl(rtype, name, typed_args, args, return)
#define __583e_FLIST(decl) \
	\
	\
	\
	\
	decl( \
		int64_t, \
		StructStorage_get_int64, \
		( \
			const void* struct_storage, \
			int32_t field_id, \
			void** out_exception \
		), \
		( \
			struct_storage, \
			field_id, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		double, \
		StructStorage_get_double, \
		( \
			const void* struct_storage, \
			int32_t field_id, \
			void** out_exception \
		), \
		( \
			struct_storage, \
			field_id, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void*, \
		StructStorage_get_pointer, \
		( \
			const void* struct_storage, \
			int32_t field_id, \
			void** out_exception \
		), \
		( \
			struct_storage, \
			field_id, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		VideoWorker_errStreamWriteFunc, \
		( \
			void* err_stream, \
			const void* data, \
			uint64_t bytes_count \
		), \
		( \
			err_stream, \
			data, \
			bytes_count \
		), \
		) \
	\
	\
	decl( \
		void, \
		VideoWorker_setThisVW, \
		( \
			void* video_worker, \
			void* this_vw, \
			void** out_exception \
		), \
		( \
			video_worker, \
			this_vw, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		VideoWorker_getMethodName, \
		( \
			void* video_worker, \
			void* name_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			void** out_exception \
		), \
		( \
			video_worker, \
			name_stream, \
			binary_stream_write_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		VideoWorker_getStreamsCount, \
		( \
			void* video_worker, \
			void** out_exception \
		), \
		( \
			video_worker, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_getTrackingConveyorSize, \
		( \
			void* video_worker, \
			int32_t stream_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			stream_id, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_addVideoFrame, \
		( \
			void* video_worker, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t stream_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			stream_id, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_addVideoFrameWithTimestamp, \
		( \
			void* video_worker, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t stream_id, \
			const uint64_t timestamp_microsec, \
			void** out_exception \
		), \
		( \
			video_worker, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			stream_id, \
			timestamp_microsec, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_addVideoFrameWithTimestamp_with_crop, \
		( \
			void* video_worker, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t image_with_crop, \
			int32_t image_crop_info_offset_x, \
			int32_t image_crop_info_offset_y, \
			int32_t image_crop_info_data_image_width, \
			int32_t image_crop_info_data_image_height, \
			int32_t stream_id, \
			const uint64_t timestamp_microsec, \
			void** out_exception \
		), \
		( \
			video_worker, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			image_with_crop, \
			image_crop_info_offset_x, \
			image_crop_info_offset_y, \
			image_crop_info_data_image_width, \
			image_crop_info_data_image_height, \
			stream_id, \
			timestamp_microsec, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		VideoWorker_addDepthFrame, \
		( \
			void* video_worker, \
			const int32_t depth_map_rows, \
			const int32_t depth_map_cols, \
			const float depth_map_2_image_offset_x, \
			const float depth_map_2_image_offset_y, \
			const float depth_map_2_image_scale_x, \
			const float depth_map_2_image_scale_y, \
			const float horizontal_fov, \
			const float vertical_fov, \
			const float depth_unit_in_millimeters, \
			uint16_t const* const depth_data, \
			const int32_t depth_data_stride_in_bytes, \
			int32_t stream_id, \
			const uint64_t timestamp_microsec, \
			void** out_exception \
		), \
		( \
			video_worker, \
			depth_map_rows, \
			depth_map_cols, \
			depth_map_2_image_offset_x, \
			depth_map_2_image_offset_y, \
			depth_map_2_image_scale_x, \
			depth_map_2_image_scale_y, \
			horizontal_fov, \
			vertical_fov, \
			depth_unit_in_millimeters, \
			depth_data, \
			depth_data_stride_in_bytes, \
			stream_id, \
			timestamp_microsec, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		VideoWorker_resetTrackerOnStream, \
		( \
			void* video_worker, \
			int32_t stream_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			stream_id, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		VideoWorker_resetStream, \
		( \
			void* video_worker, \
			int32_t stream_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			stream_id, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		VideoWorker_checkExceptions, \
		( \
			void* video_worker, \
			void** out_exception \
		), \
		( \
			video_worker, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		VideoWorker_addTrackingCallback, \
		( \
			void* video_worker, \
			void* callback, \
			void* userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_addTrackingCallbackU, \
		( \
			void* video_worker, \
			void* callback, \
			void* userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		VideoWorker_removeTrackingCallback, \
		( \
			void* video_worker, \
			int32_t callback_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback_id, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		VideoWorker_addMatchFoundCallback, \
		( \
			void* video_worker, \
			void* callback, \
			void* userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		VideoWorker_removeMatchFoundCallback, \
		( \
			void* video_worker, \
			int32_t callback_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback_id, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		VideoWorker_addStiPersonOutdatedCallbackU, \
		( \
			void* video_worker, \
			void* callback, \
			void* userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		VideoWorker_removeStiPersonOutdatedCallback, \
		( \
			void* video_worker, \
			int32_t callback_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback_id, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		VideoWorker_addTrackingLostCallback, \
		( \
			void* video_worker, \
			void* callback, \
			void* userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_addTrackingLostCallbackU, \
		( \
			void* video_worker, \
			void* callback, \
			void* userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		VideoWorker_removeTrackingLostCallback, \
		( \
			void* video_worker, \
			int32_t callback_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback_id, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		VideoWorker_toggleProcessing, /* #old */ \
		( \
			void* video_worker, \
			int32_t stream_id, \
			int32_t do_processing, \
			void** out_exception \
		), \
		( \
			video_worker, \
			stream_id, \
			do_processing, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		VideoWorker_toggleSomething, \
		( \
			void* video_worker, \
			int32_t stream_id, \
			int32_t something, \
			void** out_exception \
		), \
		( \
			video_worker, \
			stream_id, \
			something, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		VideoWorker_setDatabase, \
		( \
			void* video_worker, \
			int32_t acceleration, \
			int32_t elements_count, \
			uint64_t const* elements_ids, \
			uint64_t const* persons_ids, \
			pbio::facerec::TemplateImpl const* const* elements_templates, \
			float const* elements_thresholds, \
			void** out_exception \
		), \
		( \
			video_worker, \
			acceleration, \
			elements_count, \
			elements_ids, \
			persons_ids, \
			elements_templates, \
			elements_thresholds, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		VideoWorker_addMatchFoundCallbackExt, \
		( \
			void* video_worker, \
			void* callback, \
			void* userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_addMatchFoundCallbackU, \
		( \
			void* video_worker, \
			void* callback, \
			void* userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_addTemplateCreatedCallback, \
		( \
			void* video_worker, \
			void* const callback, \
			void* const userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		VideoWorker_addTemplateCreatedCallbackU, \
		( \
			void* video_worker, \
			void* const callback, \
			void* const userdata, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback, \
			userdata, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		VideoWorker_removeTemplateCreatedCallback, \
		( \
			void* video_worker, \
			int32_t callback_id, \
			void** out_exception \
		), \
		( \
			video_worker, \
			callback_id, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		VideoWorker_setParameter, \
		( \
			void* video_worker, \
			const char* param_name, \
			double param_value, \
			void** out_exception \
		), \
		( \
			video_worker, \
			param_name, \
			param_value, \
			out_exception \
		), \
		) \
	\
	\
	\
	\
	decl( \
		int32_t, \
		DepthLivenessEstimator_estimateLiveness, \
		( \
			void* depth_liveness_estimator, \
			pbio::facerec::RawSampleImpl const* raw_sample, \
			int32_t depth_map_rows, \
			int32_t depth_map_cols, \
			float depth_map_2_image_offset_x, \
			float depth_map_2_image_offset_y, \
			float depth_map_2_image_scale_x, \
			float depth_map_2_image_scale_y, \
			float horizontal_fov, \
			float vertical_fov, \
			float depth_unit_in_millimeters, \
			uint16_t const* depth_data, \
			int32_t depth_data_stride_in_bytes, \
			void** out_exception \
		), \
		( \
			depth_liveness_estimator, \
			raw_sample, \
			depth_map_rows, \
			depth_map_cols, \
			depth_map_2_image_offset_x, \
			depth_map_2_image_offset_y, \
			depth_map_2_image_scale_x, \
			depth_map_2_image_scale_y, \
			horizontal_fov, \
			vertical_fov, \
			depth_unit_in_millimeters, \
			depth_data, \
			depth_data_stride_in_bytes, \
			out_exception \
		), \
		return ) \
	\
	\
	\
	\
	decl( \
		void, \
		CameraCalibrator_initCalibration, \
		( \
			void* camera_calibrator, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t pattern_type, \
			int32_t pattern_width, \
			int32_t pattern_height, \
			int32_t fix_aspect_ratio, \
			int32_t calib_zero_tangent_dist, \
			int32_t calib_fix_principal_point, \
			int32_t rational_distortion_model, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			image_width, \
			image_height, \
			pattern_type, \
			pattern_width, \
			pattern_height, \
			fix_aspect_ratio, \
			calib_zero_tangent_dist, \
			calib_fix_principal_point, \
			rational_distortion_model, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		CameraCalibrator_addImage, \
		( \
			void* camera_calibrator, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t* pattern_found, \
			int32_t* pattern_accepted, \
			float* pattern_out, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			pattern_found, \
			pattern_accepted, \
			pattern_out, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		CameraCalibrator_addImage_with_crop, \
		( \
			void* camera_calibrator, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t image_with_crop, \
			int32_t image_crop_info_offset_x, \
			int32_t image_crop_info_offset_y, \
			int32_t image_crop_info_data_image_width, \
			int32_t image_crop_info_data_image_height, \
			int32_t* pattern_found, \
			int32_t* pattern_accepted, \
			float* pattern_out, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			image_with_crop, \
			image_crop_info_offset_x, \
			image_crop_info_offset_y, \
			image_crop_info_data_image_width, \
			image_crop_info_data_image_height, \
			pattern_found, \
			pattern_accepted, \
			pattern_out, \
			out_exception \
		), \
		) \
	\
	decl( \
		float, \
		CameraCalibrator_getPatternSpaceCoverProgress, \
		( \
			void* camera_calibrator, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		CameraCalibrator_getTip, \
		( \
			void* camera_calibrator, \
			float* pattern_out, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			pattern_out, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		float, \
		CameraCalibrator_calibrate, \
		( \
			void* camera_calibrator, \
			int32_t max_used_patterns_count, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			max_used_patterns_count, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		CameraCalibrator_saveCameraParameters, \
		( \
			void* camera_calibrator, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			binary_stream, \
			binary_stream_write_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		CameraCalibrator_loadCameraParameters, \
		( \
			void* camera_calibrator, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_read_func_type binary_stream_read_func, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			binary_stream, \
			binary_stream_read_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		CameraCalibrator_undistort, \
		( \
			void* camera_calibrator, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			float alpha, \
			void* result_buffer, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			alpha, \
			result_buffer, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		CameraCalibrator_undistort_with_crop, \
		( \
			void* camera_calibrator, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t image_with_crop, \
			int32_t image_crop_info_offset_x, \
			int32_t image_crop_info_offset_y, \
			int32_t image_crop_info_data_image_width, \
			int32_t image_crop_info_data_image_height, \
			float alpha, \
			void* result_buffer, \
			void** out_exception \
		), \
		( \
			camera_calibrator, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			image_with_crop, \
			image_crop_info_offset_x, \
			image_crop_info_offset_y, \
			image_crop_info_data_image_width, \
			image_crop_info_data_image_height, \
			alpha, \
			result_buffer, \
			out_exception \
		), \
		) \
	\
	\
	\
	\
	\
	decl( \
		void, \
		AgeGenderEstimator_estimateAgeGender, \
		( \
			void* age_gender_estimator, \
			const pbio::facerec::RawSampleImpl* rawsample, \
			int32_t* age_class, \
			int32_t* gender, \
			float* age_years, \
			void** out_exception \
		), \
		( \
			age_gender_estimator, \
			rawsample, \
			age_class, \
			gender, \
			age_years, \
			out_exception \
		), \
		) \
	\
	\
	decl( \
		int32_t, \
		EmotionsEstimator_estimateEmotions, \
		( \
			void* emotions_estimator, \
			const pbio::facerec::RawSampleImpl* rawsample, \
			void* data_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			void** out_exception \
		), \
		( \
			emotions_estimator, \
			rawsample, \
			data_stream, \
			binary_stream_write_func, \
			out_exception \
		), \
		return ) \
	\
	\
	decl( \
		void, \
		QualityEstimator_estimateQuality, \
		( \
			void* quality_estimator, \
			const pbio::facerec::RawSampleImpl* rawsample, \
			int32_t *lighting, \
			int32_t *noise, \
			int32_t *sharpness, \
			int32_t *flare, \
			void** out_exception \
		), \
		( \
			quality_estimator, \
			rawsample, \
			lighting, \
			noise, \
			sharpness, \
			flare, \
			out_exception \
		), \
		) \
	\
	\
	decl( \
		float, \
		FaceQualityEstimator_estimateQuality, \
		( \
			void* face_quality_estimator, \
			const pbio::facerec::RawSampleImpl* rawsample, \
			void** out_exception \
		), \
		( \
			face_quality_estimator, \
			rawsample, \
			out_exception \
		), \
		return ) \
	\
	\
	decl( \
		void, \
		LivenessEstimator_addSample, \
		( \
			void* liveness_estimator, \
			const pbio::facerec::RawSampleImpl* rawsample, \
			void** out_exception \
		), \
		( \
			liveness_estimator, \
			rawsample, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		LivenessEstimator_estimateLiveness, \
		( \
			void* liveness_estimator, \
			void** out_exception \
		), \
		( \
			liveness_estimator, \
			out_exception \
		), \
		return ) \
	\
	\
	\
	\
	decl( \
		void, \
		TemplatesIndex_getMethodName, \
		( \
			void* templates_index, \
			void* name_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			void** out_exception \
		), \
		( \
			templates_index, \
			name_stream, \
			binary_stream_write_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		int64_t, \
		TemplatesIndex_size, \
		( \
			void* templates_index, \
			void** out_exception \
		), \
		( \
			templates_index, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::TemplateImpl*, \
		TemplatesIndex_at, \
		( \
			void* templates_index, \
			int64_t index, \
			void** out_exception \
		), \
		( \
			templates_index, \
			index, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		TemplatesIndex_reserveSearchMemory, \
		( \
			void* templates_index, \
			int64_t queries_count, \
			void** out_exception \
		), \
		( \
			templates_index, \
			queries_count, \
			out_exception \
		), \
		) \
	\
	\
	\
	\
	\
	\
	decl( \
		void, \
		Template_getMethodName, \
		( \
			void* templ, \
			void* name_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			void** out_exception \
		), \
		( \
			templ, \
			name_stream, \
			binary_stream_write_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Template_save, \
		( \
			void* templ, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			void** out_exception \
		), \
		( \
			templ, \
			binary_stream, \
			binary_stream_write_func, \
			out_exception \
		), \
		) \
	\
	\
	\
	\
	\
	decl( \
		void, \
		Recognizer_getMethodName, \
		( \
			void* recognizer, \
			void* name_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			void** out_exception \
		), \
		( \
			recognizer, \
			name_stream, \
			binary_stream_write_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		pbio::facerec::TemplateImpl*, \
		Recognizer_processing, \
		( \
			void* recognizer, \
			const pbio::facerec::RawSampleImpl* raw_sample, \
			void** out_exception \
		), \
		( \
			recognizer, \
			raw_sample, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::TemplateImpl*, \
		Recognizer_loadTemplate, \
		( \
			void* recognizer, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_read_func_type binary_stream_read_func, \
			void** out_exception \
		), \
		( \
			recognizer, \
			binary_stream, \
			binary_stream_read_func, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		Recognizer_verifyMatch, /* #old */ \
		( \
			void* recognizer, \
			const pbio::facerec::TemplateImpl* template1, \
			const pbio::facerec::TemplateImpl* template2, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			void** out_exception \
		), \
		( \
			recognizer, \
			template1, \
			template2, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_verifyMatch_v2, \
		( \
			void* recognizer, \
			const pbio::facerec::TemplateImpl* template1, \
			const pbio::facerec::TemplateImpl* template2, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			double* result_score, \
			void** out_exception \
		), \
		( \
			recognizer, \
			template1, \
			template2, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			result_score, \
			out_exception \
		), \
		) \
	\
	decl( \
		pbio::facerec::TemplatesIndexImpl*, \
		Recognizer_createIndex, \
		( \
			void* recognizer, \
			int64_t templates_count, \
			pbio::facerec::TemplateImpl const* const* templates, \
			int32_t search_threads_count, \
			void** out_exception \
		), \
		( \
			recognizer, \
			templates_count, \
			templates, \
			search_threads_count, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		Recognizer_search, /* #old */ \
		( \
			void* recognizer, \
			int32_t acceleration, \
			int32_t queries_count, \
			pbio::facerec::TemplateImpl const* const* query_templates, \
			const pbio::facerec::TemplatesIndexImpl* templates_index, \
			int64_t k, \
			int64_t* result_i_ptr, \
			float* result_distance_ptr, \
			float* result_far_ptr, \
			float* result_frr_ptr, \
			void** out_exception \
		), \
		( \
			recognizer, \
			acceleration, \
			queries_count, \
			query_templates, \
			templates_index, \
			k, \
			result_i_ptr, \
			result_distance_ptr, \
			result_far_ptr, \
			result_frr_ptr, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_search_v2, \
		( \
			void* recognizer, \
			int32_t acceleration, \
			int32_t queries_count, \
			pbio::facerec::TemplateImpl const* const* query_templates, \
			const pbio::facerec::TemplatesIndexImpl* templates_index, \
			int64_t k, \
			int64_t* result_i_ptr, \
			float* result_distance_ptr, \
			float* result_far_ptr, \
			float* result_frr_ptr, \
			float* result_score_ptr, \
			void** out_exception \
		), \
		( \
			recognizer, \
			acceleration, \
			queries_count, \
			query_templates, \
			templates_index, \
			k, \
			result_i_ptr, \
			result_distance_ptr, \
			result_far_ptr, \
			result_frr_ptr, \
			result_score_ptr, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_getROCCurvePointByDistanceThreshold, /* #old */ \
		( \
			void* recognizer, \
			double distance_threshold, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			void** out_exception \
		), \
		( \
			recognizer, \
			distance_threshold, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_getROCCurvePointByDistanceThreshold_v2, \
		( \
			void* recognizer, \
			double distance_threshold, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			double* result_score, \
			void** out_exception \
		), \
		( \
			recognizer, \
			distance_threshold, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			result_score, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_getROCCurvePointByFAR, /* #old */ \
		( \
			void* recognizer, \
			double desired_far, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			void** out_exception \
		), \
		( \
			recognizer, \
			desired_far, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_getROCCurvePointByFAR_v2, \
		( \
			void* recognizer, \
			double desired_far, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			double* result_score, \
			void** out_exception \
		), \
		( \
			recognizer, \
			desired_far, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			result_score, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_getROCCurvePointByFRR, /* #old */ \
		( \
			void* recognizer, \
			double desired_frr, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			void** out_exception \
		), \
		( \
			recognizer, \
			desired_frr, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_getROCCurvePointByFRR_v2, \
		( \
			void* recognizer, \
			double desired_frr, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			double* result_score, \
			void** out_exception \
		), \
		( \
			recognizer, \
			desired_frr, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			result_score, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_getROCCurvePointByScoreThreshold, \
		( \
			void* recognizer, \
			double score_threshold, \
			double* result_distance, \
			double* result_fa_r, \
			double* result_fr_r, \
			double* result_score, \
			void** out_exception \
		), \
		( \
			recognizer, \
			score_threshold, \
			result_distance, \
			result_fa_r, \
			result_fr_r, \
			result_score, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Recognizer_chooseRepresentativeTemplatesSet, \
		( \
			void* recognizer, \
			pbio::facerec::TemplateImpl const* const* const templates, \
			int8_t const* const templates_inviolable_flags, \
			const int32_t templates_count, \
			int32_t* const choosen_set_indexes_out, \
			const int32_t set_size, \
			void** out_exception \
		), \
		( \
			recognizer, \
			templates, \
			templates_inviolable_flags, \
			templates_count, \
			choosen_set_indexes_out, \
			set_size, \
			out_exception \
		), \
		) \
	\
	\
	\
	\
	\
	decl( \
		void, \
		RawSample_getRectangle, \
		( \
			void* rawsample, \
			int32_t *x, \
			int32_t *y, \
			int32_t *width, \
			int32_t *height, \
			void** out_exception \
		), \
		( \
			rawsample, \
			x, \
			y, \
			width, \
			height, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_getLandmarks, \
		( \
			void* rawsample, \
			void* landmarks_floats_vector, \
			pbio::facerec::capi::assign_floats_vector_func_type assign_floats_vector_func, \
			void** out_exception \
		), \
		( \
			rawsample, \
			landmarks_floats_vector, \
			assign_floats_vector_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_getLeftEye, \
		( \
			void* rawsample, \
			float* point_x, \
			float* point_y, \
			void** out_exception \
		), \
		( \
			rawsample, \
			point_x, \
			point_y, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_getRightEye, \
		( \
			void* rawsample, \
			float* point_x, \
			float* point_y, \
			void** out_exception \
		), \
		( \
			rawsample, \
			point_x, \
			point_y, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_getAngles, \
		( \
			void* rawsample, \
			float* yaw, \
			float* pitch, \
			float* roll, \
			void** out_exception \
		), \
		( \
			rawsample, \
			yaw, \
			pitch, \
			roll, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		RawSample_getID, \
		( \
			void* rawsample, \
			void** out_exception \
		), \
		( \
			rawsample, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		RawSample_getType, \
		( \
			void* rawsample, \
			void** out_exception \
		), \
		( \
			rawsample, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		RawSample_getFaceCutRectangle, \
		( \
			void* rawsample, \
			int32_t cut_type, \
			float* corners, \
			void** out_exception \
		), \
		( \
			rawsample, \
			cut_type, \
			corners, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_save, \
		( \
			void* rawsample, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			int32_t format, \
			void** out_exception \
		), \
		( \
			rawsample, \
			binary_stream, \
			binary_stream_write_func, \
			format, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_save_without_image, \
		( \
			void* rawsample, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			double space_translation_x, \
			double space_translation_y, \
			double space_scale, \
			void** out_exception \
		), \
		( \
			rawsample, \
			binary_stream, \
			binary_stream_write_func, \
			space_translation_x, \
			space_translation_y, \
			space_scale, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		RawSample_getFrameID, \
		( \
			void* rawsample, \
			void** out_exception \
		), \
		( \
			rawsample, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		int32_t, \
		RawSample_hasOriginalImage, \
		( \
			void* rawsample, \
			void** out_exception \
		), \
		( \
			rawsample, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		RawSample_getOriginalImage, \
		( \
			void* rawsample, \
			int32_t* rows, \
			int32_t* cols, \
			int32_t* format, \
			void const** data, \
			void** out_exception \
		), \
		( \
			rawsample, \
			rows, \
			cols, \
			format, \
			data, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_cutFaceImage, \
		( \
			void* rawsample, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			int32_t format, \
			int32_t cut_type, \
			void** out_exception \
		), \
		( \
			rawsample, \
			binary_stream, \
			binary_stream_write_func, \
			format, \
			cut_type, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_cutFaceImage_original_raw_image, \
		( \
			void* rawsample, \
			const void* original_image_data, \
			int32_t original_image_width, \
			int32_t original_image_height, \
			int32_t original_image_format, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			int32_t format, \
			int32_t cut_type, \
			void** out_exception \
		), \
		( \
			rawsample, \
			original_image_data, \
			original_image_width, \
			original_image_height, \
			original_image_format, \
			binary_stream, \
			binary_stream_write_func, \
			format, \
			cut_type, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		RawSample_cutFaceImage_original_raw_image_with_crop, \
		( \
			void* rawsample, \
			const void* original_image_data, \
			int32_t original_image_width, \
			int32_t original_image_height, \
			int32_t original_image_format, \
			int32_t original_image_with_crop, \
			int32_t original_image_crop_info_offset_x, \
			int32_t original_image_crop_info_offset_y, \
			int32_t original_image_crop_info_data_image_width, \
			int32_t original_image_crop_info_data_image_height, \
			void* binary_stream, \
			pbio::facerec::capi::binary_stream_write_func_type binary_stream_write_func, \
			int32_t format, \
			int32_t cut_type, \
			void** out_exception \
		), \
		( \
			rawsample, \
			original_image_data, \
			original_image_width, \
			original_image_height, \
			original_image_format, \
			original_image_with_crop, \
			original_image_crop_info_offset_x, \
			original_image_crop_info_offset_y, \
			original_image_crop_info_data_image_width, \
			original_image_crop_info_data_image_height, \
			binary_stream, \
			binary_stream_write_func, \
			format, \
			cut_type, \
			out_exception \
		), \
		) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		RawSample_downscaleToPreferredSize, \
		( \
			void* rawsample, \
			void** out_exception \
		), \
		( \
			rawsample, \
			out_exception \
		), \
		return ) \
	\
	\
	\
	\
	decl( \
		void, \
		Capturer_capture_raw_image, \
		( \
			void* capturer, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			void* result_pointers_vector, \
			pbio::facerec::capi::assign_pointers_vector_func_type assign_pointers_vector_func, \
			void** out_exception \
		), \
		( \
			capturer, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			result_pointers_vector, \
			assign_pointers_vector_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Capturer_capture_raw_image_with_crop, \
		( \
			void* capturer, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t image_with_crop, \
			int32_t image_crop_info_offset_x, \
			int32_t image_crop_info_offset_y, \
			int32_t image_crop_info_data_image_width, \
			int32_t image_crop_info_data_image_height, \
			void* result_pointers_vector, \
			pbio::facerec::capi::assign_pointers_vector_func_type assign_pointers_vector_func, \
			void** out_exception \
		), \
		( \
			capturer, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			image_with_crop, \
			image_crop_info_offset_x, \
			image_crop_info_offset_y, \
			image_crop_info_data_image_width, \
			image_crop_info_data_image_height, \
			result_pointers_vector, \
			assign_pointers_vector_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		void, \
		Capturer_capture_encoded_image, \
		( \
			void* capturer, \
			const void* data, \
			int32_t data_size, \
			void* result_pointers_vector, \
			pbio::facerec::capi::assign_pointers_vector_func_type assign_pointers_vector_func, \
			void** out_exception \
		), \
		( \
			capturer, \
			data, \
			data_size, \
			result_pointers_vector, \
			assign_pointers_vector_func, \
			out_exception \
		), \
		) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		Capturer_manualCapture_raw_image_eyes_points, \
		( \
			void* capturer, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			float left_eye_x, \
			float left_eye_y, \
			float right_eye_x, \
			float right_eye_y, \
			void** out_exception \
		), \
		( \
			capturer, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			left_eye_x, \
			left_eye_y, \
			right_eye_x, \
			right_eye_y, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		Capturer_manualCapture_raw_image_eyes_points_with_crop, \
		( \
			void* capturer, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t image_with_crop, \
			int32_t image_crop_info_offset_x, \
			int32_t image_crop_info_offset_y, \
			int32_t image_crop_info_data_image_width, \
			int32_t image_crop_info_data_image_height, \
			float left_eye_x, \
			float left_eye_y, \
			float right_eye_x, \
			float right_eye_y, \
			void** out_exception \
		), \
		( \
			capturer, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			image_with_crop, \
			image_crop_info_offset_x, \
			image_crop_info_offset_y, \
			image_crop_info_data_image_width, \
			image_crop_info_data_image_height, \
			left_eye_x, \
			left_eye_y, \
			right_eye_x, \
			right_eye_y, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		Capturer_manualCapture_raw_image_points_vector, \
		( \
			void* capturer, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			float const* const points, \
			const int32_t points_count, \
			void** out_exception \
		), \
		( \
			capturer, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			points, \
			points_count, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		Capturer_manualCapture_raw_image_points_vector_with_crop, \
		( \
			void* capturer, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t image_with_crop, \
			int32_t image_crop_info_offset_x, \
			int32_t image_crop_info_offset_y, \
			int32_t image_crop_info_data_image_width, \
			int32_t image_crop_info_data_image_height, \
			float const* const points, \
			const int32_t points_count, \
			void** out_exception \
		), \
		( \
			capturer, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			image_with_crop, \
			image_crop_info_offset_x, \
			image_crop_info_offset_y, \
			image_crop_info_data_image_width, \
			image_crop_info_data_image_height, \
			points, \
			points_count, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		Capturer_manualCapture_encoded_image_eyes_points, \
		( \
			void* capturer, \
			const void* data, \
			int32_t data_size, \
			float left_eye_x, \
			float left_eye_y, \
			float right_eye_x, \
			float right_eye_y, \
			void** out_exception \
		), \
		( \
			capturer, \
			data, \
			data_size, \
			left_eye_x, \
			left_eye_y, \
			right_eye_x, \
			right_eye_y, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		Capturer_manualCapture_encoded_image_points_vector, \
		( \
			void* capturer, \
			const void* data, \
			int32_t data_size, \
			float const* const points, \
			const int32_t points_count, \
			void** out_exception \
		), \
		( \
			capturer, \
			data, \
			data_size, \
			points, \
			points_count, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		Capturer_resetHistory, \
		( \
			void* capturer, \
			void** out_exception \
		), \
		( \
			capturer, \
			out_exception \
		), \
		) \
	\
	decl( \
		int32_t, \
		Capturer_getType, \
		( \
			void* capturer, \
			void** out_exception \
		), \
		( \
			capturer, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		Capturer_setParameter, \
		( \
			void* capturer, \
			const char* param_name, \
			double param_value, \
			void** out_exception \
		), \
		( \
			capturer, \
			param_name, \
			param_value, \
			out_exception \
		), \
		) \
	\
	\
	\
	\
	\
	decl( \
		pbio::facerec::StructStorageImpl*, \
		FacerecService_getLicenseState, \
		( \
			void* service, \
			void** out_exception \
		), \
		( \
			service, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		FacerecService_forceOnlineLicenseUpdate, \
		( \
			void* service, \
			void** out_exception \
		), \
		( \
			service, \
			out_exception \
		), \
		) \
	\
	decl( \
		pbio::facerec::QualityEstimatorImpl*, \
		FacerecService_createQualityEstimator, \
		( \
			void* service, \
			const char* ini_file, \
			void** out_exception \
		), \
		( \
			service, \
			ini_file, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::FaceQualityEstimatorImpl*, \
		FacerecService_createFaceQualityEstimator, \
		( \
			void* service, \
			const char* ini_file, \
			void** out_exception \
		), \
		( \
			service, \
			ini_file, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::AgeGenderEstimatorImpl*, \
		FacerecService_createAgeGenderEstimator, \
		( \
			void* service, \
			const char* ini_file, \
			void** out_exception \
		), \
		( \
			service, \
			ini_file, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::LivenessEstimatorImpl*, \
		FacerecService_createLivenessEstimator, \
		( \
			void* service, \
			void** out_exception \
		), \
		( \
			service, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		FacerecService_loadRawSample, \
		( \
			void* service, \
			void* binary_stream, \
			::pbio::facerec::capi::binary_stream_read_func_type binary_stream_read_func, \
			void** out_exception \
		), \
		( \
			service, \
			binary_stream, \
			binary_stream_read_func, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		FacerecService_loadRawSampleWithoutImage_raw_image, \
		( \
			void* service, \
			void* binary_stream, \
			::pbio::facerec::capi::binary_stream_read_func_type binary_stream_read_func, \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t image_with_crop, \
			int32_t image_crop_info_offset_x, \
			int32_t image_crop_info_offset_y, \
			int32_t image_crop_info_data_image_width, \
			int32_t image_crop_info_data_image_height, \
			double space_translation_x, \
			double space_translation_y, \
			double space_scale, \
			void** out_exception \
		), \
		( \
			service, \
			binary_stream, \
			binary_stream_read_func, \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			image_with_crop, \
			image_crop_info_offset_x, \
			image_crop_info_offset_y, \
			image_crop_info_data_image_width, \
			image_crop_info_data_image_height, \
			space_translation_x, \
			space_translation_y, \
			space_scale, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RawSampleImpl*, \
		FacerecService_loadRawSampleWithoutImage_encoded_image, \
		( \
			void* service, \
			void* binary_stream, \
			::pbio::facerec::capi::binary_stream_read_func_type binary_stream_read_func, \
			const void* data, \
			int32_t data_size, \
			double space_translation_x, \
			double space_translation_y, \
			double space_scale, \
			void** out_exception \
		), \
		( \
			service, \
			binary_stream, \
			binary_stream_read_func, \
			data, \
			data_size, \
			space_translation_x, \
			space_translation_y, \
			space_scale, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::EmotionsEstimatorImpl*, \
		FacerecService_createEmotionsEstimator, \
		( \
			void* service, \
			const char* ini_file, \
			void** out_exception \
		), \
		( \
			service, \
			ini_file, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::CapturerImpl*, \
		FacerecService_createCapturerE, \
		( \
			void* service, \
			const char* ini_file, \
			const int32_t overridden_count, \
			char const* const* const overridden_keys, \
			double const* const overridden_values, \
			void** out_exception \
		), \
		( \
			service, \
			ini_file, \
			overridden_count, \
			overridden_keys, \
			overridden_values, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::CameraCalibratorImpl*, \
		FacerecService_createCameraCalibrator, \
		( \
			void* service, \
			void** out_exception \
		), \
		( \
			service, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::RecognizerImpl*, \
		FacerecService_createRecognizer2, \
		( \
			void* service, \
			const char* ini_file, \
			const int32_t processing, \
			const int32_t matching, \
			const int32_t processing_less_memory_consumption, \
			void** out_exception \
		), \
		( \
			service, \
			ini_file, \
			processing, \
			matching, \
			processing_less_memory_consumption, \
			out_exception \
		), \
		return ) \
	\
	\
	decl( \
		pbio::facerec::VideoWorkerImpl*, \
		FacerecService_createVideoWorker, /* #old */ \
		( \
			void* service, \
			const ::pbio::facerec::capi::VideoWorker_TrackingCallbackFunc trackingCallback, \
			const ::pbio::facerec::capi::VideoWorker_TemplateCreatedCallbackFunc templateCreatedCallback, \
			const ::pbio::facerec::capi::VideoWorker_MatchFoundCallbackFunc matchFoundCallback, \
			const ::pbio::facerec::capi::VideoWorker_TrackingLostCallbackFunc trackingLostCallback, \
			const char* video_worker_ini_file, \
			const int32_t vw_overridden_count, \
			char const* const* const vw_overridden_keys, \
			double const* const vw_overridden_values, \
			const char* recognizer_ini_file, \
			const int32_t streams_count, \
			const int32_t processing_threads_count, \
			const int32_t matching_threads_count, \
			void** out_exception \
		), \
		( \
			service, \
			trackingCallback, \
			templateCreatedCallback, \
			matchFoundCallback, \
			trackingLostCallback, \
			video_worker_ini_file, \
			vw_overridden_count, \
			vw_overridden_keys, \
			vw_overridden_values, \
			recognizer_ini_file, \
			streams_count, \
			processing_threads_count, \
			matching_threads_count, \
			out_exception \
		), \
		return ) \
	\
	\
	\
	decl( \
		pbio::facerec::VideoWorkerImpl*, \
		FacerecService_createVideoWorker_sti_age_gender_emotions, \
		( \
			void* service, \
			const ::pbio::facerec::capi::VideoWorker_TrackingCallbackFunc trackingCallback, \
			const ::pbio::facerec::capi::VideoWorker_TemplateCreatedCallbackFunc templateCreatedCallback, \
			const ::pbio::facerec::capi::VideoWorker_MatchFoundCallbackFunc matchFoundCallback, \
			const ::pbio::facerec::capi::VideoWorker_TrackingLostCallbackFunc trackingLostCallback, \
			const ::pbio::facerec::capi::VideoWorker_StiPersonOutdatedCallbackFunc stiPersonOutdatedCallback, \
			const char* video_worker_ini_file, \
			const int32_t vw_overridden_count, \
			char const* const* const vw_overridden_keys, \
			double const* const vw_overridden_values, \
			const char* recognizer_ini_file, \
			const int32_t streams_count, \
			const int32_t processing_threads_count, \
			const int32_t matching_threads_count, \
			const uint32_t  short_time_identification_enabled, \
			const float short_time_identification_distance_threshold, \
			const float short_time_identification_outdate_time_seconds, \
			const int32_t age_gender_threads_count, \
			const int32_t emotions_threads_count, \
			void** out_exception \
		), \
		( \
			service, \
			trackingCallback, \
			templateCreatedCallback, \
			matchFoundCallback, \
			trackingLostCallback, \
			stiPersonOutdatedCallback, \
			video_worker_ini_file, \
			vw_overridden_count, \
			vw_overridden_keys, \
			vw_overridden_values, \
			recognizer_ini_file, \
			streams_count, \
			processing_threads_count, \
			matching_threads_count, \
			short_time_identification_enabled, \
			short_time_identification_distance_threshold, \
			short_time_identification_outdate_time_seconds, \
			age_gender_threads_count, \
			emotions_threads_count, \
			out_exception \
		), \
		return ) \
	\
	\
	\
	decl( \
		pbio::facerec::DepthLivenessEstimatorImpl*, \
		FacerecService_createDepthLivenessEstimator, \
		( \
			void* service, \
			const char* ini_file, \
			void** out_exception \
		), \
		( \
			service, \
			ini_file, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		pbio::facerec::DepthLivenessEstimatorImpl*, \
		FacerecService_createDepthLivenessEstimatorE, \
		( \
			void* service, \
			const char* ini_file, \
			const int32_t overridden_count, \
			char const* const* const overridden_keys, \
			double const* const overridden_values, \
			void** out_exception \
		), \
		( \
			service, \
			ini_file, \
			overridden_count, \
			overridden_keys, \
			overridden_values, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void*, \
		FacerecService_constructor2, \
		( \
			char const* conf_dir, \
			char const* license_dir, \
			char const* dll_path, \
			void** out_exception \
		), \
		( \
			conf_dir, \
			license_dir, \
			dll_path, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void*, \
		FacerecService_constructor3, \
		( \
			void* ae_ptr, \
			char const* conf_dir, \
			char const* license_dir, \
			char const* dll_path, \
			void** out_exception \
		), \
		( \
			ae_ptr, \
			conf_dir, \
			license_dir, \
			dll_path, \
			out_exception \
		), \
		return ) \
	\
	decl( \
		void, \
		FacerecService_toggleAlgorithmsCacheKepp, \
		( \
			int32_t cache_keep_enabled, \
			void** out_exception \
		), \
		( \
			cache_keep_enabled, \
			out_exception \
		), \
		) \
	\
	\
	\
	\
	\
	decl( \
		void*, \
		InternalImageBuffer_constructor, \
		( \
			int32_t width, \
			int32_t height, \
			int32_t format, \
			int32_t* out_data_size, \
			void** out_data_ptr, \
			void** out_imagetptr_ptr, \
			void** out_exception \
		), \
		( \
			width, \
			height, \
			format, \
			out_data_size, \
			out_data_ptr, \
			out_imagetptr_ptr, \
			out_exception \
		), \
		return ) \
	\
	\
	decl( \
		void*, \
		InternalImageBuffer_constructor_from_android_image, \
		( \
			void* jni_env_ptr, \
			void* jobject_image_ptr, \
			int32_t* out_width, \
			int32_t* out_height, \
			int32_t* out_format, \
			int32_t* out_data_size, \
			void** out_data_ptr, \
			void** out_imagetptr_ptr, \
			void** out_exception \
		), \
		( \
			jni_env_ptr, \
			jobject_image_ptr, \
			out_width, \
			out_height, \
			out_format, \
			out_data_size, \
			out_data_ptr, \
			out_imagetptr_ptr, \
			out_exception \
		), \
		return ) \
	\
	\
	\
	\
	decl( \
		void, \
		RawImage_convertYUV2ARGB, \
		( \
			const void* image_data, \
			int32_t image_width, \
			int32_t image_height, \
			int32_t image_format, \
			int32_t image_with_crop, \
			int32_t image_crop_info_offset_x, \
			int32_t image_crop_info_offset_y, \
			int32_t image_crop_info_data_image_width, \
			int32_t image_crop_info_data_image_height, \
			int32_t downscale_x2, \
			void* result_buffer, \
			void** out_exception \
		), \
		( \
			image_data, \
			image_width, \
			image_height, \
			image_format, \
			image_with_crop, \
			image_crop_info_offset_x, \
			image_crop_info_offset_y, \
			image_crop_info_data_image_width, \
			image_crop_info_data_image_height, \
			downscale_x2, \
			result_buffer, \
			out_exception \
		), \
		) \
	\
	\
	\
	\
	decl( \
		uint32_t, \
		apiException_code, \
		( void* exception ), \
		( exception ), \
		return ) \
	\
	decl( \
		char const*, \
		apiException_what, \
		( void* exception ), \
		( exception ), \
		return ) \
	\
	decl( \
		void, \
		apiObject_destructor, \
		(void* object), \
		(object), \
		)


#endif  // __pbio__imp_exp_macro_1bcdd890e87a41b786dfa83f9c4236ad
