#include "SingleCalibration.h"

using namespace std;
using namespace cv;

SingleCalibration::SingleCalibration()
{
	aspectRatio = 1;
	pattern = CHESSBOARD;
	nframes = 10;
	delay = 1000;
	outputFilename = ofToDataPath("out_camera_data.yml");
	writePoints = true;
	writeExtrinsics = true;
	flags = CALIB_FIX_ASPECT_RATIO;
	flipVertical = false;
	videofile = false;
	showUndistorted = false;
	bStartCapture = false;
	prevTimestamp = 0;
	nimages = nframes;
	j = 0;
	bPreUpdateResult = true;
	msg = "Press 'g' to start";

	cameraNum = 1;
	additionalViewNum = 0;
	undistortImage = false;
}


SingleCalibration::~SingleCalibration()
{
}

bool SingleCalibration::init(int localImage_width, int localImage_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type)
{
	boardSize.width = chess_width;
	boardSize.height = chess_height;
	squareSize = square_size;

	if (squareSize <= 0)
	{
		ofLog(OF_LOG_ERROR, "Invalid board square width");
		return false;
	}
	if (nframes <= 3)
	{
		ofLog(OF_LOG_ERROR, "Invalid number of images");
		return false;
	}
	if (aspectRatio <= 0)
	{
		ofLog(OF_LOG_ERROR, "Invalid aspect ratio");
		return false;
	}
	if (delay <= 0)
	{
		ofLog(OF_LOG_ERROR, "Invalid delay");
		return false;
	}
	if (boardSize.width <= 0)
	{
		ofLog(OF_LOG_ERROR, "Invalid board width");
		return false;
	}
	if (boardSize.height <= 0)
	{
		ofLog(OF_LOG_ERROR, "Invalid board height");
		return false;
	}
	mode = CAPTURING;
	image_width = localImage_width;
	image_height = localImage_height;
	cvImage.allocate(image_width, image_height);
	for (size_t k = 0; k < cameraNum; k++)
	{
		calibrationView[0].allocate(image_width, image_height);
	}
	for (size_t k = 0; k < additionalViewNum; k++)
	{
		calibrationView[cameraNum + k].allocate(image_width, 2*image_height);
	}
	imageSize.width = image_width;
	imageSize.height = image_height;
	imagePoints.resize(nimages);

	return true;
}

bool SingleCalibration::main(ofImage hdImage, string localOutputFileName)
{
	this->outputFilename = ofToDataPath(localOutputFileName);

	return main(hdImage);
}

bool SingleCalibration::main(ofImage hdImage)
{
	Mat view, viewGray;

	cvImage.setFromPixels(hdImage.getPixels());
	Mat tempMatHdCvImage = cvarrToMat(cvImage.getCvImage());

	return main(tempMatHdCvImage);
}

bool SingleCalibration::main(ofPixels ldPixels, string localOutputFileName)
{
	this->outputFilename = ofToDataPath(localOutputFileName);

	return main(ldPixels);
}

bool SingleCalibration::main(ofPixels ldPixels)
{
	Mat view, viewGray;

	ofImage ldImage;
	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);
	cvImage.setFromPixels(ldImage.getPixels());

	Mat tempMatLdCvImage = cvarrToMat(cvImage.getCvImage());

	return main(tempMatLdCvImage);
}

bool SingleCalibration::main(Mat tempMatCvImage)
{
	Mat view;
	tempMatCvImage.copyTo(view);
	string msg = "Press 'g' to start";
	int baseLine = 0;
	Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
	Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

	if (mode == CAPTURING)
	{
		if (bStartCapture && j < nimages)
		{
			if (clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC)
			{
				for (size_t k = 0; k < cameraNum; k++)
				{
					preUpdateCalibList(view, imagePoints[j]);
				}
				for (size_t k = 0; k < cameraNum; k++)
				{
					updateCalibList(view, imagePoints[j]);
				}
				prevTimestamp = clock();
				j++;
			}
			msg = format("%d/%d", j, nimages);
		}
		if (j >= nimages)
		{
			if (runAndSave(outputFilename, imagePoints, imageSize,
				boardSize, pattern, squareSize, aspectRatio,
				flags, cameraMatrix, distCoeffs,
				writeExtrinsics, writePoints))
				mode = CALIBRATED;
		}
	}
	else if (mode == CALIBRATED)
	{
		if (undistortImage)
		{
			Mat temp = view.clone();
			undistort(temp, view, cameraMatrix, distCoeffs);
			msg = "Undistorted image. Press 'u' to distort";
		}
		else
		{
			msg = "Distorted image. Press 'u' to undistort";
		}
	}
	putText(view, msg, textOrigin, 1, 1,
		mode != CALIBRATED ? Scalar(255, 0, 0) : Scalar(0, 255, 0));

	for (size_t k = 0; k < cameraNum; k++)
	{
		IplImage ldTemp = view;
		IplImage* pLdTemp = &ldTemp;
		calibrationView[k] = pLdTemp;
	}

	if (mode == CALIBRATED)
	{

	}

	return true;
}

void SingleCalibration::preUpdateCalibList(Mat& view, vector<Point2f>& pointBuf)
{
	bool localFound = findChessboardCorners(view, boardSize, pointBuf,
		CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
	// improve the found corners' coordinate accuracy
	if (localFound)
	{
		Mat viewGray;
		cvtColor(view, viewGray, COLOR_BGR2GRAY);
		cornerSubPix(viewGray, pointBuf, Size(11, 11),
			Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
		drawChessboardCorners(view, boardSize, Mat(pointBuf), localFound);
	}
	bPreUpdateResult &= localFound;
}

void SingleCalibration::updateCalibList(Mat& view, vector<Point2f>& pointBuf)
{
	if (bPreUpdateResult)
	{
		bitwise_not(view, view);
	}
	else
	{
		pointBuf.clear();
	}
}