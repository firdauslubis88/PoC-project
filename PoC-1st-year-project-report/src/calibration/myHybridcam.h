#pragma once
#include "myCalibration.h"
#include "myOmnidir.h"

namespace mycv {
	namespace hybridcam {
		//! @addtogroup ccalib
		//! @{

		enum {
			CALIB_USE_GUESS = 1,
			CALIB_FIX_SKEW = 2,
			CALIB_FIX_K1 = 4,
			CALIB_FIX_K2 = 8,
			CALIB_FIX_P1 = 16,
			CALIB_FIX_P2 = 32,
			CALIB_FIX_XI = 64,
			CALIB_FIX_GAMMA = 128,
			CALIB_FIX_CENTER = 256
		};

		enum {
			RECTIFY_PERSPECTIVE = 1,
			RECTIFY_CYLINDRICAL = 2,
			RECTIFY_LONGLATI = 3,
			RECTIFY_STEREOGRAPHIC = 4
		};

		enum {
			XYZRGB = 1,
			XYZ = 2
		};
		/*
		void projectPoints(cv::InputArray objectPoints, cv::OutputArray imagePoints, cv::InputArray rvec, cv::InputArray tvec,
			cv::InputArray K, double xi, cv::InputArray D, cv::OutputArray jacobian = cv::noArray());
		*/
		/*
		void undistortPoints(cv::InputArray distorted, cv::OutputArray undistorted, cv::InputArray K, cv::InputArray D, cv::InputArray xi, cv::InputArray R);
		*/
		/*
		void initUndistortRectifyMap(cv::InputArray K, cv::InputArray D, cv::InputArray xi, cv::InputArray R, cv::InputArray P, const cv::Size& size,
			int mltype, cv::OutputArray map1, cv::OutputArray map2, int flags);
		*/
		/*
		void undistortImage(cv::InputArray distorted, cv::OutputArray undistorted, cv::InputArray K, cv::InputArray D, cv::InputArray xi, int flags,
			cv::InputArray Knew = cv::noArray(), const cv::Size& new_size = cv::Size(), cv::InputArray R = cv::Mat::eye(3, 3, CV_64F));
		*/
		/*
		double calibrate(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints, cv::Size size,
			cv::InputOutputArray K, cv::InputOutputArray xi, cv::InputOutputArray D, cv::OutputArrayOfArrays rvecs, cv::OutputArrayOfArrays tvecs,
			int flags, cv::TermCriteria criteria, cv::OutputArray idx = cv::noArray());
		*/
		double stereoCalibrate(cv::InputOutputArrayOfArrays objectPoints, cv::InputOutputArrayOfArrays imagePoints1, cv::InputOutputArrayOfArrays imagePoints2,
			const cv::Size& imageSize1, const cv::Size& imageSize2, cv::InputOutputArray K1, cv::InputOutputArray xi1, cv::InputOutputArray D1, cv::InputOutputArray K2, cv::InputOutputArray xi2,
			cv::InputOutputArray D2, cv::OutputArray rvec, cv::OutputArray tvec, cv::OutputArrayOfArrays rvecsL, cv::OutputArrayOfArrays tvecsL, int flags, cv::TermCriteria criteria, cv::OutputArray idx = cv::noArray());
		void stereoRectify(cv::InputArray R, cv::InputArray T, cv::OutputArray R1, cv::OutputArray R2);
		void stereoReconstruct(cv::InputArray image1, cv::InputArray image2, cv::InputArray K1, cv::InputArray D1, cv::InputArray xi1,
			cv::InputArray K2, cv::InputArray D2, cv::InputArray xi2, cv::InputArray R, cv::InputArray T, int flag, int numDisparities, int SADWindowSize,
			cv::OutputArray disparity, cv::OutputArray image1Rec, cv::OutputArray image2Rec, const cv::Size& newSize = cv::Size(), cv::InputArray Knew = cv::noArray(),
			cv::OutputArray pointCloud = cv::noArray(), int pointType = XYZRGB);

		namespace internal
		{
			void initializeCalibration(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints, cv::Size size, cv::OutputArrayOfArrays omAll,
				cv::OutputArrayOfArrays tAll, cv::OutputArray K, double& xi, cv::OutputArray idx = cv::noArray());

