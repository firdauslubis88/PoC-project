#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "NSCalibration.h"

class BaseCalibration
{
public:
	BaseCalibration() {};
	~BaseCalibration() {};


	virtual bool init(int image_width, int image_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type) = 0;
	virtual bool main(ofImage hdImage) = 0;
	virtual bool main(ofImage hdImage, std::string outputFileName) = 0;
	virtual bool main(ofPixels ldPixels) = 0;
	virtual bool main(ofPixels ldPixels, std::string outputFileName) = 0;
	virtual bool main(cv::Mat tempMatCvImage) = 0;

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

protected:
	bool preUpdateCalibList(cv::Mat& view, std::vector<cv::Point2f>& pointBuf);
	void updateCalibList(cv::Mat& view, std::vector<cv::Point2f>& pointBuf);

private:
	std::vector<std::vector<cv::Point2f> > imagePoints;
};

