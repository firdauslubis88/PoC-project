#pragma once
#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
//#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
//#include "opencv2/ccalib/omnidir.hpp"

#include <iostream>
#include <ctime>

#include "myCalibration.h"
#include "myOmnidir.h"

//using namespace std;
//using namespace cv;

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

enum INPUT_TYPE {
	bLiveVideo,
	bVideoInput,
	bFileInput
};

namespace {
	double computeReprojectionErrors(
		const std::vector<std::vector<cv::Point3f> >& objectPoints,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
		std::vector<float>& perViewErrors)
	{
		std::vector<cv::Point2f> imagePoints2;
		int i, totalPoints = 0;
		double totalErr = 0, err;
		perViewErrors.resize(objectPoints.size());

		for (i = 0; i < (int)objectPoints.size(); i++)
		{
			mycv::projectPoints(cv::Mat(objectPoints[i]), rvecs[i], tvecs[i],
				cameraMatrix, distCoeffs, imagePoints2);
			err = norm(cv::Mat(imagePoints[i]), cv::Mat(imagePoints2), cv::NORM_L2);
			int n = (int)objectPoints[i].size();
			perViewErrors[i] = (float)std::sqrt(err*err / n);
			totalErr += err*err;
			totalPoints += n;
		}

		return std::sqrt(totalErr / totalPoints);
	}

	double computeReprojectionErrors(
		const std::vector<std::vector<cv::Point3f> >& objectPoints,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
		std::vector<float>& perViewErrors, const cv::Mat& matXi, const cv::Mat& matIdx)
	{
		std::vector<cv::Point2f> imagePoints2;
		int i, totalPoints = 0;
		double totalErr = 0, err;
		perViewErrors.resize(objectPoints.size());
		int nimages = matIdx.total();
		/*
		cout << objectPoints.size() << endl;
		cout << nimages << endl;
		cout << tvecs.size() << endl;
		*/
		for (i = 0; i < nimages; i++)
		{
			/*
			cout << "i = \t" << i << endl;
			cout << matIdx.at<int>(i) << endl;
			cout << rvecs[matIdx.at<int>(i)] << endl;
			cout << tvecs[matIdx.at<int>(i)] << endl;
			*/
			mycv::omnidir::projectPoints(cv::Mat(objectPoints[matIdx.at<int>(i)]), imagePoints2, rvecs[i], tvecs[i],
				cameraMatrix, matXi.at<double>(0), distCoeffs);
			err = norm(cv::Mat(imagePoints[i]), cv::Mat(imagePoints2), cv::NORM_L2);
			int n = (int)objectPoints[i].size();
			perViewErrors[i] = (float)std::sqrt(err*err / n);
			totalErr += err*err;
			totalPoints += n;
		}

		return std::sqrt(totalErr / totalPoints);
	}

	void calcChessboardCorners(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners, Pattern patternType = CHESSBOARD)
	{
		corners.resize(0);

		switch (patternType)
		{
		case CHESSBOARD:
		case CIRCLES_GRID:
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					corners.push_back(cv::Point3f(float(j*squareSize),
						float(i*squareSize), 0));
			break;

		case ASYMMETRIC_CIRCLES_GRID:
			for (int i = 0; i < boardSize.height; i++)
				for (int j = 0; j < boardSize.width; j++)
					corners.push_back(cv::Point3f(float((2 * j + i % 2)*squareSize),
						float(i*squareSize), 0));
			break;

		default:
			CV_Error(cv::Error::StsBadArg, "Unknown pattern type\n");
		}
	}

	bool runCalibration(std::vector<std::vector<cv::Point2f> > imagePoints,
		cv::Size imageSize, cv::Size boardSize, Pattern patternType,
		float squareSize, float aspectRatio,
		int flags, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
		std::vector<float>& reprojErrs,
		double& totalAvgErr)
	{
		cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
		if (flags & cv::CALIB_FIX_ASPECT_RATIO)
			cameraMatrix.at<double>(0, 0) = aspectRatio;

		distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

		std::vector<std::vector<cv::Point3f> > objectPoints(1);
		calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

		objectPoints.resize(imagePoints.size(), objectPoints[0]);

		double rms = cv::calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
			distCoeffs, rvecs, tvecs, flags | cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5 | cv::CALIB_FIX_K3);
		///*|CALIB_FIX_K3*/|CALIB_FIX_K4|CALIB_FIX_K5);
		printf("RMS error reported by calibrateCamera: %g\n", rms);

		bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

		totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
			rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

		return ok;
	}

	bool runCalibration(std::vector<std::vector<cv::Point2f> > imagePoints,
		cv::Size imageSize, cv::Size boardSize, Pattern patternType,
		float squareSize, float aspectRatio,
		int flags, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
		std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
		std::vector<float>& reprojErrs,
		double& totalAvgErr, cv::Mat& xi, bool localDualSphereMode)
	{
		if (!localDualSphereMode)
		{
			return runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize, aspectRatio, flags, cameraMatrix, distCoeffs, rvecs, tvecs, reprojErrs, totalAvgErr);
		}
		else
		{
			/*
			cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
			if (flags & cv::CALIB_FIX_ASPECT_RATIO)
				cameraMatrix.at<double>(0, 0) = aspectRatio;

			distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
			*/
			std::vector<std::vector<cv::Point3f> > objectPoints(1);
			calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

			objectPoints.resize(imagePoints.size(), objectPoints[0]);

			/*
			double rms = cv::calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
				distCoeffs, rvecs, tvecs, flags | cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5);
			*/

			cv::Mat matIdx;
			double rms = mycv::omnidir::calibrate(objectPoints, imagePoints, imageSize, cameraMatrix, xi,
				distCoeffs, rvecs, tvecs, flags, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 100, 1e-5), matIdx);

			printf("RMS error reported by calibrateCamera: %g\n", rms);

			bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
