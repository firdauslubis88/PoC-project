#include "StereoCalibration.h"

using namespace std;
using namespace cv;

StereoCalibration::StereoCalibration()
{
	aspectRatio = 1;
	pattern = CHESSBOARD;
	nframes = 10;
	delay = 1000;
	outputFilename = ofToDataPath("out_camera_data.yml");
	flags = CALIB_FIX_ASPECT_RATIO;
	flipVertical = false;
	showUndistorted = false;
	bStartCapture = false;
	prevTimestamp = 0;
	showRectified = true;
	useCalibrated = true;
	undistortImage = false;
	nimages = nframes;
	j = 0;
	isVerticalStereo = true;
	trackImage = false;
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
	rectifyCvImage.allocate(image_width, 2 * image_height);
	ldCalibrationView.allocate(image_width, image_height);
	hdCalibrationView.allocate(image_width, image_height);
	rectifyCalibrationView.allocate(image_width, 2 * image_height);
	imagePoints[0].resize(nimages);
	imagePoints[1].resize(nimages);

	imageSize.width = image_width;
	imageSize.height = image_height;
	return false;
}

bool StereoCalibration::main2(ofPixels ldPixels, ofImage hdImage)
{
	Mat ldView, hdView, rectifyView;

	ofImage ldImage;

	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);

	ldCvImage.setFromPixels(ldImage.getPixels());
	hdCvImage.setFromPixels(hdImage.getPixels());
	Mat tempMatLdCvImage = cvarrToMat(ldCvImage.getCvImage());
	Mat tempMatHdCvImage = cvarrToMat(hdCvImage.getCvImage());
	tempMatLdCvImage.copyTo(ldView);
	tempMatHdCvImage.copyTo(hdView);

	bothImages.push_back(ldView);
	bothImages.push_back(hdView);

	int i, k;

	if (mode == CAPTURING)
	{
		if (bStartCapture && j < nimages)
		{
			if (clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC)
			{
				bool ldFound = false;
				vector<Point2f>& ldCorners = imagePoints[0][j];
				ldFound = findChessboardCorners(ldView, boardSize, ldCorners,
					CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
				if (ldFound)
				{
					Mat ldViewGray;
					cvtColor(ldView, ldViewGray, COLOR_BGR2GRAY);
					cornerSubPix(ldViewGray, ldCorners, Size(11, 11), Size(-1, -1),
						TermCriteria(TermCriteria::COUNT + TermCriteria::EPS,
							30, 0.1));
					drawChessboardCorners(ldView, boardSize, ldCorners, ldFound);
				}
				vector<Point2f>& hdCorners = imagePoints[1][j];
				bool hdFound = false;
				hdFound = findChessboardCorners(hdView, boardSize, hdCorners,
					CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
				if (hdFound)
				{
					Mat hdViewGray;
					cvtColor(hdView, hdViewGray, COLOR_BGR2GRAY);
					cornerSubPix(hdViewGray, hdCorners, Size(11, 11), Size(-1, -1),
						TermCriteria(TermCriteria::COUNT + TermCriteria::EPS,
							30, 0.1));
					drawChessboardCorners(hdView, boardSize, hdCorners, hdFound);
				}
				bool bTake = hdFound & ldFound;
				if (bTake)
				{
					bitwise_not(ldView, ldView);
					bitwise_not(hdView, hdView);
					j++;
				}
				else
				{
					ldCorners.clear();
					hdCorners.clear();
				}
			}
		}
		if (j >= nimages)
		{
			objectPoints.resize(nimages);

			for (i = 0; i < nimages; i++)
			{
				for (j = 0; j < boardSize.height; j++)
					for (k = 0; k < boardSize.width; k++)
						objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
			}

			std::cout << "Running stereo calibration ...\n";

			cameraMatrix[0] = initCameraMatrix2D(objectPoints, imagePoints[0], imageSize, 0);
			cameraMatrix[1] = initCameraMatrix2D(objectPoints, imagePoints[1], imageSize, 0);

			double rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
				cameraMatrix[0], distCoeffs[0],
				cameraMatrix[1], distCoeffs[1],
				imageSize, R, T, E, F,
				CALIB_FIX_ASPECT_RATIO |
				CALIB_FIX_K4 | CALIB_FIX_K5,
				TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 100, 1e-5));
			std::cout << "done with RMS error=" << rms << endl;

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
			std::cout << "average epipolar err = " << err / npoints << endl;

			// save intrinsic parameters
			FileStorage fs("intrinsics.yml", FileStorage::WRITE);
			if (fs.isOpened())
			{
				fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] <<
					"M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
				fs.release();
			}
			else
				std::cout << "Error: can not save the intrinsic parameters\n";

			//			Mat R1, R2, P1, P2, Q;
			Rect validRoi[2];

			cv::stereoRectify(cameraMatrix[0], distCoeffs[0],
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
				std::cout << "Error: can not save the extrinsic parameters\n";

			mode = CALIBRATED;
		}
		prevTimestamp = clock();
	}
	else if (mode == CALIBRATED)
	{
		Mat canvas;
		vector<Point2f> ldPoint, hdPoint;
		//		Mat ldMat = Mat(2,1,CV_64FC1), hdMat = Mat(2, 1, CV_64FC1);
		if (trackImage)
		{
			pair<Point2f, Point2f>trackedPair = alignment.track(ldView, hdView, 213, 160, 213, 160);
			//			cout << trackedPair.first << endl;
			circle(bothImages[0], trackedPair.first, 2, Scalar(255, 0, 0), -1);
			circle(bothImages[1], trackedPair.second, 2, Scalar(255, 0, 0), -1);
			/*
			ldMat.at<double>(0, 0) = trackedPair.first.x;
			ldMat.at<double>(1, 0) = trackedPair.first.y;
			hdMat.at<double>(0, 0) = trackedPair.second.x;
			hdMat.at<double>(1, 0) = trackedPair.second.y;
			*/
			ldPoint.push_back(trackedPair.first);
			hdPoint.push_back(trackedPair.second);
			//			cout << "BEFORE:\t" << ldPoint << endl;
			undistortPoints(ldPoint, ldPoint, cameraMatrix[0], distCoeffs[0], Mat(), cameraMatrix[0]);
			undistortPoints(hdPoint, hdPoint, cameraMatrix[1], distCoeffs[1], Mat(), cameraMatrix[1]);
			//			cout << "AFTER:\t" << ldPoint << endl;
			Mat triangulationResult = Mat(4, 1, CV_64FC1);
			Mat cam0, cam1, RT0, RT1;
			RT0 = Mat(3, 4, CV_64FC1);
			RT1 = Mat(3, 4, CV_64FC1);
			RT0(Rect(0, 0, 3, 3)) = Mat::eye(3, 3, CV_64FC1);
			RT0(Rect(3, 0, 1, 3)) = Mat::zeros(3, 1, CV_64FC1);
			R.copyTo(RT1(Rect(0, 0, 3, 3)));
			T.copyTo(RT1(Rect(3, 0, 1, 3)));
			//			cout << RT0 << endl;
			//			cout << RT1 << endl;

			triangulatePoints(P1, P2, ldPoint, hdPoint, triangulationResult);
			cout << "Distance:\t" << triangulationResult.at<float>(2, 0) / triangulationResult.at<float>(3, 0) << endl;
			//			cout << triangulationResult << endl;
		}
		if (undistortImage)
		{
			//Precompute maps for cv::remap()
			initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
			initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);
			double sf = 1.0;
			int w, h;
			if (!isVerticalStereo)
			{
				//				sf = 600. / (imageSize.width>imageSize.height ? imageSize.width : imageSize.height);
				w = imageSize.width;// cvRound(imageSize.width*sf);
				h = imageSize.height;// cvRound(imageSize.height*sf);
				canvas.create(h, w * 2, CV_8UC3);
			}
			else
			{
				//				sf = 300. / (imageSize.width>imageSize.height ? imageSize.width : imageSize.height);
				w = imageSize.width;// cvRound(imageSize.width*sf);
				h = imageSize.height;// cvRound(imageSize.height*sf);
				canvas.create(h * 2, w, CV_8UC3);
			}

			for (k = 0; k < 2; k++)
			{
				Mat img = bothImages[k], rimg, cimg;
				remap(img, rimg, rmap[k][0], rmap[k][1], INTER_LINEAR);
				//				cvtColor(rimg, cimg, COLOR_GRAY2BGR);
				cimg = rimg.clone();
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
				for (int j = 0; j < canvas.rows; j += 16)
					line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
			else
				for (int j = 0; j < canvas.cols; j += 16)
					line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
			rectifyView = canvas.clone();
		}
		else
		{
			rectifyView = Mat(2 * imageSize.height, imageSize.width, CV_8UC3);
			ldView.copyTo(rectifyView(Rect(0, 0, imageSize.width, imageSize.height)));
			hdView.copyTo(rectifyView(Rect(0, imageSize.height, imageSize.width, imageSize.height)));
		}
		/*
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
		*/
	}
	bothImages.clear();
	IplImage ldTemp = ldView;
	IplImage* pLdTemp = &ldTemp;
	ldCalibrationView = pLdTemp;
	IplImage hdTemp = hdView;
	IplImage* pHdTemp = &hdTemp;
	hdCalibrationView = pHdTemp;
	if (mode == CALIBRATED)
	{
		IplImage rectifyTemp = rectifyView;
		IplImage* pRectifyTemp = &rectifyTemp;
		rectifyCalibrationView = pRectifyTemp;
	}
	return true;
}

