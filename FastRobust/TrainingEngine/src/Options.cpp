#include "StdAfx.h"
#include "Options.h"

// Dec. 2011, Min-Hyuk Sung
BORA::Options::Options()
{
	Init();
}

// Default
// Dec. 2011, Min-Hyuk Sung
void BORA::Options::Init()
{
	//img_path_ = "\\";
	//img_file_name_extantion_ = "";
	//img_file_name_ = "";
	saveType = BORA::DBTYPE::VER1;
		
	//struct DebugFileOptions
	debugfileoptions_.isDump_					= false;
	debugfileoptions_.dumpEachTrainingImage_	= false;
	debugfileoptions_.dumpFeatureData_			= false;
	debugfileoptions_.dumpDescriptionAsCSV_		= false;

	//struct CornerDetection
	cornerdetection_.wishFeatureNumber_ = -1;
	cornerdetection_.threshold_ = DEFAULT_CORNERDETECTION_THRESHOLD;

	//struct Indexing
	indexing_.enumIndexing_ = Indexing::INDEX_13;

	//struct PatchTruncation
	patchtruncation_.enumTruncationType_ = PatchTruncation::TRUNCATION_BY_AMOUNT;
	patchtruncation_.unionTruncationValue_.amount_ = DEFAULT_PATCHTRUNCATION_AMOUNT;
	patchtruncation_.truncationExactCut	= DEFAULT_PATCHTRUNCATION_EXACT_CUT;


	// image - struct TransformFilter
	//image_.transformfilter_.interpolation_	= CV_INTER_CUBIC;
	image_.transformfilter_.interpolation_	= CV_INTER_AREA;
	image_.transformfilter_.wraping_falg_	= CV_WARP_FILL_OUTLIERS;

    // image - struct Rotation
    image_.rotation_.basic_rotation_		= DEFAULT_IMAGE_BASIC_ROTATION;
    image_.rotation_.basic_rotation_min_	= DEFAULT_IMAGE_BASIC_ROTATION_MIN;
    image_.rotation_.basic_rotation_max_	= DEFAULT_IMAGE_BASIC_ROTATION_MAX;

    image_.rotation_.detail_rotation_		= DEFAULT_IMAGE_DETAIL_ROTATION;
    image_.rotation_.detail_rotation_min_	= DEFAULT_IMAGE_DETAIL_ROTATION_MIN;
    image_.rotation_.detail_rotation_max_	= DEFAULT_IMAGE_DETAIL_ROTATION_MAX;

	// image - struct Scale
    image_.scale_.scale_max_		= DEFAULT_IMAGE_SCALE_MAX;
    image_.scale_.basic_scale_		= DEFAULT_IMAGE_BASIC_SCALE	;
    image_.scale_.basic_scale_num_	= DEFAULT_IMAGE_BASIC_SCALE_NUM;

    image_.scale_.detail_scale_		= DEFAULT_IMAGE_DETAIL_SCALE;
    image_.scale_.detail_scale_num_	= DEFAULT_IMAGE_DETAIL_SCALE_NUM;

	// image
	image_.random_blur_.use_random_blur_				= false;
    image_.blur_.use_blur_								= false;
	image_.noise_.use_noise_							= false;
	image_.random_noise_.use_random_noise_				= false;
	image_.perspective_.use_perspective_				= false;
	image_.affine_.use_affine_							= false;
	image_.spacepartitioning_.use_space_partitioning_	= false;
	image_.spacepartitioning_.space_partion_basic_num_	= 1;
}
 
