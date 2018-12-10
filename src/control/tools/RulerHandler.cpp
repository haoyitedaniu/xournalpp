#include "RulerHandler.h"

#include "gui/XournalView.h"
#include "control/Control.h"
#include "undo/InsertUndoAction.h"
#include <cmath>

RulerHandler::RulerHandler(XournalView* xournal, XojPageView* redrawable, PageRef page)
 : BaseStrokeHandler(xournal, redrawable, page)
{
	XOJ_INIT_TYPE(RulerHandler);
}

RulerHandler::~RulerHandler()
{
	XOJ_CHECK_TYPE(RulerHandler);

	XOJ_RELEASE_TYPE(RulerHandler);
}

void RulerHandler::snapToGrid(double& x, double& y)
{
	XOJ_CHECK_TYPE(RulerHandler);

	if(!xournal->getControl()->isGridSnapping())
	{
		return;
	}

	/**
	 * Snap points to a grid:
	 * If x/y coordinates are under a certain tolerance,
	 * fix the point to the grid intersection value
	 */
	double gridSize = 14.17;
	double tolerance = 2.5;

	double xRem = fmod(x,gridSize);
	double yRem = fmod(y,gridSize);

	bool snapX = false;
	bool snapY = false;

	double tmpX = 0;
	double tmpY = 0;

	if (xRem < tolerance) 
	{
		tmpX = x - xRem;
		snapX = true;
	}
	if (xRem > gridSize - tolerance )
	{
		tmpX = x + (gridSize - xRem);
		snapX = true;
	}
	if (yRem < tolerance) 
	{
		tmpY = y - yRem;
		snapY = true;
	}
	if (yRem > gridSize - tolerance )
	{
		tmpY = y + (gridSize - yRem);
		snapY = true;
	}

	if (snapX && snapY)
	{
		x = tmpX;
		y = tmpY;
	}
}

void RulerHandler::snapRotation(double& x, double& y)
{
	XOJ_CHECK_TYPE(RulerHandler);

	Point firstPoint = stroke->getPoint(0);

	//snap to a grid - get the angle of the points
	//if it's near 0, pi/4, 3pi/4, pi, or the negatives
	//within epsilon, fix it to that value.
	
	double dist = sqrt(pow(x - firstPoint.x, 2.0) + pow(y - firstPoint.y, 2.0));
	double angle = atan2((y - firstPoint.y), (x - firstPoint.x));
	double epsilon = 0.1;
	
	if (std::abs(angle) < epsilon)
	{
		x = dist + firstPoint.x;
		y = firstPoint.y;
	}
	else if (std::abs(angle - M_PI / 4.0) < epsilon)
	{
		x = dist / sqrt(2.0) + firstPoint.x;
		y = dist / sqrt(2.0) + firstPoint.y;
	}
	else if (std::abs(angle - 3.0 * M_PI / 4.0) < epsilon)
	{
		x = -dist / sqrt(2.0) + firstPoint.x;
		y = dist / sqrt(2.0) + firstPoint.y;
	}
	else if (std::abs(angle + M_PI / 4.0) < epsilon)
	{
		x = dist / sqrt(2.0) + firstPoint.x;
		y = -dist / sqrt(2.0) + firstPoint.y;
	}
	else if (std::abs(angle + 3.0 * M_PI / 4.0) < epsilon)
	{
		x = -dist / sqrt(2.0) + firstPoint.x;
		y = -dist / sqrt(2.0) + firstPoint.y;
	}
	else if (std::abs(std::abs(angle) - M_PI) < epsilon)
	{
		x = -dist + firstPoint.x;
		y = firstPoint.y;
	}
	else if (std::abs(angle - M_PI / 2.0) < epsilon)
	{
		x = firstPoint.x;
		y = dist + firstPoint.y;
	}
	else if (std::abs(angle + M_PI / 2.0) < epsilon)
	{
		x = firstPoint.x;
		y = -dist + firstPoint.y;
	}
}


void RulerHandler::drawShape(Point& currentPoint, bool shiftDown)
{
	XOJ_CHECK_TYPE(RulerHandler);

	double x = currentPoint.x;
	double y = currentPoint.y;

	/**
	 * Snap first point to grid (if enabled)
	 */
	if (!shiftDown && xournal->getControl()->isGridSnapping())
	{
		Point firstPoint = stroke->getPoint(0);
		snapToGrid(firstPoint.x,firstPoint.y);
		stroke->setFirstPoint(firstPoint.x,firstPoint.y);
	}

	if (stroke->getPointCount() < 2)
	{
		stroke->addPoint(currentPoint);
	}
	else if (shiftDown)
	{
		x = currentPoint.x;
		y = currentPoint.y;
	}
	else
	{
		if (xournal->getControl()->isRotationSnapping())
		{
			snapRotation(x,y);
		}

		if (xournal->getControl()->isGridSnapping())
		{
			snapToGrid(x,y);
		}
	}

	stroke->setLastPoint(x, y);
}

