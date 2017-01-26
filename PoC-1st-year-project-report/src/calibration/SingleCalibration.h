#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "BaseCalibration.h"

class SingleCalibration
{
public:
	SingleCalibration();
	~SingleCalibration();

public:
	bool init(int image_width, int image_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type);
	bool main(ofImage hdImage);
	bool main(ofImage hdImage, std::string outputFileName);
	bool main(ofPixels ldPixels);
	bool main(ofPixels ldPixels, std::string outputFileName);
	bool main(cv::Mat tempMatCvImage);
	void preUpdateCalibList(cv::Mat& view, std::vector<cv::Point2f>& pointBuf);
	void updateCalibList(cv::Mat& view, std::vector<cv::Point2f>& pointBuf);

private:
	std::vector<std::vector<cv::Point2f> > imagePoints;
	ofxCvColorImage cvImage;
	bool bStartCapture;
	cv::Size boardSize, imageSize;
	float squareSize, aspectRatio;
	cv::Mat cameraMatrix, distCoeffs;
	std::string outputFilename;
	std::string inputFilename;
	int i, nframes, nimages;
	bool writeExtrinsics, writePoints;
	bool undistortImage = false;
	int flags = 0;
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
	int image_width, image_height, j;
	bool bPreUpdateResult;
	int cameraNum = 1;
	std::string msg;
};

