#include "database.h"
#include "videoframe.h"

#include <QDir>
#include <QDebug>

#include <stdexcept>
#include <fstream>


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

Database::Database(
	const std::string database_dir_path,
	const pbio::Recognizer::Ptr recognizer,
	const pbio::Capturer::Ptr capturer,
	const float fa_r)
{
	const float distance_threshold = recognizer->getROCCurvePointByFAR(fa_r).distance;

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

	for(const auto &dir: dirs)
	{
		QDir person_dir(dir.filePath());

		QString name = dir.baseName();

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

			// Detect the face
			const std::vector<pbio::RawSample::Ptr> captured_samples = capturer->capture(frame);

			if(captured_samples.size() != 1)
			{
				qDebug() << "\n\nWARNING: detected" << captured_samples.size() <<
					"faces on" << path << "image instead of one, image ignored\n\n";
				continue;
			}

			const pbio::RawSample::Ptr sample = captured_samples[0];

			// Make a template
			const pbio::Template::Ptr templ = recognizer->processing(*sample);

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
