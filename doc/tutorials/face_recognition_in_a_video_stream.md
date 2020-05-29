# Face Recognition in a Video Stream  

In this tutorial, you'll learn how to recognize faces in a video stream. For recognition, you can use a ready-made database of faces from the Face SDK distribution package. The database includes the images of several famous people. Recognized faces are highlighted with a green rectangle. The name and image of a recognized person are displayed next to his/her face in a video stream.  This tutorial is based on [Face Detection and Tracking in a Video Stream](face_detection_and_tracking_in_a_video_stream.md) and the corresponding project.  

You can find the tutorial project in Face SDK: *examples/tutorials/face_recognition_with_video_worker*

<p align="center">
<img width="600" src="../img/fourth_2.png"><br>
</p>

## Setting Up the Project

1. In [Face Detection and Tracking in a Video Stream](face_detection_and_tracking_in_a_video_stream.md), we set only two parameters of Face SDK (a path to Face SDK and a configuration file name for the `VideoWorker` object). However, in this tutorial, we need to set several more parameters: we will add a path to the database, a configuration file name with a recognition method, and FAR. For convenience, we'll modify several files. Specify all the parameters in the `FaceSdkParameters` structure. In `facesdkparameters.h`, specify the path to the `video_worker_lbf.xml` configuration file. 

**facesdkparameters.h**
```cpp
struct FaceSdkParameters
{
    ...
    std::string videoworker_config = "video_worker_lbf.xml";
};
```

2. Pass the `face_sdk_parameters` structure to the constructor of the `Worker` object.

**viewwindow.h**
```cpp
class ViewWindow : public QWidget
{
	Q_OBJECT

    public:
	    explicit ViewWindow(
		    QWidget *parent,
		    pbio::FacerecService::Ptr service,
		    FaceSdkParameters face_sdk_parameters);
    ...
    private:
    ...
        std::shared_ptr<Worker> _worker;

        pbio::FacerecService::Ptr _service;
};
```

**viewwindow.cpp**
```cpp
ViewWindow::ViewWindow(
    QWidget *parent,
    pbio::FacerecService::Ptr service,
    FaceSdkParameters face_sdk_parameters) :
QWidget(parent),
ui(new Ui::ViewWindow),
_service(service)
{
    ui->setupUi(this);
    ...
    _worker = std::make_shared<Worker>(
        _service,
        face_sdk_parameters);
    ...
};
```

**worker.h**
```cpp
#include "qcameracapture.h"
#include "facesdkparameters.h"
...
class Worker
{
    ...
    Worker(
        const pbio::FacerecService::Ptr service,
        const FaceSdkParameters face_sdk_parameters);
    ...
};
```

**worker.cpp**
```cpp
Worker::Worker(
    const pbio::FacerecService::Ptr service,
    const FaceSdkParameters face_sdk_parameters)
{
    pbio::FacerecService::Config vwconfig(face_sdk_parameters.videoworker_config);
    ...
}
```

3. In this project, we're interested only in face detection in a video stream (creating a bounding rectangle) and face recognition. Please note that in the first project (`detection_and_tracking_with_video_worker`), which you can use as a reference for this project, we also displayed anthropometric points and angles. If you don't want to display this info, you can just remove unnecessary visualization from the first project.

## Creating the Database of Faces

1. First of all, we have to create a database of faces. To check face recognition, you can use the ready-made database from Face SDK. It includes images of three famous people (Elon Musk, Emilia Clarke, Lionel Messi).  To check recognition, you should copy the database to the project root folder (next to a .pro file), run the project, open an image from the database, and point a camera at the screen. You can also add your picture to the database. To do this, you have to create a new folder in the database, specify your name in a folder name, and copy your picture to the folder (in the same way as other folders in the database). 

2. Create a new `Database` class to work with the database: **Add New > C++ > C++ Class > Choose... > Class name – Database > Next > Project Management (default settings) > Finish**. In `database.h`, include the headers `QImage` and `QString` to work with images and strings and `libfacerec.h` to integrate Face SDK.

