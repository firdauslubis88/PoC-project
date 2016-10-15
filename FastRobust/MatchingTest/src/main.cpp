#include "FastRobustMatching.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

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
#undef TARGET_IMAGE_RESIZE

int main(int argc, char **argv)
{
	IplImage *m_target_image = cvLoadImage(TARGET_IMAGE_FILE);
	if(!m_target_image)
	{
		printf("Failed to load a image file: %s", TARGET_IMAGE_FILE);
		return false;
	}

#ifdef TARGET_IMAGE_RESIZE
	unsigned int target_width  = ceil(m_target_image->width  * TARGET_IMAGE_RESIZE);
	unsigned int target_height = ceil(m_target_image->height * TARGET_IMAGE_RESIZE);

	IplImage *m_resize_target_image = cvCreateImage(cvSize(target_width, target_height),
		m_target_image->depth, m_target_image->nChannels);

	cvResize(m_target_image, m_resize_target_image, CV_INTER_CUBIC);
	cvReleaseImage( &m_target_image );
	m_target_image = m_resize_target_image;
	m_resize_target_image = NULL;
#endif


	unsigned int input_width  = INPUT_WIDTH;
	unsigned int input_height = INPUT_HEIGHT;

	IplImage *m_input_image = NULL;

#ifdef INPUT_IMAGE_FILE
	m_input_image = cvLoadImage(INPUT_IMAGE_FILE);
	if(!m_input_image)
	{
		printf("Failed to load a image file: %s", INPUT_IMAGE_FILE);
		return FALSE;
	}
	input_width  = m_input_image->width;
	input_height = m_input_image->height;
#endif

	IplImage *m_show_image = cvCreateImage(cvSize(m_target_image->width + input_width,
		max(m_target_image->height, (int)input_height)), IPL_DEPTH_8U, 3);
	
	unsigned int window_width  = m_show_image->width;
	unsigned int window_height = m_show_image->height;

	int target_width_offset = input_width;
	int  input_width_offset = 0;

	int target_height_offset = ( max(m_target_image->height, (int)input_height) - m_target_image->height ) / 2;
	int  input_height_offset = ( max(m_target_image->height, (int)input_height) - input_height ) / 2;

	// Load feature data file
	if( !FastRobustMatching::initialize(FEATURE_FILE, input_width, input_height) )
	{
		return false;
	}

	CvCapture* capture = cvCaptureFromCAM(0);
#ifndef INPUT_IMAGE_FILE
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, input_width);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, input_height);
#endif


	if( capture )
	{
		cvNamedWindow("FastRobust - TEST");
		cvResizeWindow( "FastRobust - TEST", window_width, window_height );

		while(1)
		{

			if( !cvGrabFrame( capture ))
				break;

#ifndef INPUT_IMAGE_FILE
			m_input_image = cvRetrieveFrame( capture );
#endif
			// Set 'm_show_image'
			cvSetZero(m_show_image);			
			cvSetImageROI(m_show_image, cvRect(input_width, target_height_offset, m_target_image->width, m_target_image->height));
			cvCopy(m_target_image, m_show_image, NULL);

			cvSetImageROI(m_show_image, cvRect(0, input_height_offset, input_width, input_height));
			cvCopy(m_input_image, m_show_image, NULL);
			cvResetImageROI(m_show_image);
			IplImage* testImage = cvCreateImage(cvGetSize(m_target_image), 8, 1);
/*
			printf("%d\n", m_target_image->height);
			printf("%d\n", m_target_image->width);
			printf("%s\n", m_target_image->channelSeq);
			
			cvCvtColor(m_target_image, testImage, CV_BGR2GRAY);
*/

			bool isMatched = FastRobustMatching::matching(m_input_image);


			FastRobustMatching::MatchPair pairs;
			FastRobustMatching::get_matched_inlier_pairs(pairs);
			for(FastRobustMatching::MatchPair::iterator it = pairs.begin(); it != pairs.end(); ++it)
			{
				int target_pos_x = it->first.x;
				int target_pos_y = it->first.y;

				int input_pos_x = it->second.x;
				int input_pos_y = it->second.y;

#ifdef TARGET_IMAGE_RESIZE
				target_pos_x = ceil(target_pos_x * TARGET_IMAGE_RESIZE);
				target_pos_y = ceil(target_pos_y * TARGET_IMAGE_RESIZE);
#endif

				cvLine(m_show_image,
					cvPoint( input_pos_x +  input_width_offset,  input_pos_y +  input_height_offset),	// Input
					cvPoint(target_pos_x + target_width_offset, target_pos_y + target_height_offset),	// Target	
					cvScalar(255, 255, 0));
			}
			cvShowImage("FastRobust - TEST", m_show_image);

			if( cvWaitKey(10) >= 0 )
				break;
		}

		cvReleaseCapture( &capture );
//		cvDestroyWindow( "T9-camera" );
	}

	if(m_input_image) cvReleaseImage(&m_input_image);
	cvReleaseImage(&m_target_image);
	cvReleaseImage(&m_show_image);

	return true;
}