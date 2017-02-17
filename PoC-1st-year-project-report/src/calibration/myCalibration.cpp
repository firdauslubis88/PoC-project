#include "myCalibration.h"

using namespace cv;

static const char* cvDistCoeffErr = "Distortion coefficients must be 1x4, 4x1, 1x5, 5x1, 1x8, 8x1, 1x12, 12x1, 1x14 or 14x1 floating-point vector";

static int dbCmp(const void* _a, const void* _b)
{
	double a = *(const double*)_a;
	double b = *(const double*)_b;

	return (a > b) - (a < b);
}

static void
icvGetRectangles(const CvMat* cameraMatrix, const CvMat* distCoeffs,
	const CvMat* R, const CvMat* newCameraMatrix, CvSize imgSize,
	cv::Rect_<float>& inner, cv::Rect_<float>& outer)
{
	const int N = 9;
	int x, y, k;
	cv::Ptr<CvMat> _pts(cvCreateMat(1, N*N, CV_32FC2));
	CvPoint2D32f* pts = (CvPoint2D32f*)(_pts->data.ptr);

	for (y = k = 0; y < N; y++)
		for (x = 0; x < N; x++)
			pts[k++] = cvPoint2D32f((float)x*imgSize.width / (N - 1),
			(float)y*imgSize.height / (N - 1));

	cvUndistortPoints(_pts, _pts, cameraMatrix, distCoeffs, R, newCameraMatrix);

	float iX0 = -FLT_MAX, iX1 = FLT_MAX, iY0 = -FLT_MAX, iY1 = FLT_MAX;
	float oX0 = FLT_MAX, oX1 = -FLT_MAX, oY0 = FLT_MAX, oY1 = -FLT_MAX;
	// find the inscribed rectangle.
	// the code will likely not work with extreme rotation matrices (R) (>45%)
	for (y = k = 0; y < N; y++)
		for (x = 0; x < N; x++)
		{
			CvPoint2D32f p = pts[k++];
			oX0 = MIN(oX0, p.x);
			oX1 = MAX(oX1, p.x);
			oY0 = MIN(oY0, p.y);
			oY1 = MAX(oY1, p.y);

			if (x == 0)
				iX0 = MAX(iX0, p.x);
			if (x == N - 1)
				iX1 = MIN(iX1, p.x);
			if (y == 0)
				iY0 = MAX(iY0, p.y);
			if (y == N - 1)
				iY1 = MIN(iY1, p.y);
		}
	inner = cv::Rect_<float>(iX0, iY0, iX1 - iX0, iY1 - iY0);
	outer = cv::Rect_<float>(oX0, oY0, oX1 - oX0, oY1 - oY0);
}

static void collectCalibrationData(InputArrayOfArrays objectPoints,
	InputArrayOfArrays imagePoints1,
	InputArrayOfArrays imagePoints2,
	Mat& objPtMat, Mat& imgPtMat1, Mat* imgPtMat2,
	Mat& npoints)
{
	int nimages = (int)objectPoints.total();
	int i, j = 0, ni = 0, total = 0;
	CV_Assert(nimages > 0 && nimages == (int)imagePoints1.total() &&
		(!imgPtMat2 || nimages == (int)imagePoints2.total()));

	for (i = 0; i < nimages; i++)
	{
		ni = objectPoints.getMat(i).checkVector(3, CV_32F);
		if (ni <= 0)
			CV_Error(CV_StsUnsupportedFormat, "objectPoints should contain vector of vectors of points of type Point3f");
		int ni1 = imagePoints1.getMat(i).checkVector(2, CV_32F);
		if (ni1 <= 0)
			CV_Error(CV_StsUnsupportedFormat, "imagePoints1 should contain vector of vectors of points of type Point2f");
		CV_Assert(ni == ni1);

		total += ni;
	}

	npoints.create(1, (int)nimages, CV_32S);
	objPtMat.create(1, (int)total, CV_32FC3);
	imgPtMat1.create(1, (int)total, CV_32FC2);
	Point2f* imgPtData2 = 0;

	if (imgPtMat2)
	{
		imgPtMat2->create(1, (int)total, CV_32FC2);
		imgPtData2 = imgPtMat2->ptr<Point2f>();
	}

	Point3f* objPtData = objPtMat.ptr<Point3f>();
	Point2f* imgPtData1 = imgPtMat1.ptr<Point2f>();

	for (i = 0; i < nimages; i++, j += ni)
	{
		Mat objpt = objectPoints.getMat(i);
		Mat imgpt1 = imagePoints1.getMat(i);
		ni = objpt.checkVector(3, CV_32F);
		npoints.at<int>(i) = ni;
		memcpy(objPtData + j, objpt.ptr(), ni * sizeof(objPtData[0]));
		memcpy(imgPtData1 + j, imgpt1.ptr(), ni * sizeof(imgPtData1[0]));

		if (imgPtData2)
		{
			Mat imgpt2 = imagePoints2.getMat(i);
			int ni2 = imgpt2.checkVector(2, CV_32F);
			CV_Assert(ni == ni2);
			memcpy(imgPtData2 + j, imgpt2.ptr(), ni * sizeof(imgPtData2[0]));
		}
	}
}

static Mat prepareCameraMatrix(Mat& cameraMatrix0, int rtype)
{
	Mat cameraMatrix = Mat::eye(3, 3, rtype);
	if (cameraMatrix0.size() == cameraMatrix.size())
		cameraMatrix0.convertTo(cameraMatrix, rtype);
	return cameraMatrix;
}

static Mat prepareDistCoeffs(Mat& distCoeffs0, int rtype, int outputSize = 14)
{
	CV_Assert((int)distCoeffs0.total() <= outputSize);
	Mat distCoeffs = Mat::zeros(distCoeffs0.cols == 1 ? Size(1, outputSize) : Size(outputSize, 1), rtype);
	if (distCoeffs0.size() == Size(1, 4) ||
		distCoeffs0.size() == Size(1, 5) ||
		distCoeffs0.size() == Size(1, 8) ||
		distCoeffs0.size() == Size(1, 12) ||
		distCoeffs0.size() == Size(1, 14) ||
		distCoeffs0.size() == Size(4, 1) ||
		distCoeffs0.size() == Size(5, 1) ||
		distCoeffs0.size() == Size(8, 1) ||
		distCoeffs0.size() == Size(12, 1) ||
		distCoeffs0.size() == Size(14, 1))
	{
		Mat dstCoeffs(distCoeffs, Rect(0, 0, distCoeffs0.cols, distCoeffs0.rows));
		distCoeffs0.convertTo(dstCoeffs, rtype);
	}
	return distCoeffs;
}

