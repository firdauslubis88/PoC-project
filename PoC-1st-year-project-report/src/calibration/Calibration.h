#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Alignment.h"
#include "NSCalibration.h"

class Calibration
{
public:
	Calibration();
	~Calibration();

	bool init(int image_width, int image_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type, int localCameraNum, int localAdditionalViewNum);
	bool main(ofImage hdImage);
	bool main(ofImage hdImage, std::string outputFileName);
	bool main(ofPixels ldPixels);
	bool main(ofPixels* imagePixels);
	bool main(ofPixels ldPixels, std::string outputFileName);
	bool main(cv::Mat tempMatCvImage);
	void preUpdateCalibList(cv::Mat& view, std::vector<cv::Point2f>& pointBuf);
	void updateCalibList(cv::Mat& view, std::vector<cv::Point2f>& pointBuf);

	ofxCvColorImage *cvImages, *calibrationView;
	int mode, cameraNum, additionalViewNum;
	bool bStartCapture, useCalibrated, undistortImage, trackImage;
	cv::Mat *cameraMatrix, *distCoeffs, *tempMatView;

private:
	bool main2(ofPixels* imagePixels);
	bool main2(ofPixels ldPixels, ofImage hdImage);
	bool runAndSave2(const std::string& outputFilename,
		const std::vector<std::vector<cv::Point2f> >* imagePoints,
		cv::Size imageSize, cv::Size boardSize, Pattern patternType, float squareSize,
		float aspectRatio, int flags, cv::Mat* cameraMatrix,
		cv::Mat* distCoeffs, bool writeExtrinsics, bool writePoints, cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F, cv::Mat& R1, cv::Mat& R2, cv::Mat& P1, cv::Mat& P2, cv::Mat& Q, cv::Rect* validROI);
//	float tracking(std::vector<cv::Mat>& matView);
	cv::Mat postProcessing(const std::vector<cv::Mat>& matView, const cv::Mat* cameraMatrix, const cv::Mat* distCoeffs, cv::Size imageSize, const cv::Mat R1, const cv::Mat P1, const cv::Mat R2, const cv::Mat P2, cv::Rect* validRoi, bool isVerticalStereo, bool useCalibrated);
	float tracking(std::vector<cv::Mat>& matView);
	Alignment alignment;

	cv::Rect validRoi[2];
	cv::Mat R, T, E, F;
	cv::Mat R1, R2, P1, P2, Q;
	bool isVerticalStereo;

	std::vector<std::vector<cv::Point2f> > *imagePoints;
	cv::Size boardSize, imageSize;
	float squareSize, aspectRatio;
	std::string outputFilename;
	std::string inputFilename;
	int i, nframes, nimages;
	bool writeExtrinsics, writePoints;
	int flags = 0;
	bool flipVertical;
	bool showUndistorted;
	bool videofile;

	int delay;
	clock_t prevTimestamp;
	int cameraId = 0;
	vector<string> imageList;
	Pattern pattern;
	int image_width, image_height, j;
	bool bPreUpdateResult;
	std::string msg;
};

