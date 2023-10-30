part of face_sdk_3divi;


/// Rectangle in an image.
class Rectangle
{
  ///Constructor.
    Rectangle(int x, int y, int width, int height):
          x = x,
          y = y,
          width = width,
          height =height;

  /// X coordinate of the top-left corner.
  int x;

  /// Y coordinate of the top-left corner.
  int y;

  /// Width of the rectangle.
  int width;

  /// Height of the rectangle.
  int height;

  String toString(){
    return "x: $x, y: $y, width: $width, height: $height";
  }
}
