#ifndef DRAWFUNCTION_H
#define DRAWFUNCTION_H

#include "worker.h"

class DrawFunction
{
public:
	DrawFunction();

	static QImage Draw(
		const Worker::DrawingData &data);
};

#endif // DRAWFUNCTION_H
