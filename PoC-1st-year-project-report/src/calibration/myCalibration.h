#pragma once
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/core/affine.hpp"

#include <opencv2/calib3d.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/detail/distortion_model.hpp"


namespace mycv{
	//! type of the robust estimation algorithm
	enum {
		LMEDS = 4, //!< least-median algorithm
		RANSAC = 8, //!< RANSAC algorithm
		RHO = 16 //!< RHO algorithm
	};

	enum {
		SOLVEPNP_ITERATIVE = 0,
		SOLVEPNP_EPNP = 1, //!< EPnP: Efficient Perspective-n-Point Camera Pose Estimation @cite lepetit2009epnp
		SOLVEPNP_P3P = 2, //!< Complete Solution Classification for the Perspective-Three-Point Problem @cite gao2003complete
		SOLVEPNP_DLS = 3, //!< A Direct Least-Squares (DLS) Method for PnP  @cite hesch2011direct
		SOLVEPNP_UPNP = 4  //!< Exhaustive Linearization for Robust Camera Pose and Focal Length Estimation @cite penate2013exhaustive

	};

	enum {
		CALIB_CB_ADAPTIVE_THRESH = 1,
		CALIB_CB_NORMALIZE_IMAGE = 2,
		CALIB_CB_FILTER_QUADS = 4,
		CALIB_CB_FAST_CHECK = 8
	};

	enum {
		CALIB_CB_SYMMETRIC_GRID = 1,
		CALIB_CB_ASYMMETRIC_GRID = 2,
		CALIB_CB_CLUSTERING = 4
	};

	enum {
		CALIB_USE_INTRINSIC_GUESS = 0x00001,
		CALIB_FIX_ASPECT_RATIO = 0x00002,
		CALIB_FIX_PRINCIPAL_POINT = 0x00004,
		CALIB_ZERO_TANGENT_DIST = 0x00008,
		CALIB_FIX_FOCAL_LENGTH = 0x00010,
		CALIB_FIX_K1 = 0x00020,
		CALIB_FIX_K2 = 0x00040,
		CALIB_FIX_K3 = 0x00080,
		CALIB_FIX_K4 = 0x00800,
		CALIB_FIX_K5 = 0x01000,
		CALIB_FIX_K6 = 0x02000,
		CALIB_RATIONAL_MODEL = 0x04000,
		CALIB_THIN_PRISM_MODEL = 0x08000,
		CALIB_FIX_S1_S2_S3_S4 = 0x10000,
		CALIB_TILTED_MODEL = 0x40000,
		CALIB_FIX_TAUX_TAUY = 0x80000,
		CALIB_USE_QR = 0x100000, //!< use QR instead of SVD decomposition for solving. Faster but potentially less precise
								 // only for stereo
								 CALIB_FIX_INTRINSIC = 0x00100,
								 CALIB_SAME_FOCAL_LENGTH = 0x00200,
								 // for stereo rectification
								 CALIB_ZERO_DISPARITY = 0x00400,
								 CALIB_USE_LU = (1 << 17), //!< use LU instead of SVD decomposition for solving. much faster but potentially less precise
	};

	//! the algorithm for finding fundamental matrix
	enum {
		FM_7POINT = 1, //!< 7-point algorithm
		FM_8POINT = 2, //!< 8-point algorithm
		FM_LMEDS = 4, //!< least-median algorithm
		FM_RANSAC = 8  //!< RANSAC algorithm
	};


	void Rodrigues(cv::InputArray src, cv::OutputArray dst, cv::OutputArray jacobian = cv::noArray());

	cv::Mat findHomography(cv::InputArray srcPoints, cv::InputArray dstPoints,
		int method = 0, double ransacReprojThreshold = 3,
		cv::OutputArray mask = cv::noArray(), const int maxIters = 2000,
		const double confidence = 0.995);

	/** @overload */
	cv::Mat findHomography(cv::InputArray srcPoints, cv::InputArray dstPoints,
		cv::OutputArray mask, int method = 0, double ransacReprojThreshold = 3);

