//Kamil Dyjak

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<vector>
#include<string>
#include<iostream>
#include"Blob.h"


using namespace std;
using namespace cv;

const Scalar COLOR_BLACK = Scalar(0.0, 0.0, 0.0);
const Scalar COLOR_WHITE = Scalar(255.0, 255.0, 255.0);
const Scalar COLOR_YELLOW = Scalar(0.0, 255.0, 255.0);
const Scalar COLOR_GREEN = Scalar(0.0, 200.0, 0.0);
const Scalar COLOR_BLUE = Scalar(255.0, 0.0, 0.0);
const Scalar COLOR_RED = Scalar(0.0, 0.0, 255.0);


Mat frame, frame_gray, frame1, frame1_gray, diff, thresh, thresh_copy;
int carIndex = 0;

double distance(Point point, Point point1) {
	int deltX = abs(point.x - point1.x);
	int deltY = abs(point.y - point1.y);

	return (sqrt(pow(deltX,2) + pow(deltY,2)));
};

void showContours(Size size, vector<vector<Point>> contours, String window) {

	Mat image(size, CV_8UC3, COLOR_BLACK);

	drawContours(image, contours, -1, COLOR_WHITE, -1);

	imshow(window, image);
};

void showBlobs(Size size, vector<Blob> blobs, String window) {
	Mat image(size, CV_8UC3, COLOR_BLACK);

	vector<vector<Point>> contour;

	for (int i = 0; i < blobs.size(); i++) {
		if (blobs[i]._noMatchCount < 7) {
			contour.push_back(blobs[i]._contour);
		}
	}
	drawContours(image, contour, -1, COLOR_WHITE, -1);

	imshow(window, image);
};

void compareAndMatch(vector<Blob>& cars, vector<Blob>& current) {
	for (int i = 0; i < cars.size(); i++) {
		cars[i]._match = false;
	}

	for (int i = 0; i < current.size(); i++) {
		Point cuerrentCenter = current[i]._position;
		double minDist = 1000000;
		int index = -1;
		for (int j = 0; j < cars.size(); j++) {
			Point test = cars[j]._position;
			double dist = distance(cuerrentCenter, test);
			if (dist < minDist) {
				minDist = dist;
				index = j;
			}
		}
		if (index != -1 && minDist < current[i]._diagonalSize) {
			cars[index]._contour = current[i]._contour;
			cars[index]._boundingRect = current[i]._boundingRect;
			cars[index]._prevPosition = cars[index]._position;
			cars[index]._position = current[i]._position;
			cars[index]._diagonalSize = current[i]._diagonalSize;
			cars[index]._aspectRatio = current[i]._aspectRatio;
			cars[index]._match = true;
		}
		else {
			current[i]._match = true;
			current[i]._id = carIndex++;
			cars.push_back(current[i]);
		}
		
	}

	vector<Blob>::iterator it;

	for (int i = 0; i < cars.size(); i++) {
		if (!cars[i]._match) {
			cars[i]._noMatchCount++;
		}
	}

	for (it = cars.begin(); it != cars.end(); ) {
		if (it->_noMatchCount == 7) {
			it = cars.erase(it);
		}
		else {
			it++;
		}
	}

}


