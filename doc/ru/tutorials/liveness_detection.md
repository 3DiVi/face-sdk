# Определение принадлежности лица живому человеку (3D Liveness)

В этом туториале Вы узнаете, как определить, принадлежит ли лицо на видеопотоке реальному человеку, при помощи Face SDK и RGDB-сенсора. Для удобства мы будем называть эту функцию *Liveness*. Как правило, liveness-проверка используется для предотвращения злонамеренных действий с использованием фото вместо реального лица.

В Face SDK есть возможность определять принадлежность лица реальному человеку посредством анализа карты глубины, либо RGB-изображения с камеры. Первый способ наиболее точный, поэтому в данном туториале мы рассмотрим только его.

В качестве основы для данного проекта был взят туториал [Распознавание лиц на видеопотоке](face_recognition_in_a_video_stream.md). В данном проекте, как и в предыдущем, используется готовая база лиц для распознавания. После запуска готового проекта на экране будут отображаться цветное изображение и карта глубины, при помощи которой Вы сможете скорректировать свое положение относительно сенсора: для корректной работы liveness-детектора требуется, чтобы лицо находилось на достаточном расстоянии от сенсора, при этом качество карты глубины с сенсора должно быть приемлемым. На цветном изображении задетектированное и распознанное лицо будет выделяться зеленым прямоугольником, рядом с ним будет отображаться фото и имя из базы, а также liveness-статус `REAL`. В случае, если человек не распознан, liveness-статус будет `REAL`, но ограничивающий прямоугольник будет красным. Если перед камерой будет лицо с изображения или видео, то прямоугольник будет красным и распознавание не будет производиться, в этом случае liveness-статус будет `FAKE`.

Помимо Face SDK и Qt для данного проекта Вам потребуется:

