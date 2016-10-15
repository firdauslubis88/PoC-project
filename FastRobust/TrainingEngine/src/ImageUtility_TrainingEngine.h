/*******************************************************************
 ImageUtility_TrainingEngine class

 TrainingEngine 에서 쓰이는 ImageUtility 클래스.
 GeneralFunction 프로젝트의 ImageUtility 클래스를 상속 받는다.
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "ImageUtility.h"
#include "Image.h"
#include "Options.h"
#include "DebugFunc.h"

namespace BORA
{
	struct ATransformRule
	{
		CvMat *H_;
		CvMat *invH_;
		
		bool apply_blur_first_;
		unsigned int blur_level_;
		unsigned int noise_level_;

		bool perspective_;
		CvMat *pH_;
		CvMat *invpH_;

		ATransformRule(void);
		~ATransformRule(void);

		void Init();
	};

	class ImageUtility_TrainingEngine :	public BORA::ImageUtility
	{
	public:
		static void ImageTransform(const BORA::Image &_src, const BORA::ATransformRule &_rule, BORA::Image &_dst,
			unsigned int _TRANSFORM_OPT = CV_INTER_CUBIC);
	};

}