# Детекция и распознавание лиц в масках

Методы для распознавания лиц в масках были добавлены в Face SDK 3.4. Для детекции  и распознавания лиц в масках необходимо использовать конфигурационный файл `video_worker_fdatracker_blf_fda.xml` для детекции и один из указанных ниже конфигурационных файлов для распознавания: 

* `method9v30mask_recognizer.xml`
* `method9v300mask_recognizer.xml`
* `method9v1000mask_recognizer.xml`

Указанные методы (9.30, 9.300, 9.1000) были оптимизированы для распознавания лиц в масках. Более подробная информация о методах распознавания и конфигурационных файлах представлена в пункте [Идентификация лиц](/doc/ru/development/face_identification.md).

Для того, чтобы включить функцию детекции и распознавания лиц в Face SDK, следуйте инструкциям в пунктах, указанных ниже. 

## Использование предсобранного приложения video_recognition_demo

1. При запуске *video_recognition_demo* передайте конфигурационный файл `video_worker_fdatracker_blf_fda.xml` в параметре `--vw_config_file`. 
2. [Опционально] Передайте один из указанных выше методов распознавания в параметре `--method_config`.

Полную инструкцию по запуску сэмпла и подробную информацию о параметрах запуска см. в пункте [cpp/video_recognition_demo](/doc/ru/samples/cpp/video_recognition_demo.md).

## Сборка примера из исходного кода

1. Передайте `video_worker_fdatracker_blf_fda.xml` в параметре `video_worker_config` при создании объекта `VideoWorker` ([С++](/examples/cpp/video_recognition_demo/src/main.cpp#L125), [C#](/examples/csharp/video_recognition_demo/src/Main.cs#L126), [Java](/examples/android/video_recognition_demo/src/main/java/com/vdt/face_recognition/video_recognition_demo/VidRecDemo.java#L106)).
2. [Опционально] Передайте один из указанных выше методов распознавания в параметре `recognizer_ini_file`.
3. Пересоберите приложение. 

Пример создания объекта `VideoWorker` и подробную информацию о параметрах см. в пункте [Детекция лиц](/doc/ru/development/video_stream_processing.md#детекция-лиц). 