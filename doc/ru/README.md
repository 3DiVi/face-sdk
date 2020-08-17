<p align="center">
<a href="https://face.3divi.com/ru"><img src="../img/3divi_logo.png" width="600" height="150" title="3DiVi Logo" alt="Logo"></a>
</p>

<div align="center"><a href="../../README.md">English</a> | Русский </div>

# Введение

**3DiVi Face SDK** – это набор программных компонентов (библиотек) для разработки решений по распознаванию лиц любой сложности – от простых приложений до порталов, таких как Azure Face API или AWS Rekognition.

Функции Face SDK: 
* [Детекция и трекинг лиц](development/face_capturing.md) на изображениях и видео 
* [Распознавание лиц](development/face_identification.md) на изображениях и видео 
* [Оценка лиц](development/face_estimation.md)(пол, возраст, эмоции, принадлежность лица живому человеку) на изображениях и видео
* [Обработка видеопотока](development/video_stream_processing.md) – детекция, трекинг и распознавание лиц на видеопотоках 

# Поддерживаемые платформы и API

В настоящий момент доступна поддержка следующих платформ и архитектур:

* Windows (x86 32-bit, x86 64-bit)
* Linux (x86 32-bit, x86 64-bit, ARM 32-bit, ARM 64-bit)
* Android (ARM 32-bit, ARM 64-bit)
* iOS (ARM 64-bit)

Face SDK включает в себя следующие API:

* C++ API (для платформ Windows, Linux, Android, iOS)
* Java-обертка (для платформ Windows, Linux, Android)
* C#-обертка (для платформ Windows, Linux)

# Начало работы с Face SDK 

* [Компоненты](components.md) – важная информация о компонентах Face SDK
* [Сценарии использования](use_cases.md) – схемы, показывающие возможные сценарии использования и связь между компонентами
* [Сэмплы](samples) – программы на языках C++/C#/Java, демонстрирующие функции Face SDK
* [Туториалы](tutorials) – пошаговые туториалы по разработке проектов с трекингом и распознаванием лиц, а также определением пола, возраста и эмоций лица
* [Лицензии](licenses.md) – информация о разных типах лицензий, активации и привязке лицензий 
* [Параметры производительности](performance_parameters.md) – загрузка ЦП и использование памяти, качество идентификации (FAR/FRR) 

# Пробная версия Face SDK 