**database.h**
```cpp
#include <QImage>
#include <QString>

#include <facerec/libfacerec.h>

class Database
{
    public:

    Database();
}
```

3. In `database.cpp`, include the headers `database.h` and `videoframe.h` (implementation of the `IRawImage` interface, which is used by `VideoWorker` to receive the frames). Also include necessary headers for working with the file system, debugging, exception handling, and working with files. 

**database.cpp**
```cpp
#include "database.h"
#include "videoframe.h"

#include <QDir>
#include <QDebug>

#include <stdexcept>
#include <fstream>
```

4. In `database.h`, add a constructor and set the path to the database. Specify the `Recognizer` object to create templates, the `Capturer` object to detect faces and `far`. What is FAR? FAR is frequency that the system makes false accepts. False accept means that a system claims a pair of pictures are a match, when they are actually pictures of different individuals. The `vw_elements` vector contains the elements of the `VideoWorker` database. The `thumbnails` and `names` vectors contain the previews of images and names of people from the database.

**database.h**
```cpp
class Database
{
    public:
    ...
    // Create a database
    Database(
        const std::string database_dir_path,
        pbio::Recognizer::Ptr recognizer,
        pbio::Capturer::Ptr capturer,
        const float fa_r);

    std::vector<pbio::VideoWorker::DatabaseElement> vw_elements;
    std::vector<QImage> thumbnails;
    std::vector<QString> names;
}
```

5. In `database.cpp`, implement the `Database` constructor, which was declared in the previous subsection. The `distance_threshold` value means the recognition distance. Since this distance is different for different recognition methods, we get it based on the `FAR` value using the `getROCCurvePointByFAR` method. 

**database.cpp**
```cpp
Database::Database(
    const std::string database_dir_path,
    pbio::Recognizer::Ptr recognizer,
    pbio::Capturer::Ptr capturer,
    const float fa_r)
{
    const float distance_threshold = recognizer->getROCCurvePointByFAR(fa_r).distance;
}
```

6. In the `database_dir` variable, specify the path to the database with faces. If this path doesn't exist, you'll see the exception `"database directory doesn't exist"`.  Create a new `person_id` variable to store the id of a person from the database (name of a folder in the database) and the `element_id` variable to store the id of an element in the database (an image of a person from the database). In the `dirs` list, create a list of all subdirectories of the specified directory with the database. 

**database.cpp**
```cpp
Database::Database(
    const std::string database_dir_path,
    pbio::Recognizer::Ptr recognizer,
    pbio::Capturer::Ptr capturer,
    const float fa_r)
{
    ...

    QDir database_dir(QString::fromStdString(database_dir_path));

    if (!database_dir.exists())
    {
        throw std::runtime_error(database_dir_path + ": database directory doesn't exist");
    }

    int person_id = 0;
    int element_id_counter = 0;

    QFileInfoList dirs = database_dir.entryInfoList(
        QDir::AllDirs | QDir::NoDotAndDotDot,
        QDir::DirsFirst);
}
```

