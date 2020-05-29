#ifndef __PBIO_API__PBIO__DEPTH_MAP_RAW_H_
#define __PBIO_API__PBIO__DEPTH_MAP_RAW_H_


namespace pbio
{
	/** \~English
		\brief
		A depth map aligned with the original color image.
		\n Correspondence between the coordinates of the original image and depth map is as follows:
		\n &nbsp; &nbsp; let (x; y) be a point on the depth map, then a  corresponding point on the original image is
		\n &nbsp; &nbsp; (depth_map_2_image_offset_x + depth_map_2_image_scale_x * x; depth_map_2_image_offset_y + depth_map_2_image_scale_y * y).

		\~Russian
		\brief
		Карта глубины, отрегистрированная в соответствии с исходным цветным изображением.
		\n Соответствие между координатами исходного изображения и картой глубины:
		\n &nbsp; &nbsp; пусть (x; y) - точка на карте глубины, тогда соответствующая точка на исходном изображении - это:
		\n &nbsp; &nbsp; (depth_map_2_image_offset_x + depth_map_2_image_scale_x * x; depth_map_2_image_offset_y + depth_map_2_image_scale_y * y).
	*/
	struct DepthMapRaw
	{
		/** \~English
			\brief Height of the depth map.
			\~Russian
			\brief Высота карты глубины.
		*/
		int depth_map_rows;   
		
		/** \~English
			\brief Width of the depth map.
			\~Russian
			\brief Ширина карты глубины.
		*/
		int depth_map_cols;           
		
		/** \~English
			\brief Parameter of correspondence between the coordinates of the original image and depth map.
			\~Russian
			\brief Параметр соответствия координат исходного изображения и карты глубины.
		*/
		float depth_map_2_image_offset_x;  
		
		/** \~English
			\brief Parameter of correspondence between the coordinates of the original image and depth map.
			\~Russian
			\brief Параметр соответствия координат исходного изображения и карты глубины.
		*/
		float depth_map_2_image_offset_y;  
		
		/** \~English
			\brief Parameter of correspondence between the coordinates of the original image and depth map.
			\~Russian
			\brief Параметр соответствия координат исходного изображения и карты глубины.
		*/
		float depth_map_2_image_scale_x;   
		
		/** \~English
			\brief Parameter of correspondence between the coordinates of the original image and depth map.
			\~Russian
			\brief Параметр соответствия координат исходного изображения и карты глубины.
		*/
		float depth_map_2_image_scale_y; 
		
		/** \~English
			\brief The horizontal viewing angle of the depth map in degrees.
			\~Russian
			\brief Горизонтальный угол обзора карты глубины в градусах.
		*/
		float horizontal_fov;  

		/** \~English
			\brief The vertical viewing angle of the depth map in degrees.
			\~Russian
			\brief Вертикальный угол обзора карты глубины в градусах.
		*/
		float vertical_fov;  
		
		/** \~English
			\brief Scale of values stored in the depth_data.
			\~Russian
			\brief Масштаб значений, содержащихся в depth_data.
		*/
		float depth_unit_in_millimeters;   
		
		/** \~English
			\brief Pointer to the depth raw data.
			\~Russian
			\brief Указатель на данные глубины.
		*/
		uint16_t const* depth_data;
		
		/** \~English
			\brief Stride in bytes.
			\~Russian
			\brief Шаг в байтах.
		*/
		int depth_data_stride_in_bytes;

		DepthMapRaw()
		{
			depth_map_rows = 0;
			depth_map_cols = 0;
			depth_map_2_image_offset_x = 0;
			depth_map_2_image_offset_y = 0;
			depth_map_2_image_scale_x = 0;
			depth_map_2_image_scale_y = 0;
			horizontal_fov = 0;
			vertical_fov = 0;
			depth_unit_in_millimeters = 0;
			depth_data = 0;
			depth_data_stride_in_bytes = 0;
		}
	};

}  // pbio namespace


#endif  // __PBIO_API__PBIO__DEPTH_MAP_RAW_H_
