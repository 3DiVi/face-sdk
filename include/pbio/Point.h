#ifndef __PBIO_API__PBIO__POINT_H_
#define __PBIO_API__PBIO__POINT_H_


namespace pbio
{
	/** \~English
		\brief 3D point.
		\~Russian
		\brief 3D-точка.
	*/
	struct Point
	{
		/** \~English
			\brief X coordinate.
			\~Russian
			\brief Координата X.
		*/
		float x;

		/** \~English
			\brief Y coordinate.
			\~Russian
			\brief Координата Y.
		*/ 
		float y;
		/** \~English
			\brief Z coordinate.
			\~Russian
			\brief Координата Z.
		*/
		float z;
	};

}  // pbio namespace


#endif  // __PBIO_API__PBIO__POINT_H_
