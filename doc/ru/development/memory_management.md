# Управление памятью

## С++

Все интерфейсные объекты хранятся в умных указателях (см. `pbio::LightSmartPtr` и `pbio::HeavySmartPtr`). Для удобства каждый интерфейсный объект `T` объявляет его собственный тип указателя `T::Ptr` (например, `pbio::Template::Ptr`), поэтому не используйте `pbio::LightSmartPtr` или `pbio::HeavySmartPtr` напрямую. Большинство объектов используют `pbio::LightSmartPtr`, который в целях значительного снижения потребления памяти не позволяет хранить наследованные типы или использовать особый deleter.

Если вы используете OpenCV / boost / C++11, то можете определить один из перечисленных ниже макросов, чтобы настроить libfacerec на использование определенного типа указателей вместо `pbio::HeavySmartPtr`:

* PBIO_OPENCV_SMART_POINTER - для использования `cv::Ptr`,
* PBIO_BOOST_SMART_POINTER - для использования `boost::shared_ptr`,
* PBIO_CXX11_SMART_POINTER - для использования `std::shared_ptr`.

Если ни один из этих макросов не определен, будет использован указатель, определенный в [pbio/shared_ptr/heavy_shared_ptr.h](../../../include/pbio/shared_ptr/heavy_shared_ptr.h) - усеченная версия умного указателя из OpenCV, не требующая использования OpenCV.

Во всех случаях `T::Ptr` является потокобезопасным умным указателем.

Библиотека *libfacerec* автоматически выгрузится, когда не останется интерфейсных объектов.

## Java

Для освобождения памяти от внутренних объектов до того, как это автоматически сделает сборщик мусора, используйте метод `dispose()`. После того, как этот метод вызван, от объекта можно вызвать только метод `isDisposed()`, в случае вызова любого другого метода будет выброшено исключение.

Пример:
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

Для освобождения памяти от внутренних объектов до того, как это автоматически сделает сборщик мусора, используйте метод `Dispose()`. После того, как этот метод вызван, объект может получать только значения свойства `IsDisposed`, в противном случае будет выброшено исключение.

Пример:
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
