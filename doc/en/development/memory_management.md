# Memory Management

## C++

All libfacerec interface objects are stored in smart pointers (see `pbio::LightSmartPtr` and `pbio::HeavySmartPtr`).
For convenience, each interface object `T` declares its own pointer type `T::Ptr` (for example, `pbio::Template::Ptr`), so do not use `pbio::LightSmartPtr` or `pbio::HeavySmartPtr` directly.
Most of the objects use `pbio::LightSmartPtr` which not allow to store inherited types or use specific deleters in order to significantly reduce memory consumption.

If you use OpenCV / boost / C++ 11, you can set one of the following macros, so that the libfacerec uses a specific type of a smart pointer instead of `pbio::HeavySmartPtr`:

* PBIO_OPENCV_SMART_POINTER - for using `cv::Ptr`
* PBIO_BOOST_SMART_POINTER - for using `boost::shared_ptr`
* PBIO_CXX11_SMART_POINTER - for using `std::shared_ptr`

If none of these macros is defined, a pointer defined in [pbio/shared_ptr/heavy_shared_ptr.h](../../../include/pbio/shared_ptr/heavy_shared_ptr.h) is used, which is actually a simplified version of an OpenCV smart pointer.

In any case, `T::Ptr` is a thread-safe shared smart pointer.

libfacerec library will be automatically unloaded when no interface object remains.

## Java

To release memory from internal objects before it is automatically done by the garbage collector, use the `dispose()` method. After this method is called, only the `isDisposed()` method can be called from the object, otherwise, if any other method is called, an exception will be thrown.

Example:
```java 
...
Capturer capturer = service.createCapturer(service.new Config("common_capturer.xml"));
...
capturer.dispose(); // release internal objects
if (capturer.isDisposed())
{
    System.out.println("object is disposed");
}
capturer.capture(image); // exception, object is disposed
// this line never be reached
```

## C#

To release memory from internal objects before it is automatically done by the garbage collector, use the `Dispose()` method. After this method is called, access only to the `isDisposed` property is allowed, otherwise, an exception will be thrown.

Example:
```cs
...
Capturer capturer = service.createCapturer("common_capturer.xml");
...
capturer.Dispose(); // release internal objects
if (capturer.IsDisposed)
{
    Console.WriteLine("object is disposed");
}
capturer.capture(image); // exception, object is disposed
// this line never be reached
```

## Python

Destructors are called when an object gets destroyed. Python has a garbage collector that handles memory management automatically. 

Example:
```python
...
capturer = service.create_capturer("common_capturer.xml")
...
del capturer  # release internal objects

capturer.capture(image)  # exception, object is disposed
# this line never be reached
```
