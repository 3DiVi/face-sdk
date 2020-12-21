# Детекция и трекинг лиц на видеопотоке

В этом туториале вы узнаете, как осуществлять детекцию и трекинг лиц на видеопотоке с камеры при помощи объекта `VideoWorker` из Face SDK API. Трекаемые лица будут выделяться зеленым прямоугольником.

Помимо Face SDK и Qt, для данного проекта Вам потребуется **камера, подключенная к ПК** (например, веб-камера). Проект можно запустить как на Windows, так и на Ubuntu (версия 16.04 или выше).

Готовый демо-проект вы можете найти в дистрибутиве Face SDK: [examples/tutorials/detection_and_tracking_with_video_worker](/examples/tutorials/detection_and_tracking_with_video_worker)

Содержание туториала: 

* [Создаем проект Qt](#создаем-проект-qt)
* [Выводим изображение с камеры](#выводим-изображение-с-камеры)
* [Детектим и отслеживаем лица на видео](#детектим-и-отслеживаем-лица-на-видео)

<p align="center">
<img width="600" src="../../img/first_1.png"><br>
</p>

## Создаем проект Qt

1. Запускаем Qt и создаем новый проект: **File > New File or Project > Application > Qt Widgets Application > Choose...**

<p align="center">
<img width="600" src="../../img/first_2.png"><br>
</p>

2. Называем проект, например, *detection_and_tracking_with_video_worker*, и выбираем путь. Кликаем **Next**, в разделе **Kit Selection** выбираем платформу для приложения, например, **Desktop**. Кликаем **Details** и отмечаем конфигурацию сборки **Release** (**Debug** в данном проекте нам не понадобится).
3. В окне **Class Information** оставляем настройки по умолчанию, кликаем **Next**, в окне **Project Management** также оставляем настройки по умолчанию и кликаем **Finish**.
4. Дадим заголовок главному окну приложения: в дереве проекта открываем двойным кликом файл **Forms > mainwindow.ui**. В правой части редактора во вкладке свойств указываем название окна: **windowTitle > Face SDK Tracking**.

<p align="center">
<img width="300" src="../../img/first_3.png"><br>
</p>

5. Для выравнивания виджетов по сетке перенесем на виджет окна **MainWindow** объект **Grid Layout**. Вызываем у виджета **MainWindow** контекстное меню правым кликом и выбираем **Layout > Lay Out in a Grid**. Объект **Grid Layout** должен растянуться по размерам виджета **MainWindow**. Изменим имя **Layout > layoutName > viewLayout**.

<p align="center">
<img width="300" src="../../img/first_4.png"><br>
</p>

6. Запускаем проект, кликнув **Run** (Ctrl+R). На экране должно появиться пустое окно с заголовком **Face SDK Tracking**.

<p align="center">
<img width="330" src="../../img/first_5.png"><br>
</p>

## Выводим изображение с камеры

1. Для того, чтобы мы могли использовать в проекте камеру, нам нужно подключить мультимедийные виджеты Qt. Для этого добавляем в pro-файл проекта строку:

**detection_and_tracking_with_video_worker.pro**
```cpp
...
QT  += multimedia multimediawidgets
...
```

2. Создадим новый класс `QCameraCapture` для получения изображения с камеры: **Add New > C++ > C++ Class > Choose… > Class name – QCameraCapture > Base class – QObject > Next > Project Management** (настройки по умолчанию) **> Finish**. В заголовочном файле `qcameracapture.h` создаем класс `CameraSurface`, который будет предоставлять кадры с камеры через коллбэк-функцию `present`.

**qcameracapture.h**
```cpp
#include <QCamera>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QVideoFrame>
class CameraSurface : public QAbstractVideoSurface
{
    Q_OBJECT
    public:
        explicit CameraSurface(QObject* parent = 0);
        bool present(const QVideoFrame& frame);
        QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType type = QAbstractVideoBuffer::NoHandle) const;
        bool start(const QVideoSurfaceFormat& format);
    signals:
        void frameUpdatedSignal(const QVideoFrame&);
};
```

3. Описываем реализацию класса в файле `qcameracapture.cpp`. Определяем конструктор `CameraSurface::CameraSurface` и метод `supportedPixelFormats`. В методе `CameraSurface::supportedPixelFormats` перечисляем форматы, поддерживаемые Face SDK (RGB24, BGR24, NV12, NV21). Изображение с некоторых камер приходит в формате RGB32, поэтому мы также указываем его в списке. Этот формат не поддерживается Face SDK, поэтому далее мы оформим преобразование изображения формата RGB32 в формат RGB24.

**qcameracapture.cpp**
```cpp
#include "qcameracapture.h"
...
CameraSurface::CameraSurface(QObject* parent) :
    QAbstractVideoSurface(parent)
{
}
QList<QVideoFrame::PixelFormat> CameraSurface::supportedPixelFormats(
    QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle)
        {
        return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_RGB32
            << QVideoFrame::Format_RGB24
            << QVideoFrame::Format_BGR24
            << QVideoFrame::Format_NV12
            << QVideoFrame::Format_NV21;
        }
        return QList<QVideoFrame::PixelFormat>();
}
```

4. В методе `CameraSurface::start` проверяем формат изображения. Запускаем камеру, если формат поддерживается, иначе обрабатываем ошибку.

**qcameracapture.cpp**
```cpp
...
bool CameraSurface::start(const QVideoSurfaceFormat& format)
{
    if (!supportedPixelFormats(format.handleType()).contains(format.pixelFormat()))
    {
        qDebug() << format.handleType() << " " << format.pixelFormat() << " - format is not supported.";
            return false;
    }
    return QAbstractVideoSurface::start(format);
}
```

5. В методе `CameraSurface::present` обрабатываем новый кадр. Если кадр прошел проверку, то посылается сигнал обновления кадра `frameUpdatedSignal`. Далее с этим сигналом будет связан слот `frameUpdatedSlot`, где кадр будет обработан.

**qcameracapture.cpp**
```cpp
...
bool CameraSurface::present(const QVideoFrame& frame)
{
    if (!frame.isValid())
    {
        return false;
    }
    emit frameUpdatedSignal(frame);
    return true;
}
```

6. Конструктор класса `QCameraCapture` принимает указатель на родительский виджет (`parent`), id камеры и разрешение изображения (ширина и высота), которые будут сохранены в соответствующие поля класса.

**qcameracapture.h**
```cpp
class QCameraCapture : public QObject
{
    Q_OBJECT
    public:
        explicit QCameraCapture(
            QWidget* parent,
            const int cam_id,
            const int res_width,
            const int res_height);
        ...
    private:
        QObject* _parent;
        int cam_id;
        int res_width;
        int res_height;
}
```

7. Добавляем в класс `QCameraCapture` объекты камеры `m_camera` и `m_surface`.

**qcameracapture.h**
```cpp
#include <QScopedPointer>
class QCameraCapture : public QObject
{
    ...
    private:
        ...
        QScopedPointer<QCamera> m_camera;
        QScopedPointer<CameraSurface> m_surface;
};
```

8. Подключаем заголовочный файл `stdexcept` в файле `qcameracapture.cpp` для генерации исключений. В списке инициализации конструктора `QCameraCapture::QCameraCapture` сохраняем указатель на родительский виджет, id камеры и разрешение изображения. В теле конструктора получаем список доступных камер. Список камер должен содержать хотя бы одну камеру, в противном случае будет выброшено исключение `runtime_error`. Также проверяем, что камера с запрашиваемым id есть в списке. Создаем камеру и подключаем сигналы камеры к слотам-обработчикам объекта. При изменении статуса камера посылает сигнал `statusChanged`. Создаем объект `CameraSurface` для отрисовки кадров с камеры. Подключаем сигнал `CameraSurface::frameUpdatedSignal` к слоту `QCameraCapture::frameUpdatedSlot`.

**qcameracapture.cpp**
```cpp
#include <QCameraInfo>
#include <stdexcept>
...
QCameraCapture::QCameraCapture(
    QWidget* parent,
    const int cam_id,
    const int res_width,
    const int res_height) : QObject(parent),
_parent(parent),
cam_id(cam_id),
res_width(res_width),
res_height(res_height)
{
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    qDebug() << "Available cameras:";
    for (const QCameraInfo &cameraInfo : availableCameras)
        {
        qDebug() << cameraInfo.deviceName() << " " << cameraInfo.description();
    }
    if(availableCameras.empty())
    {
        throw std::runtime_error("List of available cameras is empty");
    }
    if (!(cam_id >= 0 && cam_id < availableCameras.size()))
    {
        throw std::runtime_error("Invalid camera index");
    }
    const QCameraInfo cameraInfo = availableCameras[cam_id];
    m_camera.reset(new QCamera(cameraInfo));
    connect(m_camera.data(), &QCamera::statusChanged, this, &QCameraCapture::onStatusChanged);
    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &QCameraCapture::cameraError);
    m_surface.reset(new CameraSurface());
    m_camera->setViewfinder(m_surface.data());
    connect(m_surface.data(), &CameraSurface::frameUpdatedSignal, this, &QCameraCapture::frameUpdatedSlot);
}
```

9. В деструкторе `QCameraCapture` останавливаем камеру.

**qcameracapture.h**
```cpp
class QCameraCapture : public QObject
{
    ...
    explicit QCameraCapture(...);
    virtual ~QCameraCapture();
    ...
}
```

**qcameracapture.cpp**
```cpp
QCameraCapture::~QCameraCapture()
{
    if (m_camera)
    {
        m_camera->stop();
    }
}
```

10. Добавляем метод `QCameraCapture::frameUpdatedSlot` ‒ обработчик сигнала `CameraSurface::frameUpdatedSignal`. В этом методе мы конвертируем объект `QVideoFrame` в `QImage` и отправляем сигнал о том, что доступен новый кадр. Создаем `FramePtr` – указатель на изображение. Если изображение получено в формате RGB32, преобразуем его в RGB888.

**qcameracapture.h**
```cpp
#include <memory>
class QCameraCapture : public QObject
{
    Q_OBJECT
    public:
        typedef std::shared_ptr<QImage> FramePtr;
        ...
    signals:
        void newFrameAvailable();
        ...
    public slots:
        void frameUpdatedSlot(const QVideoFrame&);
        ...
}
```

**qcameracapture.cpp**
```cpp
void QCameraCapture::frameUpdatedSlot(
    const QVideoFrame& frame)
{
    QVideoFrame cloneFrame(frame);
    cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
    if (cloneFrame.pixelFormat() == QVideoFrame::Format_RGB24 ||
        cloneFrame.pixelFormat() == QVideoFrame::Format_RGB32)
    {
        QImage image((const uchar*)cloneFrame.bits(),
            cloneFrame.width(),
            cloneFrame.height(),
            QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));
 
           
        if (image.format() == QImage::Format_RGB32)
        {
            image = image.convertToFormat(QImage::Format_RGB888);
        }
        FramePtr frame = FramePtr(new QImage(image));
    }
    cloneFrame.unmap();
    emit newFrameAvailable();
}
```

11. Добавляем методы запуска (`start`) и остановки (`stop`) камеры `QCameraCapture`.

**qcameracapture.h**
```cpp
class QCameraCapture : public QObject
{
    ...
    public:
        ...
        void start();
        void stop();
        ...
}
```

**qcameracapture.cpp**
```cpp
void QCameraCapture::start()
{
    m_camera->start();
}
void QCameraCapture::stop()
{
    m_camera->stop();
}
```

12. В методе `QCameraCapture::onStatusChanged` обрабатываем изменение статуса камеры на `LoadedStatus`. Проверяем, поддерживает ли камера запрашиваемое разрешение. Устанавливаем запрашиваемое разрешение, если оно поддерживается камерой, иначе устанавливается разрешение по умолчанию (640 x 480), заданное статическими полями класса `default_res_width`, `default_res_height`.

**qcameracapture.h**
```cpp
class QCameraCapture {
        ...
    private slots:
        void onStatusChanged();
        ...
    private:
        static const int default_res_width;
        static const int default_res_height;
        ...
}
```

**qcameracapture.cpp**
```cpp
const int QCameraCapture::default_res_width = 640;
const int QCameraCapture::default_res_height = 480;
...
void QCameraCapture::onStatusChanged()
{
    if (m_camera->status() == QCamera::LoadedStatus)
    {
        bool found = false;
        const QList<QSize> supportedResolutions = m_camera->supportedViewfinderResolutions();
        for (const QSize &resolution : supportedResolutions)
        {
            if (resolution.width() == res_width &&
                resolution.height() == res_height)
            {
                found = true;
            }
        }
        if (!found)
        {
            qDebug() << "Resolution: " << res_width << "x" << res_width << " unsupported";
            qDebug() << "Set default resolution: " << default_res_width << "x" << default_res_height;
            res_width = default_res_width;
            res_height = default_res_height;
        }
        QCameraViewfinderSettings viewFinderSettings;
        viewFinderSettings.setResolution(
            res_width,
            res_height);
            m_camera->setViewfinderSettings(viewFinderSettings);
    }
}
```

13. В методе `cameraError` выводим сообщение об ошибках камеры, если они возникают.

**qcameracapture.h**
```cpp
class QCameraCapture : public QObject
{
    ...
    private slots:
        ...
        void cameraError();
    ...
}
```

**qcameracapture.cpp**
```cpp
void QCameraCapture::cameraError()
{
    qDebug() << "Camera error: " << m_camera->errorString();
}
```

14. Создаем новый класс `Worker`: **Add New > C++ > C++ Class > Choose… > Class name - Worker > Next > Finish**. Класс `Worker` через метод `addFrame` будет сохранять последний кадр с камеры и отдавать этот кадр через метод `getDataToDraw`.

**worker.h**
```cpp
#include "qcameracapture.h"
#include <mutex>
class Worker
{
    public:
        // Данные для отрисовки
        struct DrawingData
        {
            bool updated;
            QCameraCapture::FramePtr frame;
            DrawingData() : updated(false)
            {
            }
        };
        Worker();
        void addFrame(QCameraCapture::FramePtr frame);
        void getDataToDraw(DrawingData& data);
    private:
        DrawingData _drawing_data;
        std::mutex _drawing_data_mutex;
};
```

**worker.cpp**
```cpp
void Worker::getDataToDraw(DrawingData &data)
{
    const std::lock_guard<std::mutex> guard(_drawing_data_mutex);
    data = _drawing_data;
    _drawing_data.updated = false;
}
void Worker::addFrame(QCameraCapture::FramePtr frame)
{
    const std::lock_guard<std::mutex> guard(_drawing_data_mutex);
    _drawing_data.frame = frame;
    _drawing_data.updated = true;
}
```

15. Отображение кадров будет выполняться в классе `ViewWindow`. Создаем виджет **ViewWindow: Add > New > Qt > Designer Form Class > Choose... > Template > Widget** (настройки по умолчанию) **> Next > Name – ViewWindow > Project Management** (настройки по умолчанию) **> Finish**.
16. В редакторе (**Design**) перетаскиваем на виджет объект **Grid Layout**: вызываем у виджета **ViewWindow** контекстное меню правым кликом и выбираем **Lay out > Lay Out in a Grid**. Объект **Grid Layout** позволяет размещать виджеты в сетке, он растягивается по размерам виджета **ViewWindow**. Далее добавляем на **gridLayout** объект **Label** и на панели свойств задаем ему имя **frame: QObject > objectName > frame**.

<p align="center">
<img width="300" src="../../img/first_6.png"><br>
</p>

17. Удаляем текст по умолчанию в **QLabel > text**.

<p align="center">
<img width="300" src="../../img/first_7.png"><br>
</p>

18. В класс `ViewWindow` добавляем камеру `_qCamera` и инициализируем ее в конструкторе. Через статические поля `camera_image_width` и `camera_image_height` задаем требуемое разрешение изображения 1280x720. Флаг `_running` хранит состояние запуска камеры: `true` - камера запущена, `false` - не запущена (остановлена).

**viewwindow.h**
```cpp
#include "qcameracapture.h"
#include <QWidget>
namespace Ui {
class ViewWindow;
}
class ViewWindow : public QWidget
{
    Q_OBJECT
    public:
        explicit ViewWindow(QWidget *parent);
        ~ViewWindow();
    private:
        Ui::ViewWindow *ui;
        static const int camera_image_width;
        static const int camera_image_height;
        QScopedPointer<QCameraCapture> _qCamera;
        bool _running;
};
```

**viewwindow.cpp**
```cpp
const int ViewWindow::camera_image_width = 1280;
const int ViewWindow::camera_image_height = 720;
ViewWindow::ViewWindow(QWidget *parent) :
QWidget(parent),
ui(new Ui::ViewWindow())
{
    ui->setupUi(this);
    _running = false;
    _qCamera.reset(new QCameraCapture(
        this,
        0, // id камеры 
        camera_image_width,
        camera_image_height));
}
```

19. Добавляем в класс `ViewWindow` объект `Worker` и инициализируем его в конструкторе.

**viewwindow.h**
```cpp
#include "worker.h"
class ViewWindow : public QWidget
{
    ...
    private:
        ...
        std::shared_ptr<Worker> _worker;
};
```

**viewwindow.cpp**
```cpp
ViewWindow::ViewWindow(QWidget *parent) :
QWidget(parent),
ui(new Ui::ViewWindow())
{
    ...
    _worker = std::make_shared<Worker>();
    ...
}
```

20. Кадры будут подаваться в `Worker` из `QCameraCapture`. Модифицируем классы `QCameraCapture` и `ViewWindow`.

**qcameracapture.h**
```cpp
...
class Worker;
class QCameraCapture : public QObject
{
    ...
    public:
    ...
        QCameraCapture(
        ...
        std::shared_ptr<Worker> worker,
        ...);
    ...
    private:
        ...
        std::shared_ptr<Worker> _worker;
    ...
};
```

**qcameracapture.cpp**
```cpp
#include "worker.h"
...
QCameraCapture::QCameraCapture(
    ...
    std::shared_ptr<Worker> worker,
    ...) :
_worker(worker),
...
void QCameraCapture::frameUpdatedSlot(
    const QVideoFrame& frame)
{
    if (cloneFrame.pixelFormat() == QVideoFrame::Format_RGB24 ||
        cloneFrame.pixelFormat() == QVideoFrame::Format_RGB32)
    {
        ...
        FramePtr frame = FramePtr(new QImage(image));
        _worker->addFrame(frame);
    }
...
}
```

**viewwindow.cpp**
```cpp
ViewWindow::ViewWindow(QWidget *parent) :
QWidget(parent),
ui(new Ui::ViewWindow())
{
    ...
    _qCamera.reset(new QCameraCapture(
        this,
        _worker,
        ...));
}
```

21. Сигнал о появлении нового кадра `QCameraCapture::newFrameAvailable` обрабатывается в слоте `ViewWindow::draw`, который выводит изображение с камеры на виджет кадров.

**viewwindow.h**
```cpp
class ViewWindow : public QWidget
{
    ...
    private slots:
        void draw();
    ...
}
```

**viewwindow.cpp**
```cpp
ViewWindow::ViewWindow(QWidget *parent) :
QWidget(parent),
ui(new Ui::ViewWindow())
{
    ...
    connect(_qCamera.data(), &QCameraCapture::newFrameAvailable, this, &ViewWindow::draw);
}
void ViewWindow::draw()
{
    Worker::DrawingData data;
    _worker->getDataToDraw(data);
    // Если данные те же самые, отрисовка не производится
    if(!data.updated)
    {
        return;
    }
    // Отрисовка
    const QImage image = data.frame->copy();
    ui->frame->setPixmap(QPixmap::fromImage(image));
}
```

22. В методе `runProcessing` запускаем камеру, в методе `stopProcessing` – останавливаем.

**viewwindow.h**
```cpp
class ViewWindow : public QWidget
{
    public:
    ...
    void runProcessing();
    void stopProcessing();
    ...
}
```

**viewwindow.cpp**
```cpp
void ViewWindow::stopProcessing()
{
    if (!_running)
        return;
    _qCamera->stop();
    _running = false;
}
void ViewWindow::runProcessing()
{
    if (_running)
        return;
    _qCamera->start();
    _running = true;
}
```

23. Останавливаем камеру в деструкторе `~ViewWindow`.

**viewwindow.cpp**
```cpp
ViewWindow::~ViewWindow()
{
    stopProcessing();
    delete ui;
}
```

24. Подключаем виджет камеры к главному окну приложения: создаем окно просмотра и запускаем обработку в конструкторе `MainWindow`. В деструкторе `~MainWindow` останавливаем обработку.

**mainwindow.h**
```cpp
...
class ViewWindow;
class MainWindow : public QMainWindow
{
    ...
    private:
        Ui::MainWindow *ui;
        QScopedPointer<ViewWindow> _view;
};
```

**mainwindow.cpp**
```cpp
#include "viewwindow.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    try
    {
        _view.reset(new ViewWindow(this));
        ui->viewLayout->addWidget(_view.data());
        _view->runProcessing();
    }
    catch(std::exception &ex)
    {
        QMessageBox::critical(
            this,
            "Initialization Error",
            ex.what());
        throw;
    }
}
MainWindow::~MainWindow()
{
    if (_view)
    {
        _view->stopProcessing();
    }
    delete ui;
}
...
```

25. Модифицируем функцию `main` для перехвата возможных исключений.

**main.cpp**
```cpp
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
    catch(std::exception& ex)
    {
        qDebug() << "Exception caught: " << ex.what();
    }
    catch(...)
    {
        qDebug() << "Unknown exception caught";
    }
    return 0;
}
```

26. Запустите проект. Должно появиться окно с изображением с камеры.

_**Примечание:** При запуске проекта на Windows с некоторыми камерами изображение может перевернуто или отзеркалено, это связано с особенностями обработки изображений Qt. В этом случае Вам потребуется дополнительно обработать изображение, например, при помощи [QImage::mirrored()](https://doc.qt.io/qt-5/qimage.html)._

<p align="center">
<img width="600" src="../../img/first_8.png"><br>
</p>

## Детектим и отслеживаем лица на видео

1. Скачайте и распакуйте дистрибутив Face SDK, как это описано в пункте [Приступая к работе](/README_rus.md#приступая-к-работе). В корневой папке дистрибутива должны находиться папки *bin* и *lib*, соответствующие Вашей платформе.
2. Для того, чтобы осуществлять детекцию и трекинг лиц на изображении с камеры, нам необходимо интегрировать Face SDK в наш проект. Указываем путь до корневой папки дистрибутива Face SDK (там находятся нужные нам заголовочные файлы) в переменной `FACE_SDK_PATH` в pro-файле проекта. Также указываем путь до папки `include`. В случае, если пути не прописаны, будет выводиться ошибка *“Empty path to Face SDK”*.

**detection_and_tracking_with_video_worker.pro**
```cpp
...
# Set path to FaceSDK root directory
FACE_SDK_PATH =
isEmpty(FACE_SDK_PATH) {
    error("Empty path to Face SDK")
}
DEFINES += FACE_SDK_PATH=\\\"$$FACE_SDK_PATH\\\"
INCLUDEPATH += $${FACE_SDK_PATH}/include
...
```

_**Примечание:** При написании пути до Face SDK используйте слэш ("/")._

3. [Для Linux] Для сборки проекта добавляем в pro-файл следующую опцию:

**detection_and_tracking_with_video_worker.pro**
```cpp
...
unix: LIBS += -ldl
...
```

4. Кроме того, требуется указать путь до библиотеки *facerec* и конфигурационных файлов. Создадим класс `FaceSdkParameters` для хранения конфигурации (**Add New > C++ > C++ Header File > FaceSdkParameters**) и используем его в `MainWindow`.

**facesdkparameters.h**
```cpp
#include <string>
// Обработка и настройки Face SDK
struct FaceSdkParameters
{
    std::string face_sdk_path = FACE_SDK_PATH;
};
```

**mainwindow.h**
```cpp
#include <QMainWindow>
#include "facesdkparameters.h"
...
class MainWindow : public QMainWindow
{
    private:
    ...
    FaceSdkParameters _face_sdk_parameters;
}
```

5. Подключаем Face SDK: добавляем заголовочные файлы в `mainwindow.h` и метод `initFaceSdkService` для инициализации сервисов Face SDK. Создаем объект `FacerecService` – компонент для создания модулей Face SDK, вызывая статический метод `FacerecService::createService`, передаем путь до библиотеки и путь до директории с конфигурационными файлами в try-catch блоке, чтобы перехватить возможные исключения. В случае успешной инициализации функция `initFaceSdkService` вернет `true`, иначе появится окно с ошибкой и вернется `false`.

**mainwindow.h**
```cpp
#include <facerec/libfacerec.h>
class MainWindow : public QMainWindow
{
    ...
    private:
        bool initFaceSdkService();
    private:
        ...
        pbio::FacerecService::Ptr _service;
        ...
}
```

**mainwindow.cpp**
```cpp
bool MainWindow::initFaceSdkService()
{
    // Подключаем Face SDK
    QString error;
    try
    {
        #ifdef _WIN32
            std::string facerec_lib_path = _face_sdk_parameters.face_sdk_path + "/bin/facerec.dll";
        #else
            std::string facerec_lib_path = _face_sdk_parameters.face_sdk_path + "/lib/libfacerec.so";
        #endif
        _service = pbio::FacerecService::createService(
            facerec_lib_path,
                _face_sdk_parameters.face_sdk_path + "/conf/facerec");
        return true;
    }
    catch(const std::exception &e)
    {
        error = tr("Can't init Face SDK service: '") + e.what() + "'.";
    }
    catch(...)
    {
        error = tr("Can't init Face SDK service: ... exception.");
    }
    QMessageBox::critical(
        this,
        tr("Face SDK error"),
        error + "\n" +
        tr("Try to change face sdk parameters."));
    return false;
}
```

6. В конструкторе `MainWindow::MainWindow` добавляем вызов инициализации сервиса. В случае ошибки выбрасываем исключение `std::runtime_error`.

**mainwindow.cpp**
```cpp
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ...
    if (!initFaceSdkService())
    {
        throw std::runtime_error("Face SDK initialization error");
    }
    ...
}
```

7. Добавим передачу сервиса `FacerecService` и параметров Face SDK в конструктор `ViewWindow`, где они будут использованы для создания модуля трекинга `VideoWorker`. В поля класса сохраняем сервис и параметры.

**mainwindow.cpp**
```cpp
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ...
    _view.reset(new ViewWindow(
        this,
        _service,
        _face_sdk_parameters));
    ...
}
```

**viewwindow.h**
```cpp
#include "facesdkparameters.h"
#include <facerec/libfacerec.h>
...
class ViewWindow : public QWidget
{
    Q_OBJECT
    public:
        ViewWindow(
            QWidget *parent,
            pbio::FacerecService::Ptr service,
            FaceSdkParameters face_sdk_parameters);
    ...
    private:
        ...
        pbio::FacerecService::Ptr _service;
        FaceSdkParameters _face_sdk_parameters;
};
```

**viewwindow.cpp**
```cpp
...
ViewWindow::ViewWindow(
    QWidget *parent,
    pbio::FacerecService::Ptr service,
    FaceSdkParameters face_sdk_parameters) :
QWidget(parent),
ui(new Ui::ViewWindow()),
_service(service),
_face_sdk_parameters(face_sdk_parameters)
...
```

8. Модифицируем класс `Worker` для работы с Face SDK. Класс `Worker` будет принимать указатель на объект `FacerecService` и имя конфигурационного файла модуля трекинга. Класс `Worker` создает компонент `VideoWorker` из Face SDK, осуществляющий трекинг лиц, подает ему кадры и обрабатывает коллбэки, в которых приходят результаты трекинга. Реализуем конструктор – в нем создаем объект `VideoWorker`, указывая конфигурационный файл, метод распознавателя (в данном случае это пустая строка, т.к. распознавание лиц не используется), количество видеопотоков (в данном случае `1`, т.к. используется только одна камера).

**worker.h**
```cpp
#include <facerec/libfacerec.h>
class Worker
{
    ...
    public:
        ...
        Worker(
            const pbio::FacerecService::Ptr service,
            const std::string videoworker_config);
    private:
        ...
        pbio::VideoWorker::Ptr _video_worker;
};
```

**worker.cpp**
```cpp
#include "worker.h"
#include "videoframe.h"
Worker::Worker(
    const pbio::FacerecService::Ptr service,
    const std::string videoworker_config)
{
    pbio::FacerecService::Config vwconfig(videoworker_config);
    _video_worker = service->createVideoWorker(
        vwconfig,
        "",  // Распознавание не производится 
        1,   // streams_count
        0,   // processing_threads_count
        0);  // matching_threads_count
}
```

_**Примечание:** Помимо детекции и трекинга лиц объект `VideoWorker` может использоваться для распознавания лиц на нескольких потоках. В этом случае указывается метод распознавателя и потоки `processing_threads_count` и `matching_threads_count`._

9. Подписываемся на коллбэки от класса `VideoWorker` – `TrackingCallback` (лицо найдено и отслеживается), `TrackingLostCallback` (потеря трекинга лица) – и удаляем их в деструкторе.

**worker.h**
```cpp
class Worker
{
    public:
        ...
        Worker(...);
        ~Worker();
    ...
    private:
        ...
        static void TrackingCallback(
            const pbio::VideoWorker::TrackingCallbackData &data,
            void* const userdata);
        static void TrackingLostCallback(
            const pbio::VideoWorker::TrackingLostCallbackData &data,
            void* const userdata);
        int _tracking_callback_id;
        int _tracking_lost_callback_id;
};
```

**worker.cpp**
```cpp
Worker::Worker(...)
{
    ...
    _tracking_callback_id =
        _video_worker->addTrackingCallbackU(
            TrackingCallback,
            this);
    _tracking_lost_callback_id =
        _video_worker->addTrackingLostCallbackU(
            TrackingLostCallback,
            this);
}
Worker::~Worker()
{
    _video_worker->removeTrackingCallback(_tracking_callback_id);
    _video_worker->removeTrackingLostCallback(_tracking_lost_callback_id);
}
```

10. Для обработки исключений подключаем заголовочный файл `cassert`. В коллбэке `TrackingCallback` результат приходит в виде структуры `TrackingCallbackData`, которая хранит данные обо всех отслеживаемых лицах. Вывод превью синхронизируется с выводом результата. Мы не можем сразу вывести кадр, который отдается в `VideoWorker`, потому что он будет обработан чуть позже. Поэтому кадры складываются в очередь, и при получении результата мы можем найти кадр, который соответствует этому результату. Часть кадров может быть пропущена объектом `VideoWorker` при большой нагрузке, поэтому не всегда для каждого кадра есть результат. В алгоритме ниже из очереди извлекается изображение, соответствующее последнему полученному результату. Сохраняем найденные лица для кадра, чтобы потом их использовать при визуализации. Для синхронизации изменения общих данных в `TrackingCallback` и `TrackingLostCallback` используются мьютексы `std::mutex`.

**worker.h**
```cpp
#include <cassert>
...
class Worker
{
    ...
    public:
        // Информация о лице 
        struct FaceData
        {
            pbio::RawSample::Ptr sample;
            bool lost;
            int frame_id;
            FaceData() : lost(true)
            {
            }
        };
        // Данные для отрисовки
        struct DrawingData
        {
            ...
            int frame_id;
            // map<track_id , face>
            std::map<int, FaceData> faces;
            ...
        };
    ...
};
```

**worker.cpp**
```cpp
...
// static
void Worker::TrackingCallback(
    const pbio::VideoWorker::TrackingCallbackData &data,
    void * const userdata)
{
    // Проверка аргументов
    assert(userdata);
    // frame_id - на каком кадре происходит отрисовка, samples - данные о лицах, которые будут отрисовываться
    const int frame_id = data.frame_id;
    const std::vector<pbio::RawSample::Ptr> &samples = data.samples;
    // Информация о пользователе - указатель на Worker
    // Передаем указатель
    Worker &worker = *reinterpret_cast<Worker*>(userdata);
    // Получить кадр с id, равным frame_id
    QCameraCapture::FramePtr frame;
    {
        const std::lock_guard<std::mutex> guard(worker._frames_mutex);
        auto& frames = worker._frames;
        // Поиск в worker._frames
        for(;;)
        {
            // Кадры уже должны быть получены 
            assert(frames.size() > 0);
            // Проверяем, что frame_id приходят в порядке возрастания 
            assert(frames.front().first <= frame_id);
            if(frames.front().first == frame_id)
            {
                // Кадр найден 
                frame = frames.front().second;
                frames.pop();
                break;
            }
            else
            {
                // Кадр был пропущен(i.e. the worker._frames.front())
                std::cout << "skipped " << ":" << frames.front().first << std::endl;
                frames.pop();
            }
        }
    }
    // Обновляем информацию
    {
        const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);
        // Кадр
        worker._drawing_data.frame = frame;
        worker._drawing_data.frame_id = frame_id;
        worker._drawing_data.updated = true;
        // Сэмплы
        for(size_t i = 0; i < samples.size(); ++i)
        {
            FaceData &face = worker._drawing_data.faces[samples[i]->getID()];
            face.frame_id = samples[i]->getFrameID();  // Может отличаться от frame_id
            face.lost = false;
            face.sample = samples[i];
        }
    }
}
```

11. Реализуем коллбэк `TrackingLostCallback`, в котором помечаем, что трекаемое лицо пропало из кадра.

**worker.cpp**
```cpp
...
// static
void Worker::TrackingLostCallback(
    const pbio::VideoWorker::TrackingLostCallbackData &data,
    void* const userdata)
{
    const int track_id = data.track_id;
    // Информация о пользователе - указатель на Worker
    // Передаем указатель
    Worker &worker = *reinterpret_cast<Worker*>(userdata);
    {
        const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);
        
        FaceData &face = worker._drawing_data.faces[track_id];
        assert(!face.lost);
        face.lost = true;
    }
}
```

12. Объект `VideoWorker` принимает кадры через интерфейс `pbio::IRawImage`. Создадим заголовочный файл `VideoFrame`: **Add New > C++ > C++ Header File > VideoFrame**. Подключаем интерфейс `pbio::IRawImage` в файле `videoframe.h` и реализуем этот интерфейс для класса `QImage`. Интерфейс `pbio::IRawImage` позволяет получить указатель на данные изображения, его формат, высоту и ширину.

**videoframe.h**

<details>
  <summary>Нажмите, чтобы развернуть</summary>
	
```cpp
#include "qcameracapture.h"
#include <pbio/IRawImage.h>

class VideoFrame : public pbio::IRawImage
{
public:
	VideoFrame(){}

	virtual ~VideoFrame(){}

	virtual const unsigned char* data() const throw();

	virtual int32_t width() const throw();

	virtual int32_t height() const throw();

	virtual int32_t format() const throw();

	QCameraCapture::FramePtr& frame();

	const QCameraCapture::FramePtr& frame() const;

private:
	QCameraCapture::FramePtr _frame;
};


inline
const unsigned char* VideoFrame::data() const throw()
{
	if(_frame->isNull() || _frame->size().isEmpty())
	{
		return NULL;
	}

	return _frame->bits();
}

inline
int32_t VideoFrame::width() const throw()
{
	return _frame->width();
}


inline
int32_t VideoFrame::height() const throw()
{
	return _frame->height();
}


inline
int32_t VideoFrame::format() const throw()
{
	if(_frame->format() == QImage::Format_Grayscale8)
	{
		return (int32_t) FORMAT_GRAY;
	}

	if(_frame->format() == QImage::Format_RGB888)
	{
		return (int32_t) FORMAT_RGB;
	}

	return -1;
}

inline
QCameraCapture::FramePtr& VideoFrame::frame()
{
	return _frame;
}

inline
const QCameraCapture::FramePtr& VideoFrame::frame() const
{
	return _frame;
}
```
</details>

13. В методе `addFrame` подаем кадры в `VideoWorker`. Если при обработке коллбэков возникают исключения, они выбрасываются повторно в методе `checkExceptions`. Для хранения кадров заведем очередь `_frames`, в которую будем складывать id кадра и соответствующее изображение, чтобы в коллбэке `TrackingCallback` найти кадр, соответствующий результату обработки. Для синхронизации изменения общих данных также используются мьютексы `std::mutex`.

**worker.h**
```cpp
#include <queue>
...
class Worker : public QObject
{
    ...
    private:
        ...
        std::queue<std::pair<int, QCameraCapture::FramePtr> > _frames;
        std::mutex _frames_mutex;
        ...
};
```

**worker.cpp**
```cpp
#include "videoframe.h"
...
void Worker::addFrame(
    QCameraCapture::FramePtr frame)
{
    VideoFrame video_frame;
    video_frame.frame() = frame;
    const std::lock_guard<std::mutex> guard(_frames_mutex);
    const int stream_id = 0;
    const int frame_id = _video_worker->addVideoFrame(
        video_frame,
        stream_id);
    _video_worker->checkExceptions();
    _frames.push(std::make_pair(frame_id, video_frame.frame()));
}
```

14. Модифицируем метод `getDataToDraw`, чтобы не рисовать лица, для которых был вызван `TrackingLostCallback`.

**worker.cpp**
```cpp
void Worker::getDataToDraw(DrawingData &data)
{
    ...
        // Удаляем сэмплы, для которых был вызван TrackingLostCallback
        {
            for(auto it = _drawing_data.faces.begin();
                it != _drawing_data.faces.end();)
            {
                const std::map<int, FaceData>::iterator i = it;
                ++it; // i может быть удален, поэтому инкрементируем его на данном этапе
                FaceData &face = i->second;
                // Оставляем трекаемые лица 
                if(!face.lost)
                    continue;
                _drawing_data.updated = true;
                // Удаляем лица 
                _drawing_data.faces.erase(i);
            }
        }
    ...
}
```

15. Модифицируем класс `QCameraCapture` для перехвата исключений, которые могут возникнуть в `Worker::addFrame`.

**qcameracapture.cpp**
```cpp
#include <QMessageBox>
...
void QCameraCapture::frameUpdatedSlot(
    const QVideoFrame& frame)
{
    ...
    if (cloneFrame.pixelFormat() == QVideoFrame::Format_RGB24 ||
        cloneFrame.pixelFormat() == QVideoFrame::Format_RGB32)
    {
        QImage image(...);
        if (image.format() == QImage::Format_RGB32)
        {
            image = image.convertToFormat(QImage::Format_RGB888);
        }
        try
        {
            FramePtr frame = FramePtr(new QImage(image));
            _worker->addFrame(frame);
        }
        catch(std::exception& ex)
        {
            stop();
            cloneFrame.unmap();
            QMessageBox::critical(
                _parent,
                tr("Face SDK error"),
                ex.what());
                return;
        }
    }
    ...
}
```

16. Создадим класс `DrawFunction`, который будет содержать метод для отрисовки результатов трекинга на изображении: **Add New > C++ > C++ Class > Choose… > Class name – DrawFunction**.

**drawfunction.h**
```cpp
#include "worker.h"

class DrawFunction
{
public:
	DrawFunction();

	static QImage Draw(
		const Worker::DrawingData &data);
};
```

**drawfunction.cpp**
```cpp
#include "drawfunction.h"
#include <QPainter>

DrawFunction::DrawFunction()
{
}

// static
QImage DrawFunction::Draw(
	const Worker::DrawingData &data)
{
	QImage result = data.frame->copy();

	QPainter painter(&result);

	// Клонируем информацию о лицах 
	std::vector<std::pair<int, Worker::FaceData> > faces_data(data.faces.begin(), data.faces.end());

	// Рисуем лица 
	for(const auto &face_data : faces_data)
	{
		const Worker::FaceData &face = face_data.second;

		painter.save();

		// Визуализация лица в кадре 
		if(face.frame_id == data.frame_id && !face.lost)
		{
			const pbio::RawSample& sample = *face.sample;
			QPen pen;

			// Отрисовка ограничивающего прямоугольника лица 
			{
				// Получаем ограничивающий прямоугольник лица 
				const pbio::RawSample::Rectangle bounding_box = sample.getRectangle();

				pen.setWidth(3);
				pen.setColor(Qt::green);
				painter.setPen(pen);
				painter.drawRect(bounding_box.x, bounding_box.y, bounding_box.width, bounding_box.height);
			}
		}

		painter.restore();
	}

	painter.end();

	return result;
}
```

17. В конструкторе `ViewWindow` передаем указатель на объект `FacerecService` и имя конфигурационного файла модуля трекинга при создании `Worker`. В методе `Draw` рисуем результаты трекинга на изображении через вызов `DrawFunction::Draw`.

**viewwindow.cpp**
```cpp
#include "drawfunction.h"
...
ViewWindow::ViewWindow(...) :
...
{
    ...
    _worker = std::make_shared<Worker>(
        _service,
        "video_worker_lbf.xml");
    ...
}
...
void ViewWindow::draw()
{
    ...
    // Отрисовка
    const QImage image = DrawFunction::Draw(data);
    ...
}
```

18. Запустите проект. Теперь на изображении с камеры детектятся и отслеживаются лица (они выделяются зеленым прямугольником). Больше информации об использовании объекта `VideoWorker` вы можете найти в разделе [Обработка видеопотока](../development/video_stream_processing.md).

<p align="center">
<img width="600" src="../../img/first_1.png"><br>
</p>
