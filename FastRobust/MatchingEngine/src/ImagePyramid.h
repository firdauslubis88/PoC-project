/*
 *  �̹��� �Ƕ�̵�
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
		void setImage(IplImage *_data);             //�Է¹��� �̹����� �Ƕ�̵� ����
		void setImage(Image &_data);
		unsigned int getDescriptor(vector<MatchDescriptor> &dst, MatchOption &opt);     //���� �Ƕ�̵忡�� ��ũ���� ���
	public:
		void inline init(int _width, int _height);
		void inline clear();
		IplImage *data[IMAGE_PYRAMID_DEPTH];
	};
}