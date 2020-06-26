# test_identify

Программа демонстрирует создание, сохранение, загрузку и сравнение шаблонов.

Программа работает в трех режимах:

* **enrollment** – детекция лиц и создание шаблонов по группе изображений и сохранение полученных шаблонов в файл
* **identifying** – детекция лица на одном изображении, создание шаблона и сравнение полученного шаблона с группой шаблонов, созданных в режиме **enrollment**
* **verify** – детекция лиц и создание шаблонов на паре изображений и сравнение полученных двух шаблонов

Параметры запуска в режиме **enrollment**:

* путь до файла библиотеки `libfacerec.so` или `facerec.dll`
* путь до каталога *conf/facerec*
* имя конфигурационного файла распознавателя
* `enroll`
* путь до каталога с изображениями (например каталог *bin/set1*)
* путь до текстового файла содержащего списк имен файлов изображений (например *bin/set1/list.txt*)
* путь для сохранения шаблонов в файл

Параметры запуска в режиме **identifying**:

* путь до файла библиотеки `libfacerec.so` или `facerec.dll`
* путь до каталога *conf/facerec*
* имя конфигурационного файла распознавателя
* `identify`
* путь до файла изображения
* путь до файла с шаблонами, созданного в режиме `enrollment`

Параметры запуска в режиме **verify**:

* путь до файла библиотеки `libfacerec.so` или `facerec.dll`
* путь до каталога *conf/facerec*
* имя конфигурационного файла распознавателя
* `verify`
* путь до первого файла изображения
* путь до второго файла изображения

Пример запуска из каталога *bin* в режиме **identifying**:

* Linux: `./test_identify ../lib/libfacerec.so ../conf/facerec method9v300_recognizer.xml enroll set1 set1/list.txt templates.bin`
* Windows: `test_identify facerec.dll ../conf/facerec method9v300_recognizer.xml enroll set1 set1/list.txt templates.bin`

Пример запуска из каталога *bin* в режиме **enrollment**:

* Linux: `./test_identify ../lib/libfacerec.so ../conf/facerec method9v300_recognizer.xml identify set2/01100.jpg templates.bin`
* Windows: `test_identify facerec.dll ../conf/facerec method9v300_recognizer.xml identify set2/01100.jpg templates.bin`

Пример запуска из каталога *bin* в режиме **verify**:

* Linux: `./test_identify ../lib/libfacerec.so ../conf/facerec method9v300_recognizer.xml verify set1/01100.jpg set2/01100.jpg`
* Windows: `test_identify facerec.dll ../conf/facerec method9v300_recognizer.xml verify set1/01100.jpg set2/01100.jpg`

Статус обработки, результаты сравнений и сообщения об ошибках (в случае их возникновения) отображаются в консоли. 

Исходный код: [test_identify.cpp](../../../../examples/cpp/test_identify/test_identify.cpp)

Для сборки не требуется дополнительных библиотек.
