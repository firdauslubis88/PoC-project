/*******************************************************************
 FAST9 Corner Detect 클래스

 MImage로부터 Corner를 검출해서 돌려주는 클래스
*******************************************************************/
#pragma once
#include "Image.h"
#include "GeneralDataType.h"
namespace BORA
{
	class FAST9
	{
	public:
		/* MImage로부터 Corners 좌표를 돌려주는 함수 */
		static void		FindCorners(const BORA::Image &_src, const unsigned int _threshold, BORA::POSITIONS &_corners);

		/* IplImage로 부터 Corners 좌표를 돌려주는 함수 */
		static void		FindCornersIpl(const IplImage *_src, const unsigned int &_threshold, BORA::POSITIONS &_corners);
		
		/* MImage와 원하는 특징점수를 주는 Threshold를 반환하는 함수 */
		static unsigned int	FindThresholdIpl(const BORA::Image &_src, const unsigned int _targetFeatureNumber);

		

	private:
		inline static bool Compare(const int x, const int y)
		{
			return x >= y;
		}


		/* 기존의 FAST9 Corner Detect 함수 */
		static void make_offsets(int pixel[], int row_stride);
		static int  fast9_corner_score(const BORA::BYTE* p, const int pixel[], int bstart);
		static void fast9_detect(const BORA::BYTE* im, int xsize, int ysize, int stride, int b, BORA::POSITIONS &ret_corners);
		static void fast9_score(const BORA::BYTE* i, int stride, int num_corners, int b, BORA::POSITIONS &corners, std::vector<int> &scores);
		static void fast9_nonmax_suppression(BORA::POSITIONS &corners, std::vector<int> &scores, int num_corners, BORA::POSITIONS &ret_nonmax);
	};
}