CV_IMPL void mycvProjectPoints2(const CvMat* objectPoints,
	const CvMat* r_vec,
	const CvMat* t_vec,
	const CvMat* A,
	const CvMat* distCoeffs,
	CvMat* imagePoints, CvMat* dpdr,
	CvMat* dpdt, CvMat* dpdf,
	CvMat* dpdc, CvMat* dpdk,
	double aspectRatio)
{
	Ptr<CvMat> matM, _m;
	Ptr<CvMat> _dpdr, _dpdt, _dpdc, _dpdf, _dpdk;

	int i, j, count;
	int calc_derivatives;
	const CvPoint3D64f* M;
	CvPoint2D64f* m;
	double r[3], R[9], dRdr[27], t[3], a[9], k[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, fx, fy, cx, cy;
	Matx33d matTilt = Matx33d::eye();
	Matx33d dMatTiltdTauX(0, 0, 0, 0, 0, 0, 0, -1, 0);
	Matx33d dMatTiltdTauY(0, 0, 0, 0, 0, 0, 1, 0, 0);
	CvMat _r, _t, _a = cvMat(3, 3, CV_64F, a), _k;
	CvMat matR = cvMat(3, 3, CV_64F, R), _dRdr = cvMat(3, 9, CV_64F, dRdr);
	double *dpdr_p = 0, *dpdt_p = 0, *dpdk_p = 0, *dpdf_p = 0, *dpdc_p = 0;
	int dpdr_step = 0, dpdt_step = 0, dpdk_step = 0, dpdf_step = 0, dpdc_step = 0;
	bool fixedAspectRatio = aspectRatio > FLT_EPSILON;

	if (!CV_IS_MAT(objectPoints) || !CV_IS_MAT(r_vec) ||
		!CV_IS_MAT(t_vec) || !CV_IS_MAT(A) ||
		/*!CV_IS_MAT(distCoeffs) ||*/ !CV_IS_MAT(imagePoints))
		CV_Error(CV_StsBadArg, "One of required arguments is not a valid matrix");

	int total = objectPoints->rows * objectPoints->cols * CV_MAT_CN(objectPoints->type);
	if (total % 3 != 0)
	{
		//we have stopped support of homogeneous coordinates because it cause ambiguity in interpretation of the input data
		CV_Error(CV_StsBadArg, "Homogeneous coordinates are not supported");
	}
	count = total / 3;

	if (CV_IS_CONT_MAT(objectPoints->type) &&
		(CV_MAT_DEPTH(objectPoints->type) == CV_32F || CV_MAT_DEPTH(objectPoints->type) == CV_64F) &&
		((objectPoints->rows == 1 && CV_MAT_CN(objectPoints->type) == 3) ||
		(objectPoints->rows == count && CV_MAT_CN(objectPoints->type)*objectPoints->cols == 3) ||
			(objectPoints->rows == 3 && CV_MAT_CN(objectPoints->type) == 1 && objectPoints->cols == count)))
	{
		matM.reset(cvCreateMat(objectPoints->rows, objectPoints->cols, CV_MAKETYPE(CV_64F, CV_MAT_CN(objectPoints->type))));
		cvConvert(objectPoints, matM);
	}
	else
	{
		//        matM = cvCreateMat( 1, count, CV_64FC3 );
		//        cvConvertPointsHomogeneous( objectPoints, matM );
		CV_Error(CV_StsBadArg, "Homogeneous coordinates are not supported");
	}

	if (CV_IS_CONT_MAT(imagePoints->type) &&
		(CV_MAT_DEPTH(imagePoints->type) == CV_32F || CV_MAT_DEPTH(imagePoints->type) == CV_64F) &&
		((imagePoints->rows == 1 && CV_MAT_CN(imagePoints->type) == 2) ||
		(imagePoints->rows == count && CV_MAT_CN(imagePoints->type)*imagePoints->cols == 2) ||
			(imagePoints->rows == 2 && CV_MAT_CN(imagePoints->type) == 1 && imagePoints->cols == count)))
	{
		_m.reset(cvCreateMat(imagePoints->rows, imagePoints->cols, CV_MAKETYPE(CV_64F, CV_MAT_CN(imagePoints->type))));
		cvConvert(imagePoints, _m);
	}
	else
	{
		//        _m = cvCreateMat( 1, count, CV_64FC2 );
		CV_Error(CV_StsBadArg, "Homogeneous coordinates are not supported");
	}

	M = (CvPoint3D64f*)matM->data.db;
	m = (CvPoint2D64f*)_m->data.db;

	if ((CV_MAT_DEPTH(r_vec->type) != CV_64F && CV_MAT_DEPTH(r_vec->type) != CV_32F) ||
		(((r_vec->rows != 1 && r_vec->cols != 1) ||
			r_vec->rows*r_vec->cols*CV_MAT_CN(r_vec->type) != 3) &&
			((r_vec->rows != 3 && r_vec->cols != 3) || CV_MAT_CN(r_vec->type) != 1)))
		CV_Error(CV_StsBadArg, "Rotation must be represented by 1x3 or 3x1 "
			"floating-point rotation vector, or 3x3 rotation matrix");

	if (r_vec->rows == 3 && r_vec->cols == 3)
	{
		_r = cvMat(3, 1, CV_64FC1, r);
		cvRodrigues2(r_vec, &_r);
		cvRodrigues2(&_r, &matR, &_dRdr);
		cvCopy(r_vec, &matR);
	}
	else
	{
		_r = cvMat(r_vec->rows, r_vec->cols, CV_MAKETYPE(CV_64F, CV_MAT_CN(r_vec->type)), r);
		cvConvert(r_vec, &_r);
		cvRodrigues2(&_r, &matR, &_dRdr);
	}

	if ((CV_MAT_DEPTH(t_vec->type) != CV_64F && CV_MAT_DEPTH(t_vec->type) != CV_32F) ||
		(t_vec->rows != 1 && t_vec->cols != 1) ||
		t_vec->rows*t_vec->cols*CV_MAT_CN(t_vec->type) != 3)
		CV_Error(CV_StsBadArg,
			"Translation vector must be 1x3 or 3x1 floating-point vector");

	_t = cvMat(t_vec->rows, t_vec->cols, CV_MAKETYPE(CV_64F, CV_MAT_CN(t_vec->type)), t);
	cvConvert(t_vec, &_t);

	if ((CV_MAT_TYPE(A->type) != CV_64FC1 && CV_MAT_TYPE(A->type) != CV_32FC1) ||
		A->rows != 3 || A->cols != 3)
		CV_Error(CV_StsBadArg, "Instrinsic parameters must be 3x3 floating-point matrix");

	cvConvert(A, &_a);
	fx = a[0]; fy = a[4];
	cx = a[2]; cy = a[5];

	if (fixedAspectRatio)
		fx = fy*aspectRatio;

	if (distCoeffs)
	{
		if (!CV_IS_MAT(distCoeffs) ||
			(CV_MAT_DEPTH(distCoeffs->type) != CV_64F &&
				CV_MAT_DEPTH(distCoeffs->type) != CV_32F) ||
				(distCoeffs->rows != 1 && distCoeffs->cols != 1) ||
			(distCoeffs->rows*distCoeffs->cols*CV_MAT_CN(distCoeffs->type) != 4 &&
				distCoeffs->rows*distCoeffs->cols*CV_MAT_CN(distCoeffs->type) != 5 &&
				distCoeffs->rows*distCoeffs->cols*CV_MAT_CN(distCoeffs->type) != 8 &&
				distCoeffs->rows*distCoeffs->cols*CV_MAT_CN(distCoeffs->type) != 12 &&
				distCoeffs->rows*distCoeffs->cols*CV_MAT_CN(distCoeffs->type) != 14))
			CV_Error(CV_StsBadArg, cvDistCoeffErr);

		_k = cvMat(distCoeffs->rows, distCoeffs->cols,
			CV_MAKETYPE(CV_64F, CV_MAT_CN(distCoeffs->type)), k);
		cvConvert(distCoeffs, &_k);
		if (k[12] != 0 || k[13] != 0)
		{
			detail::computeTiltProjectionMatrix(k[12], k[13],
				&matTilt, &dMatTiltdTauX, &dMatTiltdTauY);
		}
	}

	if (dpdr)
	{
		if (!CV_IS_MAT(dpdr) ||
			(CV_MAT_TYPE(dpdr->type) != CV_32FC1 &&
				CV_MAT_TYPE(dpdr->type) != CV_64FC1) ||
			dpdr->rows != count * 2 || dpdr->cols != 3)
			CV_Error(CV_StsBadArg, "dp/drot must be 2Nx3 floating-point matrix");

		if (CV_MAT_TYPE(dpdr->type) == CV_64FC1)
		{
			_dpdr.reset(cvCloneMat(dpdr));
		}
		else
			_dpdr.reset(cvCreateMat(2 * count, 3, CV_64FC1));
		dpdr_p = _dpdr->data.db;
		dpdr_step = _dpdr->step / sizeof(dpdr_p[0]);
	}

	if (dpdt)
	{
		if (!CV_IS_MAT(dpdt) ||
			(CV_MAT_TYPE(dpdt->type) != CV_32FC1 &&
				CV_MAT_TYPE(dpdt->type) != CV_64FC1) ||
			dpdt->rows != count * 2 || dpdt->cols != 3)
			CV_Error(CV_StsBadArg, "dp/dT must be 2Nx3 floating-point matrix");

		if (CV_MAT_TYPE(dpdt->type) == CV_64FC1)
		{
			_dpdt.reset(cvCloneMat(dpdt));
		}
		else
			_dpdt.reset(cvCreateMat(2 * count, 3, CV_64FC1));
		dpdt_p = _dpdt->data.db;
		dpdt_step = _dpdt->step / sizeof(dpdt_p[0]);
	}

	if (dpdf)
	{
		if (!CV_IS_MAT(dpdf) ||
			(CV_MAT_TYPE(dpdf->type) != CV_32FC1 && CV_MAT_TYPE(dpdf->type) != CV_64FC1) ||
			dpdf->rows != count * 2 || dpdf->cols != 2)
			CV_Error(CV_StsBadArg, "dp/df must be 2Nx2 floating-point matrix");

		if (CV_MAT_TYPE(dpdf->type) == CV_64FC1)
		{
			_dpdf.reset(cvCloneMat(dpdf));
		}
		else
			_dpdf.reset(cvCreateMat(2 * count, 2, CV_64FC1));
		dpdf_p = _dpdf->data.db;
		dpdf_step = _dpdf->step / sizeof(dpdf_p[0]);
	}

	if (dpdc)
	{
		if (!CV_IS_MAT(dpdc) ||
			(CV_MAT_TYPE(dpdc->type) != CV_32FC1 && CV_MAT_TYPE(dpdc->type) != CV_64FC1) ||
			dpdc->rows != count * 2 || dpdc->cols != 2)
			CV_Error(CV_StsBadArg, "dp/dc must be 2Nx2 floating-point matrix");

		if (CV_MAT_TYPE(dpdc->type) == CV_64FC1)
		{
			_dpdc.reset(cvCloneMat(dpdc));
		}
		else
			_dpdc.reset(cvCreateMat(2 * count, 2, CV_64FC1));
		dpdc_p = _dpdc->data.db;
		dpdc_step = _dpdc->step / sizeof(dpdc_p[0]);
	}

	if (dpdk)
	{
		if (!CV_IS_MAT(dpdk) ||
			(CV_MAT_TYPE(dpdk->type) != CV_32FC1 && CV_MAT_TYPE(dpdk->type) != CV_64FC1) ||
			dpdk->rows != count * 2 || (dpdk->cols != 14 && dpdk->cols != 12 && dpdk->cols != 8 && dpdk->cols != 5 && dpdk->cols != 4 && dpdk->cols != 2))
			CV_Error(CV_StsBadArg, "dp/df must be 2Nx14, 2Nx12, 2Nx8, 2Nx5, 2Nx4 or 2Nx2 floating-point matrix");

		if (!distCoeffs)
			CV_Error(CV_StsNullPtr, "distCoeffs is NULL while dpdk is not");

		if (CV_MAT_TYPE(dpdk->type) == CV_64FC1)
		{
			_dpdk.reset(cvCloneMat(dpdk));
		}
		else
			_dpdk.reset(cvCreateMat(dpdk->rows, dpdk->cols, CV_64FC1));
		dpdk_p = _dpdk->data.db;
		dpdk_step = _dpdk->step / sizeof(dpdk_p[0]);
	}

	calc_derivatives = dpdr || dpdt || dpdf || dpdc || dpdk;

	for (i = 0; i < count; i++)
	{
		double X = M[i].x, Y = M[i].y, Z = M[i].z;
		double x = R[0] * X + R[1] * Y + R[2] * Z + t[0];
		double y = R[3] * X + R[4] * Y + R[5] * Z + t[1];
		double z = R[6] * X + R[7] * Y + R[8] * Z + t[2];
		double r2, r4, r6, a1, a2, a3, cdist, icdist2;
		double xd, yd, xd0, yd0, invProj;
		Vec3d vecTilt;
		Vec3d dVecTilt;
		Matx22d dMatTilt;
		Vec2d dXdYd;

		z = z ? 1. / z : 1;
		x *= z; y *= z;

		r2 = x*x + y*y;
		r4 = r2*r2;
		r6 = r4*r2;
		a1 = 2 * x*y;
		a2 = r2 + 2 * x*x;
		a3 = r2 + 2 * y*y;
		cdist = 1 + k[0] * r2 + k[1] * r4 + k[4] * r6;
		icdist2 = 1. / (1 + k[5] * r2 + k[6] * r4 + k[7] * r6);
		xd0 = x*cdist*icdist2 + k[2] * a1 + k[3] * a2 + k[8] * r2 + k[9] * r4;
		yd0 = y*cdist*icdist2 + k[2] * a3 + k[3] * a1 + k[10] * r2 + k[11] * r4;

		// additional distortion by projecting onto a tilt plane
		vecTilt = matTilt*Vec3d(xd0, yd0, 1);
		invProj = vecTilt(2) ? 1. / vecTilt(2) : 1;
		xd = invProj * vecTilt(0);
		yd = invProj * vecTilt(1);

		m[i].x = xd*fx + cx;
		m[i].y = yd*fy + cy;

		if (calc_derivatives)
		{
			if (dpdc_p)
			{
				dpdc_p[0] = 1; dpdc_p[1] = 0; // dp_xdc_x; dp_xdc_y
				dpdc_p[dpdc_step] = 0;
				dpdc_p[dpdc_step + 1] = 1;
				dpdc_p += dpdc_step * 2;
			}

			if (dpdf_p)
			{
				if (fixedAspectRatio)
				{
					dpdf_p[0] = 0; dpdf_p[1] = xd*aspectRatio; // dp_xdf_x; dp_xdf_y
					dpdf_p[dpdf_step] = 0;
					dpdf_p[dpdf_step + 1] = yd;
				}
				else
				{
					dpdf_p[0] = xd; dpdf_p[1] = 0;
					dpdf_p[dpdf_step] = 0;
					dpdf_p[dpdf_step + 1] = yd;
				}
				dpdf_p += dpdf_step * 2;
			}
			for (int row = 0; row < 2; ++row)
				for (int col = 0; col < 2; ++col)
					dMatTilt(row, col) = matTilt(row, col)*vecTilt(2)
					- matTilt(2, col)*vecTilt(row);
			double invProjSquare = (invProj*invProj);
			dMatTilt *= invProjSquare;
			if (dpdk_p)
			{
				dXdYd = dMatTilt*Vec2d(x*icdist2*r2, y*icdist2*r2);
				dpdk_p[0] = fx*dXdYd(0);
				dpdk_p[dpdk_step] = fy*dXdYd(1);
				dXdYd = dMatTilt*Vec2d(x*icdist2*r4, y*icdist2*r4);
				dpdk_p[1] = fx*dXdYd(0);
				dpdk_p[dpdk_step + 1] = fy*dXdYd(1);
				if (_dpdk->cols > 2)
				{
					dXdYd = dMatTilt*Vec2d(a1, a3);
					dpdk_p[2] = fx*dXdYd(0);
					dpdk_p[dpdk_step + 2] = fy*dXdYd(1);
					dXdYd = dMatTilt*Vec2d(a2, a1);
					dpdk_p[3] = fx*dXdYd(0);
					dpdk_p[dpdk_step + 3] = fy*dXdYd(1);
					if (_dpdk->cols > 4)
					{
						dXdYd = dMatTilt*Vec2d(x*icdist2*r6, y*icdist2*r6);
						dpdk_p[4] = fx*dXdYd(0);
						dpdk_p[dpdk_step + 4] = fy*dXdYd(1);

						if (_dpdk->cols > 5)
						{
							dXdYd = dMatTilt*Vec2d(
								x*cdist*(-icdist2)*icdist2*r2, y*cdist*(-icdist2)*icdist2*r2);
							dpdk_p[5] = fx*dXdYd(0);
							dpdk_p[dpdk_step + 5] = fy*dXdYd(1);
							dXdYd = dMatTilt*Vec2d(
								x*cdist*(-icdist2)*icdist2*r4, y*cdist*(-icdist2)*icdist2*r4);
							dpdk_p[6] = fx*dXdYd(0);
							dpdk_p[dpdk_step + 6] = fy*dXdYd(1);
							dXdYd = dMatTilt*Vec2d(
								x*cdist*(-icdist2)*icdist2*r6, y*cdist*(-icdist2)*icdist2*r6);
							dpdk_p[7] = fx*dXdYd(0);
							dpdk_p[dpdk_step + 7] = fy*dXdYd(1);
							if (_dpdk->cols > 8)
							{
								dXdYd = dMatTilt*Vec2d(r2, 0);
								dpdk_p[8] = fx*dXdYd(0); //s1
								dpdk_p[dpdk_step + 8] = fy*dXdYd(1); //s1
								dXdYd = dMatTilt*Vec2d(r4, 0);
								dpdk_p[9] = fx*dXdYd(0); //s2
								dpdk_p[dpdk_step + 9] = fy*dXdYd(1); //s2
								dXdYd = dMatTilt*Vec2d(0, r2);
								dpdk_p[10] = fx*dXdYd(0);//s3
								dpdk_p[dpdk_step + 10] = fy*dXdYd(1); //s3
								dXdYd = dMatTilt*Vec2d(0, r4);
								dpdk_p[11] = fx*dXdYd(0);//s4
								dpdk_p[dpdk_step + 11] = fy*dXdYd(1); //s4
								if (_dpdk->cols > 12)
								{
									dVecTilt = dMatTiltdTauX * Vec3d(xd0, yd0, 1);
									dpdk_p[12] = fx * invProjSquare * (
										dVecTilt(0) * vecTilt(2) - dVecTilt(2) * vecTilt(0));
									dpdk_p[dpdk_step + 12] = fy*invProjSquare * (
										dVecTilt(1) * vecTilt(2) - dVecTilt(2) * vecTilt(1));
									dVecTilt = dMatTiltdTauY * Vec3d(xd0, yd0, 1);
									dpdk_p[13] = fx * invProjSquare * (
										dVecTilt(0) * vecTilt(2) - dVecTilt(2) * vecTilt(0));
									dpdk_p[dpdk_step + 13] = fy * invProjSquare * (
										dVecTilt(1) * vecTilt(2) - dVecTilt(2) * vecTilt(1));
								}
							}
						}
					}
				}
				dpdk_p += dpdk_step * 2;
			}

			if (dpdt_p)
			{
				double dxdt[] = { z, 0, -x*z }, dydt[] = { 0, z, -y*z };
				for (j = 0; j < 3; j++)
				{
					double dr2dt = 2 * x*dxdt[j] + 2 * y*dydt[j];
					double dcdist_dt = k[0] * dr2dt + 2 * k[1] * r2*dr2dt + 3 * k[4] * r4*dr2dt;
					double dicdist2_dt = -icdist2*icdist2*(k[5] * dr2dt + 2 * k[6] * r2*dr2dt + 3 * k[7] * r4*dr2dt);
					double da1dt = 2 * (x*dydt[j] + y*dxdt[j]);
					double dmxdt = (dxdt[j] * cdist*icdist2 + x*dcdist_dt*icdist2 + x*cdist*dicdist2_dt +
						k[2] * da1dt + k[3] * (dr2dt + 4 * x*dxdt[j]) + k[8] * dr2dt + 2 * r2*k[9] * dr2dt);
					double dmydt = (dydt[j] * cdist*icdist2 + y*dcdist_dt*icdist2 + y*cdist*dicdist2_dt +
						k[2] * (dr2dt + 4 * y*dydt[j]) + k[3] * da1dt + k[10] * dr2dt + 2 * r2*k[11] * dr2dt);
					dXdYd = dMatTilt*Vec2d(dmxdt, dmydt);
					dpdt_p[j] = fx*dXdYd(0);
					dpdt_p[dpdt_step + j] = fy*dXdYd(1);
				}
				dpdt_p += dpdt_step * 2;
			}

			if (dpdr_p)
			{
				double dx0dr[] =
				{
					X*dRdr[0] + Y*dRdr[1] + Z*dRdr[2],
					X*dRdr[9] + Y*dRdr[10] + Z*dRdr[11],
					X*dRdr[18] + Y*dRdr[19] + Z*dRdr[20]
				};
				double dy0dr[] =
				{
					X*dRdr[3] + Y*dRdr[4] + Z*dRdr[5],
					X*dRdr[12] + Y*dRdr[13] + Z*dRdr[14],
					X*dRdr[21] + Y*dRdr[22] + Z*dRdr[23]
				};
				double dz0dr[] =
				{
					X*dRdr[6] + Y*dRdr[7] + Z*dRdr[8],
					X*dRdr[15] + Y*dRdr[16] + Z*dRdr[17],
					X*dRdr[24] + Y*dRdr[25] + Z*dRdr[26]
				};
				for (j = 0; j < 3; j++)
				{
					double dxdr = z*(dx0dr[j] - x*dz0dr[j]);
					double dydr = z*(dy0dr[j] - y*dz0dr[j]);
					double dr2dr = 2 * x*dxdr + 2 * y*dydr;
					double dcdist_dr = (k[0] + 2 * k[1] * r2 + 3 * k[4] * r4)*dr2dr;
					double dicdist2_dr = -icdist2*icdist2*(k[5] + 2 * k[6] * r2 + 3 * k[7] * r4)*dr2dr;
					double da1dr = 2 * (x*dydr + y*dxdr);
					double dmxdr = (dxdr*cdist*icdist2 + x*dcdist_dr*icdist2 + x*cdist*dicdist2_dr +
						k[2] * da1dr + k[3] * (dr2dr + 4 * x*dxdr) + (k[8] + 2 * r2*k[9])*dr2dr);
					double dmydr = (dydr*cdist*icdist2 + y*dcdist_dr*icdist2 + y*cdist*dicdist2_dr +
						k[2] * (dr2dr + 4 * y*dydr) + k[3] * da1dr + (k[10] + 2 * r2*k[11])*dr2dr);
					dXdYd = dMatTilt*Vec2d(dmxdr, dmydr);
					dpdr_p[j] = fx*dXdYd(0);
					dpdr_p[dpdr_step + j] = fy*dXdYd(1);
				}
				dpdr_p += dpdr_step * 2;
			}
		}
	}

	if (_m != imagePoints)
		cvConvert(_m, imagePoints);

	if (_dpdr != dpdr)
		cvConvert(_dpdr, dpdr);

	if (_dpdt != dpdt)
		cvConvert(_dpdt, dpdt);

	if (_dpdf != dpdf)
		cvConvert(_dpdf, dpdf);

	if (_dpdc != dpdc)
		cvConvert(_dpdc, dpdc);

	if (_dpdk != dpdk)
		cvConvert(_dpdk, dpdk);
}

CV_IMPL void mycvInitIntrinsicParams2D(const CvMat* objectPoints,
	const CvMat* imagePoints, const CvMat* npoints,
	CvSize imageSize, CvMat* cameraMatrix,
	double aspectRatio)
{
	Ptr<CvMat> matA, _b, _allH;

	int i, j, pos, nimages, ni = 0;
	double a[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 1 };
	double H[9], f[2];
	CvMat _a = cvMat(3, 3, CV_64F, a);
	CvMat matH = cvMat(3, 3, CV_64F, H);
	CvMat _f = cvMat(2, 1, CV_64F, f);

	assert(CV_MAT_TYPE(npoints->type) == CV_32SC1 &&
		CV_IS_MAT_CONT(npoints->type));
	nimages = npoints->rows + npoints->cols - 1;

	if ((CV_MAT_TYPE(objectPoints->type) != CV_32FC3 &&
		CV_MAT_TYPE(objectPoints->type) != CV_64FC3) ||
		(CV_MAT_TYPE(imagePoints->type) != CV_32FC2 &&
			CV_MAT_TYPE(imagePoints->type) != CV_64FC2))
		CV_Error(CV_StsUnsupportedFormat, "Both object points and image points must be 2D");

	if (objectPoints->rows != 1 || imagePoints->rows != 1)
		CV_Error(CV_StsBadSize, "object points and image points must be a single-row matrices");

	matA.reset(cvCreateMat(2 * nimages, 2, CV_64F));
	_b.reset(cvCreateMat(2 * nimages, 1, CV_64F));
	a[2] = (!imageSize.width) ? 0.5 : (imageSize.width)*0.5;
	a[5] = (!imageSize.height) ? 0.5 : (imageSize.height)*0.5;
	_allH.reset(cvCreateMat(nimages, 9, CV_64F));

	// extract vanishing points in order to obtain initial value for the focal length
	for (i = 0, pos = 0; i < nimages; i++, pos += ni)
	{
		double* Ap = matA->data.db + i * 4;
		double* bp = _b->data.db + i * 2;
		ni = npoints->data.i[i];
		double h[3], v[3], d1[3], d2[3];
		double n[4] = { 0,0,0,0 };
		CvMat _m, matM;
		cvGetCols(objectPoints, &matM, pos, pos + ni);
		cvGetCols(imagePoints, &_m, pos, pos + ni);

		cvFindHomography(&matM, &_m, &matH);
		memcpy(_allH->data.db + i * 9, H, sizeof(H));

		H[0] -= H[6] * a[2]; H[1] -= H[7] * a[2]; H[2] -= H[8] * a[2];
		H[3] -= H[6] * a[5]; H[4] -= H[7] * a[5]; H[5] -= H[8] * a[5];

		for (j = 0; j < 3; j++)
		{
			double t0 = H[j * 3], t1 = H[j * 3 + 1];
			h[j] = t0; v[j] = t1;
			d1[j] = (t0 + t1)*0.5;
			d2[j] = (t0 - t1)*0.5;
			n[0] += t0*t0; n[1] += t1*t1;
			n[2] += d1[j] * d1[j]; n[3] += d2[j] * d2[j];
		}

		for (j = 0; j < 4; j++)
			n[j] = 1. / std::sqrt(n[j]);

		for (j = 0; j < 3; j++)
		{
			h[j] *= n[0]; v[j] *= n[1];
			d1[j] *= n[2]; d2[j] *= n[3];
		}

		Ap[0] = h[0] * v[0]; Ap[1] = h[1] * v[1];
		Ap[2] = d1[0] * d2[0]; Ap[3] = d1[1] * d2[1];
		bp[0] = -h[2] * v[2]; bp[1] = -d1[2] * d2[2];
	}

	cvSolve(matA, _b, &_f, CV_NORMAL + CV_SVD);
	a[0] = std::sqrt(fabs(1. / f[0]));
	a[4] = std::sqrt(fabs(1. / f[1]));
	if (aspectRatio != 0)
	{
		double tf = (a[0] + a[4]) / (aspectRatio + 1.);
		a[0] = aspectRatio*tf;
		a[4] = tf;
	}

	cvConvert(&_a, cameraMatrix);
}

CV_IMPL double mycvStereoCalibrate(const CvMat* _objectPoints, const CvMat* _imagePoints1,
	const CvMat* _imagePoints2, const CvMat* _npoints,
	CvMat* _cameraMatrix1, CvMat* _distCoeffs1,
	CvMat* _cameraMatrix2, CvMat* _distCoeffs2,
	CvSize imageSize, CvMat* matR, CvMat* matT,
	CvMat* matE, CvMat* matF,
	int flags,
	CvTermCriteria termCrit)
{
	const int NINTRINSIC = 18;
	Ptr<CvMat> npoints, err, J_LR, Je, Ji, imagePoints[2], objectPoints, RT0;
	double reprojErr = 0;

	double A[2][9], dk[2][14] = { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0 } }, rlr[9];
	CvMat K[2], Dist[2], om_LR, T_LR;
	CvMat R_LR = cvMat(3, 3, CV_64F, rlr);
	int i, k, p, ni = 0, ofs, nimages, pointsTotal, maxPoints = 0;
	int nparams;
	bool recomputeIntrinsics = false;
	double aspectRatio[2] = { 0,0 };

	CV_Assert(CV_IS_MAT(_imagePoints1) && CV_IS_MAT(_imagePoints2) &&
		CV_IS_MAT(_objectPoints) && CV_IS_MAT(_npoints) &&
		CV_IS_MAT(matR) && CV_IS_MAT(matT));

	CV_Assert(CV_ARE_TYPES_EQ(_imagePoints1, _imagePoints2) &&
		CV_ARE_DEPTHS_EQ(_imagePoints1, _objectPoints));

	CV_Assert((_npoints->cols == 1 || _npoints->rows == 1) &&
		CV_MAT_TYPE(_npoints->type) == CV_32SC1);

	nimages = _npoints->cols + _npoints->rows - 1;
	npoints.reset(cvCreateMat(_npoints->rows, _npoints->cols, _npoints->type));
	cvCopy(_npoints, npoints);

	for (i = 0, pointsTotal = 0; i < nimages; i++)
	{
		maxPoints = MAX(maxPoints, npoints->data.i[i]);
		pointsTotal += npoints->data.i[i];
	}

	objectPoints.reset(cvCreateMat(_objectPoints->rows, _objectPoints->cols,
		CV_64FC(CV_MAT_CN(_objectPoints->type))));
	cvConvert(_objectPoints, objectPoints);
	cvReshape(objectPoints, objectPoints, 3, 1);

	for (k = 0; k < 2; k++)
	{
		const CvMat* points = k == 0 ? _imagePoints1 : _imagePoints2;
		const CvMat* cameraMatrix = k == 0 ? _cameraMatrix1 : _cameraMatrix2;
		const CvMat* distCoeffs = k == 0 ? _distCoeffs1 : _distCoeffs2;

		int cn = CV_MAT_CN(_imagePoints1->type);
		CV_Assert((CV_MAT_DEPTH(_imagePoints1->type) == CV_32F ||
			CV_MAT_DEPTH(_imagePoints1->type) == CV_64F) &&
			((_imagePoints1->rows == pointsTotal && _imagePoints1->cols*cn == 2) ||
			(_imagePoints1->rows == 1 && _imagePoints1->cols == pointsTotal && cn == 2)));

		K[k] = cvMat(3, 3, CV_64F, A[k]);
		Dist[k] = cvMat(1, 14, CV_64F, dk[k]);

		imagePoints[k].reset(cvCreateMat(points->rows, points->cols, CV_64FC(CV_MAT_CN(points->type))));
		cvConvert(points, imagePoints[k]);
		cvReshape(imagePoints[k], imagePoints[k], 2, 1);

		if (flags & (CV_CALIB_FIX_INTRINSIC | CV_CALIB_USE_INTRINSIC_GUESS |
			CV_CALIB_FIX_ASPECT_RATIO | CV_CALIB_FIX_FOCAL_LENGTH))
			cvConvert(cameraMatrix, &K[k]);

		if (flags & (CV_CALIB_FIX_INTRINSIC | CV_CALIB_USE_INTRINSIC_GUESS |
			CV_CALIB_FIX_K1 | CV_CALIB_FIX_K2 | CV_CALIB_FIX_K3 | CV_CALIB_FIX_K4 | CV_CALIB_FIX_K5 | CV_CALIB_FIX_K6))
		{
			CvMat tdist = cvMat(distCoeffs->rows, distCoeffs->cols,
				CV_MAKETYPE(CV_64F, CV_MAT_CN(distCoeffs->type)), Dist[k].data.db);
			cvConvert(distCoeffs, &tdist);
		}

		if (!(flags & (CV_CALIB_FIX_INTRINSIC | CV_CALIB_USE_INTRINSIC_GUESS)))
		{
			cvCalibrateCamera2(objectPoints, imagePoints[k],
				npoints, imageSize, &K[k], &Dist[k], NULL, NULL, flags);
		}
	}

	if (flags & CV_CALIB_SAME_FOCAL_LENGTH)
	{
		static const int avg_idx[] = { 0, 4, 2, 5, -1 };
		for (k = 0; avg_idx[k] >= 0; k++)
			A[0][avg_idx[k]] = A[1][avg_idx[k]] = (A[0][avg_idx[k]] + A[1][avg_idx[k]])*0.5;
	}

	if (flags & CV_CALIB_FIX_ASPECT_RATIO)
	{
		for (k = 0; k < 2; k++)
			aspectRatio[k] = A[k][0] / A[k][4];
	}

	recomputeIntrinsics = (flags & CV_CALIB_FIX_INTRINSIC) == 0;

	err.reset(cvCreateMat(maxPoints * 2, 1, CV_64F));
	Je.reset(cvCreateMat(maxPoints * 2, 6, CV_64F));
	J_LR.reset(cvCreateMat(maxPoints * 2, 6, CV_64F));
	Ji.reset(cvCreateMat(maxPoints * 2, NINTRINSIC, CV_64F));
	cvZero(Ji);

	// we optimize for the inter-camera R(3),t(3), then, optionally,
	// for intrinisic parameters of each camera ((fx,fy,cx,cy,k1,k2,p1,p2) ~ 8 parameters).
	nparams = 6 * (nimages + 1) + (recomputeIntrinsics ? NINTRINSIC * 2 : 0);

	// storage for initial [om(R){i}|t{i}] (in order to compute the median for each component)
	RT0.reset(cvCreateMat(6, nimages, CV_64F));

	CvLevMarq solver(nparams, 0, termCrit);

	if (flags & CALIB_USE_LU) {
		solver.solveMethod = DECOMP_LU;
	}

	if (recomputeIntrinsics)
	{
		uchar* imask = solver.mask->data.ptr + nparams - NINTRINSIC * 2;
		if (!(flags & CV_CALIB_RATIONAL_MODEL))
			flags |= CV_CALIB_FIX_K4 | CV_CALIB_FIX_K5 | CV_CALIB_FIX_K6;
		if (!(flags & CV_CALIB_THIN_PRISM_MODEL))
			flags |= CV_CALIB_FIX_S1_S2_S3_S4;
		if (!(flags & CV_CALIB_TILTED_MODEL))
			flags |= CV_CALIB_FIX_TAUX_TAUY;
		if (flags & CV_CALIB_FIX_ASPECT_RATIO)
			imask[0] = imask[NINTRINSIC] = 0;
		if (flags & CV_CALIB_FIX_FOCAL_LENGTH)
			imask[0] = imask[1] = imask[NINTRINSIC] = imask[NINTRINSIC + 1] = 0;
		if (flags & CV_CALIB_FIX_PRINCIPAL_POINT)
			imask[2] = imask[3] = imask[NINTRINSIC + 2] = imask[NINTRINSIC + 3] = 0;
		if (flags & CV_CALIB_ZERO_TANGENT_DIST)
			imask[6] = imask[7] = imask[NINTRINSIC + 6] = imask[NINTRINSIC + 7] = 0;
		if (flags & CV_CALIB_FIX_K1)
			imask[4] = imask[NINTRINSIC + 4] = 0;
		if (flags & CV_CALIB_FIX_K2)
			imask[5] = imask[NINTRINSIC + 5] = 0;
		if (flags & CV_CALIB_FIX_K3)
			imask[8] = imask[NINTRINSIC + 8] = 0;
		if (flags & CV_CALIB_FIX_K4)
			imask[9] = imask[NINTRINSIC + 9] = 0;
		if (flags & CV_CALIB_FIX_K5)
			imask[10] = imask[NINTRINSIC + 10] = 0;
		if (flags & CV_CALIB_FIX_K6)
			imask[11] = imask[NINTRINSIC + 11] = 0;
		if (flags & CV_CALIB_FIX_S1_S2_S3_S4)
		{
			imask[12] = imask[NINTRINSIC + 12] = 0;
			imask[13] = imask[NINTRINSIC + 13] = 0;
			imask[14] = imask[NINTRINSIC + 14] = 0;
			imask[15] = imask[NINTRINSIC + 15] = 0;
		}
		if (flags & CV_CALIB_FIX_TAUX_TAUY)
		{
			imask[16] = imask[NINTRINSIC + 16] = 0;
			imask[17] = imask[NINTRINSIC + 17] = 0;
		}
	}

	/*
	Compute initial estimate of pose
	For each image, compute:
	R(om) is the rotation matrix of om
	om(R) is the rotation vector of R
	R_ref = R(om_right) * R(om_left)'
	T_ref_list = [T_ref_list; T_right - R_ref * T_left]
	om_ref_list = {om_ref_list; om(R_ref)]
	om = median(om_ref_list)
	T = median(T_ref_list)
	*/
	for (i = ofs = 0; i < nimages; ofs += ni, i++)
	{
		ni = npoints->data.i[i];
		CvMat objpt_i;
		double _om[2][3], r[2][9], t[2][3];
		CvMat om[2], R[2], T[2], imgpt_i[2];

		objpt_i = cvMat(1, ni, CV_64FC3, objectPoints->data.db + ofs * 3);
		for (k = 0; k < 2; k++)
		{
			imgpt_i[k] = cvMat(1, ni, CV_64FC2, imagePoints[k]->data.db + ofs * 2);
			om[k] = cvMat(3, 1, CV_64F, _om[k]);
			R[k] = cvMat(3, 3, CV_64F, r[k]);
			T[k] = cvMat(3, 1, CV_64F, t[k]);

			// FIXME: here we ignore activePoints[k] because of
			// the limited API of cvFindExtrnisicCameraParams2
			cvFindExtrinsicCameraParams2(&objpt_i, &imgpt_i[k], &K[k], &Dist[k], &om[k], &T[k]);
			cvRodrigues2(&om[k], &R[k]);
			if (k == 0)
			{
				// save initial om_left and T_left
				solver.param->data.db[(i + 1) * 6] = _om[0][0];
				solver.param->data.db[(i + 1) * 6 + 1] = _om[0][1];
				solver.param->data.db[(i + 1) * 6 + 2] = _om[0][2];
				solver.param->data.db[(i + 1) * 6 + 3] = t[0][0];
				solver.param->data.db[(i + 1) * 6 + 4] = t[0][1];
				solver.param->data.db[(i + 1) * 6 + 5] = t[0][2];
			}
		}
		cvGEMM(&R[1], &R[0], 1, 0, 0, &R[0], CV_GEMM_B_T);
		cvGEMM(&R[0], &T[0], -1, &T[1], 1, &T[1]);
		cvRodrigues2(&R[0], &T[0]);
		RT0->data.db[i] = t[0][0];
		RT0->data.db[i + nimages] = t[0][1];
		RT0->data.db[i + nimages * 2] = t[0][2];
		RT0->data.db[i + nimages * 3] = t[1][0];
		RT0->data.db[i + nimages * 4] = t[1][1];
		RT0->data.db[i + nimages * 5] = t[1][2];
	}

	// find the medians and save the first 6 parameters
	for (i = 0; i < 6; i++)
	{
		qsort(RT0->data.db + i*nimages, nimages, CV_ELEM_SIZE(RT0->type), dbCmp);
		solver.param->data.db[i] = nimages % 2 != 0 ? RT0->data.db[i*nimages + nimages / 2] :
			(RT0->data.db[i*nimages + nimages / 2 - 1] + RT0->data.db[i*nimages + nimages / 2])*0.5;
	}

	if (recomputeIntrinsics)
		for (k = 0; k < 2; k++)
		{
			double* iparam = solver.param->data.db + (nimages + 1) * 6 + k*NINTRINSIC;
			if (flags & CV_CALIB_ZERO_TANGENT_DIST)
				dk[k][2] = dk[k][3] = 0;
			iparam[0] = A[k][0]; iparam[1] = A[k][4]; iparam[2] = A[k][2]; iparam[3] = A[k][5];
			iparam[4] = dk[k][0]; iparam[5] = dk[k][1]; iparam[6] = dk[k][2];
			iparam[7] = dk[k][3]; iparam[8] = dk[k][4]; iparam[9] = dk[k][5];
			iparam[10] = dk[k][6]; iparam[11] = dk[k][7];
			iparam[12] = dk[k][8];
			iparam[13] = dk[k][9];
			iparam[14] = dk[k][10];
			iparam[15] = dk[k][11];
			iparam[16] = dk[k][12];
			iparam[17] = dk[k][13];
		}

	om_LR = cvMat(3, 1, CV_64F, solver.param->data.db);
	T_LR = cvMat(3, 1, CV_64F, solver.param->data.db + 3);

	for (;;)
	{
		const CvMat* param = 0;
		CvMat tmpimagePoints;
		CvMat *JtJ = 0, *JtErr = 0;
		double *_errNorm = 0;
		double _omR[3], _tR[3];
		double _dr3dr1[9], _dr3dr2[9], /*_dt3dr1[9],*/ _dt3dr2[9], _dt3dt1[9], _dt3dt2[9];
		CvMat dr3dr1 = cvMat(3, 3, CV_64F, _dr3dr1);
		CvMat dr3dr2 = cvMat(3, 3, CV_64F, _dr3dr2);
		//CvMat dt3dr1 = cvMat(3, 3, CV_64F, _dt3dr1);
		CvMat dt3dr2 = cvMat(3, 3, CV_64F, _dt3dr2);
		CvMat dt3dt1 = cvMat(3, 3, CV_64F, _dt3dt1);
		CvMat dt3dt2 = cvMat(3, 3, CV_64F, _dt3dt2);
		CvMat om[2], T[2], imgpt_i[2];
		CvMat dpdrot_hdr, dpdt_hdr, dpdf_hdr, dpdc_hdr, dpdk_hdr;
		CvMat *dpdrot = &dpdrot_hdr, *dpdt = &dpdt_hdr, *dpdf = 0, *dpdc = 0, *dpdk = 0;

		if (!solver.updateAlt(param, JtJ, JtErr, _errNorm))
			break;
		reprojErr = 0;

		cvRodrigues2(&om_LR, &R_LR);
		om[1] = cvMat(3, 1, CV_64F, _omR);
		T[1] = cvMat(3, 1, CV_64F, _tR);

		if (recomputeIntrinsics)
		{
			double* iparam = solver.param->data.db + (nimages + 1) * 6;
			double* ipparam = solver.prevParam->data.db + (nimages + 1) * 6;
			dpdf = &dpdf_hdr;
			dpdc = &dpdc_hdr;
			dpdk = &dpdk_hdr;
			if (flags & CV_CALIB_SAME_FOCAL_LENGTH)
			{
				iparam[NINTRINSIC] = iparam[0];
				iparam[NINTRINSIC + 1] = iparam[1];
				ipparam[NINTRINSIC] = ipparam[0];
				ipparam[NINTRINSIC + 1] = ipparam[1];
			}
			if (flags & CV_CALIB_FIX_ASPECT_RATIO)
			{
				iparam[0] = iparam[1] * aspectRatio[0];
				iparam[NINTRINSIC] = iparam[NINTRINSIC + 1] * aspectRatio[1];
				ipparam[0] = ipparam[1] * aspectRatio[0];
				ipparam[NINTRINSIC] = ipparam[NINTRINSIC + 1] * aspectRatio[1];
			}
			for (k = 0; k < 2; k++)
			{
				A[k][0] = iparam[k*NINTRINSIC + 0];
				A[k][4] = iparam[k*NINTRINSIC + 1];
				A[k][2] = iparam[k*NINTRINSIC + 2];
				A[k][5] = iparam[k*NINTRINSIC + 3];
				dk[k][0] = iparam[k*NINTRINSIC + 4];
				dk[k][1] = iparam[k*NINTRINSIC + 5];
				dk[k][2] = iparam[k*NINTRINSIC + 6];
				dk[k][3] = iparam[k*NINTRINSIC + 7];
				dk[k][4] = iparam[k*NINTRINSIC + 8];
				dk[k][5] = iparam[k*NINTRINSIC + 9];
				dk[k][6] = iparam[k*NINTRINSIC + 10];
				dk[k][7] = iparam[k*NINTRINSIC + 11];
				dk[k][8] = iparam[k*NINTRINSIC + 12];
				dk[k][9] = iparam[k*NINTRINSIC + 13];
				dk[k][10] = iparam[k*NINTRINSIC + 14];
				dk[k][11] = iparam[k*NINTRINSIC + 15];
				dk[k][12] = iparam[k*NINTRINSIC + 16];
				dk[k][13] = iparam[k*NINTRINSIC + 17];
			}
		}

		for (i = ofs = 0; i < nimages; ofs += ni, i++)
		{
			ni = npoints->data.i[i];
			CvMat objpt_i, _part;

			om[0] = cvMat(3, 1, CV_64F, solver.param->data.db + (i + 1) * 6);
			T[0] = cvMat(3, 1, CV_64F, solver.param->data.db + (i + 1) * 6 + 3);

			if (JtJ || JtErr)
				cvComposeRT(&om[0], &T[0], &om_LR, &T_LR, &om[1], &T[1], &dr3dr1, 0,
					&dr3dr2, 0, 0, &dt3dt1, &dt3dr2, &dt3dt2);
			else
				cvComposeRT(&om[0], &T[0], &om_LR, &T_LR, &om[1], &T[1]);

			objpt_i = cvMat(1, ni, CV_64FC3, objectPoints->data.db + ofs * 3);
			err->rows = Je->rows = J_LR->rows = Ji->rows = ni * 2;
			cvReshape(err, &tmpimagePoints, 2, 1);

			cvGetCols(Ji, &dpdf_hdr, 0, 2);
			cvGetCols(Ji, &dpdc_hdr, 2, 4);
			cvGetCols(Ji, &dpdk_hdr, 4, NINTRINSIC);
			cvGetCols(Je, &dpdrot_hdr, 0, 3);
			cvGetCols(Je, &dpdt_hdr, 3, 6);

			for (k = 0; k < 2; k++)
			{
				double l2err;
				imgpt_i[k] = cvMat(1, ni, CV_64FC2, imagePoints[k]->data.db + ofs * 2);

				if (JtJ || JtErr)
					cvProjectPoints2(&objpt_i, &om[k], &T[k], &K[k], &Dist[k],
						&tmpimagePoints, dpdrot, dpdt, dpdf, dpdc, dpdk,
						(flags & CV_CALIB_FIX_ASPECT_RATIO) ? aspectRatio[k] : 0);
				else
					cvProjectPoints2(&objpt_i, &om[k], &T[k], &K[k], &Dist[k], &tmpimagePoints);
				cvSub(&tmpimagePoints, &imgpt_i[k], &tmpimagePoints);

				l2err = cvNorm(&tmpimagePoints, 0, CV_L2);

				if (JtJ || JtErr)
				{
					int iofs = (nimages + 1) * 6 + k*NINTRINSIC, eofs = (i + 1) * 6;
					assert(JtJ && JtErr);

					if (k == 1)
					{
						// d(err_{x|y}R) ~ de3
						// convert de3/{dr3,dt3} => de3{dr1,dt1} & de3{dr2,dt2}
						for (p = 0; p < ni * 2; p++)
						{
							CvMat de3dr3 = cvMat(1, 3, CV_64F, Je->data.ptr + Je->step*p);
							CvMat de3dt3 = cvMat(1, 3, CV_64F, de3dr3.data.db + 3);
							CvMat de3dr2 = cvMat(1, 3, CV_64F, J_LR->data.ptr + J_LR->step*p);
							CvMat de3dt2 = cvMat(1, 3, CV_64F, de3dr2.data.db + 3);
							double _de3dr1[3], _de3dt1[3];
							CvMat de3dr1 = cvMat(1, 3, CV_64F, _de3dr1);
							CvMat de3dt1 = cvMat(1, 3, CV_64F, _de3dt1);

							cvMatMul(&de3dr3, &dr3dr1, &de3dr1);
							cvMatMul(&de3dt3, &dt3dt1, &de3dt1);

							cvMatMul(&de3dr3, &dr3dr2, &de3dr2);
							cvMatMulAdd(&de3dt3, &dt3dr2, &de3dr2, &de3dr2);

							cvMatMul(&de3dt3, &dt3dt2, &de3dt2);

							cvCopy(&de3dr1, &de3dr3);
							cvCopy(&de3dt1, &de3dt3);
						}

						cvGetSubRect(JtJ, &_part, cvRect(0, 0, 6, 6));
						cvGEMM(J_LR, J_LR, 1, &_part, 1, &_part, CV_GEMM_A_T);

						cvGetSubRect(JtJ, &_part, cvRect(eofs, 0, 6, 6));
						cvGEMM(J_LR, Je, 1, 0, 0, &_part, CV_GEMM_A_T);

						cvGetRows(JtErr, &_part, 0, 6);
						cvGEMM(J_LR, err, 1, &_part, 1, &_part, CV_GEMM_A_T);
					}

					cvGetSubRect(JtJ, &_part, cvRect(eofs, eofs, 6, 6));
					cvGEMM(Je, Je, 1, &_part, 1, &_part, CV_GEMM_A_T);

					cvGetRows(JtErr, &_part, eofs, eofs + 6);
					cvGEMM(Je, err, 1, &_part, 1, &_part, CV_GEMM_A_T);

					if (recomputeIntrinsics)
					{
						cvGetSubRect(JtJ, &_part, cvRect(iofs, iofs, NINTRINSIC, NINTRINSIC));
						cvGEMM(Ji, Ji, 1, &_part, 1, &_part, CV_GEMM_A_T);
						cvGetSubRect(JtJ, &_part, cvRect(iofs, eofs, NINTRINSIC, 6));
						cvGEMM(Je, Ji, 1, &_part, 1, &_part, CV_GEMM_A_T);
						if (k == 1)
						{
							cvGetSubRect(JtJ, &_part, cvRect(iofs, 0, NINTRINSIC, 6));
							cvGEMM(J_LR, Ji, 1, &_part, 1, &_part, CV_GEMM_A_T);
						}
						cvGetRows(JtErr, &_part, iofs, iofs + NINTRINSIC);
						cvGEMM(Ji, err, 1, &_part, 1, &_part, CV_GEMM_A_T);
					}
				}

				reprojErr += l2err*l2err;
			}
		}
		if (_errNorm)
			*_errNorm = reprojErr;
	}

	cvRodrigues2(&om_LR, &R_LR);
	if (matR->rows == 1 || matR->cols == 1)
		cvConvert(&om_LR, matR);
	else
		cvConvert(&R_LR, matR);
	cvConvert(&T_LR, matT);

	if (recomputeIntrinsics)
	{
		cvConvert(&K[0], _cameraMatrix1);
		cvConvert(&K[1], _cameraMatrix2);

		for (k = 0; k < 2; k++)
		{
			CvMat* distCoeffs = k == 0 ? _distCoeffs1 : _distCoeffs2;
			CvMat tdist = cvMat(distCoeffs->rows, distCoeffs->cols,
				CV_MAKETYPE(CV_64F, CV_MAT_CN(distCoeffs->type)), Dist[k].data.db);
			cvConvert(&tdist, distCoeffs);
		}
	}

	if (matE || matF)
	{
		double* t = T_LR.data.db;
		double tx[] =
		{
			0, -t[2], t[1],
			t[2], 0, -t[0],
			-t[1], t[0], 0
		};
		CvMat Tx = cvMat(3, 3, CV_64F, tx);
		double e[9], f[9];
		CvMat E = cvMat(3, 3, CV_64F, e);
		CvMat F = cvMat(3, 3, CV_64F, f);
		cvMatMul(&Tx, &R_LR, &E);
		if (matE)
			cvConvert(&E, matE);
		if (matF)
		{
			double ik[9];
			CvMat iK = cvMat(3, 3, CV_64F, ik);
			cvInvert(&K[1], &iK);
			cvGEMM(&iK, &E, 1, 0, 0, &E, CV_GEMM_A_T);
			cvInvert(&K[0], &iK);
			cvMatMul(&E, &iK, &F);
			cvConvertScale(&F, matF, fabs(f[8]) > 0 ? 1. / f[8] : 1);
		}
	}

	return std::sqrt(reprojErr / (pointsTotal * 2));
}

