# Детекция лиц в масках

В Face SDK 3.3 добавлена возможность детектирования лиц в масках. Чтобы включить данную функцию, необходимо использовать конфигурационный файл  `video_worker_fdatracker.mask.xml` (по умолчанию в сэмплах используется файл `video_worker_fdatracker.xml`). Вы можете сделать это одним из указанных ниже способов.

## Использовать предсобранное приложение video_recognition_demo

При запуске *video_recognition_demo* передайте конфигурационный файл `video_worker_fdatracker.mask.xml` в параметре `--vw_config_file`. Полную инструкцию по запуску сэмпла и подробную информацию о параметрах запуска см. в пункте [cpp/video_recognition_demo](/doc/ru/samples/cpp/video_recognition_demo.md).

## Собрать пример из исходного кода

Передайте `video_worker_fdatracker.mask.xml` в параметре `video_worker_config` при создании объекта `VideoWorker` ([С++](/examples/cpp/video_recognition_demo/src/main.cpp#L125), [C#](/examples/csharp/video_recognition_demo/src/Main.cs#L126), [Java](/examples/android/video_recognition_demo/src/main/java/com/vdt/face_recognition/video_recognition_demo/VidRecDemo.java#L106)) и пересоберите приложение. Пример создания объекта `VideoWorker` и подробную информацию о параметрах см. в пункте [Детекция лиц](/doc/ru/development/video_stream_processing.md#детекция-лиц). 

## Заменить конфигурационный файл 

_**Внимание:** Замена конфигурационного файла `video_worker_fdatracker.xml` может повлиять на работу приложений, которые используют данный конфигурационный файл._

В качестве альтернативного варианта, Вы можете активировать детекцию лиц в масках, заменив конфигурационный файл: 

* Перейдите в папку *conf/facerec*. 
* Переименуйте файл `video_worker_fdatracker.xml` (например, в `video_worker_fdatracker_backup.xml`). 
* После этого переименуйте файл `video_worker_fdatracker.mask.xml` в `video_worker_fdatracker.xml`.