	cv::Vec3d RQDecomp3x3(cv::InputArray src, cv::OutputArray mtxR, cv::OutputArray mtxQ,
		cv::OutputArray Qx = cv::noArray(),
		cv::OutputArray Qy = cv::noArray(),
		cv::OutputArray Qz = cv::noArray());
	void decomposeProjectionMatrix(cv::InputArray projMatrix, cv::OutputArray cameraMatrix,
		cv::OutputArray rotMatrix, cv::OutputArray transVect,
		cv::OutputArray rotMatrixX = cv::noArray(),
		cv::OutputArray rotMatrixY = cv::noArray(),
		cv::OutputArray rotMatrixZ = cv::noArray(),
		cv::OutputArray eulerAngles = cv::noArray());

	void matMulDeriv(cv::InputArray A, cv::InputArray B, cv::OutputArray dABdA, cv::OutputArray dABdB);
	void composeRT(cv::InputArray rvec1, cv::InputArray tvec1,
		cv::InputArray rvec2, cv::InputArray tvec2,
		cv::OutputArray rvec3, cv::OutputArray tvec3,
		cv::OutputArray dr3dr1 = cv::noArray(), cv::OutputArray dr3dt1 = cv::noArray(),
		cv::OutputArray dr3dr2 = cv::noArray(), cv::OutputArray dr3dt2 = cv::noArray(),
		cv::OutputArray dt3dr1 = cv::noArray(), cv::OutputArray dt3dt1 = cv::noArray(),
		cv::OutputArray dt3dr2 = cv::noArray(), cv::OutputArray dt3dt2 = cv::noArray());

	void projectPoints(cv::InputArray objectPoints,
		cv::InputArray rvec, cv::InputArray tvec,
		cv::InputArray cameraMatrix, cv::InputArray distCoeffs,
		cv::OutputArray imagePoints,
		cv::OutputArray jacobian = cv::noArray(),
		double aspectRatio = 0);
	bool solvePnP(cv::InputArray objectPoints, cv::InputArray imagePoints,
		cv::InputArray cameraMatrix, cv::InputArray distCoeffs,
		cv::OutputArray rvec, cv::OutputArray tvec,
		bool useExtrinsicGuess = false, int flags = mycv::SOLVEPNP_ITERATIVE);
	bool solvePnPRansac(cv::InputArray objectPoints, cv::InputArray imagePoints,
		cv::InputArray cameraMatrix, cv::InputArray distCoeffs,
		cv::OutputArray rvec, cv::OutputArray tvec,
		bool useExtrinsicGuess = false, int iterationsCount = 100,
		float reprojectionError = 8.0, double confidence = 0.99,
		cv::OutputArray inliers = cv::noArray(), int flags = mycv::SOLVEPNP_ITERATIVE);
	cv::Mat initCameraMatrix2D(cv::InputArrayOfArrays objectPoints,
		cv::InputArrayOfArrays imagePoints,
		cv::Size imageSize, double aspectRatio = 1.0);
	bool findChessboardCorners(cv::InputArray image, cv::Size patternSize, cv::OutputArray corners,
		int flags = mycv::CALIB_CB_ADAPTIVE_THRESH + mycv::CALIB_CB_NORMALIZE_IMAGE);