void mycvStereoRectify(const CvMat* _cameraMatrix1, const CvMat* _cameraMatrix2,
	const CvMat* _distCoeffs1, const CvMat* _distCoeffs2,
	CvSize imageSize, const CvMat* matR, const CvMat* matT,
	CvMat* _R1, CvMat* _R2, CvMat* _P1, CvMat* _P2,
	CvMat* matQ, int flags, double alpha, CvSize newImgSize,
	CvRect* roi1, CvRect* roi2)
{
	double _om[3], _t[3], _uu[3] = { 0,0,0 }, _r_r[3][3], _pp[3][4];
	double _ww[3], _wr[3][3], _z[3] = { 0,0,0 }, _ri[3][3];
	cv::Rect_<float> inner1, inner2, outer1, outer2;

	CvMat om = cvMat(3, 1, CV_64F, _om);
	CvMat t = cvMat(3, 1, CV_64F, _t);
	CvMat uu = cvMat(3, 1, CV_64F, _uu);
	CvMat r_r = cvMat(3, 3, CV_64F, _r_r);
	CvMat pp = cvMat(3, 4, CV_64F, _pp);
	CvMat ww = cvMat(3, 1, CV_64F, _ww); // temps
	CvMat wR = cvMat(3, 3, CV_64F, _wr);
	CvMat Z = cvMat(3, 1, CV_64F, _z);
	CvMat Ri = cvMat(3, 3, CV_64F, _ri);
	double nx = imageSize.width, ny = imageSize.height;
	int i, k;

	if (matR->rows == 3 && matR->cols == 3)
		cvRodrigues2(matR, &om);          // get vector rotation
	else
		cvConvert(matR, &om); // it's already a rotation vector
	cvConvertScale(&om, &om, -0.5); // get average rotation
	cvRodrigues2(&om, &r_r);        // rotate cameras to same orientation by averaging
	cvMatMul(&r_r, matT, &t);

	int idx = fabs(_t[0]) > fabs(_t[1]) ? 0 : 1;
	double c = _t[idx], nt = cvNorm(&t, 0, CV_L2);
	_uu[idx] = c > 0 ? 1 : -1;

	// calculate global Z rotation
	cvCrossProduct(&t, &uu, &ww);
	double nw = cvNorm(&ww, 0, CV_L2);
	if (nw > 0.0)
		cvConvertScale(&ww, &ww, acos(fabs(c) / nt) / nw);
	cvRodrigues2(&ww, &wR);

	// apply to both views
	cvGEMM(&wR, &r_r, 1, 0, 0, &Ri, CV_GEMM_B_T);
	cvConvert(&Ri, _R1);
	cvGEMM(&wR, &r_r, 1, 0, 0, &Ri, 0);
	cvConvert(&Ri, _R2);
	cvMatMul(&Ri, matT, &t);

	// calculate projection/camera matrices
	// these contain the relevant rectified image internal params (fx, fy=fx, cx, cy)
	double fc_new = DBL_MAX;
	CvPoint2D64f cc_new[2] = { { 0,0 },{ 0,0 } };

	for (k = 0; k < 2; k++) {
		const CvMat* A = k == 0 ? _cameraMatrix1 : _cameraMatrix2;
		const CvMat* Dk = k == 0 ? _distCoeffs1 : _distCoeffs2;
		double dk1 = Dk && Dk->data.ptr ? cvmGet(Dk, 0, 0) : 0;
		double fc = cvmGet(A, idx ^ 1, idx ^ 1);
		if (dk1 < 0) {
			fc *= 1 + dk1*(nx*nx + ny*ny) / (4 * fc*fc);
		}
		fc_new = MIN(fc_new, fc);
	}

	for (k = 0; k < 2; k++)
	{
		const CvMat* A = k == 0 ? _cameraMatrix1 : _cameraMatrix2;
		const CvMat* Dk = k == 0 ? _distCoeffs1 : _distCoeffs2;
		CvPoint2D32f _pts[4];
		CvPoint3D32f _pts_3[4];
		CvMat pts = cvMat(1, 4, CV_32FC2, _pts);
		CvMat pts_3 = cvMat(1, 4, CV_32FC3, _pts_3);

		for (i = 0; i < 4; i++)
		{
			int j = (i<2) ? 0 : 1;
			_pts[i].x = (float)((i % 2)*(nx));
			_pts[i].y = (float)(j*(ny));
		}
		cvUndistortPoints(&pts, &pts, A, Dk, 0, 0);
		cvConvertPointsHomogeneous(&pts, &pts_3);

		//Change camera matrix to have cc=[0,0] and fc = fc_new
		double _a_tmp[3][3];
		CvMat A_tmp = cvMat(3, 3, CV_64F, _a_tmp);
		_a_tmp[0][0] = fc_new;
		_a_tmp[1][1] = fc_new;
		_a_tmp[0][2] = 0.0;
		_a_tmp[1][2] = 0.0;
		cvProjectPoints2(&pts_3, k == 0 ? _R1 : _R2, &Z, &A_tmp, 0, &pts);
		CvScalar avg = cvAvg(&pts);
		cc_new[k].x = (nx) / 2 - avg.val[0];
		cc_new[k].y = (ny) / 2 - avg.val[1];
	}

	// vertical focal length must be the same for both images to keep the epipolar constraint
	// (for horizontal epipolar lines -- TBD: check for vertical epipolar lines)
	// use fy for fx also, for simplicity

	// For simplicity, set the principal points for both cameras to be the average
	// of the two principal points (either one of or both x- and y- coordinates)
	if (flags & CV_CALIB_ZERO_DISPARITY)
	{
		cc_new[0].x = cc_new[1].x = (cc_new[0].x + cc_new[1].x)*0.5;
		cc_new[0].y = cc_new[1].y = (cc_new[0].y + cc_new[1].y)*0.5;
	}
	else if (idx == 0) // horizontal stereo
		cc_new[0].y = cc_new[1].y = (cc_new[0].y + cc_new[1].y)*0.5;
	else // vertical stereo
		cc_new[0].x = cc_new[1].x = (cc_new[0].x + cc_new[1].x)*0.5;

	cvZero(&pp);
	_pp[0][0] = _pp[1][1] = fc_new;
	_pp[0][2] = cc_new[0].x;
	_pp[1][2] = cc_new[0].y;
	_pp[2][2] = 1;
	cvConvert(&pp, _P1);

	_pp[0][2] = cc_new[1].x;
	_pp[1][2] = cc_new[1].y;
	_pp[idx][3] = _t[idx] * fc_new; // baseline * focal length
	cvConvert(&pp, _P2);

	alpha = MIN(alpha, 1.);

	icvGetRectangles(_cameraMatrix1, _distCoeffs1, _R1, _P1, imageSize, inner1, outer1);
	icvGetRectangles(_cameraMatrix2, _distCoeffs2, _R2, _P2, imageSize, inner2, outer2);

	{
		newImgSize = newImgSize.width*newImgSize.height != 0 ? newImgSize : imageSize;
		double cx1_0 = cc_new[0].x;
		double cy1_0 = cc_new[0].y;
		double cx2_0 = cc_new[1].x;
		double cy2_0 = cc_new[1].y;
		double cx1 = newImgSize.width*cx1_0 / imageSize.width;
		double cy1 = newImgSize.height*cy1_0 / imageSize.height;
		double cx2 = newImgSize.width*cx2_0 / imageSize.width;
		double cy2 = newImgSize.height*cy2_0 / imageSize.height;
		double s = 1.;

		if (alpha >= 0)
		{
			double s0 = std::max(std::max(std::max((double)cx1 / (cx1_0 - inner1.x), (double)cy1 / (cy1_0 - inner1.y)),
				(double)(newImgSize.width - cx1) / (inner1.x + inner1.width - cx1_0)),
				(double)(newImgSize.height - cy1) / (inner1.y + inner1.height - cy1_0));
			s0 = std::max(std::max(std::max(std::max((double)cx2 / (cx2_0 - inner2.x), (double)cy2 / (cy2_0 - inner2.y)),
				(double)(newImgSize.width - cx2) / (inner2.x + inner2.width - cx2_0)),
				(double)(newImgSize.height - cy2) / (inner2.y + inner2.height - cy2_0)),
				s0);

			double s1 = std::min(std::min(std::min((double)cx1 / (cx1_0 - outer1.x), (double)cy1 / (cy1_0 - outer1.y)),
				(double)(newImgSize.width - cx1) / (outer1.x + outer1.width - cx1_0)),
				(double)(newImgSize.height - cy1) / (outer1.y + outer1.height - cy1_0));
			s1 = std::min(std::min(std::min(std::min((double)cx2 / (cx2_0 - outer2.x), (double)cy2 / (cy2_0 - outer2.y)),
				(double)(newImgSize.width - cx2) / (outer2.x + outer2.width - cx2_0)),
				(double)(newImgSize.height - cy2) / (outer2.y + outer2.height - cy2_0)),
				s1);

			s = s0*(1 - alpha) + s1*alpha;
		}

		fc_new *= s;
		cc_new[0] = cvPoint2D64f(cx1, cy1);
		cc_new[1] = cvPoint2D64f(cx2, cy2);

		cvmSet(_P1, 0, 0, fc_new);
		cvmSet(_P1, 1, 1, fc_new);
		cvmSet(_P1, 0, 2, cx1);
		cvmSet(_P1, 1, 2, cy1);

		cvmSet(_P2, 0, 0, fc_new);
		cvmSet(_P2, 1, 1, fc_new);
		cvmSet(_P2, 0, 2, cx2);
		cvmSet(_P2, 1, 2, cy2);
		cvmSet(_P2, idx, 3, s*cvmGet(_P2, idx, 3));

		if (roi1)
		{
			*roi1 = cv::Rect(cvCeil((inner1.x - cx1_0)*s + cx1),
				cvCeil((inner1.y - cy1_0)*s + cy1),
				cvFloor(inner1.width*s), cvFloor(inner1.height*s))
				& cv::Rect(0, 0, newImgSize.width, newImgSize.height);
		}

		if (roi2)
		{
			*roi2 = cv::Rect(cvCeil((inner2.x - cx2_0)*s + cx2),
				cvCeil((inner2.y - cy2_0)*s + cy2),
				cvFloor(inner2.width*s), cvFloor(inner2.height*s))
				& cv::Rect(0, 0, newImgSize.width, newImgSize.height);
		}
	}

	if (matQ)
	{
		double q[] =
		{
			1, 0, 0, -cc_new[0].x,
			0, 1, 0, -cc_new[0].y,
			0, 0, 0, fc_new,
			0, 0, -1. / _t[idx],
			(idx == 0 ? cc_new[0].x - cc_new[1].x : cc_new[0].y - cc_new[1].y) / _t[idx]
		};
		CvMat Q = cvMat(4, 4, CV_64F, q);
		cvConvert(&Q, matQ);
	}
}

