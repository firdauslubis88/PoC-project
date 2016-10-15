/*******************************************************************
 FAST9 Corner Detect Ŭ����

 MImage�κ��� Corner�� �����ؼ� �����ִ� Ŭ����
*******************************************************************/
#pragma once
#include "Image.h"
#include "GeneralDataType.h"
namespace BORA
{
	class FAST9
	{
	public:
		/* MImage�κ��� Corners ��ǥ�� �����ִ� �Լ� */
		static void		FindCorners(const BORA::Image &_src, const unsigned int _threshold, BORA::POSITIONS &_corners);

		/* IplImage�� ���� Corners ��ǥ�� �����ִ� �Լ� */
		static void		FindCornersIpl(const IplImage *_src, const unsigned int &_threshold, BORA::POSITIONS &_corners);
		
		/* MImage�� ���ϴ� Ư¡������ �ִ� Threshold�� ��ȯ�ϴ� �Լ� */
		static unsigned int	FindThresholdIpl(const BORA::Image &_src, const unsigned int _targetFeatureNumber);

		

	private:
		inline static bool Compare(const int x, const int y)
		{
			return x >= y;
		}


		/* ������ FAST9 Corner Detect �Լ� */
		static void make_offsets(int pixel[], int row_stride);
		static int  fast9_corner_score(const BORA::BYTE* p, const int pixel[], int bstart);
		static void fast9_detect(const BORA::BYTE* im, int xsize, int ysize, int stride, int b, BORA::POSITIONS &ret_corners);
		static void fast9_score(const BORA::BYTE* i, int stride, int num_corners, int b, BORA::POSITIONS &corners, std::vector<int> &scores);
		static void fast9_nonmax_suppression(BORA::POSITIONS &corners, std::vector<int> &scores, int num_corners, BORA::POSITIONS &ret_nonmax);
	};
}

