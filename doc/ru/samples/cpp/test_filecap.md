# test_filecap

Программа демонстрирует детекцию лиц, сохранение и загрузку `pbio::RawSample`.

Параметры запуска:

* путь до файла библиотеки `libfacerec.so` или `facerec.dll`
* путь до каталога *conf/facerec*
* имя конфигурационного файла детектора
* пути до входных изображений

Пример запуска из каталога *bin*:

* Linux: `./test_filecap ../lib/libfacerec.so ../conf/facerec common_capturer4.xml set1/01100.jpg`
* Windows: `test_filecap facerec.dll ../conf/facerec common_capturer4.xml set1/01100.jpg`

Результат детекции лиц на изображении отображается в окне. В консоли отображаются координаты и ориентации найденных лиц, а также пути, по которым сохраняются сэмплы. Сообщения об ошибках в случае их возникновения также отображаются в консоли.

Исходный код: [test_filecap.cpp](../../../../examples/cpp/test_filecap/test_filecap.cpp)

Для сборки требуется библиотека OpenCV.