//			cout << objectPoints.size() << endl;
//			cout << matIdx.total() << endl;

			totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
				rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs, xi, matIdx);
			return ok;
		}
	}


	void saveCameraParams(const std::string& filename,
		cv::Size imageSize, cv::Size boardSize,
		float squareSize, float aspectRatio, int flags,
		const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const std::vector<float>& reprojErrs,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		double totalAvgErr)
	{
		std::stringstream relativePathFilename;
		relativePathFilename << "bin\\data\\" << filename;
		cv::FileStorage fs(relativePathFilename.str(), cv::FileStorage::WRITE);

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

		if (flags & cv::CALIB_FIX_ASPECT_RATIO)
			fs << "aspectRatio" << aspectRatio;

		if (flags != 0)
		{
			sprintf(buf, "flags: %s%s%s%s",
				flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
				flags & cv::CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
				flags & cv::CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
				flags & cv::CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
			//cvWriteComment( *fs, buf, 0 );
		}

		fs << "flags" << flags;

//		printf("HERE?\n");
		fs << "camera_matrix" << cameraMatrix;
		fs << "distortion_coefficients" << distCoeffs;

		fs << "avg_reprojection_error" << totalAvgErr;
		if (!reprojErrs.empty())
			fs << "per_view_reprojection_errors" << cv::Mat(reprojErrs);

		if (!rvecs.empty() && !tvecs.empty())
		{
			CV_Assert(rvecs[0].type() == tvecs[0].type());
			cv::Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
			for (int i = 0; i < (int)rvecs.size(); i++)
			{
				cv::Mat r = bigmat(cv::Range(i, i + 1), cv::Range(0, 3));
				cv::Mat t = bigmat(cv::Range(i, i + 1), cv::Range(3, 6));

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
			cv::Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
			for (int i = 0; i < (int)imagePoints.size(); i++)
			{
				cv::Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
				cv::Mat imgpti(imagePoints[i]);
				imgpti.copyTo(r);
			}
			fs << "image_points" << imagePtMat;
		}
	}

	void saveCameraParams2(const std::string& filename,
		cv::Size imageSize, cv::Size boardSize,
		float squareSize, float aspectRatio, int flags,
		const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
		const std::vector<float>& reprojErrs,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		double totalAvgErr, const cv::Mat& matXi)
	{
		std::stringstream relativePathFilename;
		relativePathFilename << "bin\\data\\" << filename;
		cv::FileStorage fs(relativePathFilename.str(), cv::FileStorage::WRITE);

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

		if (flags & cv::CALIB_FIX_ASPECT_RATIO)
			fs << "aspectRatio" << aspectRatio;

		if (flags != 0)
		{
			sprintf(buf, "flags: %s%s%s%s",
				flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
				flags & cv::CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
				flags & cv::CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
				flags & cv::CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
			//cvWriteComment( *fs, buf, 0 );
		}

		fs << "flags" << flags;

		fs << "camera_matrix" << cameraMatrix;
		fs << "distortion_coefficients" << distCoeffs;
		fs << "xi_value" << matXi;

		fs << "avg_reprojection_error" << totalAvgErr;
		if (!reprojErrs.empty())
			fs << "per_view_reprojection_errors" << cv::Mat(reprojErrs);

		if (!rvecs.empty() && !tvecs.empty())
		{
			CV_Assert(rvecs[0].type() == tvecs[0].type());
			cv::Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
			for (int i = 0; i < (int)rvecs.size(); i++)
			{
				cv::Mat r = bigmat(cv::Range(i, i + 1), cv::Range(0, 3));
				cv::Mat t = bigmat(cv::Range(i, i + 1), cv::Range(3, 6));

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
			cv::Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
			for (int i = 0; i < (int)imagePoints.size(); i++)
			{
				cv::Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
				cv::Mat imgpti(imagePoints[i]);
				imgpti.copyTo(r);
			}
			fs << "image_points" << imagePtMat;
		}
	}

	bool readStringList(const std::string& filename, std::vector<std::string>& l)
	{
		l.resize(0);
		cv::FileStorage fs(filename, cv::FileStorage::READ);
		if (!fs.isOpened())
			return false;
		cv::FileNode n = fs.getFirstTopLevelNode();
		if (n.type() != cv::FileNode::SEQ)
			return false;
		cv::FileNodeIterator it = n.begin(), it_end = n.end();
		for (; it != it_end; ++it)
			l.push_back((std::string)*it);
		return true;
	}


	bool runAndSave(const std::string& outputFilename,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		cv::Size imageSize, cv::Size boardSize, Pattern patternType, float squareSize,
		float aspectRatio, int flags, cv::Mat& cameraMatrix,
		cv::Mat& distCoeffs, bool writeExtrinsics, bool writePoints)
	{
		std::vector<cv::Mat> rvecs, tvecs;
		std::vector<float> reprojErrs;
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
				writeExtrinsics ? rvecs : std::vector<cv::Mat>(),
				writeExtrinsics ? tvecs : std::vector<cv::Mat>(),
				writeExtrinsics ? reprojErrs : std::vector<float>(),
				writePoints ? imagePoints : std::vector<std::vector<cv::Point2f> >(),
				totalAvgErr);
		return ok;
	}

	bool runAndSave(const std::string& outputFilename,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		cv::Size imageSize, cv::Size boardSize, Pattern patternType, float squareSize,
		float aspectRatio, int flags, cv::Mat& cameraMatrix,
		cv::Mat& distCoeffs, bool writeExtrinsics, bool writePoints, cv::Mat& matXi, bool localDualSphereMode)
	{
		std::vector<cv::Mat> rvecs, tvecs;
		std::vector<float> reprojErrs;
		double totalAvgErr = 0;

		bool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
			aspectRatio, flags, cameraMatrix, distCoeffs,
			rvecs, tvecs, reprojErrs, totalAvgErr, matXi, localDualSphereMode);
		printf("%s. avg reprojection error = %.2f\n",
			ok ? "Calibration succeeded" : "Calibration failed",
			totalAvgErr);

		if (ok)
			saveCameraParams2(outputFilename, imageSize,
				boardSize, squareSize, aspectRatio,
				flags, cameraMatrix, distCoeffs,
				writeExtrinsics ? rvecs : std::vector<cv::Mat>(),
				writeExtrinsics ? tvecs : std::vector<cv::Mat>(),
				writeExtrinsics ? reprojErrs : std::vector<float>(),
				writePoints ? imagePoints : std::vector<std::vector<cv::Point2f> >(),
				totalAvgErr, matXi);

		return ok;
	}
}
