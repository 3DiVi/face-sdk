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
			const QRect rect(bounding_box.x, bounding_box.y, bounding_box.width, bounding_box.height);

			const bool recognized = face.match_database_index >= 0;

			const QColor color = recognized ?
				Qt::green :
				Qt::red;  // Red color for unrecognized

			// Draw the bounding box and liveness status
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
