#include "Calibration.h"

using namespace std;
using namespace cv;

Calibration::Calibration()
{
	aspectRatio = 1;
	pattern = CHESSBOARD;
	nframes =10;
	delay = 1000;
	outputFilename = "out_camera_data.yml";//ofToDataPath("out_camera_data.yml");
	writePoints = true;
	writeExtrinsics = true;
	flags = cv::CALIB_FIX_ASPECT_RATIO;
	flipVertical = false;
	videofile = false;
	showUndistorted = false;
	bStartCapture = false;
	prevTimestamp = 0;
	nimages = nframes;
	j = 0;
	bPreUpdateResult = true;
	msg = "Press 'g' to start";
	isVerticalStereo = true;
	additionalViewNum = 0;
	undistortImage = false;
	trackImage = false;
	dualSphereMode = false;
}


Calibration::~Calibration()
{
	if (cvImages != nullptr)
	{
		delete[] cvImages;
	}
	if (calibrationView != nullptr)
	{
		delete[] calibrationView;
	}
	if (imagePoints != nullptr)
	{
		delete[] imagePoints;
	}
	if (cameraMatrix != nullptr)
	{
		delete[] cameraMatrix;
	}
	if (distCoeffs != nullptr)
	{
		delete[] distCoeffs;
	}
	if (matXi != nullptr)
	{
		delete[] matXi;
	}
	if (tempMatView != nullptr)
	{
		delete[] tempMatView;
	}
}

bool Calibration::init(int localImage_width, int localImage_height, int chess_width, int chess_height, float square_size, INPUT_TYPE type, int localCameraNum, int localAdditionalViewNum)
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
	if (localCameraNum <= 0)
	{
		ofLog(OF_LOG_ERROR, "Invalid camera number");
		return false;
	}
	if (localAdditionalViewNum < 0)
	{
		ofLog(OF_LOG_ERROR, "Invalid additional view number");
		return false;
	}
	mode = CAPTURING;
	image_width = localImage_width;
	image_height = localImage_height;
	this->cameraNum = localCameraNum;
	this->additionalViewNum = localAdditionalViewNum;

	cvImages = new ofxCvColorImage[this->cameraNum + this->additionalViewNum];
	calibrationView = new ofxCvColorImage[this->cameraNum + this->additionalViewNum];
	imagePoints = new std::vector<std::vector<cv::Point2f> >[this->cameraNum];
	cameraMatrix = new Mat[this->cameraNum];
	distCoeffs = new Mat[this->cameraNum];
	matXi = new Mat[this->cameraNum];
	tempMatView = new Mat[this->cameraNum];
	for (size_t k = 0; k < this->cameraNum; k++)
	{
		cvImages[k].allocate(image_width, image_height);
		calibrationView[k].allocate(image_width, image_height);
		imagePoints[k].resize(nimages);
	}
	for (size_t k = 0; k < this->additionalViewNum; k++)
	{
		cvImages[this->cameraNum + k].allocate(image_width, 2 * image_height);
		calibrationView[this->cameraNum + k].allocate(image_width, 2 * image_height);
	}
	imageSize.width = image_width;
	imageSize.height = image_height;

	return true;
}

bool Calibration::main(ofImage hdImage, string localOutputFileName)
{
	this->outputFilename = ofToDataPath(localOutputFileName);

	return main(hdImage);
}

bool Calibration::main(ofImage hdImage)
{
	Mat view, viewGray;

	cvImages[0].setFromPixels(hdImage.getPixels());
	Mat tempMatHdCvImage = cvarrToMat(cvImages[0].getCvImage());

	return main(tempMatHdCvImage);
}

bool Calibration::main(ofPixels ldPixels, string localOutputFileName)
{
	this->outputFilename = ofToDataPath(localOutputFileName);

	return main(ldPixels);
}

bool Calibration::main(ofPixels ldPixels)
{
	Mat view, viewGray;

	ofImage ldImage;
	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);
	cvImages[0].setFromPixels(ldImage.getPixels());

	Mat tempMatLdCvImage = cvarrToMat(cvImages[0].getCvImage());

	return main(tempMatLdCvImage);
}

bool Calibration::main(ofPixels* ldPixels)
{
	if (this->cameraNum == 1)
	{
		return main(ldPixels[0]);
	}
	else if (this->cameraNum == 2)
	{
		return main2(ldPixels);
	}
}

