#pragma once
#include "NSCalibration.h"
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Alignment.h"

class StereoCalibration
{
public:
	StereoCalibration();
	~StereoCalibration();

	ofxCvColorImage ldCvImage, hdCvImage, rectifyCvImage;

	bool init(int image_width, int image_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type);
	bool main(ofPixels ldPixels, ofImage hdImage);
	vector<cv::Mat> bothImages, goodImageList;
	vector<vector<cv::Point2f> > imagePoints[2];
	vector<vector<cv::Point3f> > objectPoints;
	cv::Size boardSize;
	float squareSize, aspectRatio;
	int nframes, delay, mode, flags;
	Pattern pattern;
	string outputFilename;
	string inputFilename;
	bool flipVertical;
	bool showUndistorted;
	bool bStartCapture;
	bool displayCorners;
	bool showRectified, useCalibrated, undistortImage, trackImage;
	clock_t prevTimestamp;
	int j, nimages;
	cv::Mat cameraMatrix[2], distCoeffs[2];
	cv::Mat R, T, E, F;
	cv::Mat R1, R2, P1, P2, Q;
	ofxCvColorImage ldCalibrationView, hdCalibrationView, rectifyCalibrationView;
	cv::Size imageSize;
	bool isVerticalStereo;
	cv::Rect validRoi[2];
	cv::Mat rmap[2][2];
	Alignment alignment;
};

