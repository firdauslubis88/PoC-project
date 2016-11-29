#include "CombinedCamera.h"
#include "Cloning.h"



CombinedCamera::CombinedCamera(int image_width,int image_height)
{
	ldCvImage.allocate(image_width, image_height);
	hdCvImage.allocate(image_width, image_height);
	combinedCvImage.allocate(image_width, image_height);
}


CombinedCamera::~CombinedCamera()
{
}
/*
void CombinedCamera::seamlessClone(InputArray _src, InputArray _dst, InputArray _mask, Point p, OutputArray _blend, int flags)
{
	const Mat src = _src.getMat();
	const Mat dest = _dst.getMat();
	const Mat mask = _mask.getMat();
	_blend.create(dest.size(), CV_8UC3);
	Mat blend = _blend.getMat();

	int minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
	int h = mask.size().height;
	int w = mask.size().width;

	Mat gray = Mat(mask.size(), CV_8UC1);
	Mat dst_mask = Mat::zeros(dest.size(), CV_8UC1);
	Mat cs_mask = Mat::zeros(src.size(), CV_8UC3);
	Mat cd_mask = Mat::zeros(dest.size(), CV_8UC3);

	if (mask.channels() == 3)
		cvtColor(mask, gray, COLOR_BGR2GRAY);
	else
		gray = mask;

	for (int i = 0; i<h; i++)
	{
		for (int j = 0; j<w; j++)
		{
			if (gray.at<uchar>(i, j) == 255)
			{
				minx = std::min(minx, i);
				maxx = std::max(maxx, i);
				miny = std::min(miny, j);
				maxy = std::max(maxy, j);
			}
		}
	}

	int lenx = maxx - minx;
	int leny = maxy - miny;

	Mat patch = Mat::zeros(Size(leny, lenx), CV_8UC3);

	int minxd = p.y - lenx / 2;
	int maxxd = p.y + lenx / 2;
	int minyd = p.x - leny / 2;
	int maxyd = p.x + leny / 2;

	CV_Assert(minxd >= 0 && minyd >= 0 && maxxd <= dest.rows && maxyd <= dest.cols);

	Rect roi_d(minyd, minxd, leny, lenx);
	Rect roi_s(miny, minx, leny, lenx);

	Mat destinationROI = dst_mask(roi_d);
	Mat sourceROI = cs_mask(roi_s);

	gray(roi_s).copyTo(destinationROI);
	src(roi_s).copyTo(sourceROI, gray(roi_s));
	src(roi_s).copyTo(patch, gray(roi_s));

	destinationROI = cd_mask(roi_d);
	cs_mask(roi_s).copyTo(destinationROI);


	Cloning obj;
	obj.normalClone(dest, cd_mask, dst_mask, blend, flags);

}
*/
ofPixels CombinedCamera::combine(ofPixels ldPixel, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height)
{
	ofImage ldImage;

	ldImage.setFromPixels(ldPixel);
	ldImage.setImageType(OF_IMAGE_COLOR);

	ldCvImage.setFromPixels(ldImage.getPixels());
	hdCvImage.setFromPixels(hdImage.getPixels());

	if (ldCvImage.getCvImage() != NULL)
	{
		Mat tempMatHdCvImage = cvarrToMat(hdCvImage.getCvImage());
		Mat tempMatLdCvImage = cvarrToMat(ldCvImage.getCvImage());

		Mat source, target, mask, clone;
		Point cloneCenter;
		tempMatHdCvImage(Rect(x, y, width, height)).copyTo(source);
		target = tempMatLdCvImage;
		mask = Mat(source.rows, source.cols, CV_8UC1);
		mask.setTo(Scalar(255));
		cloneCenter = Point(x + width / 2, y + height / 2);

		seamlessClone(source, target, mask, cloneCenter, clone, 1);
		IplImage temp = clone;
		IplImage* pTemp = &temp;
		combinedCvImage = pTemp;
	}
	else
	{
		combinedCvImage = ldCvImage;
		combinedCvImage.setROI(x, y, width, height);
		hdCvImage.setROI(x, y, width, height);
		ofPixels hdROIPixels = hdCvImage.getRoiPixels();
		combinedCvImage.setRoiFromPixels(hdROIPixels.getData(), hdROIPixels.getWidth(), hdROIPixels.getHeight());
	}

	return combinedCvImage.getPixels();
}