// This method is the same as icvReconstructPointsFor3View, with only a few numbers adjusted for two-view geometry
CV_IMPL void
mycvTriangulatePoints(CvMat* projMatr1, CvMat* projMatr2, CvMat* projPoints1, CvMat* projPoints2, CvMat* points4D)
{
	if (projMatr1 == 0 || projMatr2 == 0 ||
		projPoints1 == 0 || projPoints2 == 0 ||
		points4D == 0)
		CV_Error(CV_StsNullPtr, "Some of parameters is a NULL pointer");

	if (!CV_IS_MAT(projMatr1) || !CV_IS_MAT(projMatr2) ||
		!CV_IS_MAT(projPoints1) || !CV_IS_MAT(projPoints2) ||
		!CV_IS_MAT(points4D))
		CV_Error(CV_StsUnsupportedFormat, "Input parameters must be matrices");

	int numPoints = projPoints1->cols;

	if (numPoints < 1)
		CV_Error(CV_StsOutOfRange, "Number of points must be more than zero");

	if (projPoints2->cols != numPoints || points4D->cols != numPoints)
		CV_Error(CV_StsUnmatchedSizes, "Number of points must be the same");

	if (projPoints1->rows != 2 || projPoints2->rows != 2)
		CV_Error(CV_StsUnmatchedSizes, "Number of proj points coordinates must be == 2");

	if (points4D->rows != 4)
		CV_Error(CV_StsUnmatchedSizes, "Number of world points coordinates must be == 4");

	if (projMatr1->cols != 4 || projMatr1->rows != 3 ||
		projMatr2->cols != 4 || projMatr2->rows != 3)
		CV_Error(CV_StsUnmatchedSizes, "Size of projection matrices must be 3x4");

	// preallocate SVD matrices on stack
	cv::Matx<double, 4, 4> matrA;
	cv::Matx<double, 4, 4> matrU;
	cv::Matx<double, 4, 1> matrW;
	cv::Matx<double, 4, 4> matrV;

	CvMat* projPoints[2] = { projPoints1, projPoints2 };
	CvMat* projMatrs[2] = { projMatr1, projMatr2 };

	/* Solve system for each point */
	for (int i = 0; i < numPoints; i++)/* For each point */
	{
		/* Fill matrix for current point */
		for (int j = 0; j < 2; j++)/* For each view */
		{
			double x, y;
			x = cvmGet(projPoints[j], 0, i);
			y = cvmGet(projPoints[j], 1, i);
			for (int k = 0; k < 4; k++)
			{
				matrA(j * 2 + 0, k) = x * cvmGet(projMatrs[j], 2, k) - cvmGet(projMatrs[j], 0, k);
				matrA(j * 2 + 1, k) = y * cvmGet(projMatrs[j], 2, k) - cvmGet(projMatrs[j], 1, k);
			}
		}
		/* Solve system for current point */
		cv::SVD::compute(matrA, matrW, matrU, matrV);

		/* Copy computed point */
		cvmSet(points4D, 0, i, matrV(3, 0));/* X */
		cvmSet(points4D, 1, i, matrV(3, 1));/* Y */
		cvmSet(points4D, 2, i, matrV(3, 2));/* Z */
		cvmSet(points4D, 3, i, matrV(3, 3));/* W */
	}
}

