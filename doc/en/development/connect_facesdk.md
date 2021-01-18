# Connecting Face SDK to Your Project 

## Adding Library to the Project

Required dependencies

To use Face SDK, you have to install the following dependencies:

* [Windows] `sense4.dll`. This library is located in the *bin* folder in Face SDK and should be located next to `facerec.dll` if you change the default location
* [Windows 64-bit] `tensorflow.dll`. This library is located in the *bin* folder of the Face SDK distribution package. It must be located in the directory from which the application is launched or you must add the path to the directory with this library to the `PATH` system variable
* [Windows 64-bit] [Microsoft Visual C++ Redistributable for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/)
* [Linux] `libusb-0.1.so.4`
* [Linux 64-bit] `libtensorflow.so.2`. This library is located in the *lib* folder in Face SDK

### C++ 

The `libfacerec` library is loaded dynamically **in runtime mode** when the static method `pbio::FacerecService::createService` is called. Therefore, **do not** link your program with `libfacerec.so` (Linux) or `facerec.dll` (Windows). You only need to add the *include* directory to include directories, or, for Linux projects, to link the `libdl` library with the `-export-dynamic` flag.

_**Warning:** You can call `pbio::FacerecService::createService` only once, otherwise, you may encounter a crash._

All *libfacerec* declarations are divided in two headers:

* [`facerec/import.h`](../../../include/facerec/import.h) – should be included only once (for example, in `main.cpp`)
* [`facerec/libfacerec.h`](../../../include/facerec/libfacerec.h) – should be included in each file that uses library

The *libfacerec* library does not have any actual third-party dependencies*, since all used third-party libraries (see [Open Source Licenses](../../open_source_licenses)) are linked statically. Therefore, any version of these libraries should work for examples, in which OpenCV or boost is used.

\* – except for the *linux x86_64* and *windows_x86_64* distribution packages. At the moment, they have a dependency on `libtensorflow.so.2` and` tensorflow.dll`, respectively.

#### Library Version

Following preprocessor constants specify the current library version:

* `LIBFACEREC_VERSION` – string representation, e.g. "1.2.34", where 1 is the major version, 2 is the minor version, 34 is the patch version
* `LIBFACEREC_VERSION_HEX` – 3-byte HEX representation that is useful in numeric comparisons, e.g. 0x010234

### Java

To use `facerec.jar` in a project, specify the path to this file in `classpath`. The `facerec.jar` Java library is a wrapper for the C++ library, so `libfacerec_jni.so` and `libfacerec.so` for Linux and Android or `facerec_jni.dll` and `facerec.dll` for Windows are required in runtime. Path to the directory containing `lifacerec_jni.so` or `facerec_jni.dll` should be specified in `java.library.path`.

### C#

To use *facerec* in a project, add reference to `FacerecCSharpWrapper.dll` in a project. Add `facerec.so` (for Linux) or `facerec.dll` (for Windows) and `FacerecCSharpWrapper.dll` to the folder with executable file.

## Using the libfacerec library 

To use the *libfacerec* library, call the `FacerecService.createService` method, which dynamically loads the library.

The `FacerecService.createService` method has an overloading signature and can be called:
* by passing the path to the directory with the license (from which the `3divi_face_sdk.lic` license is loaded)
* by passing the license in the form of a special structure: `FacerecService.License`

_**Warning:** You can call `FacerecService.createService` only once, otherwise, you may encounter a crash._

### C++ example
<details>
  <summary>Click to expand</summary>

```cpp
#include <iostream>
#include <exception>
#include <facerec/import.h>
#include <facerec/libfacerec.h>
int main (int argc, char** argv)
{
    try
    {
        pbio::FacerecService::Ptr service;
#ifdef _WIN32
        service = pbio::FacerecService::createService("../bin/facerec.dll", "../conf/facerec/");
#else
        service = pbio::FacerecService::createService("../lib/libfacerec.so", "../conf/facerec/");
#endif
        const pbio::Recognizer::Ptr recognizer = service->createRecognizer("method7v7_recognizer.xml");
        pbio::FacerecService::Config capturer_config("common_capturer4_fda.xml");
        capturer_config.overrideParameter("min_size", 200);
        pbio::Capturer::Ptr capturer = service->createCapturer(capturer_config);
        // ...
    }
    catch(const pbio::Error &e)
    {
        std::cerr << "facerec exception catched: '" << e.what() << "' code: " << std::hex << e.code() << std::endl;
    }
    catch(const std::exception &e)
    {
        std::cerr << "exception catched: '" << e.what() << "'" << std::endl;
    }
}
```
</details>