Для начала работы с Face SDK загрузите бесплатную [пробную версию Face SDK](https://face.3divi.com/ru/products/face_sdk/face_sdk_trial).

_**Примечание:** Триальная лицензия не поддерживается на платформе iOS. Пожалуйста, свяжитесь с нами для получения триальной лицензии для платформы iOS: face@3divi.com_

Для того, чтобы снять ограничения пробной версии, приобретите *лицензию Face SDK* и *Developer Pack*, который является прекрасным инструментом для разработчиков, которые хотели бы интегрировать Face SDK в свой проект, не потратив при этом много усилий и средств. За небольшую фиксированную плату Вы получаете необходимое количество лицензий разработчика, а также доступ к расширенной технической поддержке, в том числе отдельный чат с нашими старшими программистами. В зависимости от Вашего сценария использования, Вы можете приобрести лицензии для любого набора [компонентов Face SDK](components.md). 

Если у Вас есть какие-либо вопросы, напишите нам: face@3divi.com 

# Приступая к работе

1. Загрузите [бесплатную пробную версию Face SDK](https://face.3divi.com/ru/products/face_sdk/face_sdk_trial) или [приобретите лицензию Face SDK](https://face.3divi.com/ru/pricing).
2. Клонируйте или скачайте данный репозиторий.
3. Загрузите компоненты среды выполнения (*runtime*) Face SDK, используя Git LFS:
```
$ git lfs install
$ git lfs pull
```
4. Распакуйте один из архивов, в зависимости от используемой платформы, в любую директорию на вашем устройстве:  `windows_x86_64.zip`, `windows_x86_32.zip`, `linux_x86_64.tar.xz`, `linux_x86_32.tar.xz`, `linux_armhf_32.tar.xz`, `linux_aarch64.tar.xz`, `android_arm_32.tar.xz`, `android_arm_64.tar.xz`, `ios_arm_64.tar.xz`. В пути распаковки укажите корневую папку Face SDK – распакованные папки *bin* и *lib* должны находиться на одном уровне с директориями *conf, docs, examples, include, share*.

<p align="center">
<img width="700" src="../img/cpp_extract_OS.png"><br>
<b>Путь распаковки архива - корневая папка дистрибутива Face SDK</b><br>
</p>

5. Скопируйте файл лицензии Face SDK (`3divi_face_sdk.lic`) в папку *license*.
5. Вы можете ознакомиться с функционалом Face SDK, запустив [сэмплы](samples), демонстрирующие работу с C++, Java и C# API.

# Разработка 

* [Подключение Face SDK к проекту](development/connect_facesdk.md) – узнайте, как добавить библиотеку libfacerec в проект 
* [Обработка видеопотока](development/video_stream_processing.md) – детекция лиц, создание шаблонов, распознавание лиц, определение пола, возраста, эмоций, кратковременная идентификация
* [Детекция лиц](development/face_capturing.md) – пользовательский вариант детекции лиц, получения информации о лицах, антропометрических точках
* [Оценка лиц](development/face_estimation.md) – пользовательский вариант определения пола, возраста, эмоций, принадлежности лица живому человеку (2D/3D)
* [Идентификация лиц](development/face_identification.md) – пользовательский вариант идентификации лиц, методы идентификации
* [Обработка ошибок](development/error_handling.md) – обработка ошибок в C++/C#/Java
* [Управление памятью](development/memory_management.md) – управление памятью в C++/C#/Java
* [Рекомендации по камерам](guidelines_for_cameras.md) – размещение камер и съемка, рекомендуемые характеристики камер
* [Межплатформенное Face SDK API. Последняя версия в формате Doxygen](http://download.3divi.com/facesdk/0d88ba7c-9a5d-45cd-897a-406fb1fca2d4/latest_docs/russian/annotated.html)

# Как я могу предложить изменения в Face SDK? 

Если Вы обнаружили баг в нашем ПО или хотели бы предложить новую функцию в Face SDK, Вы можете создать **Issue**, после чего наши разработчики рассмотрят Ваш запрос и ответят на него. Чтобы создать **Issue**: 
1. Перейдите во вкладку **Issues** этого репозитория и нажмите **New issue**. 
2. Назовите и подробно опишите Ваш запрос.
3. Нажмите **Sumbit new issue**.

Мы всегда рады обратной связи от наших клиентов! 

См. более подробную информацию об *Issues* в [документации GitHub](https://docs.github.com/en/github/managing-your-work-on-github/creating-an-issue).

# Как я могу предложить изменения в документацию Face SDK? 

1. Скопируйте данный репозиторий: нажмите на кнопку **Fork** в правом верхнем углу страницы. 
2. Внесите изменения в копии репозитория и нажмите **Commit changes**.
3. После внесения всех изменений в репозитории отобразится сообщение: *"This branch is ... commits ahead of 3DiVi:master."*, что означает, что изменения еще не были отправлены на сервер. Чтобы просмотреть изменения, нажмите **Compare** (не забудьте выбрать репозитории и ветки).
4. Если изменения корректны и Вы готовы их предложить, нажмите **Create pull request**. 
5. Назовите запрос на включение изменений и опишите его. После этого нажмите **Create Pull Request**. 

См. более подробную информацию о запросах на включение изменений (*pull requests*) в [документации GitHub](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request). 

# Лицензии свободного ПО

* [OpenSSL](../open_source_licenses/openssl.txt) (https://www.openssl.org)
* [Crypto++](../open_source_licenses/crypto++.txt) (https://www.cryptopp.com)
* [Boost](../open_source_licenses/boost.txt) (http://www.boost.org)
* [OpenCV](../open_source_licenses/opencv.txt) (http://opencv.org)

<div align="center">© All Rights Reserved. <a href="https://3divi.com/">3DiVi Inc.</a></div>
<div align="center"><a href="mailto: face@3divi.com">face@3divi.com</a></div>