			void initializeStereoCalibration(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints1, cv::InputArrayOfArrays imagePoints2,
				const cv::Size& size1, const cv::Size& size2, cv::OutputArray om, cv::OutputArray T, cv::OutputArrayOfArrays omL, cv::OutputArrayOfArrays tL, cv::OutputArray K1, cv::OutputArray D1, cv::OutputArray K2, cv::OutputArray D2,
				double &xi1, double &xi2, int flags, cv::OutputArray idx);

			void computeJacobian(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints, cv::InputArray parameters, cv::Mat& JTJ_inv, cv::Mat& JTE, int flags,
				double epsilon);

			void computeJacobianStereo(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints1, cv::InputArrayOfArrays imagePoints2,
				cv::InputArray parameters, cv::Mat& JTJ_inv, cv::Mat& JTE, int flags, double epsilon);

			void encodeParameters(cv::InputArray K, cv::InputArrayOfArrays omAll, cv::InputArrayOfArrays tAll, cv::InputArray distoaration, double xi, cv::OutputArray parameters);

			void encodeParametersStereo(cv::InputArray K1, cv::InputArray K2, cv::InputArray om, cv::InputArray T, cv::InputArrayOfArrays omL, cv::InputArrayOfArrays tL,
				cv::InputArray D1, cv::InputArray D2, double xi1, double xi2, cv::OutputArray parameters);

			void decodeParameters(cv::InputArray paramsters, cv::OutputArray K, cv::OutputArrayOfArrays omAll, cv::OutputArrayOfArrays tAll, cv::OutputArray distoration, double& xi);

			void decodeParametersStereo(cv::InputArray parameters, cv::OutputArray K1, cv::OutputArray K2, cv::OutputArray om, cv::OutputArray T, cv::OutputArrayOfArrays omL,
				cv::OutputArrayOfArrays tL, cv::OutputArray D1, cv::OutputArray D2, double& xi1, double& xi2);

			void estimateUncertainties(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints, cv::InputArray parameters, cv::Mat& errors, cv::Vec2d& std_error, double& rms, int flags);

			void estimateUncertaintiesStereo(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints1, cv::InputArrayOfArrays imagePoints2, cv::InputArray parameters, cv::Mat& errors,
				cv::Vec2d& std_error, double& rms, int flags);

			double computeMeanReproErr(cv::InputArrayOfArrays imagePoints, cv::InputArrayOfArrays proImagePoints);

			double computeMeanReproErr(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints, cv::InputArray K, cv::InputArray D, double xi, cv::InputArrayOfArrays omAll,
				cv::InputArrayOfArrays tAll);

			double computeMeanReproErrStereo(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints1, cv::InputArrayOfArrays imagePoints2, cv::InputArray K1, cv::InputArray K2,
				cv::InputArray D1, cv::InputArray D2, double xi1, double xi2, cv::InputArray om, cv::InputArray T, cv::InputArrayOfArrays omL, cv::InputArrayOfArrays TL);

			void checkFixed(cv::Mat &G, int flags, int n);

			void subMatrix(const cv::Mat& src, cv::Mat& dst, const std::vector<int>& cols, const std::vector<int>& rows);

			void flags2idx(int flags, std::vector<int>& idx, int n);

			void flags2idxStereo(int flags, std::vector<int>& idx, int n);

			void fillFixed(cv::Mat&G, int flags, int n);

			void fillFixedStereo(cv::Mat& G, int flags, int n);

			double findMedian(const cv::Mat& row);

			cv::Vec3d findMedian3(cv::InputArray mat);

			void getInterset(cv::InputArray idx1, cv::InputArray idx2, cv::OutputArray inter1, cv::OutputArray inter2, cv::OutputArray inter_ori);

			void compose_motion(cv::InputArray _om1, cv::InputArray _T1, cv::InputArray _om2, cv::InputArray _T2, cv::Mat& om3, cv::Mat& T3, cv::Mat& dom3dom1,
				cv::Mat& dom3dT1, cv::Mat& dom3dom2, cv::Mat& dom3dT2, cv::Mat& dT3dom1, cv::Mat& dT3dT1, cv::Mat& dT3dom2, cv::Mat& dT3dT2);

			//void JRodriguesMatlab(const Mat& src, Mat& dst);

			//void dAB(InputArray A, InputArray B, OutputArray dABdA, OutputArray dABdB);
		} // internal
	}
}
