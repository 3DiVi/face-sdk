#include "drawfunction.h"

#include <QPainter>
#include <QMatrix>
#include <QQuaternion>

DrawFunction::DrawFunction()
{

}

// static
QImage DrawFunction::Draw(
	const Worker::DrawingData &data)
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

			// Draw the bounding box
			{
				// Get the bounding box
				const pbio::RawSample::Rectangle bounding_box = sample.getRectangle();

				pen.setWidth(3);

				pen.setColor(Qt::green);
				painter.setPen(pen);
				painter.drawRect(bounding_box.x, bounding_box.y, bounding_box.width, bounding_box.height);
			}

			// Draw anthropometric points
			{
				// Get the points
				const std::vector<pbio::RawSample::Point> points = sample.getLandmarks();
				pen.setColor(Qt::red);
				pen.setWidth(1);

				painter.setPen(pen);
				painter.setBrush(Qt::red);

				for(auto &point : points)
				{
					painter.drawEllipse(QPoint(point.x, point.y), 2, 2);
				}
			}

			// Draw angles
			{
				const pbio::RawSample::Angles angles = sample.getAngles();

				QMatrix3x3 rotation = QQuaternion::fromEulerAngles(
					angles.pitch,
					angles.yaw,
					angles.roll).toRotationMatrix();

				// Invert along the y-axis
				for(int i = 0; i < 3; ++i)
					rotation(i, 1) *= -1;

				const QPointF axis_origin(
					(sample.getLeftEye().x + sample.getRightEye().x) * 0.5f,
					(sample.getLeftEye().y + sample.getRightEye().y) * 0.5f);

				const float face_size = std::sqrt(
					std::pow((float)sample.getRectangle().width, 2.f)
					+ std::pow((float)sample.getRectangle().height, 2.f));

				const float axis_length = face_size * 0.5f;

				// Colors of axes
				const QColor axis_colors[] = {
					QColor(255, 255, 50), // x-axis
					QColor(50, 255, 50),  // y-axis
					QColor(255, 50, 50)   // z-axis
				};

				pen.setWidth(3);
				for(int c = 0; c < 3; ++c)
				{
					pen.setColor(axis_colors[c]);
					painter.setPen(pen);
					painter.drawLine(
						axis_origin,
						axis_origin + axis_length * QPointF(rotation(0, c), rotation(1, c)));
				}
			}
		}

		painter.restore();
	}

	painter.end();

	return result;
}
