#ifndef _MATCHER_OPTION_H_
#define _MATCHER_OPTION_H_

//#include <cstdio>
//#include <iostream>
#include <vector>
#include "FAST9.h"

namespace BORA{
	//option
	enum HomographyType {
		H_KIST_PROSAC = 0,
		H_OPENCV_RANSAC = 1
	};

	class MatchOption{
	public:
		MatchOption()
			:dissimilar_score(2),
			stop_score(0),
			threshold(50),
			remove_outlier(true),
			homographyType(H_KIST_PROSAC),
			one_point_mode(false),
			point(0,0)
		{}

		int dissimilar_score;          //score ����
		int stop_score;                //stop_score �����̸� �� ������ �ʰ� �ߴ�, 0�̸� ������ ��
		int threshold;                 //threshold
		bool remove_outlier;           //outlier ���� ����
		std::vector<Position> edge_points;        //outlier ���� �� ��ȯ �� ��ǥ
		HomographyType homographyType; //ȣ��׶��� �˰��� ����
		bool one_point_mode;
		Position point;
	};
}

#endif