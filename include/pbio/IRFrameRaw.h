#ifndef __PBIO_API__PBIO__IR_FRAME_RAW_H_
#define __PBIO_API__PBIO__IR_FRAME_RAW_H_


namespace pbio
{
	/** \~English
		\brief
		An IR frame aligned with an original color image.
		\n Correspondence between the coordinates of the original image and the IR frame is as follows:
		\n &nbsp; &nbsp; let (x; y) be a point on the IR frame, then a  corresponding point on the original image is
		\n &nbsp; &nbsp; (ir_frame_2_image_offset_x + ir_frame_image_2_scale_x * x; ir_frame_2_image_offset_y + ir_frame_2_image_scale_y * y).

		\~Russian
		\brief
		Инфракрасное изображение, отрегистрированное в соответствии с исходным цветным изображением.
		\n Соответствие между координатами исходного изображения и инфракрасного изображения:
		\n &nbsp; &nbsp; пусть (x; y) - точка на инфракрасном изображении, тогда соответствующая точка на исходном изображении - это:
		\n &nbsp; &nbsp; (ir_frame_2_image_offset_x + ir_frame_2_image_scale_x * x; ir_frame_2_image_offset_y + ir_frame_2_image_scale_y * y).
	*/
	struct IRFrameRaw
	{
		/** \~English
			\brief Height of an IR frame.
			\~Russian
			\brief Высота IR кадра.
		*/
		int ir_frame_rows;
		
		/** \~English
			\brief Width of an IR frame.
			\~Russian
			\brief Ширина IR кадра.
		*/
		int ir_frame_cols;

		/** \~English
			\brief Parameter of correspondence between the coordinates of an original image and an IR frame.
			\~Russian
			\brief Параметр соответствия координат исходного изображения и инфракрасного.
		*/
		float ir_frame_2_image_offset_x;

		/** \~English
			\brief Parameter of correspondence between the coordinates of an original image and an IR frame.
			\~Russian
			\brief Параметр соответствия координат исходного изображения и инфракрасного.
		*/
		float ir_frame_2_image_offset_y;

		/** \~English
			\brief Parameter of correspondence between the coordinates of an original image and an IR frame.
			\~Russian
			\brief Параметр соответствия координат исходного изображения и инфракрасного.
		*/
		float ir_frame_2_image_scale_x;

		/** \~English
			\brief Parameter of correspondence between the coordinates of an original image and an IR frame.
			\~Russian
			\brief Параметр соответствия координат исходного изображения и инфракрасного.
		*/
		float ir_frame_2_image_scale_y;

		/** \~English
			\brief The horizontal viewing angle of an IR frame in degrees.
			\~Russian
			\brief Горизонтальный угол обзора инфракрасного изображения в градусах.
		*/
		float ir_horizontal_fov;

		/** \~English
			\brief The vertical viewing angle of an IR frame in degrees.
			\~Russian
			\brief Вертикальный угол обзора инфракрасного изображения в градусах.
		*/
		float ir_vertical_fov;

		/** \~English
			\brief Pointer to IR data.
			\~Russian
			\brief Указатель на данные ИК кадра.
		*/
		uint16_t const* ir_frame_data;

		/** \~English
			\brief Stride in bytes.
			\~Russian
			\brief Шаг в байтах.
		*/
		int32_t ir_data_stride_in_bytes;

		IRFrameRaw()
		{
			ir_frame_rows = 0;
			ir_frame_cols = 0;
			ir_frame_2_image_offset_x = 0;
			ir_frame_2_image_offset_y = 0;
			ir_frame_2_image_scale_x = 0;
			ir_frame_2_image_scale_y = 0;
			ir_horizontal_fov = 0;
			ir_vertical_fov = 0;
			ir_frame_data = 0;
			ir_data_stride_in_bytes = 0;
		}
	};

}  // pbio namespace


#endif  // __PBIO_API__PBIO__IR_FRAME_RAW_H_
