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

		int dissimilar_score;          //score 제한
		int stop_score;                //stop_score 이하이면 더 비교하지 않고 중단, 0이면 끝까지 비교
		int threshold;                 //threshold
		bool remove_outlier;           //outlier 제거 여부
		std::vector<Position> edge_points;        //outlier 제거 후 변환 할 좌표
		HomographyType homographyType; //호모그라피 알고리즘 종류
		bool one_point_mode;
		Position point;
	};
}

#endif