#ifndef __PBIO_API__PBIO__RECTANGLE_H_4b09c81da6d04740b80cbd2a1f3a3313
#define __PBIO_API__PBIO__RECTANGLE_H_4b09c81da6d04740b80cbd2a1f3a3313


namespace pbio
{
	/** \~English
		\brief Rectangle in an image.
		\~Russian
		\brief Прямоугольник на изображении.
	*/
	struct Rectangle
	{
		/** \~English
			\brief Default constructor.
			\~Russian
			\brief Конструктор по умолчанию.
		*/
		Rectangle(){}

		/** \~English
			\brief Constructor.
			\~Russian
			\brief Конструктор.
		*/
		Rectangle(int x, int y, int width, int height):x(x), y(y), width(width), height(height) { }

		/** \~English
			\brief X coordinate of the top-left corner.
			\~Russian
			\brief Координата X левого верхнего угла прямоугольника.
		*/
		int x;

		/** \~English
			\brief Y coordinate of the top-left corner.
			\~Russian
			\brief Координата Y левого верхнего угла прямоугольника.
		*/
		int y;

		/** \~English
			\brief Width of the rectangle.
			\~Russian
			\brief Ширина прямоугольника.
		*/
		int width;

		/** \~English
			\brief Height of the rectangle.
			\~Russian
			\brief Высота прямоугольника.
		*/
		int height;
	};
}  // pbio namespace


#endif  // __PBIO_API__PBIO__RECTANGLE_H_4b09c81da6d04740b80cbd2a1f3a3313
