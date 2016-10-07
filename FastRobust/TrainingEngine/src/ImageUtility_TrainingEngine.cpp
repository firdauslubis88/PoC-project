#include "StdAfx.h"
#include "ImageUtility_TrainingEngine.h"


BORA::ATransformRule::ATransformRule( void )
	: H_(NULL)
	, invH_(NULL)
	, apply_blur_first_(true)
	, blur_level_(0)
	, noise_level_(0)
	, perspective_(false)
{
	H_		= cvCreateMat(2, 3, CV_32FC1);
	invH_	= cvCreateMat(2, 3, CV_32FC1);

	pH_		= cvCreateMat(3, 3, CV_32FC1);
	invpH_	= cvCreateMat(3, 3, CV_32FC1);
}

void BORA::ATransformRule::Init()
{
	cvSetZero(H_);
	cvSetZero(invH_);

	cvSetZero(pH_);
	cvSetZero(invpH_);
}

BORA::ATransformRule::~ATransformRule( void )
{
	cvReleaseMat(&H_);
	cvReleaseMat(&invH_);

	cvReleaseMat(&pH_);
	cvReleaseMat(&invpH_);
}

void BORA::ImageUtility_TrainingEngine::
	ImageTransform( const BORA::Image &_src, const BORA::ATransformRule &_rule, BORA::Image &_dst,
	unsigned int _TRANSFORM_OPT )
{
	const IplImage *src_img = _src.getIplImage();
	// Make a clone of Iplimage of _src
	_dst = _src;
	IplImage *dst_img = _dst.getIplImage();
	cvSetZero(dst_img);

	if( _rule.blur_level_ == 0 && _rule.noise_level_ == 0 )
	{
		// 블러나 노이즈를 적용 아무것도 안시킬때
		// 실질적인 이미지 변환
		// TODO : 옵션에 대한건 추후에 하도록 한다.
		cvWarpAffine(src_img, dst_img, _rule.H_, _TRANSFORM_OPT);
	}
	else
	{
		if( _rule.blur_level_ != 0 && _rule.noise_level_ != 0 )
		{
			// 블러나 노이즈를 둘다 적용 시킬 때
			// 누가 먼저냐?
			if(_rule.apply_blur_first_)
			{
				// 블러 적용
				cvSmooth(src_img, dst_img, CV_GAUSSIAN, _rule.blur_level_);

				// 노이즈 적용
				CvRNG rng = cvRNG(0x12345);
				IplImage *src_tmp = cvCloneImage(dst_img);
				IplImage *tmp_img = cvCloneImage(dst_img);
				cvRandArr(&rng, tmp_img, CV_RAND_UNI, cvScalar(1, 1, 1), cvScalar(_rule.noise_level_, _rule.noise_level_, _rule.noise_level_));
				cvAdd(src_tmp, tmp_img, dst_img);
				cvReleaseImage(&src_tmp);
				cvReleaseImage(&tmp_img);
			}
			else
			{
				// 노이즈 적용
				CvRNG rng = cvRNG(0x12345);
				IplImage *tmp_img = cvCloneImage(dst_img);
				cvRandArr(&rng, tmp_img, CV_RAND_UNI, cvScalar(1, 1, 1), cvScalar(_rule.noise_level_, _rule.noise_level_, _rule.noise_level_));
				cvAdd(src_img, tmp_img, dst_img);
				cvReleaseImage(&tmp_img);

				// 블러 적용
				IplImage *tmp_img2 = cvCloneImage(dst_img);
				cvSmooth(tmp_img2, dst_img, CV_GAUSSIAN, _rule.blur_level_);
				cvReleaseImage(&tmp_img2);
			}
		}
		else
		{
			// 둘중 하나만 적용 시킬 때
			if( _rule.blur_level_ != 0 )
			{
				// 블러만 적용
				cvSmooth(src_img, dst_img, CV_GAUSSIAN, _rule.blur_level_);
			}
			else
			{
				// 노이즈만 적용
				CvRNG rng = cvRNG(0x12345);
				IplImage *tmp_img = cvCloneImage(dst_img);
				cvRandArr(&rng, tmp_img, CV_RAND_UNI, cvScalar(1, 1, 1), cvScalar(_rule.noise_level_, _rule.noise_level_, _rule.noise_level_));
				cvAdd(src_img, tmp_img, dst_img);
				cvReleaseImage(&tmp_img);
			}
		}

		// 실질적인 이미지 변환
		// TODO : 옵션에 대한건 추후에 하도록 한다.
		IplImage *tpm_src_img = cvCloneImage(dst_img);
		cvSetZero(dst_img);
		cvWarpAffine(tpm_src_img, dst_img, _rule.H_, _TRANSFORM_OPT);
		cvReleaseImage(&tpm_src_img);
	}

	// 퍼스펙티브를 적용하는 거라면 회전을 다 시키고 난후에 한다.
	// 코드상으로 bin 에서만 perspective_변수가 true로 넘어올 수 있다.
	if(_rule.perspective_)
	{
		IplImage *tpm_src_img = cvCloneImage(dst_img);
		cvSetZero(dst_img);
		cvWarpPerspective(tpm_src_img, dst_img, _rule.pH_, _TRANSFORM_OPT);
		cvReleaseImage(&tpm_src_img);
	}
}