bool StereoCalibration::main(ofPixels ldPixels, ofImage hdImage)
{
	Mat ldView, hdView, ldViewGray, hdViewGray, rectifyView;

	ofImage ldImage;

	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);

	ldCvImage.setFromPixels(ldImage.getPixels());
	hdCvImage.setFromPixels(hdImage.getPixels());
	Mat tempMatLdCvImage = cvarrToMat(ldCvImage.getCvImage());
	Mat tempMatHdCvImage = cvarrToMat(hdCvImage.getCvImage());
	tempMatLdCvImage.copyTo(ldView);
	tempMatHdCvImage.copyTo(hdView);

	bothImages.push_back(ldView);
	bothImages.push_back(hdView);

	int i, k;

	if (mode == CAPTURING)
	{
		if (bStartCapture && j < nimages)
		{
			bool ldFound = false;
			bool hdFound = false;
			vector<Point2f>& ldCorners = imagePoints[0][j];
			ldFound = findChessboardCorners(ldView, boardSize, ldCorners,
				CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
			if (ldFound)
			{
				cvtColor(ldView, ldViewGray, COLOR_BGR2GRAY);
				cornerSubPix(ldViewGray, ldCorners, Size(11, 11), Size(-1, -1),
					TermCriteria(TermCriteria::COUNT + TermCriteria::EPS,
						30, 0.1));
				drawChessboardCorners(ldView, boardSize, ldCorners, ldFound);
			}
			vector<Point2f>& hdCorners = imagePoints[1][j];
			hdFound = findChessboardCorners(hdView, boardSize, hdCorners,
				CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
			if (hdFound)
			{
				cvtColor(hdView, hdViewGray, COLOR_BGR2GRAY);
				cornerSubPix(hdViewGray, hdCorners, Size(11, 11), Size(-1, -1),
					TermCriteria(TermCriteria::COUNT + TermCriteria::EPS,
						30, 0.1));
				drawChessboardCorners(hdView, boardSize, hdCorners, hdFound);
			}
			if (hdFound && ldFound && (clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC))
			{
				j++;
				prevTimestamp = clock();
				bitwise_not(ldView, ldView);
				bitwise_not(hdView, hdView);
			}
			else
			{
				ldCorners.clear();
				hdCorners.clear();
			}
		}
		if(j >= nimages)
		{
			objectPoints.resize(nimages);

			for (i = 0; i < nimages; i++)
			{
				for (j = 0; j < boardSize.height; j++)
					for (k = 0; k < boardSize.width; k++)
						objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
			}

			std::cout << "Running stereo calibration ...\n";

			cameraMatrix[0] = initCameraMatrix2D(objectPoints, imagePoints[0], imageSize, 0);
			cameraMatrix[1] = initCameraMatrix2D(objectPoints, imagePoints[1], imageSize, 0);

			double rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
				cameraMatrix[0], distCoeffs[0],
				cameraMatrix[1], distCoeffs[1],
				imageSize, R, T, E, F,
				CALIB_FIX_ASPECT_RATIO |
				CALIB_FIX_K4 | CALIB_FIX_K5,
				TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 100, 1e-5));
			std::cout << "done with RMS error=" << rms << endl;

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
			std::cout << "average epipolar err = " << err / npoints << endl;

			// save intrinsic parameters
			FileStorage fs("intrinsics.yml", FileStorage::WRITE);
			if (fs.isOpened())
			{
				fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] <<
					"M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
				fs.release();
			}
			else
				std::cout << "Error: can not save the intrinsic parameters\n";

//			Mat R1, R2, P1, P2, Q;
			Rect validRoi[2];

			cv::stereoRectify(cameraMatrix[0], distCoeffs[0],
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
				std::cout << "Error: can not save the extrinsic parameters\n";

			mode = CALIBRATED;
		}
	}
	else if (mode == CALIBRATED)
	{
		Mat canvas;
		vector<Point2f> ldPoint, hdPoint;
//		Mat ldMat = Mat(2,1,CV_64FC1), hdMat = Mat(2, 1, CV_64FC1);
		if (trackImage)
		{
			pair<Point2f, Point2f>trackedPair = alignment.track(ldView, hdView, 213, 160, 213, 160);
//			cout << trackedPair.first << endl;
			circle(bothImages[0], trackedPair.first, 2, Scalar(255, 0, 0), -1);
			circle(bothImages[1], trackedPair.second, 2, Scalar(255, 0, 0), -1);
			/*
			ldMat.at<double>(0, 0) = trackedPair.first.x;
			ldMat.at<double>(1, 0) = trackedPair.first.y;
			hdMat.at<double>(0, 0) = trackedPair.second.x;
			hdMat.at<double>(1, 0) = trackedPair.second.y;
			*/
			ldPoint.push_back(trackedPair.first);
			hdPoint.push_back(trackedPair.second);
//			cout << "BEFORE:\t" << ldPoint << endl;
			undistortPoints(ldPoint, ldPoint, cameraMatrix[0], distCoeffs[0], Mat(), cameraMatrix[0]);
			undistortPoints(hdPoint, hdPoint, cameraMatrix[1], distCoeffs[1], Mat(), cameraMatrix[1]);
//			cout << "AFTER:\t" << ldPoint << endl;
			Mat triangulationResult = Mat(4, 1, CV_64FC1);
			Mat cam0, cam1, RT0, RT1;
			RT0 = Mat(3, 4, CV_64FC1);
			RT1 = Mat(3, 4, CV_64FC1);
			RT0(Rect(0, 0, 3, 3)) = Mat::eye(3, 3, CV_64FC1);
			RT0(Rect(3, 0, 1, 3)) = Mat::zeros(3, 1, CV_64FC1);
			R.copyTo(RT1(Rect(0, 0, 3, 3)));
			T.copyTo(RT1(Rect(3, 0, 1, 3)));
//			cout << RT0 << endl;
//			cout << RT1 << endl;

			triangulatePoints(P1, P2, ldPoint, hdPoint, triangulationResult);
			cout << "Distance:\t" << triangulationResult.at<float>(2, 0)/triangulationResult.at<float>(3, 0) << endl;
//			cout << triangulationResult << endl;
		}
		if (undistortImage)
		{
			//Precompute maps for cv::remap()
			initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
			initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);
			double sf = 1.0;
			int w, h;
			if (!isVerticalStereo)
			{
//				sf = 600. / (imageSize.width>imageSize.height ? imageSize.width : imageSize.height);
				w = imageSize.width;// cvRound(imageSize.width*sf);
				h = imageSize.height;// cvRound(imageSize.height*sf);
				canvas.create(h, w * 2, CV_8UC3);
			}
			else
			{
//				sf = 300. / (imageSize.width>imageSize.height ? imageSize.width : imageSize.height);
				w = imageSize.width;// cvRound(imageSize.width*sf);
				h = imageSize.height;// cvRound(imageSize.height*sf);
				canvas.create(h * 2, w, CV_8UC3);
			}

			for (k = 0; k < 2; k++)
			{
				Mat img = bothImages[k], rimg, cimg;
				remap(img, rimg, rmap[k][0], rmap[k][1], INTER_LINEAR);
//				cvtColor(rimg, cimg, COLOR_GRAY2BGR);
				cimg = rimg.clone();
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
				for (int j = 0; j < canvas.rows; j += 16)
					line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
			else
				for (int j = 0; j < canvas.cols; j += 16)
					line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
			rectifyView = canvas.clone();
		}
		else 
		{
			rectifyView = Mat(2 * imageSize.height, imageSize.width, CV_8UC3);
			ldView.copyTo(rectifyView(Rect(0, 0, imageSize.width, imageSize.height)));
			hdView.copyTo(rectifyView(Rect(0, imageSize.height, imageSize.width, imageSize.height)));
		}
	/*
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
		*/
	}
	bothImages.clear();
	IplImage ldTemp = ldView;
	IplImage* pLdTemp = &ldTemp;
	ldCalibrationView = pLdTemp;
	IplImage hdTemp = hdView;
	IplImage* pHdTemp = &hdTemp;
	hdCalibrationView = pHdTemp;
	if (mode == CALIBRATED)
	{
		IplImage rectifyTemp = rectifyView;
		IplImage* pRectifyTemp = &rectifyTemp;
		rectifyCalibrationView = pRectifyTemp;
	}
	return true;
}
