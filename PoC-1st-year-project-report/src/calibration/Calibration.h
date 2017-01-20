#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "NSCalibration.h"

class Calibration
{
public:
	Calibration();
	~Calibration();


	bool init(int image_width, int image_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type);
	ofPixels main(ofPixels ldPixels);
	ofPixels main(ofPixels ldPixels, int image_width, int image_height, int x, int y, int width, int height);
	bool main(ofImage hdImage);
	ofPixels main(ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height);
	ofPixels main(ofPixels ldPixels, ofImage hdImage);
	ofPixels main(ofPixels ldPixels, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height);

	ofxCvColorImage ldCvImage, hdCvImage;

	bool bStartCapture;
	Size boardSize, imageSize;
	float squareSize, aspectRatio;
	Mat cameraMatrix, distCoeffs;
	string outputFilename;
	string inputFilename = "";

	int i, nframes;
	bool writeExtrinsics, writePoints;
	bool undistortImage = false;
	int flags = 0;
	VideoCapture capture;
	bool flipVertical;
	bool showUndistorted;
	bool videofile;
	int delay;
	clock_t prevTimestamp;
	int mode = DETECTION;
	int cameraId = 0;
	vector<string> imageList;
	Pattern pattern;
	ofxCvColorImage calibrationView;

private:
	vector<vector<Point2f> > imagePoints;
};

