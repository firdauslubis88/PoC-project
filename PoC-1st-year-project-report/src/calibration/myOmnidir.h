#pragma once
#include <opencv2/ccalib/omnidir.hpp>

namespace mycv {
	namespace omnidir {
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
		/**
		* This module was accepted as a GSoC 2015 project for OpenCV, authored by
		* Baisheng Lai, mentored by Bo Li.
		*/

		/** @brief Projects points for omnidirectional camera using CMei's model

		@param objectPoints Object points in world coordinate, vector of vector of Vec3f or Mat of
		1xN/Nx1 3-channel of type CV_32F and N is the number of points. 64F is also acceptable.
		@param imagePoints Output array of image points, vector of vector of Vec2f or
		1xN/Nx1 2-channel of type CV_32F. 64F is also acceptable.
		@param rvec vector of rotation between world coordinate and camera coordinate, i.e., om
		@param tvec vector of translation between pattern coordinate and camera coordinate
		@param K Camera matrix \f$K = \vecthreethree{f_x}{s}{c_x}{0}{f_y}{c_y}{0}{0}{_1}\f$.
		@param D Input vector of distortion coefficients \f$(k_1, k_2, p_1, p_2)\f$.
		@param xi The parameter xi for CMei's model
		@param jacobian Optional output 2Nx16 of type CV_64F jacobian matrix, contains the derivatives of
		image pixel points wrt parameters including \f$om, T, f_x, f_y, s, c_x, c_y, xi, k_1, k_2, p_1, p_2\f$.
		This matrix will be used in calibration by optimization.

		The function projects object 3D points of world coordinate to image pixels, parameter by intrinsic
		and extrinsic parameters. Also, it optionally compute a by-product: the jacobian matrix containing
		contains the derivatives of image pixel points wrt intrinsic and extrinsic parameters.
		*/
		void projectPoints(cv::InputArray objectPoints, cv::OutputArray imagePoints, cv::InputArray rvec, cv::InputArray tvec,
			cv::InputArray K, double xi, cv::InputArray D, cv::OutputArray jacobian = cv::noArray());

		/** @brief Undistort 2D image points for omnidirectional camera using CMei's model

		@param distorted Array of distorted image points, vector of Vec2f
		or 1xN/Nx1 2-channel Mat of type CV_32F, 64F depth is also acceptable
		@param K Camera matrix \f$K = \vecthreethree{f_x}{s}{c_x}{0}{f_y}{c_y}{0}{0}{_1}\f$.
		@param D Distortion coefficients \f$(k_1, k_2, p_1, p_2)\f$.
		@param xi The parameter xi for CMei's model
		@param R Rotation trainsform between the original and object space : 3x3 1-channel, or vector: 3x1/1x3
		1-channel or 1x1 3-channel
		@param undistorted array of normalized object points, vector of Vec2f/Vec2d or 1xN/Nx1 2-channel Mat with the same
		depth of distorted points.
		*/
		void undistortPoints(cv::InputArray distorted, cv::OutputArray undistorted, cv::InputArray K, cv::InputArray D, cv::InputArray xi, cv::InputArray R);

		/** @brief Computes undistortion and rectification maps for omnidirectional camera image transform by a rotation R.
		It output two maps that are used for cv::remap(). If D is empty then zero distortion is used,
		if R or P is empty then identity matrices are used.

		@param K Camera matrix \f$K = \vecthreethree{f_x}{s}{c_x}{0}{f_y}{c_y}{0}{0}{_1}\f$, with depth CV_32F or CV_64F
		@param D Input vector of distortion coefficients \f$(k_1, k_2, p_1, p_2)\f$, with depth CV_32F or CV_64F
		@param xi The parameter xi for CMei's model
		@param R Rotation transform between the original and object space : 3x3 1-channel, or vector: 3x1/1x3, with depth CV_32F or CV_64F
		@param P New camera matrix (3x3) or new projection matrix (3x4)
		@param size Undistorted image size.
		@param mltype Type of the first output map that can be CV_32FC1 or CV_16SC2 . See convertMaps()
		for details.
		@param map1 The first output map.
		@param map2 The second output map.
		@param flags Flags indicates the rectification type,  RECTIFY_PERSPECTIVE, RECTIFY_CYLINDRICAL, RECTIFY_LONGLATI and RECTIFY_STEREOGRAPHIC
		are supported.
		*/
		void initUndistortRectifyMap(cv::InputArray K, cv::InputArray D, cv::InputArray xi, cv::InputArray R, cv::InputArray P, const cv::Size& size,
			int mltype, cv::OutputArray map1, cv::OutputArray map2, int flags);

		/** @brief Undistort omnidirectional images to perspective images

		@param distorted The input omnidirectional image.
		@param undistorted The output undistorted image.
		@param K Camera matrix \f$K = \vecthreethree{f_x}{s}{c_x}{0}{f_y}{c_y}{0}{0}{_1}\f$.
		@param D Input vector of distortion coefficients \f$(k_1, k_2, p_1, p_2)\f$.
		@param xi The parameter xi for CMei's model.
		@param flags Flags indicates the rectification type,  RECTIFY_PERSPECTIVE, RECTIFY_CYLINDRICAL, RECTIFY_LONGLATI and RECTIFY_STEREOGRAPHIC
		@param Knew Camera matrix of the distorted image. If it is not assigned, it is just K.
		@param new_size The new image size. By default, it is the size of distorted.
		@param R Rotation matrix between the input and output images. By default, it is identity matrix.
		*/
		void undistortImage(cv::InputArray distorted, cv::OutputArray undistorted, cv::InputArray K, cv::InputArray D, cv::InputArray xi, int flags,
			cv::InputArray Knew = cv::noArray(), const cv::Size& new_size = cv::Size(), cv::InputArray R = cv::Mat::eye(3, 3, CV_64F));

