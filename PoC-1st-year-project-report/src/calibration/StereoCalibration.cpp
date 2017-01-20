#include "StereoCalibration.h"

using namespace std;
using namespace cv;

StereoCalibration::StereoCalibration()
{
	aspectRatio = 1;
	pattern = CHESSBOARD;
	nframes = 10;
	delay = 1000;
	outputFilename = "out_camera_data.yml";
	flags = CALIB_FIX_ASPECT_RATIO;
	flipVertical = false;
	showUndistorted = false;
	bStartCapture = false;
	prevTimestamp = 0;
}


StereoCalibration::~StereoCalibration()
{
}

bool StereoCalibration::init(int image_width, int image_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type)
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

	ldCvImage.allocate(image_width, image_height);
	hdCvImage.allocate(image_width, image_height);
	return false;
}
*/
/*
bool StereoCalibration::main(ofPixels ldPixels, ofImage hdImage)
{
	Mat view, viewGray;

	ofImage ldImage;

	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);

	ldCvImage.setFromPixels(ldImage.getPixels());
	hdCvImage.setFromPixels(hdImage.getPixels());
	Mat tempMatLdCvImage = cvarrToMat(ldCvImage.getCvImage());
	Mat tempMatHdCvImage = cvarrToMat(hdCvImage.getCvImage());
	bothImages.push_back(tempMatLdCvImage);
	bothImages.push_back(tempMatHdCvImage);

	const int maxScale = 2;
	// ARRAY AND VECTOR STORAGE:


	int i, j, k, nimages = nframes;

	imagePoints[0].resize(nimages);
	imagePoints[1].resize(nimages);

	for (k = 0; k < 2; k++)
	{
//		const string& filename = imagelist[i * 2 + k];
		Mat img = bothImages[k];// imread(filename, 0);

		bool found = false;
		vector<Point2f>& corners = imagePoints[k][j];
		for (int scale = 1; scale <= maxScale; scale++)
		{
			Mat timg;
			if (scale == 1)
				timg = img;
			else
				resize(img, timg, Size(), scale, scale);
			found = findChessboardCorners(timg, boardSize, corners,
				CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
			if (found)
			{
				if (scale > 1)
				{
					Mat cornersMat(corners);
					cornersMat *= 1. / scale;
				}
				break;
			}
		}
		if (displayCorners)
		{
			Mat cimg, cimg1;
			cvtColor(img, cimg, COLOR_GRAY2BGR);
			drawChessboardCorners(cimg, boardSize, corners, found);
			double sf = 640. / MAX(img.rows, img.cols);
			resize(cimg, cimg1, Size(), sf, sf);
			imshow("corners", cimg1);
			char c = (char)waitKey(500);
			if (c == 27 || c == 'q' || c == 'Q') //Allow ESC to quit
				exit(-1);
		}
		else
			putchar('.');
		if (!found)
			break;
		cornerSubPix(img, corners, Size(11, 11), Size(-1, -1),
			TermCriteria(TermCriteria::COUNT + TermCriteria::EPS,
				30, 0.01));
	}
	goodImageList.push_back(bothImages[0]);
	goodImageList.push_back(bothImages[1]);
	j++;

	cout << j << " pairs have been successfully detected.\n";
	nimages = j;
	if (nimages < 2)
	{
		cout << "Error: too little pairs to run the calibration\n";
		return;
	}

	imagePoints[0].resize(nimages);
	imagePoints[1].resize(nimages);
	objectPoints.resize(nimages);

	for (i = 0; i < nimages; i++)
	{
		for (j = 0; j < boardSize.height; j++)
			for (k = 0; k < boardSize.width; k++)
				objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
	}

	cout << "Running stereo calibration ...\n";

	Mat cameraMatrix[2], distCoeffs[2];
	cameraMatrix[0] = initCameraMatrix2D(objectPoints, imagePoints[0], imageSize, 0);
	cameraMatrix[1] = initCameraMatrix2D(objectPoints, imagePoints[1], imageSize, 0);
	Mat R, T, E, F;

	double rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
		cameraMatrix[0], distCoeffs[0],
		cameraMatrix[1], distCoeffs[1],
		imageSize, R, T, E, F,
		CALIB_FIX_ASPECT_RATIO +
		CALIB_ZERO_TANGENT_DIST +
		CALIB_USE_INTRINSIC_GUESS +
		CALIB_SAME_FOCAL_LENGTH +
		CALIB_RATIONAL_MODEL +
		CALIB_FIX_K3 + CALIB_FIX_K4 + CALIB_FIX_K5,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 100, 1e-5));
	cout << "done with RMS error=" << rms << endl;

	// CALIBRATION QUALITY CHECK
	// because the output fundamental matrix implicitly
	// includes all the output information,
	// we can check the quality of calibration using the
	// epipolar geometry constraint: m2^t*F*m1=0
	double err = 0;
	int npoints = 0;
	vector<Vec3f> lines[2];
	for (i = 0; i < nimages; i++)
	{
		int npt = (int)imagePoints[0][i].size();
		Mat imgpt[2];
		for (k = 0; k < 2; k++)
		{
			imgpt[k] = Mat(imagePoints[k][i]);
			undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], Mat(), cameraMatrix[k]);
			computeCorrespondEpilines(imgpt[k], k + 1, F, lines[k]);
		}
		for (j = 0; j < npt; j++)
		{
			double errij = fabs(imagePoints[0][i][j].x*lines[1][j][0] +
				imagePoints[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
				fabs(imagePoints[1][i][j].x*lines[0][j][0] +
					imagePoints[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
			err += errij;
		}
		npoints += npt;
	}
	cout << "average epipolar err = " << err / npoints << endl;

	// save intrinsic parameters
	FileStorage fs("intrinsics.yml", FileStorage::WRITE);
	if (fs.isOpened())
	{
		fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] <<
			"M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
		fs.release();
	}
	else
		cout << "Error: can not save the intrinsic parameters\n";

	Mat R1, R2, P1, P2, Q;
	Rect validRoi[2];

	stereoRectify(cameraMatrix[0], distCoeffs[0],
		cameraMatrix[1], distCoeffs[1],
		imageSize, R, T, R1, R2, P1, P2, Q,
		CALIB_ZERO_DISPARITY, -1, imageSize, &validRoi[0], &validRoi[1]);

	fs.open("extrinsics.yml", FileStorage::WRITE);
	if (fs.isOpened())
	{
		fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
		fs.release();
	}
	else
		cout << "Error: can not save the extrinsic parameters\n";

	// OpenCV can handle left-right
	// or up-down camera arrangements
	bool isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));

	// COMPUTE AND DISPLAY RECTIFICATION
	if (!showRectified)
		return;

	Mat rmap[2][2];
	// IF BY CALIBRATED (BOUGUET'S METHOD)
	if (useCalibrated)
	{
		// we already computed everything
	}
	// OR ELSE HARTLEY'S METHOD
	else
		// use intrinsic parameters of each camera, but
		// compute the rectification transformation directly
		// from the fundamental matrix
	{
		vector<Point2f> allimgpt[2];
		for (k = 0; k < 2; k++)
		{
			for (i = 0; i < nimages; i++)
				std::copy(imagePoints[k][i].begin(), imagePoints[k][i].end(), back_inserter(allimgpt[k]));
		}
		F = findFundamentalMat(Mat(allimgpt[0]), Mat(allimgpt[1]), FM_8POINT, 0, 0);
		Mat H1, H2;
		stereoRectifyUncalibrated(Mat(allimgpt[0]), Mat(allimgpt[1]), F, imageSize, H1, H2, 3);

		R1 = cameraMatrix[0].inv()*H1*cameraMatrix[0];
		R2 = cameraMatrix[1].inv()*H2*cameraMatrix[1];
		P1 = cameraMatrix[0];
		P2 = cameraMatrix[1];
	}

	//Precompute maps for cv::remap()
	initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
	initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

	Mat canvas;
	double sf;
	int w, h;
	if (!isVerticalStereo)
	{
		sf = 600. / MAX(imageSize.width, imageSize.height);
		w = cvRound(imageSize.width*sf);
		h = cvRound(imageSize.height*sf);
		canvas.create(h, w * 2, CV_8UC3);
	}
	else
	{
		sf = 300. / MAX(imageSize.width, imageSize.height);
		w = cvRound(imageSize.width*sf);
		h = cvRound(imageSize.height*sf);
		canvas.create(h * 2, w, CV_8UC3);
	}

	for (i = 0; i < nimages; i++)
	{
		for (k = 0; k < 2; k++)
		{
			Mat img = imread(goodImageList[i * 2 + k], 0), rimg, cimg;
			remap(img, rimg, rmap[k][0], rmap[k][1], INTER_LINEAR);
			cvtColor(rimg, cimg, COLOR_GRAY2BGR);
			Mat canvasPart = !isVerticalStereo ? canvas(Rect(w*k, 0, w, h)) : canvas(Rect(0, h*k, w, h));
			resize(cimg, canvasPart, canvasPart.size(), 0, 0, INTER_AREA);
			if (useCalibrated)
			{
				Rect vroi(cvRound(validRoi[k].x*sf), cvRound(validRoi[k].y*sf),
					cvRound(validRoi[k].width*sf), cvRound(validRoi[k].height*sf));
				rectangle(canvasPart, vroi, Scalar(0, 0, 255), 3, 8);
			}
		}

		if (!isVerticalStereo)
			for (j = 0; j < canvas.rows; j += 16)
				line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
		else
			for (j = 0; j < canvas.cols; j += 16)
				line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
		imshow("rectified", canvas);
		char c = (char)waitKey();
		if (c == 27 || c == 'q' || c == 'Q')
			break;
	}
	*/
//	tempMatHdCvImage.copyTo(view);

	/*
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
	calibrationView = pTemp;

	return true;
	return false;
}
*/
