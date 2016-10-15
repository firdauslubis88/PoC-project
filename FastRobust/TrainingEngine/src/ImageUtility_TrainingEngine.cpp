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
		// ���� ����� ���� �ƹ��͵� �Ƚ�ų��
		// �������� �̹��� ��ȯ
		// TODO : �ɼǿ� ���Ѱ� ���Ŀ� �ϵ��� �Ѵ�.
		cvWarpAffine(src_img, dst_img, _rule.H_, _TRANSFORM_OPT);
	}
	else
	{
		if( _rule.blur_level_ != 0 && _rule.noise_level_ != 0 )
		{
			// ���� ����� �Ѵ� ���� ��ų ��
			// ���� ������?
			if(_rule.apply_blur_first_)
			{
				// �� ����
				cvSmooth(src_img, dst_img, CV_GAUSSIAN, _rule.blur_level_);

				// ������ ����
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
				// ������ ����
				CvRNG rng = cvRNG(0x12345);
				IplImage *tmp_img = cvCloneImage(dst_img);
				cvRandArr(&rng, tmp_img, CV_RAND_UNI, cvScalar(1, 1, 1), cvScalar(_rule.noise_level_, _rule.noise_level_, _rule.noise_level_));
				cvAdd(src_img, tmp_img, dst_img);
				cvReleaseImage(&tmp_img);

				// �� ����
				IplImage *tmp_img2 = cvCloneImage(dst_img);
				cvSmooth(tmp_img2, dst_img, CV_GAUSSIAN, _rule.blur_level_);
				cvReleaseImage(&tmp_img2);
			}
		}
		else
		{
			// ���� �ϳ��� ���� ��ų ��
			if( _rule.blur_level_ != 0 )
			{
				// ���� ����
				cvSmooth(src_img, dst_img, CV_GAUSSIAN, _rule.blur_level_);
			}
			else
			{
				// ����� ����
				CvRNG rng = cvRNG(0x12345);
				IplImage *tmp_img = cvCloneImage(dst_img);
				cvRandArr(&rng, tmp_img, CV_RAND_UNI, cvScalar(1, 1, 1), cvScalar(_rule.noise_level_, _rule.noise_level_, _rule.noise_level_));
				cvAdd(src_img, tmp_img, dst_img);
				cvReleaseImage(&tmp_img);
			}
		}

		// �������� �̹��� ��ȯ
		// TODO : �ɼǿ� ���Ѱ� ���Ŀ� �ϵ��� �Ѵ�.
		IplImage *tpm_src_img = cvCloneImage(dst_img);
		cvSetZero(dst_img);
		cvWarpAffine(tpm_src_img, dst_img, _rule.H_, _TRANSFORM_OPT);
		cvReleaseImage(&tpm_src_img);
	}

	// �۽���Ƽ�긦 �����ϴ� �Ŷ�� ȸ���� �� ��Ű�� ���Ŀ� �Ѵ�.
	// �ڵ������ bin ������ perspective_������ true�� �Ѿ�� �� �ִ�.
	if(_rule.perspective_)
	{
		IplImage *tpm_src_img = cvCloneImage(dst_img);
		cvSetZero(dst_img);
		cvWarpPerspective(tpm_src_img, dst_img, _rule.pH_, _TRANSFORM_OPT);
		cvReleaseImage(&tpm_src_img);
	}
}