		/** @brief Perform omnidirectional camera calibration, the default depth of outputs is CV_64F.

		@param objectPoints Vector of vector of Vec3f object points in world (pattern) coordinate.
		It also can be vector of Mat with size 1xN/Nx1 and type CV_32FC3. Data with depth of 64_F is also acceptable.
		@param imagePoints Vector of vector of Vec2f corresponding image points of objectPoints. It must be the same
		size and the same type with objectPoints.
		@param size Image size of calibration images.
		@param K Output calibrated camera matrix.
		@param xi Output parameter xi for CMei's model
		@param D Output distortion parameters \f$(k_1, k_2, p_1, p_2)\f$
		@param rvecs Output rotations for each calibration images
		@param tvecs Output translation for each calibration images
		@param flags The flags that control calibrate
		@param criteria Termination criteria for optimization
		@param idx Indices of images that pass initialization, which are really used in calibration. So the size of rvecs is the
		same as idx.total().
		*/
		double calibrate(cv::InputArrayOfArrays objectPoints, cv::InputArrayOfArrays imagePoints, cv::Size size,
			cv::InputOutputArray K, cv::InputOutputArray xi, cv::InputOutputArray D, cv::OutputArrayOfArrays rvecs, cv::OutputArrayOfArrays tvecs,
			int flags, cv::TermCriteria criteria, cv::OutputArray idx = cv::noArray());

		/** @brief Stereo calibration for omnidirectional camera model. It computes the intrinsic parameters for two
		cameras and the extrinsic parameters between two cameras. The default depth of outputs is CV_64F.

		@param objectPoints Object points in world (pattern) coordinate. Its type is vector<vector<Vec3f> >.
		It also can be vector of Mat with size 1xN/Nx1 and type CV_32FC3. Data with depth of 64_F is also acceptable.
		@param imagePoints1 The corresponding image points of the first camera, with type vector<vector<Vec2f> >.
		It must be the same size and the same type as objectPoints.
		@param imagePoints2 The corresponding image points of the second camera, with type vector<vector<Vec2f> >.
		It must be the same size and the same type as objectPoints.
		@param imageSize1 Image size of calibration images of the first camera.
		@param imageSize2 Image size of calibration images of the second camera.
		@param K1 Output camera matrix for the first camera.
		@param xi1 Output parameter xi of Mei's model for the first camera
		@param D1 Output distortion parameters \f$(k_1, k_2, p_1, p_2)\f$ for the first camera
		@param K2 Output camera matrix for the first camera.
		@param xi2 Output parameter xi of CMei's model for the second camera
		@param D2 Output distortion parameters \f$(k_1, k_2, p_1, p_2)\f$ for the second camera
		@param rvec Output rotation between the first and second camera
		@param tvec Output translation between the first and second camera
		@param rvecsL Output rotation for each image of the first camera
		@param tvecsL Output translation for each image of the first camera
		@param flags The flags that control stereoCalibrate
		@param criteria Termination criteria for optimization
		@param idx Indices of image pairs that pass initialization, which are really used in calibration. So the size of rvecs is the
		same as idx.total().
		@
		*/
		double stereoCalibrate(cv::InputOutputArrayOfArrays objectPoints, cv::InputOutputArrayOfArrays imagePoints1, cv::InputOutputArrayOfArrays imagePoints2,
			const cv::Size& imageSize1, const cv::Size& imageSize2, cv::InputOutputArray K1, cv::InputOutputArray xi1, cv::InputOutputArray D1, cv::InputOutputArray K2, cv::InputOutputArray xi2,
			cv::InputOutputArray D2, cv::OutputArray rvec, cv::OutputArray tvec, cv::OutputArrayOfArrays rvecsL, cv::OutputArrayOfArrays tvecsL, int flags, cv::TermCriteria criteria, cv::OutputArray idx = cv::noArray());

		/** @brief Stereo rectification for omnidirectional camera model. It computes the rectification rotations for two cameras

		@param R Rotation between the first and second camera
		@param T Translation between the first and second camera
		@param R1 Output 3x3 rotation matrix for the first camera
		@param R2 Output 3x3 rotation matrix for the second camera
		*/
		void stereoRectify(cv::InputArray R, cv::InputArray T, cv::OutputArray R1, cv::OutputArray R2);

		/** @brief Stereo 3D reconstruction from a pair of images

		@param image1 The first input image
		@param image2 The second input image
		@param K1 Input camera matrix of the first camera
		@param D1 Input distortion parameters \f$(k_1, k_2, p_1, p_2)\f$ for the first camera
		@param xi1 Input parameter xi for the first camera for CMei's model
		@param K2 Input camera matrix of the second camera
		@param D2 Input distortion parameters \f$(k_1, k_2, p_1, p_2)\f$ for the second camera
		@param xi2 Input parameter xi for the second camera for CMei's model
		@param R Rotation between the first and second camera
		@param T Translation between the first and second camera
		@param flag Flag of rectification type, RECTIFY_PERSPECTIVE or RECTIFY_LONGLATI
		@param numDisparities The parameter 'numDisparities' in StereoSGBM, see StereoSGBM for details.
		@param SADWindowSize The parameter 'SADWindowSize' in StereoSGBM, see StereoSGBM for details.
		@param disparity Disparity map generated by stereo matching
		@param image1Rec Rectified image of the first image
		@param image2Rec rectified image of the second image
		@param newSize Image size of rectified image, see omnidir::undistortImage
		@param Knew New camera matrix of rectified image, see omnidir::undistortImage
		@param pointCloud Point cloud of 3D reconstruction, with type CV_64FC3
		@param pointType Point cloud type, it can be XYZRGB or XYZ
		*/
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
