#include "Calibration.h"

using namespace std;
using namespace cv;

Calibration::Calibration()
{
	aspectRatio = 1;
	pattern = CHESSBOARD;
	nframes = 10;
	delay = 1000;
	outputFilename = "out_camera_data.yml";
	writePoints = true;
	writeExtrinsics = true;
	flags = CALIB_FIX_ASPECT_RATIO;
	flipVertical = false;
	videofile = false;
	showUndistorted = false;
	bStartCapture = false;
	prevTimestamp = 0;
}


Calibration::~Calibration()
{
}

bool Calibration::init(int image_width, int image_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type)
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
	hdCvImage.allocate(image_width, image_height);
	ldCvImage.allocate(image_width, image_height);
	ldCalibrationView.allocate(image_width, image_height);
	hdCalibrationView.allocate(image_width, image_height);
	return true;
}

bool Calibration::main(ofImage hdImage)
{
	Mat view, viewGray;

	hdCvImage.setFromPixels(hdImage.getPixels());
	Mat tempMatHdCvImage = cvarrToMat(hdCvImage.getCvImage());
	tempMatHdCvImage.copyTo(view);

	string msg = "Press 'g' to start";
	int baseLine = 0;
	Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
	Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

	if (mode == CAPTURING && bStartCapture)
	{
		if (view.empty())
		{
			bool ok = false;
			if (imagePoints.size() > 0)
			{
				ok = runAndSave(outputFilename, imagePoints, imageSize,
					boardSize, pattern, squareSize, aspectRatio,
					flags, cameraMatrix, distCoeffs,
					writeExtrinsics, writePoints);
				return ok;
			}
			return ok;
		}

		imageSize = view.size();

		if (flipVertical)
			flip(view, view, 0);

		vector<Point2f> pointbuf;
		cvtColor(view, viewGray, COLOR_BGR2GRAY);

		bool found;
		switch (pattern)
		{
		case CHESSBOARD:
			found = findChessboardCorners(view, boardSize, pointbuf,
				CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
			break;
		case CIRCLES_GRID:
			found = findCirclesGrid(view, boardSize, pointbuf);
			break;
		case ASYMMETRIC_CIRCLES_GRID:
			found = findCirclesGrid(view, boardSize, pointbuf, CALIB_CB_ASYMMETRIC_GRID);
			break;
		default:
			ofLog(OF_LOG_ERROR, "Unknown pattern type\n");
			return false;
			break;
		}

		// improve the found corners' coordinate accuracy
		if (pattern == CHESSBOARD && found) cornerSubPix(viewGray, pointbuf, Size(11, 11),
			Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
		if (found)drawChessboardCorners(view, boardSize, Mat(pointbuf), found);

		if (mode == CAPTURING && found &&
			(clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC))
		{
			imagePoints.push_back(pointbuf);
			prevTimestamp = clock();
			bitwise_not(view, view);
		}
		msg = format("%d/%d", (int)imagePoints.size(), nframes);
	}
	if (mode == CAPTURING && imagePoints.size() >= (unsigned)nframes)
	{
		if (runAndSave(outputFilename, imagePoints, imageSize,
			boardSize, pattern, squareSize, aspectRatio,
			flags, cameraMatrix, distCoeffs,
			writeExtrinsics, writePoints))
			mode = CALIBRATED;
	}
	else if (mode == CALIBRATED && undistortImage)
	{
		Mat temp = view.clone();
		undistort(temp, view, cameraMatrix, distCoeffs);
	}
	putText(view, msg, textOrigin, 1, 1,
		mode != CALIBRATED ? Scalar(255, 0, 0) : Scalar(0, 255, 0));

	IplImage temp = view;
	IplImage* pTemp = &temp;
	hdCalibrationView = pTemp;

	return true;
}

bool Calibration::main(ofPixels ldPixels)
{
	Mat view, viewGray;

	ofImage ldImage;
	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);
	ldCvImage.setFromPixels(ldImage.getPixels());

	Mat tempMatLdCvImage = cvarrToMat(ldCvImage.getCvImage());
	tempMatLdCvImage.copyTo(view);

	string msg = "Press 'g' to start";
	int baseLine = 0;
	Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
	Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

	if (mode == CAPTURING && bStartCapture)
	{
		if (view.empty())
		{
			bool ok = false;
			if (imagePoints.size() > 0)
			{
				ok = runAndSave(outputFilename, imagePoints, imageSize,
					boardSize, pattern, squareSize, aspectRatio,
					flags, cameraMatrix, distCoeffs,
					writeExtrinsics, writePoints);
				return ok;
			}
			return ok;
		}

		imageSize = view.size();

		if (flipVertical)
			flip(view, view, 0);

		vector<Point2f> pointbuf;
		cvtColor(view, viewGray, COLOR_BGR2GRAY);

		bool found;
		switch (pattern)
		{
		case CHESSBOARD:
			found = findChessboardCorners(view, boardSize, pointbuf,
				CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
			break;
		case CIRCLES_GRID:
			found = findCirclesGrid(view, boardSize, pointbuf);
			break;
		case ASYMMETRIC_CIRCLES_GRID:
			found = findCirclesGrid(view, boardSize, pointbuf, CALIB_CB_ASYMMETRIC_GRID);
			break;
		default:
			ofLog(OF_LOG_ERROR, "Unknown pattern type\n");
			return false;
			break;
		}

		// improve the found corners' coordinate accuracy
		if (pattern == CHESSBOARD && found) cornerSubPix(viewGray, pointbuf, Size(11, 11),
			Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
		if (found)drawChessboardCorners(view, boardSize, Mat(pointbuf), found);

		if (mode == CAPTURING && found &&
			(clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC))
		{
			imagePoints.push_back(pointbuf);
			prevTimestamp = clock();
			bitwise_not(view, view);
		}
		msg = format("%d/%d", (int)imagePoints.size(), nframes);
	}
	if (mode == CAPTURING && imagePoints.size() >= (unsigned)nframes)
	{
		if (runAndSave(outputFilename, imagePoints, imageSize,
			boardSize, pattern, squareSize, aspectRatio,
			flags, cameraMatrix, distCoeffs,
			writeExtrinsics, writePoints))
			mode = CALIBRATED;
	}
	else if (mode == CALIBRATED && undistortImage)
	{
		Mat temp = view.clone();
		undistort(temp, view, cameraMatrix, distCoeffs);
	}
	putText(view, msg, textOrigin, 1, 1,
		mode != CALIBRATED ? Scalar(255, 0, 0) : Scalar(0, 255, 0));

	IplImage temp = view;
	IplImage* pTemp = &temp;
	ldCalibrationView = pTemp;

	return true;
}
