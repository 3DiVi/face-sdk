# Error Handling

## C++

In case of an error, the libfacerec library throws the `pbio::Error` exception, so the return values are always correct and no additional check is required. The `pbio::Error` class is derived from the `std::exception`. Its member function `pbio::Error::what` returns a null-terminated character sequence identifying the exception. Its member function `pbio::Error::code` returns an unsigned integer code of an exception.

Example:
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

In case of an error, an exception is thrown from the `com.vdt.face_recognition.sdk.SDKException` class derived from the  `java.lang.RuntimeException` base class. Its method `com.vdt.face_recognition.sdk.SDKException.getMessage()` returns a string with an error description, and the `com.vdt.face_recognition.sdk.SDKException.code()` method returns an unsigned integer code of exception.

Example:
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

In case of an error, an exception is thrown from the `VDT.FaceRecognition.SDK.Error` class derived from the  `VDT.FaceRecognition.SDK.Exception` class. Its property `VDT.FaceRecognition.SDK.Error.Message` returns a string with an error description, and the `VDT.FaceRecognition.SDK.Error.Code` method returns an unsigned integer code of exception.

Example:
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