cv::Mat mycv::initCameraMatrix2D(InputArrayOfArrays objectPoints,
	InputArrayOfArrays imagePoints,
	Size imageSize, double aspectRatio)
{

		Mat objPt, imgPt, npoints, cameraMatrix(3, 3, CV_64F);
	collectCalibrationData(objectPoints, imagePoints, noArray(),
		objPt, imgPt, 0, npoints);
	CvMat _objPt = objPt, _imgPt = imgPt, _npoints = npoints, _cameraMatrix = cameraMatrix;
	mycvInitIntrinsicParams2D(&_objPt, &_imgPt, &_npoints,
		imageSize, &_cameraMatrix, aspectRatio);
	return cameraMatrix;
}

void mycv::projectPoints(cv::InputArray _opoints,
	cv::InputArray _rvec,
	cv::InputArray _tvec,
	cv::InputArray _cameraMatrix,
	cv::InputArray _distCoeffs,
	cv::OutputArray _ipoints,
	cv::OutputArray _jacobian,
	double aspectRatio)
{
	cv::Mat opoints = _opoints.getMat();
	int npoints = opoints.checkVector(3), depth = opoints.depth();
	CV_Assert(npoints >= 0 && (depth == CV_32F || depth == CV_64F));

	CvMat dpdrot, dpdt, dpdf, dpdc, dpddist;
	CvMat *pdpdrot = 0, *pdpdt = 0, *pdpdf = 0, *pdpdc = 0, *pdpddist = 0;

	_ipoints.create(npoints, 1, CV_MAKETYPE(depth, 2), -1, true);
	cv::Mat imagePoints = _ipoints.getMat();
	CvMat c_imagePoints(imagePoints);
	CvMat c_objectPoints = opoints;
	cv::Mat cameraMatrix = _cameraMatrix.getMat();

	cv::Mat rvec = _rvec.getMat(), tvec = _tvec.getMat();
	CvMat c_cameraMatrix = cameraMatrix;
	CvMat c_rvec = rvec, c_tvec = tvec;

	double dc0buf[5] = { 0 };
	cv::Mat dc0(5, 1, CV_64F, dc0buf);
	cv::Mat distCoeffs = _distCoeffs.getMat();
	if (distCoeffs.empty())
		distCoeffs = dc0;
	CvMat c_distCoeffs = distCoeffs;
	int ndistCoeffs = distCoeffs.rows + distCoeffs.cols - 1;

	if (_jacobian.needed())
	{
		_jacobian.create(npoints * 2, 3 + 3 + 2 + 2 + ndistCoeffs, CV_64F);
		cv::Mat jacobian = _jacobian.getMat();
		pdpdrot = &(dpdrot = jacobian.colRange(0, 3));
		pdpdt = &(dpdt = jacobian.colRange(3, 6));
		pdpdf = &(dpdf = jacobian.colRange(6, 8));
		pdpdc = &(dpdc = jacobian.colRange(8, 10));
		pdpddist = &(dpddist = jacobian.colRange(10, 10 + ndistCoeffs));
	}

	mycvProjectPoints2(&c_objectPoints, &c_rvec, &c_tvec, &c_cameraMatrix, &c_distCoeffs,
		&c_imagePoints, pdpdrot, pdpdt, pdpdf, pdpdc, pdpddist, aspectRatio);
}

