#ifndef DRAWFUNCTION_H
#define DRAWFUNCTION_H

#include "worker.h"
#include "database.h"

class DrawFunction
{
public:
	DrawFunction();

	static QImage Draw(
		const Worker::DrawingData &data,
		const Database &database);
};

#endif // DRAWFUNCTION_H
