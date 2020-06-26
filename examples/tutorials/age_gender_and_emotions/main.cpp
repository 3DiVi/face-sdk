#include <facerec/libfacerec.h>

#include <QApplication>
#include <QString>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QFile>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cassert>


int main(int argc, char* argv[])
{
	try
	{
		QApplication a(argc, argv);

		// Path to Face SDK and an image
		const std::string face_sdk_path = FACE_SDK_PATH;
		const QString image_path = "path_to_image";

		qDebug() << "Face SDK path: " << QString::fromStdString(face_sdk_path);
		qDebug() << "Path to image: " << image_path;

		#ifdef _WIN32
			std::string facerec_lib_path = face_sdk_path + "/bin/facerec.dll";
		#else
			std::string facerec_lib_path = face_sdk_path + "/lib/libfacerec.so";
		#endif

		pbio::FacerecService::Ptr service = pbio::FacerecService::createService(
			facerec_lib_path,
			face_sdk_path + "/conf/facerec");

		// Create face detector
		const std::string capturer_name = "common_capturer4_lbf.xml";
		const int min_face_size = 20;
		const int max_face_size = 1900;
		pbio::FacerecService::Config capturer_config(capturer_name);
		capturer_config.overrideParameter("min_size", min_face_size);
		capturer_config.overrideParameter("max_size", max_face_size);
		pbio::Capturer::Ptr capturer = service->createCapturer(capturer_config);

		// Create age-gender estimator
		pbio::AgeGenderEstimator::Ptr age_gender_estimator = service->createAgeGenderEstimator("age_gender_estimator.xml");

		// Create emotion estimator
		pbio::EmotionsEstimator::Ptr emotions_estimator = service->createEmotionsEstimator("emotions_estimator.xml");

		// Open the image and read file data
		QFile image_file(image_path);
		image_file.open(QFile::ReadOnly);
		assert(image_file.isOpen());
		QByteArray file_data = image_file.readAll();

		// Detect faces on the image
		std::vector<pbio::RawSample::Ptr> samples = capturer->capture(
			(const uchar *)file_data.constData(),
			file_data.length());

		QImage image(image_path);
		QPainter painter(&image);
		QPen pen;
		pen.setWidth(2);
		painter.setPen(pen);
		painter.setFont(QFont("Arial", 15, QFont::DemiBold));

		const int margin = 32; // Margin text from a face bounding rectangle
		const int line_height = 32;

		for(const auto &sample : samples)
		{
			// Draw anthropometric points
			{
				const std::vector<pbio::RawSample::Point> points = sample->getLandmarks();

				painter.save();

				pen.setColor(Qt::green);
				pen.setWidth(1);
				painter.setPen(pen);
				painter.setBrush(Qt::green);

				for(const pbio::RawSample::Point &point:  points)
				{
					painter.drawEllipse(QPoint(point.x, point.y), 1, 1);
				}

				painter.restore();
			}

			// Draw eye points
			{
				const std::vector<QPair<pbio::Point, QColor>> eyes = {
					{sample->getLeftEye(), Qt::blue},
					{sample->getRightEye(), Qt::red}
				};

				painter.save();

				for (const auto &eye:  eyes)
				{
					const QColor &color = eye.second;
					const pbio::RawSample::Point &point = eye.first;
					pen.setColor(color);
					painter.setPen(pen);
					painter.setBrush(color);
					painter.drawEllipse(QPoint(point.x, point.y), 3, 3);
				}

				painter.restore();
			}


			// Draw a face bounding rectangle
			{
				const pbio::RawSample::Rectangle rectangle = sample->getRectangle();

				painter.save();

				pen.setColor(Qt::green);
				pen.setWidth(3);
				painter.setPen(pen);

				painter.drawRect(rectangle.x, rectangle.y, rectangle.width, rectangle.height);

				painter.restore();
			}

			// Calculate the base point to draw the information about age, gender and emotions
			const pbio::RawSample::Rectangle rectangle = sample->getRectangle();
			const QPoint base_point = QPoint(rectangle.x + rectangle.width + margin, rectangle.y);

			// Estimate and draw age and gender
			{
				const pbio::AgeGenderEstimator::AgeGender age_gender = age_gender_estimator->estimateAgeGender(*sample);

				painter.save();

				// Draw the age group
				const QMap<pbio::AgeGenderEstimator::Age, QString> age_map = {
					{pbio::AgeGenderEstimator::AGE_KID,    "Kid"},
					{pbio::AgeGenderEstimator::AGE_YOUNG,  "Young"},
					{pbio::AgeGenderEstimator::AGE_ADULT,  "Adult"},
					{pbio::AgeGenderEstimator::AGE_SENIOR, "Senior"}
				};
				QString age_group_text = QString("Age Group: %1").arg(age_map[age_gender.age]);

				painter.drawText(base_point + QPoint(0, line_height * 0), age_group_text);

				// Draw age in years
				QString age_in_years_text = QString("Age: %1").arg((int)age_gender.age_years);
				painter.drawText(base_point + QPoint(0, line_height * 1), age_in_years_text);

				// Draw gender
				QString gender = age_gender.gender == pbio::AgeGenderEstimator::GENDER_MALE ? "Male" : "Female";
				QString gender_text = QString("Gender: %1").arg(gender);
				painter.drawText(base_point + QPoint(0, line_height * 2), gender_text);

				painter.restore();
			}


			// Estimate and draw emotions
			{
				// Estimate emotions for a sample
				const std::vector<pbio::EmotionsEstimator::EmotionConfidence> emotions = emotions_estimator->estimateEmotions(*sample);

				// Emotion parameters for drawing: row number, bar color, label text
				struct EmotionParams
				{
					int row;
					QColor color;
					QString label;
				};

				// Parameters for every emotion
				const QMap<pbio::EmotionsEstimator::Emotion, EmotionParams> emotions_params = {
					{pbio::EmotionsEstimator::EMOTION_NEUTRAL,  {0, Qt::blue,   "Neutral"}},
					{pbio::EmotionsEstimator::EMOTION_HAPPY,    {1, Qt::green,  "Happy"}},
					{pbio::EmotionsEstimator::EMOTION_ANGRY,    {2, Qt::red,    "Angry"}},
					{pbio::EmotionsEstimator::EMOTION_SURPRISE, {3, Qt::yellow, "Surprise"}}
				};

				const QPoint base_emotions_point(base_point + QPoint(0, line_height * 3)); // Base point of the first emotion
				const QSizeF bar_base_size(100, 15); // Base size for the emotion confidence bar
				const QPoint bar_offset(100, 0); // Offset of the emotion bar from the emotion label

				// Iterate by emotions
				for (const auto &emotion_confidence: emotions) {
					const auto &emotion_params = emotions_params[emotion_confidence.emotion];

					const int &emotion_row = emotion_params.row;
					const QPoint base_emotion_row_point = base_emotions_point + QPoint(0, line_height * emotion_row);
					const QPoint base_text_point = base_emotion_row_point + QPoint(0, bar_base_size.height());

					painter.save();

					// Draw the emotion label
					const QString &emotion_label = emotion_params.label;
					painter.drawText(base_text_point, emotion_label);

					// Draw the confidence bar
					const QPoint base_rect_point = base_emotion_row_point + bar_offset;
					QSizeF bar_size(bar_base_size.width() * emotion_confidence.confidence, bar_base_size.height());
					const QColor &emotion_color = emotion_params.color;

					pen.setColor(emotion_color);
					pen.setWidth(1);
					painter.setPen(pen);
					painter.setBrush(QBrush(emotion_color));
					painter.drawRect(QRectF(base_rect_point, bar_size));

					painter.restore();
				}
			}
		}

		painter.end();

		// Show the image
		QLabel widget;
		widget.resize(image.width(), image.height());
		widget.setPixmap(QPixmap::fromImage(image));

		widget.show();

		return a.exec();
	}
	catch(std::exception& ex)
	{
		qDebug() << "Exception catched: " << ex.what();
	}
	catch(...)
	{
		qDebug() << "Unknown exception catched";
	}

	return 0;
}
