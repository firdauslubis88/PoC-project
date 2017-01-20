#pragma once
#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <ctime>

using namespace std;
using namespace cv;

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

enum INPUT_TYPE {
	bLiveVideo,
	bVideoInput,
	bFileInput
};

namespace {
	double computeReprojectionErrors(
		const vector<vector<Point3f> >& objectPoints,
		const vector<vector<Point2f> >& imagePoints,
		const vector<Mat>& rvecs, const vector<Mat>& tvecs,
		const Mat& cameraMatrix, const Mat& distCoeffs,
		vector<float>& perViewErrors)
	{
		vector<Point2f> imagePoints2;
		int i, totalPoints = 0;
		double totalErr = 0, err;
		perViewErrors.resize(objectPoints.size());

		for (i = 0; i < (int)objectPoints.size(); i++)
		{
			projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
				cameraMatrix, distCoeffs, imagePoints2);
			err = norm(Mat(imagePoints[i]), Mat(imagePoints2), NORM_L2);
			int n = (int)objectPoints[i].size();
			perViewErrors[i] = (float)std::sqrt(err*err / n);
			totalErr += err*err;
			totalPoints += n;
		}

		return std::sqrt(totalErr / totalPoints);
	}

	void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType = CHESSBOARD)
	{
		corners.resize(0);

		switch (patternType)
		{
		case CHESSBOARD:
		case CIRCLES_GRID:
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					corners.push_back(Point3f(float(j*squareSize),
						float(i*squareSize), 0));
			break;

		case ASYMMETRIC_CIRCLES_GRID:
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					corners.push_back(Point3f(float((2 * j + i % 2)*squareSize),
						float(i*squareSize), 0));
			break;

		default:
			CV_Error(Error::StsBadArg, "Unknown pattern type\n");
		}
	}

	bool runCalibration(vector<vector<Point2f> > imagePoints,
		Size imageSize, Size boardSize, Pattern patternType,
		float squareSize, float aspectRatio,
		int flags, Mat& cameraMatrix, Mat& distCoeffs,
		vector<Mat>& rvecs, vector<Mat>& tvecs,
		vector<float>& reprojErrs,
		double& totalAvgErr)
	{
		cameraMatrix = Mat::eye(3, 3, CV_64F);
		if (flags & CALIB_FIX_ASPECT_RATIO)
			cameraMatrix.at<double>(0, 0) = aspectRatio;

		distCoeffs = Mat::zeros(8, 1, CV_64F);

		vector<vector<Point3f> > objectPoints(1);
		calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

		objectPoints.resize(imagePoints.size(), objectPoints[0]);

		double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
			distCoeffs, rvecs, tvecs, flags | CALIB_FIX_K4 | CALIB_FIX_K5);
		///*|CALIB_FIX_K3*/|CALIB_FIX_K4|CALIB_FIX_K5);
		printf("RMS error reported by calibrateCamera: %g\n", rms);

		bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

		totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
			rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

		return ok;
	}


	void saveCameraParams(const string& filename,
		Size imageSize, Size boardSize,
		float squareSize, float aspectRatio, int flags,
		const Mat& cameraMatrix, const Mat& distCoeffs,
		const vector<Mat>& rvecs, const vector<Mat>& tvecs,
		const vector<float>& reprojErrs,
		const vector<vector<Point2f> >& imagePoints,
		double totalAvgErr)
	{
		stringstream relativePathFilename;
		relativePathFilename << "bin\\data\\" << filename;
		FileStorage fs(relativePathFilename.str(), FileStorage::WRITE);

		time_t tt;
		time(&tt);
		struct tm *t2 = localtime(&tt);
		char buf[1024];
		strftime(buf, sizeof(buf) - 1, "%c", t2);

		fs << "calibration_time" << buf;

		if (!rvecs.empty() || !reprojErrs.empty())
			fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
		fs << "image_width" << imageSize.width;
		fs << "image_height" << imageSize.height;
		fs << "board_width" << boardSize.width;
		fs << "board_height" << boardSize.height;
		fs << "square_size" << squareSize;

		if (flags & CALIB_FIX_ASPECT_RATIO)
			fs << "aspectRatio" << aspectRatio;

		if (flags != 0)
		{
			sprintf(buf, "flags: %s%s%s%s",
				flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
				flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
				flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
				flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
			//cvWriteComment( *fs, buf, 0 );
		}

		fs << "flags" << flags;

		fs << "camera_matrix" << cameraMatrix;
		fs << "distortion_coefficients" << distCoeffs;

		fs << "avg_reprojection_error" << totalAvgErr;
		if (!reprojErrs.empty())
			fs << "per_view_reprojection_errors" << Mat(reprojErrs);

		if (!rvecs.empty() && !tvecs.empty())
		{
			CV_Assert(rvecs[0].type() == tvecs[0].type());
			Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
			for (int i = 0; i < (int)rvecs.size(); i++)
			{
				Mat r = bigmat(Range(i, i + 1), Range(0, 3));
				Mat t = bigmat(Range(i, i + 1), Range(3, 6));

				CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
				CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
				//*.t() is MatExpr (not Mat) so we can use assignment operator
				r = rvecs[i].t();
				t = tvecs[i].t();
			}
			//cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
			fs << "extrinsic_parameters" << bigmat;
		}

		if (!imagePoints.empty())
		{
			Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
			for (int i = 0; i < (int)imagePoints.size(); i++)
			{
				Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
				Mat imgpti(imagePoints[i]);
				imgpti.copyTo(r);
			}
			fs << "image_points" << imagePtMat;
		}
	}

	bool readStringList(const string& filename, vector<string>& l)
	{
		l.resize(0);
		FileStorage fs(filename, FileStorage::READ);
		if (!fs.isOpened())
			return false;
		FileNode n = fs.getFirstTopLevelNode();
		if (n.type() != FileNode::SEQ)
			return false;
		FileNodeIterator it = n.begin(), it_end = n.end();
		for (; it != it_end; ++it)
			l.push_back((string)*it);
		return true;
	}


	bool runAndSave(const string& outputFilename,
		const vector<vector<Point2f> >& imagePoints,
		Size imageSize, Size boardSize, Pattern patternType, float squareSize,
		float aspectRatio, int flags, Mat& cameraMatrix,
		Mat& distCoeffs, bool writeExtrinsics, bool writePoints)
	{
		vector<Mat> rvecs, tvecs;
		vector<float> reprojErrs;
		double totalAvgErr = 0;

		bool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
			aspectRatio, flags, cameraMatrix, distCoeffs,
			rvecs, tvecs, reprojErrs, totalAvgErr);
		printf("%s. avg reprojection error = %.2f\n",
			ok ? "Calibration succeeded" : "Calibration failed",
			totalAvgErr);

		if (ok)
			saveCameraParams(outputFilename, imageSize,
				boardSize, squareSize, aspectRatio,
				flags, cameraMatrix, distCoeffs,
				writeExtrinsics ? rvecs : vector<Mat>(),
				writeExtrinsics ? tvecs : vector<Mat>(),
				writeExtrinsics ? reprojErrs : vector<float>(),
				writePoints ? imagePoints : vector<vector<Point2f> >(),
				totalAvgErr);
		return ok;
	}

}
