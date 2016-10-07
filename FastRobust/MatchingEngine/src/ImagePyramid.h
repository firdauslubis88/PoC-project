/*
 *  이미지 피라미드
 */
#define IMAGE_PYRAMID_DEPTH 1
#include <vector>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include "GeneralDataType.h"
#include "MatchDescriptor.h"
#include "MatchOption.h"

using namespace std;

namespace BORA{
	class ImagePyramid{
	public:
		ImagePyramid();
		ImagePyramid(int _width, int _height);
		~ImagePyramid();
		void setImage(IplImage *_data);             //입력받은 이미지로 피라미드 생성
		void setImage(Image &_data);
		unsigned int getDescriptor(vector<MatchDescriptor> &dst, MatchOption &opt);     //현재 피라미드에서 디스크립터 얻기
	public:
		void inline init(int _width, int _height);
		void inline clear();
		IplImage *data[IMAGE_PYRAMID_DEPTH];
	};
}