_**Note:** See more information about FAR and TAR values for different recognition methods in [Identification Performance](../performance_parameters.md#identification-performance)._

7. In the loop `for(const auto &dir: dirs)`, process each subdirectory (data about each person). The name of a folder corresponds to the name of a person. Create a list of images in `person_files`.   

**database.cpp**
```cpp
Database::Database(...)
{
    ...
    for(const auto &dir: dirs)
    {
        QDir person_dir(dir.filePath());

        QString name = dir.baseName();

        QFileInfoList person_files = person_dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    }
}
```

8. In the nested loop `for(const auto &person_file: person_files)`, process each image. If an image doesn't exist, the warning `"Can't read image"` is displayed. 

**database.cpp**
```cpp
Database::Database(...)
{
    ...
    for(const auto &dir: dirs)
    {
        ...        
        QFileInfoList person_files = person_dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        
        for(const auto &person_file: person_files)
        {
            QString path = person_file.filePath();

            qDebug() << "processing" << path << "name:" << name;

            QImage image(path);
            if(image.isNull())
            {
                qDebug() << "\n\nWARNING: cant read image" << path << "\n\n";
                continue;
            }

            if (image.format() != QImage::Format_RGB888)
            {
                image = image.convertToFormat(QImage::Format_RGB888);
            }

            VideoFrame frame;
            frame.frame() = QCameraCapture::FramePtr(new QImage(image));
        }
    }
}
```

9. Detect a face in an image using the `Capturer` object. If an image cannot be read, a face can't be found in an image or more than one face is detected, the warning is displayed and this image is ignored. 

**database.cpp**
```cpp
Database::Database(...)
{
    ...
    for(const auto &dir: dirs)
    {
        ...        
        QFileInfoList person_files = person_dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        
        for(const auto &person_file: person_files)
        {
            ...
            // Detect faces
            const std::vector<pbio::RawSample::Ptr> captured_samples = capturer->capture(frame);

            if(captured_samples.size() != 1)
            {
                qDebug() << "\n\nWARNING: detected" << captured_samples.size() <<
                    "faces on" << path << "image instead of one, image ignored\n\n";
                continue;
            }
            const pbio::RawSample::Ptr sample = captured_samples[0];
        }
    }
}
```

10. Using the `recognizer->processing` method, create a face template, which is used for recognition. 

**database.cpp**
```cpp
Database::Database(...)
{
    ...
    for(const auto &dir: dirs)
    {
        ...        
        QFileInfoList person_files = person_dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        
        for(const auto &person_file: person_files)
        {
            ...
            // Create a template
            const pbio::Template::Ptr templ = recognizer->processing(*sample);
        }
    }
}
```

11. In the structure `pbio::VideoWorker::DatabaseElement vw_element`, specify all the information about the database element that will be passed for processing to the `VideoWorker` object (element id, person id, face template, recognition threshold). Using the `push_back` method, add an element to the end of the list.

**database.cpp**
```cpp
Database::Database(...)
{
    ...
    for(const auto &dir: dirs)
    {
        ...        
        QFileInfoList person_files = person_dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        
        for(const auto &person_file: person_files)
        {
            ...
            // Prepare data for VideoWorker
            pbio::VideoWorker::DatabaseElement vw_element;
            vw_element.element_id = element_id_counter++;
            vw_element.person_id = person_id;
            vw_element.face_template = templ;
            vw_element.distance_threshold = distance_threshold;

            vw_elements.push_back(vw_element);
            thumbnails.push_back(makeThumbnail(image));
            names.push_back(name);
        }

        ++person_id;
    }
}
```

12. In `database.h`, add the `makeThumbnail` method to create a preview of a picture from the database. 

**database.cpp**
```cpp
class Database
{
    public:
        // Create a preview from a sample
        static
        QImage makeThumbnail(const QImage& image);
        ...
};
```

13. In `database.cpp`, implement the method using `makeThumbnail` to create a preview of a picture from the database, which will be displayed next to the face of a recognized person. Set the preview size (120 pixels) and scale it (keep the ratio if the image is resized).

**database.cpp**
```cpp
#include <fstream>
...
QImage Database::makeThumbnail(const QImage& image)
{
    const float thumbnail_max_side_size = 120.f;

    const float scale = thumbnail_max_side_size / std::max<int>(image.width(), image.height());

    QImage result = image.scaled(
        image.width() * scale,
        image.height() * scale,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    return result;
}
```

14. In the .pro file, set the path to the database. 

**face_recognition_with_video_worker.pro**
```cpp
...
DEFINES += FACE_SDK_PATH=\\\"$$FACE_SDK_PATH\\\"

DEFINES += DATABASE_PATH=\\\"$${_PRO_FILE_PWD_}/base\\\"

INCLUDEPATH += $${FACE_SDK_PATH}/include
...
```

15. In `facesdkparameters.h`, set the path to the database and the value of FAR. 

**facesdkparameters.h**
```cpp
struct FaceSdkParameters
{
    ...
    std::string videoworker_config = "video_worker_lbf.xml";

    std::string database_dir = DATABASE_PATH;
    const float fa_r = 1e-5;
};
```

## Searching a Face in the Database and Displaying the Result 

1. In `facesdkparameters.h`, set the path to the configuration file with the recognition method. In this project, we use the method 6v6 because it's suitable for video stream processing and provides optimal recognition speed and good quality. You can learn more about recommended recognition methods in [Face Identification](../development/face_identification.md). 

**facesdkparameters.h**
```cpp
struct FaceSdkParameters
{
    ...
    std::string videoworker_config = "video_worker_lbf.xml";

    std::string database_dir = DATABASE_PATH;
    const float fa_r = 1e-5;
    std::string method_config = "method6v6_recognizer.xml";
};
```

_**Note:** If you want to recognize faces in a video stream and you use low-performance devices, you can use the method 8.6. In this case, recognition speed is higher but recognition quality is lower compared to the method 6.6._

2. In `worker.h`, add the variable `match_database_index` to the `FaceData` structure. This variable will store the database element, if a person is recognized, or `"-1"` if a person is not recognized. Add `Database` and a callback indicating that a person is recognized (`MatchFoundCallback`).

**worker.h**
```cpp
#include "qcameracapture.h"
#include "facesdkparameters.h"
#include "database.h"
...
class Worker
{
public:

    struct FaceData
    {
        pbio::RawSample::Ptr sample;
            bool lost;
            int frame_id;
            int match_database_index;

            FaceData() :
                lost(true),
                match_database_index(-1)
            {
            }
    };
    ...
    pbio::VideoWorker::Ptr _video_worker;

    Database _database;
    ...
    static void TrackingLostCallback(
        const pbio::VideoWorker::TrackingLostCallbackData &data,
        void* const userdata);

    static void MatchFoundCallback(
        const pbio::VideoWorker::MatchFoundCallbackData &data,
        void* const userdata);

    int _tracking_callback_id;
    int _tracking_lost_callback_id;
    int _match_found_callback_id;
};
```

3. In `worker.cpp`, override the value of the parameter in the configuration file so that `MatchFoundCallback` is received for non-recognized faces too. Set the parameters of the `VideoWorker` object: in the first tutorial, we didn't recognize faces, that's why we set only the value of `streams_count`. Since in this project we're going to recognize faces in a video stream we have to specify in the constructor the path to the configuration file with the recognition method, and also the values of `processing_threads_count` (number of threads to create templates) and `matching_threads_count` (number of threads to compare the templates). In this project, we use only one stream (a webcam connected to our PC). Connect the database: pass the path to the database, create `Capturer` to detect faces and `Recognizer` to create templates, and also specify the `FAR` coefficient.  Using the `setDatabase` method, set the database for `VideoWorker`. Using the `addMatchFoundCallback` method, add the recognition event handler `MatchFound`.  

**worker.cpp**
```cpp
Worker::Worker(
    const pbio::FacerecService::Ptr service,
    const FaceSdkParameters face_sdk_parameters)
{
    pbio::FacerecService::Config vwconfig(face_sdk_parameters.videoworker_config);
   
    vwconfig.overrideParameter("not_found_match_found_callback", 1);

    _video_worker = service->createVideoWorker(
        vwconfig,
        face_sdk_parameters.method_config,
        1,   // streams_count
        1,   // processing_threads_count
        1);  // matching_threads_count

    _database = Database(
        face_sdk_parameters.database_dir,
        service->createRecognizer(face_sdk_parameters.method_config, true, false),
        service->createCapturer("common_capturer4_lbf_singleface.xml"),
        face_sdk_parameters.fa_r);

    _video_worker->setDatabase(_database.vw_elements);
    ...

    _match_found_callback_id =
        _video_worker->addMatchFoundCallbackU(
            MatchFoundCallback,
            this);
}
```

4. In the destructor `Worker::~Worker()`, remove `MatchFoundCallback`.

**worker.cpp**
```cpp
Worker::~Worker()
{
    _video_worker->removeTrackingCallback(_tracking_callback_id);
    _video_worker->removeTrackingLostCallback(_tracking_lost_callback_id);
    _video_worker->removeMatchFoundCallback(_match_found_callback_id);
}
...
```

5. In `MatchFoundCallback`, the result is received in form of the structure `MatchFoundCallbackData` that stores the information about recognized and unrecognized faces. 

**worker.cpp**
```cpp
// static
void Worker::TrackingLostCallback(
    const pbio::VideoWorker::TrackingLostCallbackData &data,
    void* const userdata)
{
    ...
}

// static
void Worker::MatchFoundCallback(
    const pbio::VideoWorker::MatchFoundCallbackData &data,
    void* const userdata)
{
    assert(userdata);

    const pbio::RawSample::Ptr &sample = data.sample;
    const pbio::Template::Ptr &templ = data.templ;
    const std::vector<pbio::VideoWorker::SearchResult> &search_results = data.search_results;

    // Information about a user - a pointer to Worker
    // Pass the pointer
    Worker &worker = *reinterpret_cast<Worker*>(userdata);

    assert(sample);
    assert(templ);
    assert(!search_results.empty());
}
```

6. When a template for a tracked person is created, it's compared with each template from the database. If the distance to the closest element is less than `distance_threshold` specified in this element, then it's a match. If a face in a video stream is not recognized, then you'll see the message `"Match not found"`. If a face is recognized, you'll see the message `"Match found with..."` and the name of the matched person. 

**worker.cpp**
```cpp
// static
void Worker::MatchFoundCallback(
    const pbio::VideoWorker::MatchFoundCallbackData &data,
    void* const userdata)
{
    ...    
    for(const auto &search_result: search_results)
    {
        const uint64_t element_id = search_result.element_id;

        if(element_id == pbio::VideoWorker::MATCH_NOT_FOUND_ID)
        {
            std::cout << "Match not found" << std::endl;
        }
        else
        {
            assert(element_id < worker._database.names.size());

            std::cout << "Match found with '"
                << worker._database.names[element_id].toStdString() << "'";
        }
    }
    std::cout << std::endl;
}
```

7. Save the data about the recognized face to display a preview. 

**worker.cpp**
```cpp
// static
void Worker::MatchFoundCallback(
    const pbio::VideoWorker::MatchFoundCallbackData &data,
    void* const userdata)
{
    ...
    const uint64_t element_id = search_results[0].element_id;

    if(element_id != pbio::VideoWorker::MATCH_NOT_FOUND_ID)
    {
        assert(element_id < worker._database.thumbnails.size());

        // Save the best matching result for rendering
        const std::lock_guard<std::mutex> guard(worker._drawing_data_mutex);

        FaceData &face = worker._drawing_data.faces[sample->getID()];

        assert(!face.lost);

        face.match_database_index = element_id;
    }
}
```

8. Run the project. The recognition results will be displayed in the console. If a face is recognized, you'll see the face id and name of a recognized person from the database. If a face isn't recognized, you'll see the message `"Match not found"`.

<p align="center">
<img width="400" src="../img/fourth_1.jpeg"><br>
</p>

## Displaying the Preview of the Recognized Face from the Database 

1. Let's make our project a little nicer. We'll display the image and name of a person from the database next to the face in a video stream. In `drawfunction.h`, add a reference to the database, because we'll need it when rendering the recognition results. 

**drawfunction.h**
```cpp
#include "database.h"

class DrawFunction
{
public:
    DrawFunction();

    static QImage Draw(
        const Worker::DrawingData &data,
        const Database &database);
};
```

2. In `drawfunction.cpp`, modify the function `DrawFunction::Draw` by passing the database to it.

**drawfunction.cpp**
```cpp
// static
QImage DrawFunction::Draw(
    const Worker::DrawingData &data,
    const Database &database)
{
    ...
    const pbio::RawSample& sample = *face.sample;
    QPen pen;
}
```

3. Save the bounding rectangle in the structure `pbio::RawSample::Rectangle`. Pass its parameters (x, y, width, height) to the `QRect rect` object. 

**drawfunction.cpp**
```cpp
QImage DrawFunction::Draw(...)
{
    ...
    // Save the face bounding rectangle
    const pbio::RawSample::Rectangle bounding_box = sample.getRectangle();
    QRect rect(bounding_box.x, bounding_box.y, bounding_box.width, bounding_box.height);
}
```

4. Create a boolean variable `recognized` that indicates whether a face is recognized or unrecognized. If a face is recognized, the bounding rectangle is green, otherwise it's red.  

**drawfunction.cpp**
```cpp
QImage DrawFunction::Draw(...)
{
    ...
    const bool recognized = face.match_database_index >= 0;

    const QColor color = recognized ?
        Qt::green :
        Qt::red;  // Unrecognized faces are highlighted with red

    // Display the face bounding rectangle
    {
        pen.setWidth(3);
        pen.setColor(color);
        painter.setPen(pen);
        painter.drawRect(rect);
    }
}
```

5. Get a relevant image from the database for a preview by `face.match_database_index`. Calculate the position of a preview in the frame. 

**drawfunction.cpp**
```cpp
QImage DrawFunction::Draw(...)
{
    ...
        // Display the image from the database
        if (recognized)
        {
            const QImage thumbnail = database.thumbnails[face.match_database_index];

            // Calculate the preview position
            QPoint preview_pos(
                rect.x() + rect.width() + pen.width(),
                rect.top());
}
```

6. Draw an image from the database in the preview. Create the object `QImage face_preview` that is higher than `thumbnail` on `text_bar_height`. The original preview image is drawn in the position (0, 0). As a result, we get a preview with a black rectangle at the bottom with the name of a person. Set the font parameters, calculate the position of a text and display the text in the preview.

**drawfunction.cpp**
```cpp
QImage DrawFunction::Draw(...)
{
    ...
    // Display the image from the database
    if (recognized)
    {
        ...
        const int text_bar_height = 20;

        QImage face_preview(
            QSize(thumbnail.width(), thumbnail.height() + text_bar_height),
            QImage::Format_RGB888);
        face_preview.fill(Qt::black);

        {
            const int font_size = 14;

            QPainter painter_preview(&face_preview);

            painter_preview.drawImage(QPoint(0, 0), thumbnail);

            painter_preview.setFont(QFont("Arial", font_size, QFont::Medium));
            pen.setColor(Qt::white);
            painter_preview.setPen(pen);
            painter_preview.drawText(
            QPoint(0, thumbnail.height() + text_bar_height - (text_bar_height - font_size) / 2),
            database.names[face.match_database_index]);
        }
    }
}
```

7. Draw `face_preview` in the frame using the `drawPixmap` method.

**drawfunction.cpp**
```cpp
// static
QImage DrawFunction::Draw(...)
{
    ...
 
    // Display the image from the database
    if (recognized)
    {
        ...
        QPixmap pixmap;
        pixmap.convertFromImage(face_preview);

        painter.drawPixmap(preview_pos, pixmap);
    }
}
```

8. In `worker.h`, add a method that returns the reference to the database. 

**worker.h**
```cpp
class Worker
{
public:
    ...

    void getDataToDraw(DrawingData& data);

    const Database& getDatabase() const
    {
        return _database;
    }
};
```

9. Modify the call to `DrawFunction::Draw` by passing the database to it. 

**viewwindow.cpp**
```cpp
void ViewWindow::draw()
{
    ...
    const QImage image = DrawFunction::Draw(data, _worker->getDatabase());

    ui->frame->setPixmap(QPixmap::fromImage(image));
}
```

10. Run the project. If a face is recognized, it will be highlighted with a green rectangle and you'll see a preview of an image from the database and a person's name. Unrecognized faces will be highlighted with a red rectangle. 

<p align="center">
<img width="600" src="../img/fourth_2.png"><br>
</p>