bool Calibration::main(Mat tempMatCvImage)
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
				bPreUpdateResult = true;
				for (size_t k = 0; k < cameraNum; k++)
				{
					preUpdateCalibList(view, imagePoints[k][j]);
				}
				for (size_t k = 0; k < cameraNum; k++)
				{
					updateCalibList(view, imagePoints[k][j]);
				}
				if (bPreUpdateResult)
				{
					j++;
				}
				prevTimestamp = clock();
			}
			msg = format("%d/%d", j, nimages);
		}
		if (j >= nimages)
		{
			if (this->cameraNum == 1)
			{
				if (!this->dualSphereMode)
				{
					if (runAndSave(outputFilename, imagePoints[0], imageSize,
						boardSize, pattern, squareSize, aspectRatio,
						flags, cameraMatrix[0], distCoeffs[0],
						writeExtrinsics, writePoints))
						mode = CALIBRATED;
				}
				else
				{
//					flags = cv::omnidir::CALIB_FIX_CENTER;
					if (runAndSave(outputFilename, imagePoints[0], imageSize,
						boardSize, pattern, squareSize, aspectRatio,
						flags, cameraMatrix[0], distCoeffs[0],
						writeExtrinsics, writePoints, matXi[0], this->dualSphereMode))
						mode = CALIBRATED;
				}
			}
		}
	}
	else if (mode == CALIBRATED)
	{
		if (undistortImage)
		{
			Mat temp = view.clone();
			if (!this->dualSphereMode)
			{
				if (!this->ptzSphereMode)
				{
					Mat _D1;
					distCoeffs[0].convertTo(_D1, CV_64F);
					cout << _D1.total() << endl;
					cout << _D1.size() << endl;
					undistort(temp, view, cameraMatrix[0], distCoeffs[0]);
				}
				else
				{
//					cout << distCoeffs[0].depth() << endl;
					Mat _D1;
					distCoeffs[0].convertTo(_D1, CV_64F);
					cout << _D1.total() << endl;
					cout << _D1.size() << endl;
					mycv::omnidir::undistortImage(temp, view, cameraMatrix[0], _D1, 0, cv::omnidir::RECTIFY_PERSPECTIVE);
				}
			}
			else
			{
				mycv::omnidir::undistortImage(temp,view,cameraMatrix[0],distCoeffs[0],matXi[0],cv::omnidir::RECTIFY_PERSPECTIVE);
			}
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

bool Calibration::main2(ofPixels* ldPixels)
{
	ofImage hdImage;
	hdImage.setFromPixels(ldPixels[1]);
	hdImage.setImageType(OF_IMAGE_COLOR);

	return main2(ldPixels[0], hdImage);
}

bool Calibration::main2(ofPixels ldPixels, ofImage hdImage)
{
	vector<Mat> matView;
	ofImage ldImage;

	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);

	cvImages[0].setFromPixels(ldImage.getPixels());
	cvImages[1].setFromPixels(hdImage.getPixels());
	Mat tempMatLdCvImage = cvarrToMat(cvImages[0].getCvImage());
	Mat tempMatHdCvImage = cvarrToMat(cvImages[1].getCvImage());
	tempMatLdCvImage.copyTo(this->tempMatView[0]);
	tempMatHdCvImage.copyTo(this->tempMatView[1]);
	matView.push_back(this->tempMatView[0]);
	matView.push_back(this->tempMatView[1]);
	if (this->additionalViewNum > 0)
	{
		Mat tempLocalMat;
		tempLocalMat = Mat(2 * 480, 640, CV_8UC3);
		matView.push_back(tempLocalMat);
	}

	int i, k;

	if (mode == CAPTURING)
	{
		if (bStartCapture && j < nimages)
		{
			if (clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC)
			{
				bPreUpdateResult = true;
				for (size_t k = 0; k < cameraNum; k++)
				{
					preUpdateCalibList(matView[k], imagePoints[k][j]);
				}
				for (size_t k = 0; k < cameraNum; k++)
				{
					updateCalibList(matView[k], imagePoints[k][j]);
				}
				if (bPreUpdateResult)
				{
					j++;
				}
				prevTimestamp = clock();
			}
			msg = format("%d/%d", j, nimages);
		}
		if (j >= nimages)
		{
			if (runAndSave2(this->outputFilename,
				this->imagePoints,
				this->imageSize, this->boardSize, this->pattern, squareSize,
				aspectRatio, flags, cameraMatrix,
				distCoeffs, writeExtrinsics, writePoints, this->R, this->T, this->E, this->F, this->R1, this->R2, this->P1, this->P2, this->Q, this->validRoi))mode = CALIBRATED;
			mode = CALIBRATED;
		}
	}
	else if (mode == CALIBRATED)
	{
		if (this->cameraNum == 2 && trackImage)
		{
			cout << "Distance:\t" << tracking(matView) << endl;
		}
		if (this->additionalViewNum > 0)
		{
			if (undistortImage)
			{
				Mat temp = postProcessing(matView, this->cameraMatrix, this->distCoeffs, this->imageSize, this->R1, this->P1, this->R2, this->P2, this->validRoi, this->isVerticalStereo, this->useCalibrated);
				matView[2] = temp.clone();
			}
			else
			{
				matView[0].copyTo(matView[2](Rect(0, 0, imageSize.width, imageSize.height)));
				matView[1].copyTo(matView[2](Rect(0, imageSize.height, imageSize.width, imageSize.height)));
			}
		}
	}
	for (size_t k = 0; k < cameraNum; k++)
	{
		IplImage ldTemp = matView[k];
		IplImage* pLdTemp = &ldTemp;
		calibrationView[k] = pLdTemp;
	}
	if (this->additionalViewNum > 0)
	{
		if (mode == CALIBRATED)
		{
			IplImage rectifyTemp = matView[2];
			IplImage* pRectifyTemp = &rectifyTemp;
			calibrationView[2] = pRectifyTemp;
		}
	}
	return true;
}

void Calibration::preUpdateCalibList(Mat& view, vector<Point2f>& pointBuf)
{
	bool localFound = cv::findChessboardCorners(view, boardSize, pointBuf,
		cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
	// improve the found corners' coordinate accuracy
	if (localFound)
	{
		Mat viewGray;
		cvtColor(view, viewGray, COLOR_BGR2GRAY);
		cornerSubPix(viewGray, pointBuf, Size(11, 11),
			Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
		cv::drawChessboardCorners(view, boardSize, Mat(pointBuf), localFound);
	}
	bPreUpdateResult &= localFound;
}

void Calibration::updateCalibList(Mat& view, vector<Point2f>& pointBuf)
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

bool Calibration::runAndSave2(const std::string& outputFilename,
	const std::vector<std::vector<cv::Point2f> >* imagePoints,
	cv::Size imageSize, cv::Size boardSize, Pattern patternType, float squareSize,
	float aspectRatio, int flags, cv::Mat* cameraMatrix,
	cv::Mat* distCoeffs, bool writeExtrinsics, bool writePoints, Mat& R, Mat& T, Mat& E, Mat& F, Mat& R1, Mat& R2, Mat& P1, Mat& P2, Mat& Q, Rect* validROI)
{
	vector<vector<cv::Point3f> > objectPoints;
	int nimages = imagePoints[0].size();
	objectPoints.resize(nimages);
	for (int i = 0; i < nimages; i++)
	{
		for (int j = 0; j < boardSize.height; j++)
			for (int k = 0; k < boardSize.width; k++)
				objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
	}

	std::cout << "Running stereo calibration ...\n";

	cameraMatrix[0] = mycv::initCameraMatrix2D(objectPoints, imagePoints[0], imageSize, 0);
	cameraMatrix[1] = mycv::initCameraMatrix2D(objectPoints, imagePoints[1], imageSize, 0);

	double rms = mycv::stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
		cameraMatrix[0], distCoeffs[0],
		cameraMatrix[1], distCoeffs[1],
		imageSize, R, T, E, F,
		cv::CALIB_FIX_ASPECT_RATIO |
		cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5,
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
	for (int i = 0; i < nimages; i++)
	{
		int npt = (int)imagePoints[0][i].size();
		Mat imgpt[2];
		for (int k = 0; k < 2; k++)
		{
			imgpt[k] = Mat(imagePoints[k][i]);
			undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], Mat(), cameraMatrix[k]);
			mycv::computeCorrespondEpilines(imgpt[k], k + 1, F, lines[k]);
		}
		for (int j = 0; j < npt; j++)
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
	mycv::stereoRectify(cameraMatrix[0], distCoeffs[0],
		cameraMatrix[1], distCoeffs[1],
		imageSize, R, T, R1, R2, P1, P2, Q,
		cv::CALIB_ZERO_DISPARITY, -1, imageSize, &validRoi[0], &validRoi[1]);

	fs.open("extrinsics.yml", FileStorage::WRITE);
	if (fs.isOpened())
	{
		fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
		fs.release();
	}
	else
		std::cout << "Error: can not save the extrinsic parameters\n";

	return true;
}

Mat Calibration::postProcessing(const vector<Mat>& matView, const Mat* cameraMatrix, const Mat* distCoeffs, Size imageSize, const Mat R1, const Mat P1, const Mat R2, const Mat P2, Rect* validRoi, bool isVerticalStereo, bool useCalibrated)
{
	Mat canvas;
	cv::Mat rmap[2][2];

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

	for (int k = 0; k < 2; k++)
	{
		Mat img = matView[k], rimg, cimg;
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

	return canvas;
}

float Calibration::tracking(vector<Mat>& matView)
{
	/*
	vector<Point2f> ldPoint, hdPoint;
	pair<Point2f, Point2f>trackedPair = Alignment::track(matView[0], matView[1], 213, 160, 213, 160);
	//			cout << trackedPair.first << endl;
	circle(matView[0], trackedPair.first, 2, Scalar(255, 0, 0), -1);
	circle(matView[1], trackedPair.second, 2, Scalar(255, 0, 0), -1);

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

	mycv::triangulatePoints(P1, P2, ldPoint, hdPoint, triangulationResult);
	return (triangulationResult.at<float>(2, 0) / triangulationResult.at<float>(3, 0));
	*/
	return 0;
}