int main() {

	Mat structElement5 = getStructuringElement(MORPH_RECT, Size(5, 5));

	VideoCapture cap;

	cap.open("CarsDrivingUnderBridge.mp4");

	double fps = cap.get(CV_CAP_PROP_FPS);

	double dWidth = cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	double dHeight = cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	namedWindow("f", CV_WINDOW_AUTOSIZE);
	namedWindow("thresh", CV_WINDOW_AUTOSIZE);
	namedWindow("contours", CV_WINDOW_AUTOSIZE);
	namedWindow("cars", CV_WINDOW_AUTOSIZE);
	namedWindow("convexHulls", CV_WINDOW_AUTOSIZE);

	vector<Blob> currentBlobs;
	vector<Blob> cars;

	int carCount = 0;

	bool first = true;
	bool debug = true;

	cv::Point crossingLine[2];

	cap >> frame;
	cap >> frame1;

	int intHorizontalLinePosition = (int)std::round((double)frame.rows * 0.60);
	crossingLine[0].x = 0;
	crossingLine[0].y = intHorizontalLinePosition;

	crossingLine[1].x = frame.cols - 1;
	crossingLine[1].y = intHorizontalLinePosition;


	while (!frame1.empty()) {

		if (!debug) {
			destroyWindow("thresh");
			destroyWindow("contours");
			destroyWindow("cars");
			destroyWindow("convexHulls");
		}

		currentBlobs.clear();

		cvtColor(frame, frame_gray, CV_BGR2GRAY);
		cvtColor(frame1, frame1_gray, CV_BGR2GRAY);

		GaussianBlur(frame_gray, frame_gray, Size(5, 5), 0);
		GaussianBlur(frame1_gray, frame1_gray, Size(5, 5), 0);

		absdiff(frame_gray, frame1_gray, diff);

		threshold(diff, thresh, 30, 255, CV_THRESH_BINARY);

		if(debug)
			imshow("thresh", thresh);

		for (int i = 0; i < 2; i++) {
			dilate(thresh, thresh, structElement5);
			dilate(thresh, thresh, structElement5);
			erode(thresh, thresh, structElement5);
		}

		thresh_copy = thresh.clone();

		vector<vector<Point>> contours;

		findContours(thresh_copy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		if(debug)
			showContours(thresh_copy.size(), contours, "contours");

		vector<vector<Point>> convexHulls(contours.size());

		for (int i = 0; i < contours.size(); i++) {
			convexHull(contours[i], convexHulls[i]);
		}

		for (auto &convexHull : convexHulls) {
			Blob possibleCar(convexHull);
			if (possibleCar._boundingRect.area() > 400 && possibleCar._boundingRect.width > 30 && possibleCar._boundingRect.height > 30 && possibleCar._diagonalSize > 60 && possibleCar._aspectRatio > 0.2 && possibleCar._aspectRatio < 4.0 && (contourArea(possibleCar._contour) / (double)possibleCar._boundingRect.area()) > 0.50 ) {
				currentBlobs.push_back(possibleCar);
			}
		}

		if(debug)
			showBlobs(thresh_copy.size(), currentBlobs, "convexHulls");

		if (first) {
			for (int i = 0; i < currentBlobs.size(); i++) {
				currentBlobs[i]._id = carIndex++;
				cars.push_back(currentBlobs[i]);
			}
			first = false;
		}
		else{
			compareAndMatch(cars, currentBlobs);
		}

		if(debug)
			showBlobs(thresh.size(), cars, "cars");

		Mat final = frame.clone();

		bool crossed = false;

		for (int i = 0; i < cars.size(); i++) {
			if (debug) {
				putText(final, to_string(cars[i]._id), cars[i]._position, FONT_HERSHEY_SIMPLEX, 1, COLOR_WHITE, 2);
				rectangle(final, cars[i]._boundingRect, COLOR_BLUE, 2);
			}
			if (cars[i]._prevPosition.y > intHorizontalLinePosition && cars[i]._position.y <= intHorizontalLinePosition) {
				carCount++;
				crossed = true;
			}		
		}

		if (debug) {
			if (crossed) {
				line(final, crossingLine[0], crossingLine[1], COLOR_GREEN, 2);
			}
			else {
				line(final, crossingLine[0], crossingLine[1], COLOR_RED, 2);
			}

			putText(final, "Car count: " + to_string(carCount), Point(20, 40), FONT_HERSHEY_SIMPLEX, 1.4, COLOR_YELLOW, 2);

		}

		if (!frame.empty())
			imshow("f", final);

		frame = frame1.clone();
		cap >> frame1;
				
		int key = waitKey(10);
		if (key == 27 || frame.empty()) {
			cap.release();
			cvDestroyAllWindows();
			break;
		}
		else if (key == 'd') {
			debug = !debug;
		}
	}

	return 0;
}