#include "StdAfx.h"
#include "ImageUtility.h"

/* _src_img의 _src_pos좌표로 부터 패치와 인덱스를 돌려주는 함수 */
void BORA::ImageUtility::GetPatchAndIndex( const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_patch, BORA::INDEX &_dst_index )
{
	// 초기화
	_dst_patch.Clear();
	_dst_index = 0;

	// 패치 수집
	const IplImage *src_img = _src_img.getIplImage();

	unsigned char center = (src_img->imageData[_src_pos.y * src_img->widthStep + _src_pos.x]);
	//unsigned int			center = static_cast<unsigned int>(src_img->imageData[_src_pos.y * src_img->widthStep + _src_pos.x]);
	BORA::PATCH		crnt_patch;
	//crnt_patch.fill(0);
	crnt_patch.assign(0);

	//std::cout << ( _src_img.isGrayImage() ? "gray image" : "color image" ) << std::endl;

	int patch_idx(-1);
	for(int y = -7 ; y <= 7 ; y += 2)
		for(int x = -7 ; x <= 7 ; x += 2)
		{
			crnt_patch[++patch_idx] = static_cast<unsigned int>(static_cast<BORA::BYTE>(src_img->imageData[(_src_pos.y + y) * src_img->widthStep + (_src_pos.x + x)]));
		}

	_dst_patch.setPATCH(crnt_patch);

	// 인덱스 계산
	CalcIndex(crnt_patch, center, _dst_index);
}
void BORA::ImageUtility::GetPatchAndIndexAndOrientation(const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_patch, BORA::INDEX &_dst_index, double &_orientation)
{
	BORA::Position shift_pos[16] = 
	{
		BORA::Position(3, 0),
		BORA::Position(-3, 0),
		
		BORA::Position(3, -1),
		BORA::Position(-3, 1),

		BORA::Position(2, -2),
		BORA::Position(-2, 2),

		BORA::Position(1, -3),
		BORA::Position(-1, 3),

		BORA::Position(0, -3),
		BORA::Position(0, 3),

		BORA::Position(-1, -3),
		BORA::Position(1, 3),

		BORA::Position(-2, -2),
		BORA::Position(2, 2),

		BORA::Position(-3, -1),
		BORA::Position(3, 1)
	};
	IplImage *src_img = cvCloneImage(_src_img.getIplImage());

	// 각도를 구하기 위해서 벡터를 사용해야 한다.
	// 실제 해당 방향으로 단위벡터를 사용했다.
	typedef std::pair<double, double> VEC;
	VEC vectors[8];

	vectors[0].first = 1.000000;
	vectors[0].second = 0.000000;

	vectors[1].first = 0.948683;
	vectors[1].second = 0.316228;

	vectors[2].first = 0.707107;
	vectors[2].second = 0.707107;

	vectors[3].first = 0.316228;
	vectors[3].second = 0.948683;

	vectors[4].first = 0.000000;
	vectors[4].second = 1.000000;

	vectors[5].first = -0.316228;
	vectors[5].second = 0.948683;

	vectors[6].first = -0.707107;
	vectors[6].second = 0.707107;

	vectors[7].first = -0.948683;
	vectors[7].second = 0.316228;

	// 서로 마주보는 픽셀간 intensity 차를 각 벡터에 곱한 합을 구한다.
	VEC vector(0, 0);
	for(unsigned int i = 0 ; i < 8 ; ++i)
	{
		BORA::Position posi			= _src_pos + shift_pos[2*i + 0];
		BORA::Position opposite_pos	= _src_pos + shift_pos[2*i + 1];

		char gap = static_cast<char>(src_img->imageData[(posi.y) * src_img->widthStep + (posi.x)])
					- static_cast<char>(src_img->imageData[(opposite_pos.y) * src_img->widthStep + (opposite_pos.x)]);

		vector.first += vectors[i].first *gap;
		vector.second += vectors[i].second *gap;
	}

	// vector가 최종적인 벡터
	double vector_scalar = sqrt(vector.first* vector.first + vector.second * vector.second);
	double vector_dot_axis_x_scalar = sqrt(vector.first *vector.first);

	// cos(angle) = |v dot x| / |v|
	// therefore, angle = acos( |v dot x| / |v| )
	_orientation = acos(vector_dot_axis_x_scalar/vector_scalar);

	// 이미지를 _orientation 만큼 회전시켜서 해당 부분의 patch를 뜯어 온다.
	// 구현은 잠시......_ _ㅋㅋㅋㅋㅋ 제길



	cvReleaseImage(&src_img);
}
void BORA::ImageUtility::GetQPatchAndIndex(const BORA::Image &_src_img, const BORA::Position &_src_pos, BORA::Patch &_dst_qpatch, BORA::INDEX &_dst_index)
{
	// 초기화
	_dst_qpatch.Clear();
	_dst_index = 0;

	// 패치 수집
	const IplImage *src_img = _src_img.getIplImage();

	unsigned int		center = static_cast<unsigned int>(src_img->imageData[_src_pos.y * src_img->widthStep + _src_pos.x]);
	BORA::PATCH & dst_qpatch = _dst_qpatch.getPATCH_ref();
	int patch_idx(-1);
	for(int y = -7 ; y <= 7 ; y += 2)
		for(int x = -7 ; x <= 7 ; x += 2)
			dst_qpatch[++patch_idx] = static_cast<unsigned int>(static_cast<BORA::BYTE>(src_img->imageData[(_src_pos.y + y) * src_img->widthStep + (_src_pos.x + x)]));

	// 인덱스 계산
	CalcIndex(dst_qpatch, center, _dst_index);

	// 퀀타이제이션
	Quantization_self(_dst_qpatch);
}