double mycv::stereoCalibrate(InputArrayOfArrays _objectPoints,
	InputArrayOfArrays _imagePoints1,
	InputArrayOfArrays _imagePoints2,
	InputOutputArray _cameraMatrix1, InputOutputArray _distCoeffs1,
	InputOutputArray _cameraMatrix2, InputOutputArray _distCoeffs2,
	Size imageSize, OutputArray _Rmat, OutputArray _Tmat,
	OutputArray _Emat, OutputArray _Fmat, int flags,
	TermCriteria criteria)
{
	int rtype = CV_64F;
	Mat cameraMatrix1 = _cameraMatrix1.getMat();
	Mat cameraMatrix2 = _cameraMatrix2.getMat();
	Mat distCoeffs1 = _distCoeffs1.getMat();
	Mat distCoeffs2 = _distCoeffs2.getMat();
	cameraMatrix1 = prepareCameraMatrix(cameraMatrix1, rtype);
	cameraMatrix2 = prepareCameraMatrix(cameraMatrix2, rtype);
	distCoeffs1 = prepareDistCoeffs(distCoeffs1, rtype);
	distCoeffs2 = prepareDistCoeffs(distCoeffs2, rtype);

	if (!(flags & CALIB_RATIONAL_MODEL) &&
		(!(flags & CALIB_THIN_PRISM_MODEL)) &&
		(!(flags & CALIB_TILTED_MODEL)))
	{
		distCoeffs1 = distCoeffs1.rows == 1 ? distCoeffs1.colRange(0, 5) : distCoeffs1.rowRange(0, 5);
		distCoeffs2 = distCoeffs2.rows == 1 ? distCoeffs2.colRange(0, 5) : distCoeffs2.rowRange(0, 5);
	}

	_Rmat.create(3, 3, rtype);
	_Tmat.create(3, 1, rtype);

	Mat objPt, imgPt, imgPt2, npoints;

	collectCalibrationData(_objectPoints, _imagePoints1, _imagePoints2,
		objPt, imgPt, &imgPt2, npoints);
	CvMat c_objPt = objPt, c_imgPt = imgPt, c_imgPt2 = imgPt2, c_npoints = npoints;
	CvMat c_cameraMatrix1 = cameraMatrix1, c_distCoeffs1 = distCoeffs1;
	CvMat c_cameraMatrix2 = cameraMatrix2, c_distCoeffs2 = distCoeffs2;
	CvMat c_matR = _Rmat.getMat(), c_matT = _Tmat.getMat(), c_matE, c_matF, *p_matE = 0, *p_matF = 0;

	if (_Emat.needed())
	{
		_Emat.create(3, 3, rtype);
		p_matE = &(c_matE = _Emat.getMat());
	}
	if (_Fmat.needed())
	{
		_Fmat.create(3, 3, rtype);
		p_matF = &(c_matF = _Fmat.getMat());
	}

	double err = mycvStereoCalibrate(&c_objPt, &c_imgPt, &c_imgPt2, &c_npoints, &c_cameraMatrix1,
		&c_distCoeffs1, &c_cameraMatrix2, &c_distCoeffs2, imageSize,
		&c_matR, &c_matT, p_matE, p_matF, flags, criteria);

	cameraMatrix1.copyTo(_cameraMatrix1);
	cameraMatrix2.copyTo(_cameraMatrix2);
	distCoeffs1.copyTo(_distCoeffs1);
	distCoeffs2.copyTo(_distCoeffs2);

	return err;
}

