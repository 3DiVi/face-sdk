# Подключение Face SDK к проекту 

## Добавление библиотеки в проект

### Требуемые зависимости

Для работы с Face SDK Вам необходимо установить следующие зависимости:

* [Windows] библиотека `sense4.dll`. Эта библиотека находится в дистрибутиве Face SDK в папке *bin* и должна оставаться рядом с библиотекой `facerec.dll` в случае перемещения.
* [Windows] библиотека `tensorflow.dll`. Эта библиотека находится в дистрибутиве Face SDK в папке *bin*. Она должна располагаться в директории, из которой запускается приложение, либо необходимо добавить путь до директории с этой библиотекой в системную перемнную `PATH` 
* [Windows] [Microsoft Visual C++ Redistributable for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/)
* [Linux] библиотека `libusb-0.1.so.4`
* [Linux] библиотека `libtensorflow.so.2`. Эта библиотека находится в дистрибутиве Face SDK в папке *lib*

### C++

Бибилотека `libfacerec` загружается динамически во время выполнения при вызове статического метода `pbio::FacerecService::createService`, поэтому не нужно линковать программу с `libfacerec.so` (Linux) или `facerec.dll` (Windows). Необходимо только добавить каталог *include* к директориям заголовочных файлов и для проектов под Linux – слинковать с библиотекой `libdl` с флагом `-export-dynamic`.

_**Предупреждение:** Вызывать `pbio::FacerecService::createService` можно только один раз, иначе может произойти сбой._

Все объявления `libfacerec` находятся в двух заголовочных файлах:
* [`facerec/import.h`](../../../include/facerec/import.h) – должен быть включен ровно один раз (например, в `main.cpp`)
* [`facerec/libfacerec.h`](../../../include/facerec/libfacerec.h) – должен быть включен в каждом файле, использующем библиотеку

Библиотека `libfacerec` не имеет фактических сторонних зависимостей, т.к. все использованные сторонние библиотеки (см. [Лицензии свободного ПО](../../open_source_licenses)) слинкованы статически, поэтому для примеров, в которых используется *OpenCV* или *boost*, должны подходить любые версии указанных библиотек.

#### Версия библиотеки

Текущая версия библиотеки содержится в следующих константах препроцессора:

* `LIBFACEREC_VERSION` – строковое представление, например, `1.2.34`, где `1` – основная версия, `2` – дополнительная версия, `34` – версия патча
* `LIBFACEREC_VERSION_HEX` – трехбайтовое представление версии, удобное для численного сравнения, например, `0x010234`

### Java

Для использования `facerec.jar` в проекте укажите путь до этого файла в `classpath`. Java-библиотека `facerec.jar` является оберткой над C++ библиотекой, поэтому необходимо наличие `libfacerec_jni.so` и `libfacerec.so` для Linux и Android или `facerec_jni.dll` и `facerec.dll` для Windows во время выполнения. Путь до директории, содержащей `libfacer_jni.so` или `facerec_jni.dll`, должен быть указан в `java.library.path`.

### C#
Для использования библиотеки необходимо добавить ссылку на `FacerecCSharpWrapper.dll` в ваш проект. После сборки проекта добавьте `facerec.so` (для Linux) или `facerec.dll` (для Windows) и `FacerecCSharpWrapper.dll` в директорию с исполняемым файлом.

## Использование библиотеки libfacerec 

Работа с библиотекой `libfacerec` начинается с вызова статического метода `FacerecService.createService`, который динамически загрузит библиотеку.

Метод `FacerecService.createService` иммеет перегруженную сигнатуру и может быть вызван: 
* с указанием пути до директории с лицензией (из которой будет подгружена лицензия `3divi_face_sdk.lic`)
* с передачей лицензии в виде специальной структуры: `FacerecService.License`

_**Предупреждение:** Вызывать `FacerecService.createService` можно только один раз, иначе может произойти сбой._

_**Примечание:** Вам не обязательно удерживать объект `FacerecService` все время работы с библиотекой. Библиотека `libfacerec` автоматически выгрузится, когда не останется интерфейсных объектов._

### C++ пример

<details>
  <summary>Нажмите, чтобы развернуть</summary>

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
        const pbio::Recognizer::Ptr recognizer = service->createRecognizer("method9v300_recognizer.xml");
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
  
### C# пример  

<details>
  <summary>Нажмите, чтобы развернуть</summary>

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
                Recognizer recognizer = service.createRecognizer("method9v300_recognizer.xml", true, true, false);
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

### Java пример  

<details>
  <summary>Нажмите, чтобы развернуть</summary>

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
            final Recognizer recognizer = service.createRecognizer("method9v300_recognizer.xml", true, true, false);
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

### Python пример 

<details>
  <summary>Нажмите, чтобы развернуть</summary>
  
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

### Примеры вызова метода createService с предварительно загруженной (сохраненной в строковой переменной) лицензией

#### C++

```cpp
std::string license_body = "<?xml version="1.0" encoding="utf-8"?><License>........"; // содержимое вашей лицензии
auto license = pbio::FacerecService::License(license_body);
const pbio::FacerecService::Ptr service = pbio::FacerecService::createService("../bin/facerec.dll", "../conf/facerec/", license);
```

#### C#

```cs
string license_body = "<?xml version="1.0" encoding="utf-8"?><License>........"; // содержимое вашей лицензии
FacerecService  service = FacerecService.createService(faceSDKRootDir + "/conf/facerec", new FacerecService.License(license_body));
```

#### Java

```java
String license_body = "<?xml version="1.0" encoding="utf-8"?><License>........"; // содержимое вашей лицензии
final FacerecService service = FacerecService.createService(faceSDKRootDir + "/lib/libfacerec.so", faceSDKRootDir + "/conf/facerec", new FacerecService.License(license_body));
```