/* 패치 데이터와 가운데 픽셀을 이용해서 인덱스를 구한다. */
void BORA::ImageUtility::CalcIndex( const BORA::PATCH &_patch, const unsigned int &_center_intensity, BORA::INDEX &_dst_index )
{
	const unsigned int &center = _center_intensity;
	// 인덱스 추출
	// patch의 mean을 구한다
	double mean_of_the_patch(0);
	for(unsigned int i = 0 ; i < 64 ; ++i)
		mean_of_the_patch += static_cast<double>(_patch[i]);
	mean_of_the_patch /= 64.0;

	// mean보다 크면 1로 세팅, 아니면 0으로 세팅
	if(_patch[9]  > mean_of_the_patch) _dst_index |= 0x1000;
	if(_patch[14] > mean_of_the_patch) _dst_index |= 0x0800;
	if(_patch[19] > mean_of_the_patch) _dst_index |= 0x0400;
	if(_patch[20] > mean_of_the_patch) _dst_index |= 0x0200;
	if(_patch[26] > mean_of_the_patch) _dst_index |= 0x0100;
	if(_patch[29] > mean_of_the_patch) _dst_index |= 0x0080;
	if(center     > mean_of_the_patch) _dst_index |= 0x0040;
	if(_patch[34] > mean_of_the_patch) _dst_index |= 0x0020;
	if(_patch[37] > mean_of_the_patch) _dst_index |= 0x0010;
	if(_patch[43] > mean_of_the_patch) _dst_index |= 0x0008;
	if(_patch[44] > mean_of_the_patch) _dst_index |= 0x0004;
	if(_patch[49] > mean_of_the_patch) _dst_index |= 0x0002;
	if(_patch[54] > mean_of_the_patch) _dst_index |= 0x0001;
}


