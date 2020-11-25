# Обработка видеопотока

Интерфейсный объект `VideoWorker` может быть использован для:

* [детекции лиц на видеопотоках](#детекция-лиц)
* [создания шаблонов лиц](#создание-шаблонов)
* [распознавания лиц](#распознавание-лиц)
* [определения пола, возраста и эмоций лиц](#определение-пола-возраста-и-эмоций)
* [сравнения лиц, найденных за установленный период, друг с другом](#кратковременная-идентификация)

Объект `VideoWorker` берет на себя всю необходимую синхронизацию и контроль потоков, вам необходимо только подавать кадры видеопотока и реализовать несколько коллбэков.

Пример использования объекта `VideoWorker` см. в [video_recognition_demo](../samples/cpp/video_recognition_demo.md).  
Узнайте, как детектировать и отслеживать лица на видеопотоке в нашем туториале [Детекция и трекинг лиц на видеопотоке](../tutorials/face_detection_and_tracking_in_a_video_stream.md).  
Узнайте, как распознавать лица на видеопотоке в нашем туториале [Распознавание лиц на видеопотоке](../tutorials/face_recognition_in_a_video_stream.md).  

## Детекция лиц

_**Примечание:** Узнайте, как детектировать лица в масках, [в нашем туториале](/doc/ru/tutorials/detection_and_recognition_of_masked_faces.md)._

Объект `VideoWorker` может быть создан с помощью метода `FacerecService.createVideoWorker`.

### C++ пример

<details>
  <summary>Нажмите, чтобы развернуть</summary>
    
```cpp
pbio::FacerecService::Config video_worker_config("video_worker_lbf.xml");
video_worker_config.overrideParameter("search_k", 3);
pbio::VideoWorker::Ptr video_worker = service->createVideoWorker(
    pbio::VideoWorker::Params()
        .video_worker_config(video_worker_config)
        .recognizer_ini_file(recognizer_config)
        .streams_count(streams_count)
        .processing_threads_count(processing_threads_count)
        .matching_threads_count(matching_threads_count)
        .age_gender_estimation_threads_count(age_gender_estimation_threads_count)
        .emotions_estimation_threads_count(emotions_estimation_threads_count)
        .short_time_identification_enabled(enable_sti)
        .short_time_identification_distance_threshold(sti_recognition_threshold)
        .short_time_identification_outdate_time_seconds(sti_outdate_time)
    );
```
</details>

### C# пример

<details>
  <summary>Нажмите, чтобы развернуть</summary>

```cs
FacerecService.Config video_worker_config = new FacerecService.Config("video_worker_lbf.xml");
video_worker_config.overrideParameter("search_k", 3);
VideoWorker video_worker = service.createVideoWorker(
    new VideoWorker.Params()
        .video_worker_config(video_worker_config)
        .recognizer_ini_file(recognizer_config)
        .streams_count(streams_count)
        .processing_threads_count(processing_threads_count)
        .matching_threads_count(matching_threads_count)
        .age_gender_estimation_threads_count(age_gender_estimation_threads_count)
        .emotions_estimation_threads_count(emotions_estimation_threads_count)
        .short_time_identification_enabled(enable_sti)
        .short_time_identification_distance_threshold(sti_recognition_threshold)
        .short_time_identification_outdate_time_seconds(sti_outdate_time)
    );
```
</details>

### Java пример 

<details>
  <summary>Нажмите, чтобы развернуть</summary>

```java
FacerecService.Config video_worker_config = service.new Config("video_worker_lbf.xml");
video_worker_config.overrideParameter("search_k", 3);
VideoWorker video_worker = service.createVideoWorker(
    new VideoWorker.Params()
        .video_worker_config(video_worker_config)
        .recognizer_ini_file(recognizer_config)
        .streams_count(streams_count)
        .processing_threads_count(processing_threads_count)
        .matching_threads_count(matching_threads_count)
        .age_gender_estimation_threads_count(age_gender_estimation_threads_count)
        .emotions_estimation_threads_count(emotions_estimation_threads_count)
        .short_time_identification_enabled(enable_sti)
        .short_time_identification_distance_threshold(sti_recognition_threshold)
        .short_time_identification_outdate_time_seconds(sti_outdate_time)
    );
```
</details>

### Python пример 

<details>
  <summary>Нажмите, чтобы развернуть</summary>
  
```python
video_worker_config = Config("video_worker_lbf.xml")
video_worker_config.override_parameter("search_k", 3)

video_worker_params = video_worker.Params()
video_worker_params.video_worker_config = video_worker_config
video_worker_params.recognizer_ini_file = recognizer_config
video_worker_params.streams_count = streams_count
video_worker_params.processing_threads_count = processing_threads_count
video_worker_params.matching_threads_count = matching_threads_count
video_worker_params.age_gender_estimation_threads_count = age_gender_estimation_threads_count
video_worker_params.emotions_estimation_threads_count = emotions_estimation_threads_count

video_worker = service.create_video_worker(video_worker_params)
```
</details>
    
Где:

* `video_worker_config` – путь до конфигурационного файла для `VideoWorker` или объект `FacerecService.Config`.
* `video_worker_params` – параметры конструктора `VideoWorker`.
* `recognizer_config` – конфигурационный файл для используемого распознавателя (см. [Идентификация лиц](face_identification.md)).
* `streams_count` – количество видеопотоков, для каждого из которых будет создан поток трекинга.
* `processing_threads_count` – количество потоков для создания шаблонов. Эти потоки общие для всех видеопотоков и распределяют ресурсы равномерно по все видеопотокам независимо от их загруженности (за исключение видеопотоков без лиц в кадре).
* `matching_threads_count` – количество потоков для сравнения шаблонов, созданных из видеопотоков, с базой. Как и потоки обработки, они распределяют работу равномерно по всем видеопотокам.
* `age_gender_estimation_threads_count` – количество потоков для определения пола и возраста. Как и потоки обработки, они распределяют работу равномерно по всем видеопотокам.
* `emotions_estimation_threads_count` – количество потоков для определения эмоций. Как и потоки обработки, они распределяют работу равномерно по всем видеопотокам.
* `enable_sti` – флаг, включающий [кратковременную идентификацию](#кратковременная-идентификация).
* `sti_recognition_threshold` – порог распознавания для [кратковременной идентификации](#кратковременная-идентификация).
* `sti_outdate_time` – длина временного интервала для [кратковременной идентификации](#кратковременная-идентификация) в секундах.

Существует три конфигурационных файла, использующие алгоритм трекинга из `common_video_capturer.xml`:
* `video_worker.xml` с набором точек *esr*
* `video_worker_lbf.xml` с набором точек *singlelbf*
* `video_worker_fda.xml` с набором точек *fda*

и три конфигурационных файла, использующие алгоритм трекинга из `fda_tracker_capturer.xml`:
* `video_worker_fdatracker.xml` с набором точек *fda*
* `video_worker_fdatracker_fake_detector.xml` с набором точек *fda*  
* `video_worker_fdatracker_blf_fda.xml` с набором точек *fda*

(см. [Антропометрические точки](face_capturing.md#антропометрические-точки), [Класс Capturer](face_capturing.md#класс-capturer))

В случае, если `VideoWorker` используется только для детекции лиц, указываются параметры `matching_thread=0` и `processing_thread=0`, и потребляется обычная лицензия [Face Detector](../components.md#facedetector). Для создания компонента *Face Detector* для одного потока необходимо указать параметр `streams_count=1`.

Для подачи кадров видеопотока используйте метод `VideoWorker.addVideoFrame`. Этот метод потокобезопасен, поэтому можно подавать кадры из разных потоков, созданных для каждого видеопотока, без дополнительной синхронизации. Метод возвращает целочисленный идентификатор кадра, который будет использоваться в коллбэках для обозначения этого кадра.

Детекция лиц осуществляется посредством использования двух коллбэков:

* `VideoWorker.TrackingCallbackU` возвращает результаты трекинга. Этот коллбэк вызывается каждый раз, когда кадр был обработан конвейером трекинга. *Tracking*-коллбэк будет вызван с `frame_id` равным `X` не раньше, чем `VideoWorker.addVideoFrame` вернет значение `X + N – 1`, где `N` – значение, которое вернул `VideoWorker.getTrackingConveyorSize`. *Tracking*-коллбэки с одинаковым идентификатором `stream_id` вызываются в порядке возрастания `frame_id`. Поэтому если был получен коллбэк с `stream_id=2` и `frame_id=102` сразу после коллбэка с `stream_id=2` и `frame_id=100`, значит, кадр с `frame_id=101` был пропущен для видеопотока 2. Большинство сэмплов созданы с кадра `frame_id`, но некоторые могут быть получены с предыдущих кадров. Используйте метод `RawSample.getFrameID` для того, чтобы определить, какому кадру сэмпл принадлежит на самом деле. Чтобы подписаться на этот коллбэк, используйте метод `VideoWorker.addTrackingCallbackU`, чтобы отписаться – метод `VideoWorker.removeTrackingCallback`, подав `callback_id`, который вы получили от `VideoWorker.addTrackingCallbackU`.

* `VideoWorker.TrackingLostCallbackU` возвращает лучший сэмпл и шаблон лица, когда трекинг потерян (например, когда человек вышел из кадра). Лучший сэмпл может быть пустым, если включен конфигурационный параметр `weak_tracks_in_tracking_callback`. Гарантируется, что этот коллбэк будет последним для пары `<stream_id, track_id>` (`track_id` равен `sample.getID()` для сэмпла в любом *VideoWorker*-коллбэке). Т.е. после него ни один *Tracking*-, *MatchFound*- или *TrackingLost*-коллбэк для видеопотока `stream_id` не может содержать сэмпла с этим же идентификатором `track_id`. Также гарантируется, что для каждой пары `<stream_id, track_id>`, которая была упомянута в *Tracking*-коллбэке, существует ровно один *TrackingLost*-коллбэк, за исключением треков, удаленных в процессе `VideoWorker.resetStream` – для них *TrackingLost*-коллбэк не будет вызван. Используйте возвращаемое значение `VideoWorker.resetStream` для освобождения памяти, выделенной для этих треков. Чтобы подписаться на этот коллбэк, используйте метод `VideoWorker.addTrackingLostCallbackU`. Чтобы отписаться от коллбэка, используйте метод `VideoWorker.removeTrackingLostCallback`, предоставив `callback_id`, который вы получили от метода `VideoWorker.addTrackingLostCallbackU`.

_**Примечание:** Исключения, возникшие в коллбэках, будут перехвачены и сгенерированы заново в методе `VideoWorker.checkExceptions`, поэтому не забывайте регулярно вызывать метод `VideoWorker.checkExceptions` для контроля ошибок._

_**Предупреждение:** Не вызывайте методы, которые изменяют состояние `VideoWorker` внутри коллбэков, во избежание взаимной блокировки. Т.е. безопасными для вызова в коллбэках являются только методы `VideoWorker.getMethodName` и `VideoWorker.getStreamsCount`._

## Создание шаблонов

Если помимо детекции требуется создание шаблонов, указываются параметры `matching_thread=0` и `processing_thread>0`, и потребляется лицензия [Video Engine Standard](../components.md#video-engine-standard-ves). Для создания компонента *Video Engine Standard* для одного потока необходимо указать параметры `streams_count=1`, `processing_threads_count=1`, `matching_threads_count=0`.

Вы можете отключить / включить создание шаблонов для конкретного видеопотока с помощью методов `VideoWorker.disableProcessingOnStream` и `VideoWorker.enableProcessingOnStream`. При старте создание шаблонов включено для всех видеопотоков.

Коллбэк `VideoWorker.TemplateCreatedCallbackU` возвращает результаты генерации шаблонов. Этот коллбэк вызывается каждый раз, когда внутри `VideoWorker` создается шаблон. Гарантируется, что этот коллбэк будет вызван после как минимум одного *Tracking*-коллбэка и перед *TrackingLost*-коллбэком с теми же `stream_id` и `track_id` (`track_id = sample->getID()`). Чтобы подписаться на этот коллбэк, используйте метод `VideoWorker.addTemplateCreatedCallbackU`. Чтобы отписаться от коллбэка, используйте метод `VideoWorker.removeTemplateCreatedCallback`, предоставив `callback_id`, который вы получили от метода `VideoWorker.addTemplateCreatedCallbackU`.

## Распознавание лиц

В случае, если требуется детекция лиц, создание шаблонов и сравнение их с базой (распознавание), указываются параметры `matching_thread>0` и `processing_thread>0`, и потребляется лицензия [Video Engine Extended](../components.md#video-engine-extended-vee). Для создания компонента *Video Engine Extended* для одного потока необходимо указать параметры `streams_count=1`, `processing_threads_count=1`, `matching_threads_count=1`.

Для задания или изменения базы используйте метод `VideoWorker.setDatabase`. Он может быть вызван в любое время.

Коллбэк `VideoWorker.MatchFoundCallbackU` возвращает результат сравнения с базой. Когда для трекаемого лица создается шаблон, он сравнивается с каждым шаблоном из базы, и если расстояние до ближайшего элемента оказывается меньше порога `distance_threshold`, указанного в этом элементе, то фиксируется совпадение. Этот коллбэк вызывается после `N` последовательных совпадений с элементами, относящимися к одному и тому же человеку.

Также выставив значение тэга `<not_found_match_found_callback>` равным `1` можно включить вызов этого коллбэка после `N` последовательных несовпадений (т.е. когда ближайший элемент оказывается дальше его порога `distance_threshold`). В этом случае идентификатор `match_result` первого элемента в `MatchFoundCallbackData.search_result` будет с нулевым расстоянием, а идентификаторы `person_id` и `element_id` будут равны `VideoWorker.MATCH_NOT_FOUND_ID`. Число `N` может быть задано в конфигурационном файле в тэге `<consecutive_match_count_for_match_found_callback>`.

Гарантируется, что этот коллбэк будет вызван после как минимум одного *Tracking*-коллбэка и перед *TrackingLost*-коллбэком с тем же `stream_id` и `track_id` (`track_id = sample->getID()`). Чтобы подписаться на этот коллбэк, используйте метод `VideoWorker.addMatchFoundCallbackU`. Чтобы отписаться от коллбэка, используйте метод `VideoWorker.removeMatchFoundCallback`, предоставив `callback_id`, который вы получили от метода `VideoWorker.addMatchFoundCallbackU`. Максимальное количество элементов, возвращаемое в `MatchFoundCallbackData.search_result`, устанавливается в конфигурационном файле под тэгом `search_k` и может быть изменено с помощью объекта `FacerecService.Config.overrideParameter`, например: `video_worker_config.overrideParameter(search_k, 3);`

## Определение пола, возраста и эмоций

Для определения пола и возраста указывается параметр `age_gender_estimation_threads_count > 0`. Для определения эмоций указывается параметр `emotions_estimation_threads_count > 0`. Данные о поле, возрасте и эмоциях приходят в коллбэк `VideoWorker.TrackingCallbackU`. Данные об эмоциях обновляются постоянно. Данные о поле и возрасте пересчитываются только в том случае, если найден сэмпл с лучшим качеством. Сразу после создания `VideoWorker` определение пола, возраста и эмоций включено.

Отключить определение пола, возраста и эмоций на конкретном потоке можно при помощи методов:
* `VideoWorker.disableAgeGenderEstimationOnStream` (пол и возраст)
* `VideoWorker.disableEmotionsEstimationOnStream` (эмоции)

Снова включить определение пола, возраста и эмоций на конкретном потоке можно при помощи методов:
* `VideoWorker.enableAgeGenderEstimationOnStream` (пол и возраст)
* `VideoWorker.enableEmotionsEstimationOnStream` (эмоции)

## Кратковременная идентификация

Кратковременная идентификация (*short time identification, STI*) позволяет узнать трек как человека, проходившего перед камерой некоторое время назад, даже если этот человек отсутвует в базе и даже если распознавание отключено. Таким образом, один и тот человек, который в течение, например, одной минуты несколько раз выходил из кадра и снова заходил в кадр, будет идентифицироваться как одно и то же лицо.

Если кратковременная идентификация включена, `VideoWorker` сравнивает треки, на которых лицо было потеряно, с другими треками, на которых лицо было потеряно не более, чем `sti_outdate_time` секунд назад. Совпавшие треки объединяются в группу `sti_person`. ID группы `sti_person` (`sti_person_id`) возвращается в коллбэке `VideoWorker.TrackingLostCallbackU`. Значение `sti_person_id` совпадает со значением `track_id` первого элемента, сформировавшего группу `sti_person`. Когда определенная группа `sti_person` превышает временной промежуток `sti_outdate_time`, вызывается коллбэк `VideoWorker.StiPersonOutdatedCallbackU`.

Кратковременная идентификация не влияет на использование лицензии. Для использования этой функции требуется по крайней мере один поток для создания шаблонов (`processing_thread>0`).