void mycv::computeCorrespondEpilines(InputArray _points, int whichImage,
	InputArray _Fmat, OutputArray _lines)
{

		double f[9];
	Mat tempF(3, 3, CV_64F, f);
	Mat points = _points.getMat(), F = _Fmat.getMat();

	if (!points.isContinuous())
		points = points.clone();

	int npoints = points.checkVector(2);
	if (npoints < 0)
	{
		npoints = points.checkVector(3);
		if (npoints < 0)
			CV_Error(Error::StsBadArg, "The input should be a 2D or 3D point set");
		Mat temp;
		cv::convertPointsFromHomogeneous(points, temp);
		points = temp;
	}
	int depth = points.depth();
	CV_Assert(depth == CV_32F || depth == CV_32S || depth == CV_64F);

	CV_Assert(F.size() == Size(3, 3));
	F.convertTo(tempF, CV_64F);
	if (whichImage == 2)
		transpose(tempF, tempF);

	int ltype = CV_MAKETYPE(MAX(depth, CV_32F), 3);
	_lines.create(npoints, 1, ltype);
	Mat lines = _lines.getMat();
	if (!lines.isContinuous())
	{
		_lines.release();
		_lines.create(npoints, 1, ltype);
		lines = _lines.getMat();
	}
	CV_Assert(lines.isContinuous());

	if (depth == CV_32S || depth == CV_32F)
	{
		const Point* ptsi = points.ptr<Point>();
		const Point2f* ptsf = points.ptr<Point2f>();
		Point3f* dstf = lines.ptr<Point3f>();
		for (int i = 0; i < npoints; i++)
		{
			Point2f pt = depth == CV_32F ? ptsf[i] : Point2f((float)ptsi[i].x, (float)ptsi[i].y);
			double a = f[0] * pt.x + f[1] * pt.y + f[2];
			double b = f[3] * pt.x + f[4] * pt.y + f[5];
			double c = f[6] * pt.x + f[7] * pt.y + f[8];
			double nu = a*a + b*b;
			nu = nu ? 1. / std::sqrt(nu) : 1.;
			a *= nu; b *= nu; c *= nu;
			dstf[i] = Point3f((float)a, (float)b, (float)c);
		}
	}
	else
	{
		const Point2d* ptsd = points.ptr<Point2d>();
		Point3d* dstd = lines.ptr<Point3d>();
		for (int i = 0; i < npoints; i++)
		{
			Point2d pt = ptsd[i];
			double a = f[0] * pt.x + f[1] * pt.y + f[2];
			double b = f[3] * pt.x + f[4] * pt.y + f[5];
			double c = f[6] * pt.x + f[7] * pt.y + f[8];
			double nu = a*a + b*b;
			nu = nu ? 1. / std::sqrt(nu) : 1.;
			a *= nu; b *= nu; c *= nu;
			dstd[i] = Point3d(a, b, c);
		}
	}
}