* RGBD-камера с поддержкой OpenNI2 или RealSense2 (например, ASUS Xtion или RealSense D415);
* [дистрибутив OpenNI2](https://structure.io/openni) или [RealSense2](https://github.com/IntelRealSense/librealsense/releases).

Готовый демо-проект вы можете найти в Face SDK: [examples/tutorials/depth_liveness_in_face_recognition](../../../examples/tutorials/depth_liveness_in_face_recognition)

Содержание туториала: 

* [Подключение библиотек OpenNI2 и RealSense2](#подключение-библиотек-openni2-и-realsense2)
* [Получение карты глубины через OpenNI2 API или RealSense2 API](#получение-карты-глубины-через-openni2-api-или-realsense2-api)
* [Обработка карты глубины при помощи VideoWorker](#обработка-карты-глубины-при-помощи-videoworker)
* [Визуализация RGB и карты глубины. Отображение информации о 3D Liveness](#визуализация-rgb-и-карты-глубины-отображение-информации-о-3d-liveness)

<p align="center">
<img width="1000" src="../../img/fifth_1.png"><br>
</p>

## Подключение библиотек OpenNI2 и RealSense2

1. Прежде всего, нам нужно подключить необходимые библиотеки для работы с камерой глубины. Вы можете использовать как OpenNI2-камеру (например, ASUS Xtion) так и камеру RealSense2. В зависимости от используемой камеры, Вам нужно прописать условие `WITH_OPENNI2=1` или `WITH_REALSENSE=1`.

**face_recognition_with_video_worker.pro**
```cpp
...
WITH_OPENNI2=1
#WITH_REALSENSE=1
isEmpty(WITH_OPENNI2): isEmpty(WITH_REALSENSE) {
    error("OpenNI2 or RealSense support should be enabled")
}
!isEmpty(WITH_OPENNI2): !isEmpty(WITH_REALSENSE) {
    error("OpenNI2 and RealSense support can't be enabled simultaneously")
}
...
```

2. [Для камер OpenNI2] Указываем путь до дистрибутива OpenNI2 и пути, по которым можно найти необходимые OpenNI2-библиотеки и заголовочные файлы.

_**Примечание:** Для Windows необходимо выполнить установку дистрибутива и указать путь до директории установки. Для Linux достаточно указать путь до распакованного дистрибутива._

**face_recognition_with_video_worker.pro**
```cpp
...
!isEmpty(WITH_OPENNI2){
    OpenniDistr = 
    isEmpty(OpenniDistr) {
        error("Empty path to OpenNI2 directory")
    }
    LIBS += -L$$OpenniDistr/Redist/
    win32: LIBS += -L$$OpenniDistr/Lib/
    INCLUDEPATH += = $$OpenniDistr/Include/
    LIBS += -lOpenNI2
    DEFINES += WITH_OPENNI2
}
...
```

3. [Для камер RealSense2] Указываем путь до дистрибутива RealSense2 и пути, по которым можно найти необходимые RealSense2-библиотеки и заголовочные файлы. В блоке win32 определяется битность платформы для формирования корректных путей до библиотек RealSense.

_**Примечание:** Для Windows необходимо выполнить установку дистрибутива и указать путь до директории установки. Для Linux необходимо выполнить установку дистрибутива, как указано [на сайте Intel RealSense](https://github.com/IntelRealSense/librealsense/). Пути до дистрибутива в pro-файле указывать не требуется._

**face_recognition_with_video_worker.pro**
```cpp
...
!isEmpty(WITH_REALSENSE){
    win32 {
        RealSenseDistr = /home/stranger/depth_liveness/librealsense/install
        isEmpty(RealSenseDistr) {
            error("Empty path to RealSense directory")
    }
        contains(QMAKE_TARGET.arch, x86_64) {
            LIBS += -L$$RealSenseDistr/lib/x64
            LIBS += -L$$RealSenseDistr/bin/x64
        } else {
            LIBS += -L$$RealSenseDistr/lib/x86
            LIBS += -L$$RealSenseDistr/bin/x86
        }
        INCLUDEPATH += $$RealSenseDistr/include/
    }
    LIBS += -lrealsense2
    DEFINES += WITH_REALSENSE
}
...
```

## Получение карты глубины через OpenNI2 API или RealSense2 API

1. На данном этапе нам необходимо получить кадры глубины с RGBD-камеры при помощи OpenNI2 API или RealSense2 API, в зависимости от используемой камеры. В данном проекте мы не будем подробно рассматривать получение кадров глубины. Мы используем заголовочные файлы, которые входят в состав одной из демо-программ Face SDK ([video_recognition_demo](../samples/cpp/video_recognition_demo.md)). В pro-файле проекта указываем путь до папки *examples/cpp/video_recognition_demo/src*, которая находится в Face SDK.

**face_recognition_with_video_worker.pro**
```cpp
...
INCLUDEPATH += $$FACE_SDK_PATH/examples/cpp/video_recognition_demo/src
...
```

2. Указываем необходимые заголовочные файлы для работы с камерами OpenNI2 и RealSense2. Подробную информацию о получении кадров глубины Вы можете найти в указанных файлах (`OpenniSource.h` и `RealSenseSource.h`).

**face_recognition_with_video_worker.pro**
```cpp
...
!isEmpty(WITH_OPENNI2){
    HEADERS += OpenniSource.h
}
else {
    HEADERS += RealSenseSource.h
}
...
```

3. Для использования математических констант определяем `_USE_MATH_DEFINES` (`cmath` подключен в файлах `OpenniSource.h` и `RealSenseSource.h`).

**face_recognition_with_video_worker.pro**
```cpp
...
unix: LIBS += -ldl
win32: DEFINES += _USE_MATH_DEFINES
...
```

**Подключение сенсора глубины для обработки кадров**

1. В предыдущих проектах мы получали изображение с веб-камеры, используя объект `QCameraCapture`. Однако в этом проекте нам требуется получать не только цветные кадры, но и кадры глубины. Для этого создадим новый класс `DepthSensorCapture`: **Add New > C++ > C++ Class > Choose… > Class name – DepthSensorCapture > Base class – QObject > Next > Project Management** (настройки по умолчанию) **> Finish**.
2. В файле `depthsensorcapture.h` подключим заголовочный файл `ImageAndDepthSource.h`. Также подключаем `QSharedPointer` для работы с указателями, `QThread` для обработки потоков, `QByteArray` для работы с массивами байтов, `memory` и `atomic` для использования умных указателей и атомарных типов соответственно. В файле `depthsensorcapture.cpp` подключаем заголовочные файлы `OpenniSource.h` и `RealSenseSource.h` для получения кадров глубины, а также `worker.h` и `depthsensorcapture.h`. Для обработки ошибок используется `assert.h`, для отображения сообщения об ошибке используется `QMessageBox`.

**depthsensorcapture.h**
```cpp
#include "ImageAndDepthSource.h"
#include <QSharedPointer>
#include <QThread>
#include <QByteArray>
#include <memory>
#include <atomic>
...
```

**depthsensorcapture.cpp**
```cpp
#if defined(WITH_OPENNI2)
#include "OpenniSource.h"
#elif defined(WITH_REALSENSE)
#include "RealSenseSource.h"
#endif
#include "worker.h"
#include "depthsensorcapture.h"
#include <assert.h>
#include <QMessageBox>
...
```

3. Определяем `RGBFramePtr` – указатель на цветное изображение и `DepthFramePtr` – указатель на кадр глубины. Конструктор класса `DepthSensorCapture` принимает родительский виджет (`parent`), а также указатель на `worker`. Данные с сенсора будут приходить в бесконечном цикле. Чтобы главный поток, в котором происходит отрисовка интерфейса, не ждал завершения цикла, мы создаем отдельный поток и перемещаем в него объект `DepthSensorCapture`.

**depthsensorcapture.h**
```cpp
...
class Worker;
class DepthSensorCapture : public QObject
{
    Q_OBJECT
public:
    typedef std::shared_ptr<QImage> FramePtr;
    typedef std::shared_ptr<QByteArray> DepthPtr;
    explicit DepthSensorCapture(
        QWidget* parent,
        std::shared_ptr<Worker> worker);
}
...
```

**depthsensorcapture.cpp**
```cpp
...
DepthSensorCapture::DepthSensorCapture(
    QWidget* parent,
    std::shared_ptr<Worker> worker) :
_parent(parent),
_worker(worker)
{
    #if defined(WITH_OPENNI2)
        depth_source.reset(new OpenniSource());
    #else
        depth_source.reset(new RealSenseSource());
    #endif
    thread.reset(new QThread());
    this->moveToThread(thread.data());
    connect(thread.data(), &QThread::started, this, &DepthSensorCapture::frameUpdatedThread);
}
...
```

4. В методе `DepthSensorCapture::start` запускаем поток получения данных, в методе `DepthSensorCapture::stop` останавливаем его.

**depthsensorcapture.h**
```cpp
...
class DepthSensorCapture : public QObject
{
    ...
    void start();
    void stop();
}
...
```

**depthsensorcapture.cpp**
```cpp
...
void DepthSensorCapture::start()
{
    if (!thread->isRunning())
    {
        thread->start();
    }
}
void DepthSensorCapture::stop()
{
    if (thread->isRunning())
    {
        shutdown = true;
        thread->quit();
        thread->wait();
    }
}
...
```

5. В методе `DepthSensorCapture::frameUpdatedThread` обрабатываем новый кадр с камеры в бесконечном цикле и передаем его в `Worker` через `addFrame`. В случае возникновения ошибки появится окно с сообщением об ошибке.

**depthsensorcapture.h**
```cpp
...
class DepthSensorCapture : public QObject
{
    ...
    signals:
        void newFrameAvailable();
    private
        void frameUpdatedThread();
    private:
        QWidget* _parent;
        std::shared_ptr<Worker> _worker;
        QSharedPointer<QThread> thread;
        QSharedPointer<ImageAndDepthSource> depth_source;
        std::atomic<bool> shutdown = {false};
};
...
```

**depthsensorcapture.cpp**
```cpp
...
void DepthSensorCapture::frameUpdatedThread()
{
    while(!shutdown)
    {
        try
        {
            ImageAndDepth data;
            depth_source->get(data);
            _worker->addFrame(data);
            emit newFrameAvailable();
        }
        catch(std::exception& ex)
        {
            stop();
            QMessageBox::critical(
                _parent,
                tr("Face SDK error"),
                ex.what());
            break;
        }
    }
}
...
```

6. Объект `VideoFrame` должен содержать в себе RGB кадр с камеры.

**depthsensorcapture.cpp**
```cpp
...
Database::Database(...)
{
    ...
    VideoFrame frame;
        frame.frame() = DepthSensorCapture::RGBFramePtr(new QImage(image));
    ...
}
...
```

7. В файле `videoframe.h` подключаем заголовочный файл `depthsensorcapture.h` для работы с камерой глубины.

**videoframe.h**
```cpp
#include "depthsensorcapture.h"
...
```

8. Интефейс `IRawImage` позволяет получить указатель на данные изображения, его формат, высоту и ширину.

**videoframe.h**
```cpp
...
class VideoFrame : public pbio::IRawImage
{
    public:
        VideoFrame(){}
        ...
        DepthSensorCapture::RGBFramePtr& frame();
        const DepthSensorCapture::RGBFramePtr& frame() const;
 
    private:
        DepthSensorCapture::RGBFramePtr _frame;
}
...
inline
DepthSensorCapture::RGBFramePtr& VideoFrame::frame()
{
    return _frame;
}
 
inline
const DepthSensorCapture::RGBFramePtr& VideoFrame::frame() const
{
    return _frame;
}
...
```

9. В методе `runProcessing` запускаем камеру, в методе `stopProcessing` – останавливаем.

**viewwindow.h**
```cpp
...
class ViewWindow : public QWidget
{
    ...
    private:
    Ui::ViewWindow *ui;
    QScopedPointer<DepthSensorCapture> _camera;
    ...
}
...
```

**viewwindow.cpp**
```cpp
#include "depthsensorcapture.h"
ViewWindow::ViewWindow(...)
{
    ...
    _camera.reset(new DepthSensorCapture(
    this,
    _worker));
    connect(_camera.data(), &DepthSensorCapture::newFrameAvailable, this, &ViewWindow::draw);
}
void ViewWindow::stopProcessing()
{
    if (!_running)
        return;
    _camera->stop();
    _running = false;
}
void ViewWindow::runProcessing()
{
    if (_running)
        return;
    _camera->start();
    _running = true;
}
...
```

10. В файле `worker.h` подключаем заголовочный файл `depthsensorcapture.h`. Структура `SharedImageAndDepth` содержит в себе указатели на RGB-кадр и кадр глубины с камеры, а также структуру `pbio::DepthMapRaw` с информацией о параметрах карты глубины (ширина, высота и т.д.). Указатели используются в `Worker`. Из-за некоторой задержки в обработке кадров определенное количество кадров будет складываться в очередь на отрисовку, поэтому для экономии памяти вместо самих кадров мы храним указатели на них.

**worker.h**
```cpp
#include "depthsensorcapture.h"
    struct SharedImageAndDepth
    {
        DepthSensorCapture::RGBFramePtr color_image;
        DepthSensorCapture::DepthFramePtr depth_image;
        pbio::DepthMapRaw depth_options;
    };
...
```

11. В структуру `DrawingData` передаем `SharedImageAndDepth frame` - кадры для отрисовки (цветное изображение и карта глубины). В коллбэке `TrackingCallback` из очереди кадров извлекается изображение, соответствующее последнему полученному результату.

**worker.h**
```cpp
...
class Worker
{
    ...
    struct DrawingData
    {
        ...       
        SharedImageAndDepth frame;
    }
    void addFrame(const ImageAndDepth& frame);
    private:
        DrawingData _drawing_data;
        std::mutex _drawing_data_mutex;
        std::queue<std::pair<int, SharedImageAndDepth> > _frames;
        ...
}
...
```

**worker.cpp**
```cpp
...
void Worker::TrackingCallback(
    const pbio::VideoWorker::TrackingCallbackData &data,
    void * const userdata)
{    
    ...
    // Get the frame with frame_id
    SharedImageAndDepth frame;
    {
        ...
    }
}
void Worker::addFrame(const ImageAndDepth& data)
{
    ...
}
...
```

## Обработка карты глубины при помощи VideoWorker

1. В конструкторе `Worker::Worker` переопределяем значения некоторых параметров объекта `VideoWorker` для обработки карты глубины. а именно:

* `depth_data_flag` (`"1"` – включен режим работы с кадрами глубины для подтверждения liveness);
* `weak_tracks_in_tracking_callback` (`“1”` – в `TrackingCallback` передаются все сэмплы, в том числе сэмплы с флагом `weak = true`).

Значение флага `weak` становится `true` в том случае, если сэмпл не прошел определенные проверки, например, если:

* на лице слишком много теней (недостаточное освещение);
* изображение нечеткое (размытое);
* лицо находится под большим углом;
* размер лица в кадре слишком маленький;
* лицо не прошло liveness-проверку (взято с фото).

Более подробное описание требований к условиям освещения, размещения камер и т.д. Вы можете найти в пункте [Рекомендации по камерам](../guidelines_for_cameras.md). Как правило, сэмплы, которые не прошли проверку, не передаются для дальнейшей обработки. Однако в данном проекте даже лица на фото (не прошедшие liveness-проверку) должны выделяться ограничивающим прямоугольником, следовательно, мы должны передавать в `TrackingCallback` в том числе сэмплы с флагом `weak = true`.

**worker.cpp**
```cpp
...
Worker::Worker(...)
{
    ...
    vwconfig.overrideParameter("depth_data_flag", 1);
    vwconfig.overrideParameter("weak_tracks_in_tracking_callback", 1);
    ...
}
...
```

2. В файле `worker.h` в структуре `FaceData` указываем перечисление `pbio::DepthLivenessEstimator` - результат определения принадлежности лица живому человеку. Доступно четыре варианта определения liveness:

* **NOT_ENOUGH_DATA** – недостаточно информации. Такая ситуация может возникнуть, если качество карты глубины неудовлетворительное, либо пользователь находится слишком близко/слишком далеко от сенсора.
* **REAL** – лицо принадлежит реальному человеку.
* **FAKE** – лицо было взято с фото/видео.
* **NOT_COMPUTED** – лицо не было проверено. Такая ситуация может возникнуть, например, если кадры с сенсора не синхронизированы (цветной кадр получен, но соответствующий ему кадр глубины не найден в определенном временном диапазоне).

Результат liveness-проверки сохраняется в переменную `face.liveness_status` для дальнейшей отрисовки.

**worker.h**
```cpp
...
class Worker
{
public:
    // Face data
    struct FaceData
    {
        ...
        pbio::DepthLivenessEstimator::Liveness liveness_status;
    }
    ...
}
...
```

**worker.cpp**
```cpp
...
void Worker::TrackingCallback(...)
{
    ...
    {
        ...
        // Update the information
        {
            const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);
            ...
            // Samples
            for(size_t i = 0; i < samples.size(); ++i)
            {
                ...
                face.liveness_status = data.samples_depth_liveness_confirmed[i];
            }
        }
    }
    ...
}
...
```

3. В методе `Worker::addFrame` подаем последний кадр глубины в FaceSDK через метод `VideoWorker::addDepthFrame` и сохраняем его для дальнейшей отрисовки.

**worker.h**
```cpp
...
class Worker
{
    ...
    private:
        ...
        DepthSensorCapture::DepthFramePtr _last_depth_image;
        pbio::DepthMapRaw _last_depth_options;
        ...
}
...
```

**worker.cpp**
```cpp
...
void Worker::addFrame(const ImageAndDepth& data)
{
    ...
    const int stream_id = 0;
 
    SharedImageAndDepth frame;
 
    if (!data.depth_image.empty())
    {
        const pbio::DepthMapRaw& depth_options = data.make_dmr();
 
        _video_worker->addDepthFrame(
            depth_options,
            stream_id,
            data.depth_timestamp_microsec);
        _last_depth_image = std::make_shared<QByteArray>(
            (const char*)data.depth_image.data(),
            depth_options.depth_data_stride_in_bytes * depth_options.depth_map_rows);
        _last_depth_options = depth_options;
 
        frame.depth_image = _last_depth_image;
        frame.depth_options = depth_options;
    }
}
...
```

4. Подготавливаем и подаем в FaceSDK цветное изображение через метод `VideoWorker::addVideoFrame`. В случае, если формат полученного цветного изображения – не RGB, а BGR, порядок байтов меняется, чтобы цвета отображались корректно. Если вместе с цветным кадром не пришел кадр глубины, используется последний полученный кадр глубины. Пара из кадра глубины и цветного изображения сохраняется в очередь `_frames`, чтобы в коллбэке `TrackingCallback` найти данные, соответствующие результату обработки.

**worker.cpp**
```cpp
...
void Worker::addFrame(const ImageAndDepth& data)
{
    ...
    if (!data.color_image.empty())
    {
        QImage image(
            (const uchar*)data.color_image.data(),
            data.color_width,
            data.color_height,
            data.color_stride_in_bytes,
            QImage::Format_RGB888);
        if (data.color_format != pbio::IRawImage::FORMAT_RGB)
        {
            image = image.rgbSwapped();
        }
 
        frame.color_image = std::make_shared<QImage>(image);
        VideoFrame video_frame;
        video_frame.frame() = frame.color_image;
        const std::lock_guard<std::mutex> guard(_frames_mutex);
        const int frame_id = _video_worker->addVideoFrame(
            video_frame,
            stream_id,
            data.image_timestamp_microsec);
        if (data.depth_image.empty())
        {
            frame.depth_image = _last_depth_image;
            frame.depth_options = _last_depth_options;
        }
        _frames.push(std::make_pair(frame_id, frame));
    }
    _video_worker->checkExceptions();
}
...
```

## Визуализация RGB и карты глубины. Отображение информации о 3D Liveness

1. Модифицируем метод рисования `DrawFunction::Draw` в файле `drawfunction.cpp`. В поле `frame` структуры `Worker::DrawingData` содержатся указатели на данные цветного изображения и карты глубины, а также параметры кадра глубины (ширина, высота и др.). Для удобства обращения к этим данным заведем на них ссылки `const QImage& color_image`, `const QByteArray& depth_array` и `const pbio::DepthMapRaw& depth_options`. Цветное изображение и карта глубины будут отображаться на `QImage result`, который представляет собой своего рода “фон” и включает в себя оба изображения (цветное – сверху, карта глубины – снизу). Перед этим нам необходимо конвертировать 16-битные значения глубины в 8-битные для корректного отображения карты глубины (в серых тонах). В переменной `max_depth_mm` указываем максимальное расстояние от сенсора до пользователя (как правило, для RGBD-камер это 10 метров).

**drawfunction.cpp**
```cpp
...
// static
QImage DrawFunction::Draw(
    const Worker::DrawingData &data,
    const Database &database)
{
    const QImage& color_image = *data.frame.color_image;
    const QByteArray& depth_array = *data.frame.depth_image;
    const pbio::DepthMapRaw& depth_options = data.frame.depth_options;
    QByteArray depth8_array;
    const uint16_t* depth_ptr = (const uint16_t*)depth_array.constData();
    const size_t elements_count_in_row = depth_options.depth_data_stride_in_bytes / sizeof(uint16_t);
    const size_t elements_count = depth_options.depth_map_rows * elements_count_in_row;
    const float max_depth_mm = 10000.f;
    for(size_t i = 0; i < elements_count; ++i)
    {
        const uint16_t depth_mm = depth_ptr[i] * depth_options.depth_unit_in_millimeters;
        const uint8_t depth_8bit = std::min<int>(255, depth_mm / max_depth_mm * 255 + 0.5);
        depth8_array += depth_8bit;
    }
    ...
}
...
```

2. Из сконвертированных значений формируем изображение глубины. Создаем объект `result`, на котором будет отображаться цветное изображение (в верхней части) и карта глубины (в нижней части). Рисуем эти изображения.

**drawfunction.cpp**
```cpp
...
QImage DrawFunction::Draw(...)
{
    ...
    QImage depth_image(
        (const uchar*)depth8_array.constData(),
        depth_options.depth_map_cols,
        depth_options.depth_map_rows,
        elements_count_in_row,
        QImage::Format_Grayscale8);
    QImage result(
        QSize(std::max<int>(color_image.width(), depth_image.width()),
            color_image.height() + depth_image.height()),
        QImage::Format_RGB888);
    QPainter painter(&result);
    painter.drawImage(QPoint(0, 0), color_image);
    painter.drawImage(QPoint(0, color_image.height()), depth_image);
    ...
}
...
```

3. Отображаем liveness-статус рядом с лицом, в зависимости от информации, полученной от liveness-детектора. Зададим параметры надписи (цвет, линия, размер). На карте глубины будет отображаться ограничивающий прямоугольник, чтобы Вы могли убедиться, что кадр глубины не смещен относительно цветного кадра.

**drawfunction.cpp**
```cpp
...
QImage DrawFunction::Draw(...)
{
    ...
    for(const auto &face_data : faces_data)
    {
        ...
        if(face.frame_id == data.frame_id && !face.lost)
        {
            ...
            // Draw the bounding box
            pen.setWidth(3);
            pen.setColor(color);
            painter.setPen(pen);
            painter.drawRect(rect);
            if (!depth_image.size().isEmpty())
            {
                const QRect rect_on_depth(
                bounding_box.x, bounding_box.y + color_image.height(),
bounding_box.width, bounding_box.height);
                painter.drawRect(rect_on_depth);
                QString liveness_str = "Liveness status: ";
                switch(face.liveness_status)
                {
                    case pbio::DepthLivenessEstimator::NOT_ENOUGH_DATA:
                        liveness_str += "NOT ENOUGH DATA";
                        break;
                    case pbio::DepthLivenessEstimator::REAL:
                        liveness_str += "REAL";
                        break;
                    case pbio::DepthLivenessEstimator::FAKE:
                        liveness_str += "FAKE";
                        break;
                    default:
                        liveness_str += "NOT COMPUTED";
                }
                const int font_size = 12;
                painter.setFont(QFont("Arial", font_size, QFont::Medium));
                pen.setColor(Qt::black);
                painter.setPen(pen);
                painter.drawText(rect.topLeft() + QPoint(1, -pen.width()), liveness_str);
                pen.setColor(Qt::white);
                painter.setPen(pen);
                painter.drawText(rect.topLeft() + QPoint(0, -pen.width()), liveness_str);
            }
            ...
        }
        ...
    }
    ...
}
...
```

4. Запускаем проект. На экране будут отображаться цветное изображение с камеры и карта глубины. На цветном изображении будет отображаться информация о найденном лице:

* **статус распознавания** (цвет ограничивающего прямоугольника: *зеленый* - пользователь есть в базе, *красный* - нет в базе/лицо с изображения или видео);
* **информация о распознанном пользователе** (его фотография из базы и имя);
* **liveness-статус**: *real* - реальный человек, *fake* - лицо взято с фото или видео, *not_enough_data* - качество карты глубины неудовлетворительное/пользователь слишком близко или далеко от сенсора, *not_computed* - рассинхронизация кадра глубины и цветного кадра.

<p align="center">
<img width="1000" src="../../img/fifth_1.png"><br>
</p>
