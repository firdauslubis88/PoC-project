#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Alignment.h"
#include "NSCalibration.h"

class StereoCalibration
{
public:
	StereoCalibration();
	~StereoCalibration();

	bool init(int image_width, int image_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type);
	bool main(ofPixels ldPixels, ofImage hdImage);
	bool main(ofPixels* imagePixels);
	bool main2(ofPixels ldPixels, ofImage hdImage);
	bool main2(ofPixels ldPixels, ofImage hdImage, std::string outputFileName);
	bool main2(ofPixels* imagePixels);
	bool main2(ofPixels* imagePixels, std::string outputFileName);
	void preUpdateCalibList(cv::Mat& view, std::vector<cv::Point2f>& pointBuf);
	void updateCalibList(cv::Mat& view, std::vector<cv::Point2f>& pointBuf);

	ofxCvColorImage ldCvImage, hdCvImage, rectifyCvImage, calibrationView[3];
	int cameraNum;

private:
	bool runAndSave(const std::string& outputFilename,
		const std::vector<std::vector<cv::Point2f> >* imagePoints,
		cv::Size imageSize, cv::Size boardSize, Pattern patternType, float squareSize,
		float aspectRatio, int flags, cv::Mat* cameraMatrix,
		cv::Mat* distCoeffs, bool writeExtrinsics, bool writePoints, cv::Mat& R, cv::Mat& T, cv::Mat& E, cv::Mat& F, cv::Mat& R1, cv::Mat& R2, cv::Mat& P1, cv::Mat& P2, cv::Mat& Q, cv::Rect* validROI);
	float tracking(std::vector<cv::Mat>& matView);
	cv::Mat StereoCalibration::postProcessing(const std::vector<cv::Mat>& matView, const cv::Mat* cameraMatrix, const cv::Mat* distCoeffs, cv::Size imageSize, const cv::Mat R1, const cv::Mat P1, const cv::Mat R2, const cv::Mat P2, cv::Rect* validRoi, bool isVerticalStereo, bool useCalibrated);

	std::vector<cv::Mat> goodImageList;
	vector<vector<cv::Point2f> > imagePoints[2];
	cv::Rect validRoi[2];
	std::string msg;
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
	bool bPreUpdateResult;
	clock_t prevTimestamp;
	int j, nimages;
	cv::Mat cameraMatrix[2], distCoeffs[2];
	cv::Mat R, T, E, F;
	cv::Mat R1, R2, P1, P2, Q;
	cv::Size imageSize;
	bool isVerticalStereo;
	Alignment alignment;
	bool writePoints;
	bool writeExtrinsics;
};

