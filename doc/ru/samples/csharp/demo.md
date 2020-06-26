# demo

Программа демонстрирует трекинг, детекцию и обрезку лиц, определение антропометрических точек и углов, а также оценку качества лица, определение пола и возраста, эмоций, принадлежности лица живому человеку (анализ цветного изображения с камеры).

Для быстрого запуска сэмпла с параметрами по умолчанию на Windows перейдите в папку *bin/csharp_demo/demo* и двойным левым щелчком мыши запустите файл `run.bat`.

<p align="center">
<img width="350" src="../../../img/demo_cs_bat.png"><br>
<b>Расположение C# demo</b>
</p>

Результаты трекинга и информация о каждом лице отображаются в окне. В левой части окна программы перечислены компоненты Face SDK, которые можно включать/выключать левым щелчком мыши. Сэмпл аналогичен **C++ demo**, более подробное описание компонентов см. в пункте [C++ demo](../cpp/demo.md).

<p align="center">
<img width="600" src="../../../img/demo_cs.png"><br>
<b>Результат запуска C# demo</b>
</p>

Для запуска приложения на Linux перейдите в директорию *bin/csharp_demos/demo* и выполните команду `run.sh <path_to_opencv_csharp>`, где `<path_to_opencv_csharp>` – путь до директории с библиотекой OpenCvSharp.

Также Вы можете запустить сэмпл **C# demo**, указав определенные параметры (например, путь до онлайн лицензии).

Для сборки сэмпла выполните следующие команды:
```
cd examples/csharp_demos/demo
dotnet publish -o publish
```

Параметры запуска:
`[--config_dir=<config_dir>] [--license_dir=<license_dir>] [--capturer_config=<capturer_config>]`

Где:

* `config_dir` – путь до каталога *conf/facerec*
* `capturer_config` – путь до конфигурационного файла детектора (подробнее о типах конфигурационных файлов см. [Класс Capturer](../../development/face_capturing.md#класс-capturer))
* `license_dir` – путь до каталога с лицензией (укажите, если нужно изменить каталог по умолчанию `license`)

**Для запуска демо на Windows:**  
Добавьте путь до директории, содержащей `facerec.dll`, в преременную окружения `PATH`.
```
set PATH=%PATH%; ..\...\..\bin
dotnet publish\csharp_demo.dll --config_dir=../../../conf/facerec --capturer_config=common_video_capturer_lbf.xml
```

**Для запуска демо на Linux**:  
Добавьте путь до директории, содержащей `libfacerec.so`, и путь до директории с собранной для linux библиотекой *OpenCVSharp* в переменную окружения `LD_LIBRARY_PATH`.
```
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../../../lib:/path/to/opencvsharp/native/libs dotnet publish/csharp_demo.dll --config_dir=../../../conf/facerec --capturer_config=common_video_capturer_lbf.xml
```

В консоли отображаются сообщения об ошибках при их возникновении.

Исходный код: [demo.cs](../../../../examples/csharp/demo/demo.cs)
