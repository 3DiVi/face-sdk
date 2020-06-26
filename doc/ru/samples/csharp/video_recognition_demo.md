# video_recognition_demo

Программа демонстрирует трекинг, детекцию и распознавание лиц из базы.

Для быстрого запуска сэмпла с параметрами по умолчанию и проверки распознавания лиц на Windows:
1. Создайте базу, которая будет использоваться при поиске лиц (см. п.1 подраздела [C++ Video Recognition Demo](../cpp/video_recognition_demo.md)).
2. Для запуска сэмпла перейдите в папку *bin/csharp_demo/video_recognition_demo* и двойным левым щелчком мыши запустите файл `run.bat`

<p align="center">
<img width="450" src="../../../img/video_rec_cs_bat.png"><br>
<b>Расположение C# video_recognition_demo</b>
</p>

3. Результаты трекинга и идентификации отображаются в окнах (по одному на каждый источник). Найденные лица выделяются зеленым кругом. В правой верхней части окна отображаются результаты распознавания: слева – найденное лицо на видео, справа – лицо из базы и имя.

<p align="center">
<img width="750" src="../../../img/video_rec_cs.png"><br>
<b>Результат запуска C# video_recognition_demo</b>
</p>

Для запуска приложения на Linux перейдите в директорию *bin/csharp_demos/video_recognition_demo* и выполните команду `run.sh <path_to_opencv_csharp>`, где `<path_to_opencv_csharp>` – путь до директории с библиотекой *OpenCvSharp*.

Также Вы можете запустить сэмпл **C# video_recognition_demo**, указав определенные параметры.

Для сборки сэмпла выполните следующие команды:
```
cd examples/csharp_demos/video_recognition_demo
dotnet publish -o publish
```

Для запуска сэмпла укажите путь до библиотеки `csharp_video_recognition_demo.dll`, параметры запуска (необязательно) и видео-источник:
```
dotnet csharp_video_recognition_demo.dll [OPTIONS] <video_source>...
```

Примеры запуска:
* Веб-камера: `dotnet csharp_video_recognition_demo.dll –config_dir ../../../conf/facerec 0`
* RTSP-поток: `dotnet csharp_video_recognition_demo.dll –config_dir ../../../conf/facerec rtsp://localhost:8554/`

Параметры запуска:
```
[--config_dir=<config_dir>] [--license_dir=<license_dir>] [--database_dir=<database_dir>] [--method_config=<method_config>] [--recognition_distance_threshold=<recognition_distance_threshold>] [--frame_fps_limit=<frame_fps_limit>] (<camera_id> | <rtsp_url>)...
```

Где:
* `config_dir` – путь до каталога *conf/facerec*
* `license_dir` – путь до каталога с лицензией (укажите, если нужно изменить каталог по умолчанию `license`)
* `database_dir` – путь до каталога с базой лиц
* `method_config` – имя конфигурационного файла распознавателя
* `recognition_distance_threshold` – порог распознавания (вещественное число)
* `camera_id | rtsp_url` – один или несколько источников, каждый источник – это число (номер вебкамеры) либо строка (URL видеопотока либо путь до видеофайла)
* `frame_fps_limit` – ограничение FPS

**Для запуска демо на Windows:**  
Добавьте путь до директории, содержащей `facerec.dll`, в преременную окружения `PATH`.
```
set PATH=%PATH%;..\...\..\bin
dotnet publish/csharp_video_recognition_demo.dll --config_dir=../../../conf/facerec --database_dir=../../../bin/base --method_config=method6v7_recognizer.xml --recognition_distance_threshold=7000 --frame_fps_limit=25 0
```

**Для запуска демо на Linux:**  
Добавьте путь до директории, содержащей `libfacerec.so`, и путь до директории с собранной для linux библиотекой *OpenCVSharp* в переменную окружения `LD_LIBRARY_PATH`.
```
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}: ../../../lib:~/opencv/lib/x86_64-linux-gnu dotnet publish/csharp_demo.dll --config_dir=../../../conf/facerec --database_dir=../../../bin/base --method_config=method6v7_recognizer.xml --recognition_distance_threshold=7000 --frame_fps_limit=25 0
```

В консоли отображается статус загрузки базы и сообщения об ошибках при их возникновении.

Исходный код: [examples/csharp/video_recognition_demo](../../../../examples/csharp/video_recognition_demo)