void mycv::stereoRectify(InputArray _cameraMatrix1, InputArray _distCoeffs1,
	InputArray _cameraMatrix2, InputArray _distCoeffs2,
	Size imageSize, InputArray _Rmat, InputArray _Tmat,
	OutputArray _Rmat1, OutputArray _Rmat2,
	OutputArray _Pmat1, OutputArray _Pmat2,
	OutputArray _Qmat, int flags,
	double alpha, Size newImageSize,
	Rect* validPixROI1, Rect* validPixROI2)
{
	Mat cameraMatrix1 = _cameraMatrix1.getMat(), cameraMatrix2 = _cameraMatrix2.getMat();
	Mat distCoeffs1 = _distCoeffs1.getMat(), distCoeffs2 = _distCoeffs2.getMat();
	Mat Rmat = _Rmat.getMat(), Tmat = _Tmat.getMat();
	CvMat c_cameraMatrix1 = cameraMatrix1;
	CvMat c_cameraMatrix2 = cameraMatrix2;
	CvMat c_distCoeffs1 = distCoeffs1;
	CvMat c_distCoeffs2 = distCoeffs2;
	CvMat c_R = Rmat, c_T = Tmat;

	int rtype = CV_64F;
	_Rmat1.create(3, 3, rtype);
	_Rmat2.create(3, 3, rtype);
	_Pmat1.create(3, 4, rtype);
	_Pmat2.create(3, 4, rtype);
	CvMat c_R1 = _Rmat1.getMat(), c_R2 = _Rmat2.getMat(), c_P1 = _Pmat1.getMat(), c_P2 = _Pmat2.getMat();
	CvMat c_Q, *p_Q = 0;

	if (_Qmat.needed())
	{
		_Qmat.create(4, 4, rtype);
		p_Q = &(c_Q = _Qmat.getMat());
	}

	CvMat *p_distCoeffs1 = distCoeffs1.empty() ? NULL : &c_distCoeffs1;
	CvMat *p_distCoeffs2 = distCoeffs2.empty() ? NULL : &c_distCoeffs2;
	mycvStereoRectify(&c_cameraMatrix1, &c_cameraMatrix2, p_distCoeffs1, p_distCoeffs2,
		imageSize, &c_R, &c_T, &c_R1, &c_R2, &c_P1, &c_P2, p_Q, flags, alpha,
		newImageSize, (CvRect*)validPixROI1, (CvRect*)validPixROI2);
}

void mycv::triangulatePoints(InputArray _projMatr1, InputArray _projMatr2,
	InputArray _projPoints1, InputArray _projPoints2,
	OutputArray _points4D)
{

	Mat matr1 = _projMatr1.getMat(), matr2 = _projMatr2.getMat();
	Mat points1 = _projPoints1.getMat(), points2 = _projPoints2.getMat();

	if ((points1.rows == 1 || points1.cols == 1) && points1.channels() == 2)
		points1 = points1.reshape(1, static_cast<int>(points1.total())).t();

	if ((points2.rows == 1 || points2.cols == 1) && points2.channels() == 2)
		points2 = points2.reshape(1, static_cast<int>(points2.total())).t();

	CvMat cvMatr1 = matr1, cvMatr2 = matr2;
	CvMat cvPoints1 = points1, cvPoints2 = points2;

	_points4D.create(4, points1.cols, points1.type());
	CvMat cvPoints4D = _points4D.getMat();

	mycvTriangulatePoints(&cvMatr1, &cvMatr2, &cvPoints1, &cvPoints2, &cvPoints4D);
}
