#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<iostream>

using namespace std;

int main() {
	//Data Structure to store cam.
	CvCapture* cap = cvCaptureFromCAM(0);
	//Image variable to store frame
	IplImage* frame;
	//Window to show livefeed
	cvNamedWindow("LiveFeed", CV_WINDOW_AUTOSIZE);
	if (!cap)
	{
		cout << "ERROR: Capture is null!\n";
	}
	while (1)
	{
		//Load the next frame
		frame = cvQueryFrame(cap);
		//If frame is not loaded break from the loop
		if (!frame)
			break;
		//Show the present frame
		cvShowImage("LiveFeed", frame);
		//Escape Sequence
		char c = cvWaitKey(33);
		//If the key pressed by user is Esc(ASCII is 27) then break out of the loop
		if (c == 27)
			break;
	}
	//CleanUp
	cvReleaseCapture(&cap);
	cvDestroyAllWindows();

}

/*
#include "FastRobustMatching.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include <algorithm>

#define TARGET_IMAGE_FILE		"../data/toy_story_3.jpg"
#define FEATURE_FILE			"../data/toy_story_3.dat"
#define TARGET_IMAGE_RESIZE		1

//#define TARGET_IMAGE_FILE		"../Bin/data/kyungwoo_test/kyungwoo_panorama_small.bmp"
//#define FEATURE_FILE			"../Bin/data/kyungwoo_test/kyungwoo_panorama_small.dat"

//#define TARGET_IMAGE_FILE		"../Bin/data/kyungwoo_test/kyungwoo_panorama_bin_009648.bmp"
//#define FEATURE_FILE			"../Bin/data/kyungwoo_test/kyungwoo_panorama_bin_009648.dat"

//#define INPUT_IMAGE_FILE		"../Bin/data/kyungwoo_panorama/bin/kyungwoo_panorama_bin_005012.bmp"
//#define INPUT_IMAGE_FILE		"../Bin/data/kyungwoo_test/kyungwoo_panorama_small.bmp"


#define INPUT_WIDTH		640
#define INPUT_HEIGHT	480

IplImage* m_input_image;
IplImage* m_show_image;
IplImage* m_real_image;
int input_width = 640;
int input_height = 480;
bool indicator = false;
int main(int argc, char **argv)
{
	CvCapture* capture = cvCaptureFromCAM(0);

	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, input_width);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, input_height);

	m_real_image = cvLoadImage("toy_story_3.jpg");
	if (capture)
	{
		cvNamedWindow("FastRobust - TEST");
//		cvResizeWindow("FastRobust - TEST", window_width, window_height);

		while (1)
		{
			if (!cvGrabFrame(capture))
				break;

			m_input_image = cvQueryFrame(capture);
			// Set 'm_show_image'
			if (!indicator)
			{
				cvSaveImage("test.jpg",m_input_image);
				indicator = true;
			}
//			cvSetImageROI(m_show_image, cvRect(0, 0, input_width, input_height));
			m_show_image = cvCreateImage(CvSize(input_width, input_height),m_input_image->depth,m_input_image->nChannels);
			cvCopy(m_input_image, m_show_image, NULL);
			cvResetImageROI(m_show_image);
//			printf("RETRIEVED!!!!\n");
			cvShowImage("FastRobust - TEST", m_show_image);

			if (cvWaitKey(10) >= 0)
				break;
		}

		cvReleaseCapture(&capture);
		cvDestroyWindow("FastRobust - TEST");
	}

	if (m_input_image) cvReleaseImage(&m_input_image);
	cvReleaseImage(&m_show_image);

	return true;
}
*/
/*
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int, char**)
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	Mat edges;
	namedWindow("edges", 1);
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		cvtColor(frame, edges, COLOR_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		imshow("edges", edges);
		if (waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}
*/