	//! finds subpixel-accurate positions of the chessboard corners
	bool find4QuadCornerSubpix(cv::InputArray img, cv::InputOutputArray corners, cv::Size region_size);
	void drawChessboardCorners(cv::InputOutputArray image, cv::Size patternSize,
		cv::InputArray corners, bool patternWasFound);
	bool findCirclesGrid(cv::InputArray image, cv::Size patternSize,
		cv::OutputArray centers, int flags = mycv::CALIB_CB_SYMMETRIC_GRID,
		const cv::Ptr<cv::FeatureDetector> &blobDetector = cv::SimpleBlobDetector::create());
	double calibrateCamera(cv::InputArrayOfArrays objectPoints,
		cv::InputArrayOfArrays imagePoints, cv::Size imageSize,
		cv::InputOutputArray cameraMatrix, cv::InputOutputArray distCoeffs,
		cv::OutputArrayOfArrays rvecs, cv::OutputArrayOfArrays tvecs,
		int flags = 0, cv::TermCriteria criteria = cv::TermCriteria(
			cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, DBL_EPSILON));
	void calibrationMatrixValues(cv::InputArray cameraMatrix, cv::Size imageSize,
		double apertureWidth, double apertureHeight,
		double& fovx, double& fovy,
		double& focalLength, cv::Point2d& principalPoint,
		double& aspectRatio);
	double stereoCalibrate(cv::InputArrayOfArrays objectPoints,
		cv::InputArrayOfArrays imagePoints1, cv::InputArrayOfArrays imagePoints2,
		cv::InputOutputArray cameraMatrix1, cv::InputOutputArray distCoeffs1,
		cv::InputOutputArray cameraMatrix2, cv::InputOutputArray distCoeffs2,
		cv::Size imageSize, cv::OutputArray R, cv::OutputArray T, cv::OutputArray E, cv::OutputArray F,
		int flags = mycv::CALIB_FIX_INTRINSIC,
		cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-6));
	void stereoRectify(cv::InputArray cameraMatrix1, cv::InputArray distCoeffs1,
		cv::InputArray cameraMatrix2, cv::InputArray distCoeffs2,
		cv::Size imageSize, cv::InputArray R, cv::InputArray T,
		cv::OutputArray R1, cv::OutputArray R2,
		cv::OutputArray P1, cv::OutputArray P2,
		cv::OutputArray Q, int flags = mycv::CALIB_ZERO_DISPARITY,
		double alpha = -1, cv::Size newImageSize = cv::Size(),
		cv::Rect* validPixROI1 = 0, CV_OUT cv::Rect* validPixROI2 = 0);
	bool stereoRectifyUncalibrated(cv::InputArray points1, cv::InputArray points2,
		cv::InputArray F, cv::Size imgSize,
		cv::OutputArray H1, cv::OutputArray H2,
		double threshold = 5);

	//! computes the rectification transformations for 3-head camera, where all the heads are on the same line.
	float rectify3Collinear(cv::InputArray cameraMatrix1, cv::InputArray distCoeffs1,
		cv::InputArray cameraMatrix2, cv::InputArray distCoeffs2,
		cv::InputArray cameraMatrix3, cv::InputArray distCoeffs3,
		cv::InputArrayOfArrays imgpt1, cv::InputArrayOfArrays imgpt3,
		cv::Size imageSize, cv::InputArray R12, cv::InputArray T12,
		cv::InputArray R13, cv::InputArray T13,
		cv::OutputArray R1, cv::OutputArray R2, cv::OutputArray R3,
		cv::OutputArray P1, cv::OutputArray P2, cv::OutputArray P3,
		cv::OutputArray Q, double alpha, cv::Size newImgSize,
		CV_OUT cv::Rect* roi1, CV_OUT cv::Rect* roi2, int flags);

	cv::Mat getOptimalNewCameraMatrix(cv::InputArray cameraMatrix, cv::InputArray distCoeffs,
		cv::Size imageSize, double alpha, cv::Size newImgSize = cv::Size(),
		cv::Rect* validPixROI = 0,
		bool centerPrincipalPoint = false);
	void convertPointsToHomogeneous(cv::InputArray src, cv::OutputArray dst);
	void convertPointsFromHomogeneous(cv::InputArray src, cv::OutputArray dst);
	void convertPointsHomogeneous(cv::InputArray src, cv::OutputArray dst);
	cv::Mat findFundamentalMat(cv::InputArray points1, cv::InputArray points2,
		int method = mycv::FM_RANSAC,
		double param1 = 3., double param2 = 0.99,
		cv::OutputArray mask = cv::noArray());

	/** @overload */
	cv::Mat findFundamentalMat(cv::InputArray points1, cv::InputArray points2,
		cv::OutputArray mask, int method = mycv::FM_RANSAC,
		double param1 = 3., double param2 = 0.99);
	cv::Mat findEssentialMat(cv::InputArray points1, cv::InputArray points2,
		cv::InputArray cameraMatrix, int method = mycv::RANSAC,
		double prob = 0.999, double threshold = 1.0,
		cv::OutputArray mask = cv::noArray());
	cv::Mat findEssentialMat(cv::InputArray points1, cv::InputArray points2,
		double focal = 1.0, cv::Point2d pp = cv::Point2d(0, 0),
		int method = mycv::RANSAC, double prob = 0.999,
		double threshold = 1.0, cv::OutputArray mask = cv::noArray());
	void decomposeEssentialMat(cv::InputArray E, cv::OutputArray R1, cv::OutputArray R2, cv::OutputArray t);
	int recoverPose(cv::InputArray E, cv::InputArray points1, cv::InputArray points2,
		cv::InputArray cameraMatrix, cv::OutputArray R, cv::OutputArray t,
		cv::InputOutputArray mask = cv::noArray());
	int recoverPose(cv::InputArray E, cv::InputArray points1, cv::InputArray points2,
		cv::OutputArray R, cv::OutputArray t,
		double focal = 1.0, cv::Point2d pp = cv::Point2d(0, 0),
		cv::InputOutputArray mask = cv::noArray());
	void computeCorrespondEpilines(cv::InputArray points, int whichImage,
		cv::InputArray F, cv::OutputArray lines);
	void triangulatePoints(cv::InputArray projMatr1, cv::InputArray projMatr2,
		cv::InputArray projPoints1, cv::InputArray projPoints2,
		cv::OutputArray points4D);
	void correctMatches(cv::InputArray F, cv::InputArray points1, cv::InputArray points2,
		cv::OutputArray newPoints1, cv::OutputArray newPoints2);
	void filterSpeckles(cv::InputOutputArray img, double newVal,
		int maxSpeckleSize, double maxDiff,
		cv::InputOutputArray buf = cv::noArray());

	//! computes valid disparity ROI from the valid ROIs of the rectified images (that are returned by cv::stereoRectify())
	cv::Rect getValidDisparityROI(cv::Rect roi1, cv::Rect roi2,
		int minDisparity, int numberOfDisparities,
		int SADWindowSize);

	//! validates disparity using the left-right check. The matrix "cost" should be computed by the stereo correspondence algorithm
	void validateDisparity(cv::InputOutputArray disparity, cv::InputArray cost,
		int minDisparity, int numberOfDisparities,
		int disp12MaxDisp = 1);
	void reprojectImageTo3D(cv::InputArray disparity,
		cv::OutputArray _3dImage, cv::InputArray Q,
		bool handleMissingValues = false,
		int ddepth = -1);
	double sampsonDistance(cv::InputArray pt1, cv::InputArray pt2, cv::InputArray F);
	int estimateAffine3D(cv::InputArray src, cv::InputArray dst,
		cv::OutputArray out, cv::OutputArray inliers,
		double ransacThreshold = 3, double confidence = 0.99);
	cv::Mat estimateAffine2D(cv::InputArray from, cv::InputArray to, cv::OutputArray inliers = cv::noArray(),
		int method = mycv::RANSAC, double ransacReprojThreshold = 3,
		size_t maxIters = 2000, double confidence = 0.99,
		size_t refineIters = 10);
	cv::Mat estimateAffinePartial2D(cv::InputArray from, cv::InputArray to, cv::OutputArray inliers = cv::noArray(),
		int method = mycv::RANSAC, double ransacReprojThreshold = 3,
		size_t maxIters = 2000, double confidence = 0.99,
		size_t refineIters = 10);
	int decomposeHomographyMat(cv::InputArray H,
		cv::InputArray K,
		cv::OutputArrayOfArrays rotations,
		cv::OutputArrayOfArrays translations,
		cv::OutputArrayOfArrays normals);
}