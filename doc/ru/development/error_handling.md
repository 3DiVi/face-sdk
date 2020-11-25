# Обработка ошибок

## C++

В случае ошибки библиотека *libfacerec* выбрасывает объект исключения `pbio::Error`, поэтому возвращаемые значения всегда корректны и дополнительных проверок не требуется. Класс `pbio::Error` является производным от `std::exception`. Его метод `pbio::Error::what` возвращает строку, идентифицирующую ошибку. Его метод `pbio::Error::code` возвращает целый беззнаковый код исключения.

Пример:
```cpp
...
try
{
    pbio::Capturer::Ptr capturer = service->createCapturer("not_exist.xml");
    ...
}
catch(const pbio::Error &e)
{
    std::cerr << "facerec exception caught: '" << e.what() << "' code: " << std::hex << e.code() << std::endl;
}
// Output must be:
// facerec exception caught: 'Assertion 'file_storage.isOpened()' failed (error creating Capturer: ini file not opened), error code: 0x6b1ae8c5. wrap code: 0x272e96e4.' code: 6b1ae8c5
```

## Java

При возникновении ошибки выбрасывается исключение класса `com.vdt.face_recognition.sdk.SDKException`, производного от базового класса `java.lang.RuntimeException`. Его метод `com.vdt.face_recognition.sdk.SDKException.getMessage()` возвращает строку с описанием ошибки, а `com.vdt.face_recognition.sdk.SDKException.code()` возвращает целый беззнаковый код исключения.

Пример:
```java
...
try
{
    service.createCapturer(service.new Config("not_exist.xml"));
}
catch (SDKException e)
{
    System.out.println("facerec exception caught: '" + e.getMessage() + "'");
}
// Output must be:
// facerec exception caught: 'Assertion 'file_storage.isOpened()' failed (error creating Capturer: ini file not opened), error code: 0x6b1ae8c5. wrap code: 0x7c7b4c95.'
```

## C#

При возникновении ошибки выбрасывается исключение класса `VDT.FaceRecognition.SDK.Error`, производного от `System.Exception`. Его свойство `VDT.FaceRecognition.SDK.Error.Message` возвращает строку с описанием ошибки, a метод `VDT.FaceRecognition.SDK.Error.Code` возвращает целый беззнаковый код исключения.

Пример:
```cs
...
try
{
    service.createCapturer("not_exist.xml");
}
catch (Error e)
{
    Console.WriteLine("facerec exception caught: '" + e.message() + "'");
}
// Output must be:
// facerec exception caught: 'Assertion 'file_storage.isOpened()' failed (error creating Capturer: ini file not opened), error code: 0x6b1ae8c5. wrap code: 0x7c7b4c95.'
```

## Python 

При возникновении ошибки выбрасывается исключение класса `Error`, производного от класса `Exception`. Его свойство `Error.message` возвращает строку с описанием ошибки, а метод `Error.code` возвращает hex код исключения.

Пример:
```python
...
from face_sdk_3divi import Error

try:
    service.create_capturer(Config("not_exist.xml"))
except Error as e:
    print("facerec exception caught: '{}'".format(e))


# Output must be:
# facerec exception caught: '0x2e5d42a1: Assertion 'file_storage.isOpened()' failed (error creating Capturer: ini file not opened. System error description: No such file or directory), error code: 0x2e5d42a1. wrap code: 0x0d33c4f6.'
```