### C# example
<details>
  <summary>Click to expand</summary>

```cs
using System;
using VDT.FaceRecognition.SDK;
namespace Example
{
    public class Example
    {
        public static void Main(string []args)
        {
            try
            {
                String faceSDKRootDir = "/path/to/face_sdk";
                FacerecService service = FacerecService.createService(faceSDKRootDir + "/conf/facerec", "");
                Recognizer recognizer = service.createRecognizer("method7v7_recognizer.xml", true, true, false);
                FacerecService.Config capturerConfig = new FacerecService.Config("common_capturer4_fda.xml");
                capturerConfig.overrideParameter("min_size", 200);
                Capturer capturer = service.createCapturer(capturerConfig);
                
                // ...
            }
            catch (Error e)
            {
                Console.WriteLine(e.Message);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}
```
</details>

### Java example
<details>
  <summary>Click to expand</summary>
  
```java
package example;
import java.lang.String;
import java.lang.Exception;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.Recognizer;
import com.vdt.face_recognition.sdk.Capturer;
import com.vdt.face_recognition.sdk.SDKException;
public class Example
{
    public static void main(String []args)
    {
        try
        {
            final String faceSDKRootDir = "/path/to/face_sdk";
            final FacerecService service = FacerecService.createService(faceSDKRootDir + "/lib/libfacerec.so", faceSDKRootDir + "/conf/facerec", "");
            final Recognizer recognizer = service.createRecognizer("method7v7_recognizer.xml", true, true, false);
            FacerecService.Config capturerConfig = service.new Config("common_capturer4_fda.xml");
            capturerConfig.overrideParameter("min_size", 200);
            final Capturer capturer = service.createCapturer(capturerConfig);
            
            // ...
        }
        catch (SDKException e)
        {
            System.out.println(e.getMessage());
        }
        catch (Exception e)
        {
            System.out.println(e.getMessage());
        }
    }
}
```
</details>

### Python example

<details>
  <summary>Click to expand</summary>
  
```python
from face_sdk_3divi import Config, FacerecService, Error


if __name__ == '__main__':
    try:
        face_sdk_root_dir = "path/to/face_sdk"
        service = FacerecService.create_service(face_sdk_root_dir + "/lib/libfacerec.so",     face_sdk_root_dir + "/conf/facerec")
        recognizer = service.create_recognizer("method7v7_recognizer.xml", True, True, False)
        capturer_config = Config("common_capturer4_fda.xml")
        capturer = service.create_capturer(capturer_config)

        # ...
    except Error as e:
        print(e)
```  
</details>

### Examples of calling the createService method with a preloaded (stored in a string variable) license

#### C++

```cpp
std::string license_body = "<?xml version="1.0" encoding="utf-8"?><License>........"; // contents of your license
auto license = pbio::FacerecService::License(license_body);
const pbio::FacerecService::Ptr service = pbio::FacerecService::createService("../bin/facerec.dll", "../conf/facerec/", license);
```

#### C#

```cs
string license_body = "<?xml version="1.0" encoding="utf-8"?><License>........"; // contents of your license
FacerecService  service = FacerecService.createService(faceSDKRootDir + "/conf/facerec", new FacerecService.License(license_body));
```

#### Java 

```java
String license_body = "<?xml version="1.0" encoding="utf-8"?><License>........"; // contents of your license
final FacerecService service = FacerecService.createService(faceSDKRootDir + "/lib/libfacerec.so", faceSDKRootDir + "/conf/facerec", new FacerecService.License(license_body));
```
