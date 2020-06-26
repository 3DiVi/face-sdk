#ifndef DATABASE_H
#define DATABASE_H

#include <QImage>
#include <QString>

#include <facerec/libfacerec.h>

class Database
{
public:
	// Make a thumbnail of a sample
	static
	QImage makeThumbnail(const QImage& image);

	Database() {}

	// Create the database
	Database(
		const std::string database_dir_path,
		const pbio::Recognizer::Ptr recognizer,
		const pbio::Capturer::Ptr capturer,
		const float fa_r);

	std::vector<pbio::VideoWorker::DatabaseElement> vw_elements;
	std::vector<QImage> thumbnails;
	std::vector<QString> names;
};

#endif // DATABASE_H