/* _src_patch로 부터 0 ~ 4 레벨로 퀀타이제이션 시킨 패치를 돌려주는 함수 */
void BORA::ImageUtility::Quantization( const BORA::Patch &_src_patch, BORA::Patch &_dst_patch )
{
	_dst_patch.Clear();
	BORA::PATCH &dst_patch = _dst_patch.getPATCH_ref();
	const BORA::PATCH &src_patch = _src_patch.getPATCH_ref();

	unsigned int scale = 5;
	/*
	double result[64];

	// 각 픽셀의 평균 계산
	double average = 0.0;
	for(unsigned int i = 0 ; i < 64 ; ++i)
		average += static_cast<double>(src_patch[i]);

	average /= 64.0;

	// 표준 편차 계산
	double std_deviation = 0.0;
	for(unsigned int i = 0 ; i < 64 ; ++i)
		std_deviation += (src_patch[i] - average)*(src_patch[i] - average);
	std_deviation /= 64.0;
	std_deviation = sqrt(std_deviation);
	
	for(unsigned int i = 0 ; i < 64 ; ++i)
		result[i] = (src_patch[i] - average)/std_deviation;
	*/

	double minValue = static_cast<double>(INT_MAX);
	double maxValue = static_cast<double>(INT_MIN);

	for(unsigned int i = 0 ; i < 64 ; ++i)
	{
		if(src_patch[i] < minValue)
			minValue = src_patch[i];
		
		if(src_patch[i] > maxValue)
			maxValue = src_patch[i];
	}

	maxValue -= minValue;

	for(unsigned int i = 0 ; i < 64 ; ++i)
	{
		dst_patch[i] = static_cast<unsigned int>(((src_patch[i] - minValue) / maxValue) * scale);
		if(dst_patch[i] == scale)	dst_patch[i] = (scale - 1);
	}
}
void BORA::ImageUtility::Quantization_self(BORA::Patch &_patch)
{
	BORA::PATCH &patch = _patch.getPATCH_ref();

	unsigned int scale = 5;
	double result[64];

	/*
	// 각 픽셀의 평균 계산
	double average = 0.0;
	for(unsigned int i = 0 ; i < 64 ; ++i)
		average += static_cast<double>(src_patch[i]);

	average /= 64.0;

	// 표준 편차 계산
	double std_deviation = 0.0;
	for(unsigned int i = 0 ; i < 64 ; ++i)
		std_deviation += (src_patch[i] - average)*(src_patch[i] - average);
	std_deviation /= 64.0;
	std_deviation = sqrt(std_deviation);
	*/
	
	for(unsigned int i = 0 ; i < 64 ; ++i)
		//result[i] = (src_patch[i] - average)/std_deviation;
		result[i] = (patch[i]);

	double minValue = static_cast<double>(INT_MAX);
	double maxValue = static_cast<double>(INT_MIN);

	for(unsigned int i = 0 ; i < 64 ; ++i)
	{
		if(result[i] < minValue)
			minValue = result[i];
		
		if(result[i] > maxValue)
			maxValue = result[i];
	}

	maxValue -= minValue;

	for(unsigned int i = 0 ; i < 64 ; ++i)
	{
		patch[i] = static_cast<unsigned int>(((result[i] - minValue) / maxValue) * scale);
		if(patch[i] == scale)	patch[i] = (scale - 1);
	}
}


/* _src이미지의 배경을 전방향 검은색으로 확장시킨 이미지를 돌려주는 함수 */
void BORA::ImageUtility::ExpandImage( const BORA::Image &_src, BORA::Image &_dst, BORA::Position &_shift_pos )
{
	unsigned int newSize(DiagonalDistance(_src));
	const unsigned int width = _src.getWidth();
	const unsigned int height = _src.getHeight();

	/* 시작점이 이동한 위치 */
	_shift_pos = Position((newSize - width) / 2, (newSize - height) / 2);

	/* 새로운 이미지의 크기는 newSize(sqrt(width^2 + height^2))이 너비와 높이가 되며 */
	IplImage *newImage = cvCreateImage( cvSize(newSize, newSize)
									  , _src.getIplImage()->depth
									  , _src.getIplImage()->nChannels );
	cvSetZero(newImage);
	
	/* 관심영역은 이동됐던 시작지점으로 부터 원본이미지의 너비와 높이가 된다. */
	cvSetImageROI(newImage, cvRect(_shift_pos.x, _shift_pos.y, width, height));
	
	cvCopy(_src.getIplImage(), newImage);
	
	cvResetImageROI(newImage);
	
	_dst = newImage;

	cvReleaseImage(&newImage);
}

