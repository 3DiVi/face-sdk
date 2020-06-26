# test_videocap

Программа демонстрирует трекинг лиц и оценку принадлежности лица реальному человеку.

Параметры запуска:

* путь до файла библиотеки `libfacerec.so` или `facerec.dll`
* путь до каталога *conf/facerec*

Пример запуска из каталога *bin*:

* Linux: `./test_videocap ../lib/libfacerec.so ../conf/facerec`
* Windows: `test_videocap facerec.dll ../conf/facerec`

Результаты трекинга и определения принадлежности лица реальному человеку отображаются в окне. Сообщения об ошибках в случае их возникновения также отображаются в консоли.

Исходный код: [test_videocap.cpp](../../../../examples/cpp/test_videocap/test_videocap.cpp)

Для сборки требуется библиотека OpenCV.
