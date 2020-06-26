#include "drawfunction.h"

#include <QPainter>

DrawFunction::DrawFunction()
{

}

// static
QImage DrawFunction::Draw(
	const Worker::DrawingData &data,
	const Database &database)
{
	QImage result = data.frame->copy();

	QPainter painter(&result);

	// Clone the information about faces
	std::vector<std::pair<int, Worker::FaceData> > faces_data(data.faces.begin(), data.faces.end());

	// Draw faces
	for(const auto &face_data : faces_data)
	{
		const Worker::FaceData &face = face_data.second;

		painter.save();

		// Visualize faces in the frame
		if(face.frame_id == data.frame_id && !face.lost)
		{
			const pbio::RawSample& sample = *face.sample;
			QPen pen;

			// Get the bounding box
			const pbio::RawSample::Rectangle bounding_box = sample.getRectangle();
			QRect rect(bounding_box.x, bounding_box.y, bounding_box.width, bounding_box.height);

			const bool recognized = face.match_database_index >= 0;

			const QColor color = recognized ?
				Qt::green :
				Qt::red;  // Red color for unrecognized

			// Draw the bounding box
			{
				pen.setWidth(3);
				pen.setColor(color);
				painter.setPen(pen);
				painter.drawRect(rect);
			}

			// Draw an image from the database
			if (recognized)
			{
				const QImage thumbnail = database.thumbnails[face.match_database_index];

				// Position of a thumbnail from the database
				QPoint preview_pos(
					rect.x() + rect.width() + pen.width(),
					rect.top());

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

				QPixmap pixmap;
				pixmap.convertFromImage(face_preview);

				painter.drawPixmap(preview_pos, pixmap);
			}
		}

		painter.restore();
	}

	painter.end();

	return result;
}
