#include "Alignment.h"

bool Alignment::alreadyCreated = false;
bool Alignment::alreadyChanged = false;
Ptr<SURF> Alignment::detector = Ptr<SURF>();
int Alignment::minHessian = 400;
float Alignment::comparisonThreshold = 0.7;
Mat Alignment::hBig = Mat();

Alignment::Alignment()
{
	comparisonThreshold = 0.7;
}

Alignment::Alignment(int minHessian)
{
	comparisonThreshold = 0.7;
}

Mat Alignment::align(Mat refImage, Mat inputImage, int x, int y, int mask_width, int mask_height)
{
	Mat outputImage;
	if (!Alignment::alreadyChanged)
	{
		if (!alreadyCreated)
		{
			Alignment::detector = SURF::create(Alignment::minHessian);
			alreadyCreated = true;
		}

		cout << "TRYING!" << endl;
		std::vector<KeyPoint> keypoints_ref, keypoints_input;
		Mat descriptors_ref, descriptors_input;
		Rect ROIRef = Rect(x, y, mask_width, mask_height);
		Mat ROIRefMask = Mat(inputImage.size(), CV_8UC1, Scalar::all(0));
		ROIRefMask(ROIRef).setTo(Scalar::all(255));

		detector->detectAndCompute(refImage, ROIRefMask, keypoints_ref, descriptors_ref);
		detector->detectAndCompute(inputImage, Mat(), keypoints_input, descriptors_input);
		Mat temp_img_kp;
		drawKeypoints(refImage, keypoints_ref, temp_img_kp, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

		FlannBasedMatcher matcher;
		vector<vector<DMatch>> kmatches;
		vector<DMatch> good_matches;

		matcher.knnMatch(descriptors_ref, descriptors_input, kmatches, 2);
		std::sort(kmatches.begin(), kmatches.end());
		int lenghtKMatches = kmatches.size();
		for (int i = 0; i < lenghtKMatches; i++)
		{
			double dist1 = kmatches[i][0].distance;
			double dist2 = kmatches[i][1].distance;
			double comp = dist1 / dist2;
			//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,  //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very  //-- small)  //-- PS.- radiusMatch can also be used here.  
			if (comp < comparisonThreshold)
			{
				good_matches.push_back(kmatches[i][0]);
			}
		}

		vector<Point2f> ptsTemp, ptsTemp2;
		for (int i = 0; i < good_matches.size(); i++)
		{
			Point3f pnt;
			//-- Get the keypoints from the good matches
			ptsTemp.push_back(keypoints_ref[good_matches[i].queryIdx].pt);
			ptsTemp2.push_back(keypoints_input[good_matches[i].trainIdx].pt);
		}
		vector<Point2f> ptsROI, ptsROI2;
		ptsROI = ptsTemp;
		ptsROI2 = ptsTemp2;

		Mat h;
		if (ptsROI.size() >= 3)
		{
			cout << ptsROI.size() << endl;
			vector<Mat> ptsxy(2), pts2xy(2);
			Rect ROI, ROI2;
			double smallestX, largestX, smallestY, largestY;
			h = estimateRigidTransform(ptsROI2, ptsROI, false);
			if (!h.empty())
			{
				h.copyTo(Alignment::hBig);
				Alignment::alreadyChanged = true;
				cout << "DONE!" << endl;
			}
		}
	}
	if (!Alignment::hBig.empty())
	{
		warpAffine(inputImage, outputImage, Alignment::hBig, Size(refImage.cols, refImage.rows));
	}
	else
	{
		outputImage = inputImage;
	}
//	cout << Alignment::hBig << endl;
	return outputImage;
}

Alignment::~Alignment()
{
}