// Dec. 2011, Min-Hyuk Sung
void BORA::ImageUtility::CropImage( BORA::Image &_src, BORA::Image &_dst, 
	const BORA::Position &_start_pos, const BORA::Position &_end_pos)
{
	if( _end_pos.x <= _start_pos.x || _end_pos.y <= _start_pos.y )
	{
		std::cerr << "warning: [ImageUtility::CropImage] Wrong range: ("
			<< _start_pos.x << ",  " << _start_pos.y << ") -> ("
			<< _end_pos.x << ",  " << _end_pos.y << ")" << std::endl;
		return;
	}

	const unsigned int width = (_end_pos.x - _start_pos.x);
	const unsigned int height = (_end_pos.y - _start_pos.y);
	cvSetImageROI(_src.getIplImage(), cvRect(_start_pos.x, _start_pos.y, width, height));


	IplImage *newImage = cvCreateImage( cvSize(width, height)
		, _src.getIplImage()->depth
		, _src.getIplImage()->nChannels );
	cvSetZero(newImage);


	cvSetImageROI(newImage, cvRect(0, 0, width, height));
	cvCopy(_src.getIplImage(), newImage, NULL);

	cvResetImageROI(_src.getIplImage());
	cvResetImageROI(newImage);

	_dst = newImage;

	cvReleaseImage(&newImage);
}

/* _src이미지를 흑백의 이미지로 바꾼다. */
void BORA::ImageUtility::ColorImageToGray( const BORA::Image &_src, BORA::Image &_dst )
{
	// 원래 흑백 이미지일 경우에는 그냥 카피
//	printf("Just ENTER\n");
	if(_src.isGrayImage())
	{
		_dst = _src;
		return;
	}
//	printf("isGrayImage\n");

	const IplImage *src_img = _src.getIplImage();

	IplImage *new_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
//	printf("cvCreateImage\n");

	// Jan. 2012, Min-Hyuk Sung
	if ( strncmp(src_img->channelSeq, "RGB", 3) == 0)
	{
//		printf("ENTER RGB\n");
		cvCvtColor(src_img, new_img, CV_RGB2GRAY);
//		printf("EXIT RGB\n");
	}
	else if ( strncmp(src_img->channelSeq, "BGR", 3) == 0)
	{
//		printf("ENTER BGR\n");
		cvCvtColor(src_img, new_img, CV_BGR2GRAY);
//		printf("EXIT BGR\n");
	}
	else
	{
//		printf("ENTER UNKNOWN\n");
		std::cerr << "error: [ImageUtility::ColorImageToGray] Unknown color model: "
			<< src_img->channelSeq << std::endl;
		system("pause");
//		printf("EXIT UNKNOWN\n");
	}
	
	
	/*
	////
	unsigned int image_size = (src_img->imageSize / 3);
	assert(new_img->imageSize >= image_size);

	for(unsigned int i = 0; i < image_size; i++)
	{
		unsigned char b = src_img->imageData[3*i+0];
		unsigned char g = src_img->imageData[3*i+1];
		unsigned char r = src_img->imageData[3*i+2];

		unsigned char y = ((77*r + 150*g + 29*b) >> 8);
		new_img->imageData[i] = y;
	}
	////
	*/
//	printf("BEFORE _dst\n");
	_dst = new_img;
//	printf("AFTER _dst\n");

	cvReleaseImage(&new_img);
//	printf("AFTER _dst\n");
}

/* 이미지의 대각선 길이를 구해서 반환한다. */
unsigned int BORA::ImageUtility::DiagonalDistance( const BORA::Image &_src )
{
	if(_src.isLoaded() == false)
		return 0u;

	return static_cast<unsigned int>(sqrt(static_cast<double>(_src.getWidth()*_src.getWidth()) + static_cast<double>(_src.getHeight()*_src.getHeight())));
}

/* 흑백 이미지를 인자로 흑백 배경의 이미지를 받아온다. */
void BORA::ImageUtility::GrayImageToColor( const BORA::Image &_src, BORA::Image &_dst )
{
	// 원래 칼라이미지이면 그냥 복사
	if(_src.isGrayImage() == false)
	{
		_dst = _src;
		return;
	}

	// 본래 이미지의 포인터를 가져온다.
	const IplImage *src_img = _src.getIplImage();

	// 칼라 이미지를 새로 만든다.
	IplImage *new_img = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 3);
	cvCvtColor(src_img, new_img, CV_GRAY2RGB);

	_dst = new_img;

	cvReleaseImage(&new_img);
}