bool BORA::Options::CheckOptionsRule()
{
	/********** TODO **********/
	//범위 체크 해주기

	/********** 특징점 찾기 옵션 **********/
	if(cornerdetection_.wishFeatureNumber_ <= 0)
	{
		//별도의 특징점 찾는 옵션이 꺼져있을때,
		//threshold가 범위[1, 254]를 벗어났으면 80으로 세팅한다.
		if(cornerdetection_.threshold_ < 1 || cornerdetection_.threshold_ > 254)
		{
			cornerdetection_.threshold_ = DEFAULT_CORNERDETECTION_THRESHOLD;
		}
	}
	else
	{
		cornerdetection_.threshold_ = 0;
	}

	/********** 인덱싱 옵션 **********/
	if(indexing_.enumIndexing_ != Indexing::INDEX_5 && 
		indexing_.enumIndexing_ != Indexing::INDEX_13)
	{
		//정해진 옵션중 아무것도 허용이 되어있지 않으면 13으로 맞춘다.
		indexing_.enumIndexing_ = Indexing::INDEX_13;
	}

	/********** 특징점 정렬/Truncation 옵션 **********/
	if(patchtruncation_.enumTruncationType_ != PatchTruncation::TRUNCATION_NONE &&
		patchtruncation_.enumTruncationType_ != PatchTruncation::TRUNCATION_BY_AMOUNT )//&&
		//patchtruncation_.enumTruncationType_ != PatchTruncation::TRUNCATION_BY_RATIO)
	{
		//정해진 옵션중 아무것도 안되 있으면, 양으로 50에 맞춘다.
		patchtruncation_.enumTruncationType_ = PatchTruncation::TRUNCATION_BY_AMOUNT;
		patchtruncation_.unionTruncationValue_.amount_
			= DEFAULT_PATCHTRUNCATION_AMOUNT;
	}
	else
	{
		switch(patchtruncation_.enumTruncationType_)
		{
		case PatchTruncation::TRUNCATION_NONE:
			patchtruncation_.unionTruncationValue_.ratio_ = 1.0;
			break;

		case PatchTruncation::TRUNCATION_BY_AMOUNT:
			if(patchtruncation_.unionTruncationValue_.amount_ < 1)
			{
				patchtruncation_.unionTruncationValue_.amount_
					= DEFAULT_PATCHTRUNCATION_AMOUNT;
			}
			break;

			/*
		case PatchTruncation::TRUNCATION_BY_RATIO:
			if(patchtruncation_.unionTruncationValue_.ratio_ < 0.0 ||
				patchtruncation_.unionTruncationValue_.ratio_ > 1.0)
			{
				patchtruncation_.unionTruncationValue_.ratio_ = 0.5;
			}
			break;
			*/
		}
	}

	/********** Image 옵션 **********/
	//Bin 생성 이미지 rotation 증가량
	if(image_.rotation_.basic_rotation_ <= 0.0 ||
		image_.rotation_.basic_rotation_ > 350.0)
	{
		image_.rotation_.basic_rotation_ = DEFAULT_IMAGE_BASIC_ROTATION;
	}

	//Bin 생성 이미지 rotation 초기값
	if(image_.rotation_.basic_rotation_min_ < 0.0 ||
		image_.rotation_.basic_rotation_min_ > 350.0)
	{
		image_.rotation_.basic_rotation_min_ = DEFAULT_IMAGE_BASIC_ROTATION_MIN;
	}

	//Bin 생성 이미지 rotation 마지막 값
	if(image_.rotation_.basic_rotation_max_ < image_.rotation_.basic_rotation_min_ ||
		image_.rotation_.basic_rotation_max_ > image_.rotation_.basic_rotation_min_ + 360.0)
	{
		image_.rotation_.basic_rotation_max_ = DEFAULT_IMAGE_BASIC_ROTATION_MAX;
	}

	//Bin 내부 생성 이미지 rotation 증가량
	if(image_.rotation_.detail_rotation_ < 0.0 ||
		image_.rotation_.detail_rotation_ > 359.0)
	{
		image_.rotation_.detail_rotation_ = DEFAULT_IMAGE_DETAIL_ROTATION;
	}

	//Bin 내부 생성 이미지 rotation 초기값
	if(image_.rotation_.detail_rotation_min_ < -359.0 ||
		image_.rotation_.detail_rotation_min_ >  359.0)
	{
		image_.rotation_.detail_rotation_ = DEFAULT_IMAGE_DETAIL_ROTATION_MIN;
	}

	//Bin 내부 생성 이미지 rotation 마지막 값
	if(image_.rotation_.detail_rotation_max_ < image_.rotation_.detail_rotation_min_ ||
		image_.rotation_.detail_rotation_max_ > image_.rotation_.detail_rotation_min_ + 360.0)
	{
		image_.rotation_.detail_rotation_max_ = DEFAULT_IMAGE_DETAIL_ROTATION_MAX;
	}

	//Bin 생성 이미지 scale 값 / Bin 내부 생성 이미지 scale 값
	if(image_.scale_.scale_max_ <= 0.0 ||
		image_.scale_.scale_max_ > 1.0)
	{
		image_.scale_.scale_max_ = DEFAULT_IMAGE_SCALE_MAX;
	}

	//Bin 생성 이미지 scale 원소 값
	if(image_.scale_.basic_scale_ <= 0.0 ||
		image_.scale_.basic_scale_ > 1.0)
	{
		image_.scale_.basic_scale_ = DEFAULT_IMAGE_BASIC_SCALE;
	}

	//Bin 생성 이미지 scale 원소 적용 횟수
	if(image_.scale_.basic_scale_num_ <= 0)
	{
		image_.scale_.basic_scale_num_ = DEFAULT_IMAGE_BASIC_SCALE_NUM;
	}

	//Bin 내부 생성 이미지 scale 원소 값
	if(image_.scale_.detail_scale_ <= 0.0 ||
		image_.scale_.detail_scale_ > 1.0)
	{
		image_.scale_.detail_scale_ = DEFAULT_IMAGE_DETAIL_SCALE;
	}

	//Bin 내부 생성 이미지 scale 원소 적용 횟수
	if(image_.scale_.detail_scale_num_ <= 0)
	{
		image_.scale_.detail_scale_num_ = DEFAULT_IMAGE_DETAIL_SCALE_NUM;
	}

	/********** 랜덤 블러 **********/
	if(image_.random_blur_.use_random_blur_)
	{
		// 랜덤 블러를 사용하게 되면, 일반 블러는 무시된다.
		image_.blur_.use_blur_ = false;
		
		// 값을 체크 한다.
		unsigned int &minValue = image_.random_blur_.min_mask_level_;
		unsigned int &maxValue = image_.random_blur_.max_mask_level_;

		if(minValue >= maxValue)
			maxValue = minValue + 2;

		if(minValue % 2 == 0 && minValue != 0)
			++minValue;
		if(maxValue % 2 == 0)
			++maxValue;
	}

	/********** 랜덤 노이즈 **********/
	if(image_.random_noise_.use_random_noise_)
	{
		// 랜덤 노이즈를 사용하게 되면, 일반 노이즈는 무시된다.
		image_.noise_.use_noise_ = false;

		// 값을 체크한다.
		unsigned int &minValue = image_.random_noise_.min_intensity_diff_;
		unsigned int &maxValue = image_.random_noise_.max_intensity_diff_;

		if(maxValue > 254)
			maxValue = 254;
		if(minValue < 10)
			minValue = 10;

		if(minValue >= maxValue)
		{
			if(maxValue - 10 < 10)
				minValue = 10;
			else
				minValue = maxValue - 10;
		}

	}

	/********** 블러 **********/
	if(image_.blur_.use_blur_ == false)
	{
		//안 사용 할꺼니까 값을 초기화 시켜놓는다.
		image_.blur_.apply_blur_first_ = false;
		image_.blur_.level_.clear();
		image_.blur_.level_.push_back(0);	//혹시 모르니까 사용안하는거 한개는 넣어 놓는다.
	}
	else
	{
		//사용 할 거면 값을 체크 한다.
		if(image_.blur_.level_.size() == 0)
		{
			// 사용한다고 했는데
			// 한개도 없으면 0과 기본값 1개를 집어 넣는다.
			image_.blur_.level_.push_back(0);
			image_.blur_.level_.push_back(3);
		}
		else
		{
			//있으면 이게 홀수인지 판단하고, 아니면 지워버린다.
			for(unsigned int i = 0 ; i < image_.blur_.level_.size() ; ++i)
			{
				if(image_.blur_.level_[i] % 2 == 0 && image_.blur_.level_[i] != 0)
				{
					//짝수 니까 지운다.
					image_.blur_.level_[i] = image_.blur_.level_[image_.blur_.level_.size()-1];
					image_.blur_.level_.pop_back();
					--i;			//i번째에 있는걸 판단하고 지웠는데, 한번도 검사를 하지 않았던 마지막께 왔으므로, i를 감소시킨다.
					//어차피 for loop exist condition 은 size이므로 문제 없음
				}
			}

			//같은게 있으면 지운다.
			for(unsigned int i = 0 ; i < image_.blur_.level_.size()-1 ; ++i)
			{
				if(find(image_.blur_.level_.begin()+i, image_.blur_.level_.end(), image_.blur_.level_[i]) == image_.blur_.level_.end())
				{
					image_.blur_.level_[i] = image_.blur_.level_[image_.blur_.level_.size()-1];
					image_.blur_.level_.pop_back();
					--i;
				}
			}

			//개수 확인을 한다. 혹시나 지워져 버리고 개수가 없으면 망이니까..
			if(image_.blur_.level_.size() == 0)
			{
				image_.blur_.level_.push_back(0);
				image_.blur_.level_.push_back(3);
			}
		}
	}

	/********** 노이즈 **********/
	if(image_.noise_.use_noise_ == false)
	{
		image_.noise_.level_.clear();
		image_.noise_.level_.push_back(0);
	}
	else
	{
		//사용할거 이므로 값을 체크 한다.
		for(unsigned int i = 0 ; i < image_.noise_.level_.size() ; ++i)
		{
			if(image_.noise_.level_[i] < 0 ||
				image_.noise_.level_[i] > 245)
			{
				image_.noise_.level_[i] = image_.noise_.level_[image_.noise_.level_.size()-1];
				image_.noise_.level_.pop_back();
				--i;
			}
		}

		//같은게 있는지 체크 한다.
		for(unsigned int i = 0 ; i < image_.noise_.level_.size()-1 ; ++i)
		{
			if(find(image_.noise_.level_.begin(), image_.noise_.level_.end(), image_.noise_.level_[i]) == image_.noise_.level_.end())
			{
				image_.noise_.level_[i] = image_.noise_.level_[image_.noise_.level_.size()-1];
				image_.noise_.level_.pop_back();
			}
		}

		//개수를 체크한다.
		if(image_.noise_.level_.size() == 0)
		{
			image_.noise_.level_.push_back(0);
			image_.noise_.level_.push_back(10);
		}
	}

	/********** affine 옵션 체크 **********/
	BORA::Options::Image::Affine &affineOpt			= image_.affine_;
	BORA::Options::Image::Affine::Factor &factor	= image_.affine_.factor_;
	BORA::Options::Image::Affine::RandomRotation &affine_rand_rotation	= image_.affine_.randomrotation_;
	BORA::Options::Image::Affine::RandomScale &affine_rand_scale		= image_.affine_.randomscale_;

	if(affineOpt.use_affine_ == true)
	{

		if(factor.theta_max_ < image_.affine_.factor_.theta_min_)
		{
			factor.theta_max_ = factor.theta_min_ +1.0;
		}

		if(factor.phi_max_ < factor.phi_min_)
		{
			factor.phi_max_ = factor.phi_min_ +1.0;
		}

		if(factor.scale_axis_x_min_ <= 0.0)
		{
			factor.scale_axis_x_min_ = 0.1;
		}

		if(factor.scale_axis_x_max_ <= factor.scale_axis_x_min_)
		{
			factor.scale_axis_x_max_ = factor.scale_axis_x_min_ * 2;
		}

		if(factor.scale_axis_y_min_ <= 0.0)
		{
			factor.scale_axis_y_min_ = 0.1;
		}

		if(factor.scale_axis_y_max_ <= factor.scale_axis_y_min_)
		{
			factor.scale_axis_y_max_ = factor.scale_axis_y_min_ * 2;
		}

		if(affine_rand_rotation.theta_random_ == false)
		{
			if(affine_rand_rotation.theta_increase_level_ < 0.0)
			{
				affine_rand_rotation.theta_increase_level_ = 1.0;
			}
		}

		if(affine_rand_rotation.phi_random_ == false)
		{
			if(affine_rand_rotation.phi_increase_level_ < 0.0)
			{
				affine_rand_rotation.phi_increase_level_ = 10.0;
			}
		}

		if(affine_rand_scale.axis_x_random_ ==false)
		{
			if(affine_rand_scale.axis_x_increase_level_ < 0.0)
			{
				affine_rand_scale.axis_x_increase_level_ = 0.3;
			}
		}

		if(affine_rand_scale.axis_y_random_ ==false)
		{
			if(affine_rand_scale.axis_y_increase_level_ < 0.0)
			{
				affine_rand_scale.axis_y_increase_level_ = 0.3;
			}
		}
	}
	else
	{
		//사용 안하는거면 전부들 default 값으로 만든다.
		image_.affine_.factor_.theta_min_	= -5.0;
		image_.affine_.factor_.theta_max_	= 5.0;
		image_.affine_.factor_.phi_min_		= -180.0;
		image_.affine_.factor_.phi_max_		= 180.0;
		image_.affine_.factor_.scale_axis_x_min_ = 0.6;
		image_.affine_.factor_.scale_axis_x_max_ = 1.2;
		image_.affine_.factor_.scale_axis_y_min_ = 0.6;
		image_.affine_.factor_.scale_axis_y_max_ = 1.2;

		image_.affine_.randomrotation_.theta_random_			= true;
		image_.affine_.randomrotation_.theta_increase_level_	= 1.0;

		image_.affine_.randomrotation_.phi_random_				= true;
		image_.affine_.randomrotation_.phi_increase_level_		= 1.0;

		image_.affine_.randomscale_.axis_x_random_				= true;
		image_.affine_.randomscale_.axis_x_increase_level_		= 0.3;

		image_.affine_.randomscale_.axis_y_random_				= true;
		image_.affine_.randomscale_.axis_y_increase_level_		= 0.3;
	}

	// Dec. 2011, Min-Hyuk Sung
	// Large Scale Image - Space Partitioning
	if(image_.spacepartitioning_.space_partion_basic_num_ > 1)
		image_.spacepartitioning_.use_space_partitioning_ = true;
	else
	{
		image_.spacepartitioning_.use_space_partitioning_ = false;
		image_.spacepartitioning_.space_partion_basic_num_ = 1;
	}

	return true;

}

