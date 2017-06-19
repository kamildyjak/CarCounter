// Blob.cpp

#include "Blob.h"

Blob::Blob(vector<Point> contour)
{
	_contour = contour;
	_boundingRect = boundingRect(_contour);

	_position.x = (_boundingRect.x + _boundingRect.x + _boundingRect.width) / 2;
	_position.y = (_boundingRect.y + _boundingRect.y + _boundingRect.height) / 2;

	_prevPosition = _position;

	_diagonalSize = sqrt(pow(_boundingRect.width, 2) + pow(_boundingRect.height, 2));
	_aspectRatio = (double)_boundingRect.width / (double)_boundingRect.height;

	_noMatchCount = 0;
	_match = false;
}