void BORA::ImageUtility::DrawPoints( const BORA::Image &_src, const BORA::POSITIONS &_positions, BORA::Image &_dst, const CvScalar _color /*= cvScalar(0.0, 255.0, 0.0)*/ )
{
	const IplImage *src_img = _src.getIplImage();

	IplImage *drawImage = cvCloneImage(src_img);
	
	for(unsigned int i = 0 ; i < _positions.size() ; ++i)
	{
		/* 원 그리기 */
		//cvCircle(drawImage, cvPoint(_positions[i].x, _positions[i].y), 2, _color, 3);
		
		/* 사각형 그리기 */
		CvPoint corner1 = cvPoint(_positions[i].x -7, _positions[i].y -7);
		CvPoint corner2 = cvPoint(_positions[i].x +7, _positions[i].y +7);
		cvRectangle(drawImage, corner1, corner2, _color, 1);
	}

	_dst = drawImage;
	cvReleaseImage(&drawImage);
}
void BORA::ImageUtility::DrawPoints_self(const BORA::POSITIONS &_positions, BORA::Image &_image, const CvScalar _color)
{
	if(_image.isLoaded() ==false)
		return;

	IplImage *img = _image.getIplImage();

	for(unsigned int i = 0 ; i < _positions.size() ; ++i)
	{
		CvPoint corner1 = cvPoint(_positions[i].x - 7, _positions[i].y - 7);
		CvPoint corner2 = cvPoint(_positions[i].x + 7, _positions[i].y + 7);
		cvRectangle(img, corner1, corner2, _color, 1);
	}
}

void BORA::ImageUtility::PatchToImage( const BORA::Patch &_patch, BORA::Image &_dst, const unsigned int _zoom)
{
	IplImage *patch_image = cvCreateImage(cvSize(8*_zoom, 8*_zoom), IPL_DEPTH_8U, 1);
	const BORA::PATCH &patch = _patch.getPATCH_ref();

	for(unsigned int y = 0 ; y < 8 ; ++y)
	{
		for(unsigned int i = 0 ; i < _zoom ; ++i)
		{
			for(unsigned int x = 0 ; x < 8 ; ++x)
			{
				for(unsigned int j = 0 ; j < _zoom ; ++j)
				{
					patch_image->imageData[((y*_zoom + i)*patch_image->widthStep) + (x*_zoom + j)] = static_cast<BORA::BYTE>(patch[y*8+x]);
				}
			}
		}
	}

	_dst = patch_image;
	cvReleaseImage(&patch_image);
}

void BORA::ImageUtility::QPatchToImage( const BORA::Patch &_qPatch, BORA::Image &_dst, const unsigned int _zoom /*= 1*/ )
{
	IplImage *patch_image = cvCreateImage(cvSize(8*_zoom, 8*_zoom), IPL_DEPTH_8U, 1);
	const BORA::PATCH &patch = _qPatch.getPATCH_ref();

	for(unsigned int y = 0 ; y < 8 ; ++y)
	{
		for(unsigned int i = 0 ; i < _zoom ; ++i)
		{
			for(unsigned int x = 0 ; x < 8 ; ++x)
			{
				for(unsigned int j = 0 ; j < _zoom ; ++j)
				{
					BORA::BYTE intensity(0);
					switch(patch[y*8+x])
					{
					case 0: intensity = BORA::LV0_INT; break;
					case 1:	intensity = BORA::LV1_INT; break;
					case 2:	intensity = BORA::LV2_INT; break;
					case 3:	intensity = BORA::LV3_INT; break;
					case 4:	intensity = BORA::LV4_INT; break;
					}

					patch_image->imageData[((y*_zoom + i)*patch_image->widthStep) + (x*_zoom + j)] = intensity;
				}
			}
		}
	}

	_dst = patch_image;
	cvReleaseImage(&patch_image);
}

bool BORA::ImageUtility::SaveImage( const BORA::Image &_img, const std::string &_path, const unsigned int &_save_type )
{
	switch(_save_type)
	{
	case BORA::SAVETYPE_IMG:
		return cvSaveImage(_path.c_str(), _img.getIplImage());
		break;

	case BORA::SAVETYPE_PATCH:
	default:
		int p[3] = {CV_IMWRITE_JPEG_QUALITY, 100, 0};
		return cvSaveImage(_path.c_str(), _img.getIplImage(), p) > 0 ? true : false;
	}
}
