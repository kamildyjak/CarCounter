// Blob.h

#ifndef MY_BLOB
#define MY_BLOB

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<vector>
#include<cmath>

using namespace std;
using namespace cv;

class Blob {
public:
	vector<Point> _contour;
	Point _position;
	Point _prevPosition;
	Rect _boundingRect;
	bool _match;
	int _noMatchCount;
	double _diagonalSize;
	double _aspectRatio;
	int _id;

	Blob(vector<Point> contour);
};

#endif    // MY_BLOB

