#include "myHybridcam.h"

using namespace cv;

double mycv::hybridcam::stereoCalibrate(InputOutputArrayOfArrays objectPoints, InputOutputArrayOfArrays imagePoints1, InputOutputArrayOfArrays imagePoints2,
	const Size& imageSize1, const Size& imageSize2, InputOutputArray K1, InputOutputArray xi1, InputOutputArray D1, InputOutputArray K2, InputOutputArray xi2,
	InputOutputArray D2, OutputArray rvec, OutputArray tvec, OutputArrayOfArrays rvecsL, OutputArrayOfArrays tvecsL, int flags, TermCriteria criteria, OutputArray idx)
{
	CV_Assert(!objectPoints.empty() && (objectPoints.type() == CV_64FC3 || objectPoints.type() == CV_32FC3));
	CV_Assert(!imagePoints1.empty() && (imagePoints1.type() == CV_64FC2 || imagePoints1.type() == CV_32FC2));
	CV_Assert(!imagePoints2.empty() && (imagePoints2.type() == CV_64FC2 || imagePoints2.type() == CV_32FC2));

	CV_Assert(((flags & CALIB_USE_GUESS) && !K1.empty() && !D1.empty() && !K2.empty() && !D2.empty()) || !(flags & CALIB_USE_GUESS));

	int depth = objectPoints.depth();

	std::vector<Mat> _objectPoints, _imagePoints1, _imagePoints2,
		_objectPointsFilt, _imagePoints1Filt, _imagePoints2Filt;
	for (int i = 0; i < (int)objectPoints.total(); ++i)
	{
		_objectPoints.push_back(objectPoints.getMat(i));
		_imagePoints1.push_back(imagePoints1.getMat(i));
		_imagePoints2.push_back(imagePoints2.getMat(i));
		if (depth == CV_32F)
		{
			_objectPoints[i].convertTo(_objectPoints[i], CV_64FC3);
			_imagePoints1[i].convertTo(_imagePoints1[i], CV_64FC2);
			_imagePoints2[i].convertTo(_imagePoints2[i], CV_64FC2);
		}
	}

	Matx33d _K1, _K2;
	Matx14d _D1, _D2;

	double _xi1, _xi2;

	std::vector<Vec3d> _omL, _TL;
	Vec3d _om, _T;

	// initializaition
	Mat _idx;
	mycv::hybridcam::internal::initializeStereoCalibration(_objectPoints, _imagePoints1, _imagePoints2, imageSize1, imageSize2, _om, _T, _omL, _TL, _K1, _D1, _K2, _D2, _xi1, _xi2, flags, _idx);
	if (idx.needed())
	{
		idx.create(1, (int)_idx.total(), CV_32S);
		_idx.copyTo(idx.getMat());
	}
	for (int i = 0; i < (int)_idx.total(); ++i)
	{
		_objectPointsFilt.push_back(_objectPoints[_idx.at<int>(i)]);
		_imagePoints1Filt.push_back(_imagePoints1[_idx.at<int>(i)]);
		_imagePoints2Filt.push_back(_imagePoints2[_idx.at<int>(i)]);
	}

}

void mycv::hybridcam::internal::initializeStereoCalibration(InputArrayOfArrays objectPoints, InputArrayOfArrays imagePoints1, InputArrayOfArrays imagePoints2,
	const Size& size1, const Size& size2, OutputArray om, OutputArray T, OutputArrayOfArrays omL, OutputArrayOfArrays tL, OutputArray K1, OutputArray D1, OutputArray K2, OutputArray D2,
	double &xi1, double &xi2, int flags, OutputArray idx)
{
	Mat idx1, idx2;
	Matx33d _K1, _K2;
	Matx14d _D1, _D2;
	Mat _xi1m, _xi2m;

	std::vector<Vec3d> omAllTemp1, omAllTemp2, tAllTemp1, tAllTemp2;

	omnidir::calibrate(objectPoints, imagePoints1, size1, _K1, _xi1m, _D1, omAllTemp1, tAllTemp1, flags, TermCriteria(3, 100, 1e-6), idx1);

	/*
	omnidir::calibrate(objectPoints, imagePoints2, size2, _K2, _xi2m, _D2, omAllTemp2, tAllTemp2, flags, TermCriteria(3, 100, 1e-6), idx2);

	// find the intersection idx
	Mat interIdx1, interIdx2, interOri;

	getInterset(idx1, idx2, interIdx1, interIdx2, interOri);
	if (idx.empty())
		idx.create(1, (int)interOri.total(), CV_32S);
	interOri.copyTo(idx.getMat());

	int n_inter = (int)interIdx1.total();

	std::vector<Vec3d> omAll1(n_inter), omAll2(n_inter), tAll1(n_inter), tAll2(n_inter);
	for (int i = 0; i < (int)interIdx1.total(); ++i)
	{
		omAll1[i] = omAllTemp1[interIdx1.at<int>(i)];
		tAll1[i] = tAllTemp1[interIdx1.at<int>(i)];
		omAll2[i] = omAllTemp2[interIdx2.at<int>(i)];
		tAll2[i] = tAllTemp2[interIdx2.at<int>(i)];
	}

	// initialize R,T
	Mat omEstAll(1, n_inter, CV_64FC3), tEstAll(1, n_inter, CV_64FC3);
	Mat R1, R2, T1, T2, omLR, TLR, RLR;
	for (int i = 0; i < n_inter; ++i)
	{
		cv::Rodrigues(omAll1[i], R1);
		cv::Rodrigues(omAll2[i], R2);
		T1 = Mat(tAll1[i]).reshape(1, 3);
		T2 = Mat(tAll2[i]).reshape(1, 3);
		RLR = R2 * R1.t();
		TLR = T2 - RLR*T1;
		cv::Rodrigues(RLR, omLR);
		omLR.reshape(3, 1).copyTo(omEstAll.col(i));
		TLR.reshape(3, 1).copyTo(tEstAll.col(i));
	}
	Vec3d omEst = internal::findMedian3(omEstAll);
	Vec3d tEst = internal::findMedian3(tEstAll);

	Mat(omEst).copyTo(om.getMat());
	Mat(tEst).copyTo(T.getMat());

	if (omL.empty())
	{
		omL.create((int)omAll1.size(), 1, CV_64FC3);
	}
	if (tL.empty())
	{
		tL.create((int)tAll1.size(), 1, CV_64FC3);
	}

	if (omL.kind() == _InputArray::STD_VECTOR_MAT)
	{
		for (int i = 0; i < n_inter; ++i)
		{
			omL.create(3, 1, CV_64F, i, true);
			tL.create(3, 1, CV_64F, i, true);
			omL.getMat(i) = Mat(omAll1[i]);
			tL.getMat(i) = Mat(tAll1[i]);
		}
	}
	else
	{
		cv::Mat(omAll1).convertTo(omL, CV_64FC3);
		cv::Mat(tAll1).convertTo(tL, CV_64FC3);
	}
	if (K1.empty())
	{
		K1.create(3, 3, CV_64F);
		K2.create(3, 3, CV_64F);
	}
	if (D1.empty())
	{
		D1.create(1, 4, CV_64F);
		D2.create(1, 4, CV_64F);
	}
	Mat(_K1).copyTo(K1.getMat());
	Mat(_K2).copyTo(K2.getMat());

	Mat(_D1).copyTo(D1.getMat());
	Mat(_D2).copyTo(D2.getMat());

	xi1 = _xi1m.at<double>(0);
	xi2 = _xi2m.at<double>(0);
	*/
}
