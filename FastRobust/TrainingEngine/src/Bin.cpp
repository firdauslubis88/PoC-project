#include "StdAfx.h"
#include "Bin.h"


BORA::Bin::Bin( unsigned int _bin_index, 
				const BORA::Image &_bw_img, 
				const BORA::BinInfo &_bin_info,
				const BORA::Options &_options )
				: bin_index_(_bin_index)
				, bw_img_(_bw_img)
				// Dec. 2011, Min-Hyuk Sung
				// ---- //
				, BinInfo(_bin_info)
				// 'indexMap' will be assigned after 'canonical_image' is defined
				//, indexMap_(bw_img_)
				, indexMap_(0, 0)
				// 'bin_img_start_pos_' and 'bin_img_scaled_size_' will be assigned after 'canonical_image' is defined
				, bin_img_start_pos_(0, 0)
				, bin_img_scaled_size_(0, 0)
				// ---- //
				, options_(_options)
				/* 사용에 편리를 위한 레퍼런스 등록 */
				, opt_img_(_options.image_)
				, opt_img_rotation_(_options.image_.rotation_)
				, opt_img_scale_(_options.image_.scale_)
				, opt_img_blur_(_options.image_.blur_)
				, opt_img_noise_(_options.image_.noise_)
				, opt_img_affine_(_options.image_.affine_)
				, opt_img_affine_factor_(_options.image_.affine_.factor_)
				, opt_img_affine_random_rotation_(_options.image_.affine_.randomrotation_)
				, opt_img_affine_random_scale_(_options.image_.affine_.randomscale_)
				, opt_corner_(_options.cornerdetection_)
				, opt_debug_(_options.debugfileoptions_)
				, opt_indexing_(_options.indexing_)
				, opt_patch_(_options.patchtruncation_)


{
	// Dec. 2011, Min-Hyuk Sung
	// Set the maximum number of generated features for each bin
	// After generating features, some of them over 'NUM_FEATURE_TRUNCATION features can be pruned
	try
	{
		features_.reserve(MAX_NUM_BIN_FEATURES);
	}
	catch (std::bad_alloc& ba)
	{
		std::cerr << "Error: [Bin::Bin] bad_alloc caught: " << ba.what() << std::endl;
		system("pause");
	}
	
}


BORA::Bin::~Bin(void)
{
	// Dec. 2011, Min-Hyuk Sung
	cano_img_.ClearIplImage();
}

void BORA::Bin::Calc( BORA::TDESCRIPTORS &_descriptors, const std::string &_debug_path )
{
	//clock_t timer;

	/* bin 디버깅 시작 */
	BORA::BinDebug bin_debug;
	if(opt_debug_.isDump_)
	{
		bin_debug.StartDebug(_debug_path, bin_index_);
		bin_debug.StartTrnTable();
	}

	// 1. hasNext를 위한 flag 초기화
	InitImageTransformVariation();
	
	/* trn 디버깅 시작 */
	// trn_debug는 hasNextTrnImg 함수에서 시작을 하게 된다.
	// trn_debug는 LearnFeatures가 끝나게 되면 정보를 쓰고 종료하게 된다.
	BORA::TrnImageDebug trn_debug;
	while(hasNextTrnImg(bin_debug, trn_debug))
	{
		// 2. training image를 생성한다.
		BORA::Image trn_img;
		BORA::ImageUtility_TrainingEngine::ImageTransform(cano_img_, trn_rule_, trn_img,
			opt_img_.transformfilter_.interpolation_);
		//BORA::DebugFunc::Show(trn_img);
		
		// 3. Features를 찾는다.
		BORA::FEATURES i_ThFeatures;
		LearningFeatures(trn_img, i_ThFeatures);
	
		
		/* 디버깅 */
		if(opt_debug_.isDump_)
		{
			BORA::Image trn_featured_img;
			BORA::ImageUtility::GrayImageToColor(trn_img, trn_featured_img);
			
			BORA::POSITIONS positions;
			BORA::DebugFunc::FeaturesPoints(i_ThFeatures, positions);

			BORA::ImageUtility::DrawPoints(trn_featured_img, positions, trn_featured_img);

			trn_debug.SaveImages(trn_img, trn_featured_img);

			// training image에서 발견되었던 patches에 대한 정보를 기록한다.
			// 하지만 patch는 feature에 담겨서 이동되므로, 실제로는 i_ThFeatures 각각이 가지고 있는
			// patch에 대한 내용을 기록하게 되는 것이다.
			trn_debug.WritePatches(i_ThFeatures);
			trn_debug.FinishDebug();
		}

		// 4. Features를 등록한다.(2px 이내면 "추가", 없으면 "생성")
		// indexMap_에 features_의 인덱스를 등록한다.
		//timer = clock();
		RegistFeatures(i_ThFeatures);
		//printf("RegistFeatures time : %lf ms\n", (double)(clock() - timer));

	}

	// 5. Features 자체를 어떻게 하나??
	// 일단은 기존과 동일하게 생성
	SortNTruncation(bin_debug);

	// 6. Features를 Descriptors로 표현한다.
	FeaturesRepresent(_descriptors);


	/* bin 디버깅 끝 */
	if(opt_debug_.isDump_)
	{
		// 이전에 training의 개요에 대한 정리를 마쳤다.
		bin_debug.FinishTrnTable();

		// 빈 자체 정보에 대해서 기록한다.
		BORA::Image			cano_features_img;
		BORA::POSITIONS		positions;
		BORA::ImageUtility::ColorImageToGray(cano_img_, cano_features_img);
		BORA::ImageUtility::GrayImageToColor(cano_features_img, cano_features_img);
		
		BORA::DebugFunc::FeaturesPoints(features_, positions);
		BORA::ImageUtility::DrawPoints(cano_features_img, positions, cano_features_img);

		// 이미지를 저장한다.
		bin_debug.SaveImages(cano_img_, cano_features_img);

		// 최종적인 features의 정보를 기록한다.
		bin_debug.WirteFeaturesInfo(features_);
		bin_debug.FinishDebug();
	}
}

/* hasNext를 위한 falg 변수 초기화 */
void BORA::Bin::InitImageTransformVariation()
{
	createdTrnImg_number_ = 0;

	rotation_flag_	= 0;
	rotation_max_	= static_cast<unsigned int>((opt_img_.rotation_.detail_rotation_max_ - opt_img_.rotation_.detail_rotation_min_) / opt_img_.rotation_.detail_rotation_);
	scale_flag_		= 0;
	
	blur_flag_		= 0;
	noise_flag_		= 0;

	affine_random_flag_			= 0;

	scale_value_	= opt_img_scale_.scale_max_;
	blur_value_		= 0;
	noise_value_	= 0;

	if(opt_img_affine_.use_affine_)
	{
		affine_theta_value_ = opt_img_affine_.randomrotation_.theta_random_ ?
			BORA::Rand(opt_img_affine_factor_.theta_min_, opt_img_affine_factor_.theta_max_) :
			opt_img_affine_factor_.theta_min_;
		
		
		affine_phi_value_ = opt_img_affine_.randomrotation_.phi_random_ ?
			BORA::Rand(opt_img_affine_factor_.phi_min_, opt_img_affine_factor_.phi_max_) :
			opt_img_affine_factor_.phi_min_;

		affine_scaleX_value_ = opt_img_affine_.randomscale_.axis_x_random_ ?
			BORA::Rand(opt_img_affine_factor_.scale_axis_x_min_, opt_img_affine_factor_.scale_axis_x_max_) :
			opt_img_affine_factor_.scale_axis_x_min_;

		affine_scaleY_value_ = opt_img_affine_.randomscale_.axis_y_random_ ?
			BORA::Rand(opt_img_affine_factor_.scale_axis_y_min_, opt_img_affine_factor_.scale_axis_y_min_) :
			opt_img_affine_factor_.scale_axis_y_min_;
		
	}
	else
	{
		affine_theta_value_		= 0.0;
		affine_phi_value_		= 0.0;
		affine_scaleX_value_	= 0.0;
		affine_scaleY_value_	= 0.0;
	}

	trn_rule_.apply_blur_first_ = opt_img_blur_.apply_blur_first_;

	// Get canonical image

	// !! NOTICE !! //
	// Assume that all canonical images have ONLY SCALE, and ROTATION variations
	// (The other transformation properties are NOT considered for canonical images)
	 
	// !! NOTICE !! //
	// Here, 'cvSize' and 'cvRotate' are used instead of 'cvWarpAffine'
	// 'cvSize' with 'CV_INTER_AREA' property provides nicely smoothed image


	// Step 1. Crop
	BORA::Image crop_img = bw_img_;
	if(opt_img_.spacepartitioning_.use_space_partitioning_)
	{
		BORA::Image crop_img_tmp;
		BORA::ImageUtility::CropImage(crop_img, crop_img_tmp,
			bin_space_partition_start_pos_, bin_space_partition_end_pos_);
		crop_img = crop_img_tmp;
	}


	// Step 2. Scaling
	unsigned int scaled_width  = ceil(crop_img.getWidth()  * bin_scale_);
	unsigned int scaled_height = ceil(crop_img.getHeight() * bin_scale_);
	IplImage *ref_scale_img = cvCreateImage( cvSize(scaled_width, scaled_height)
		, crop_img.getIplImage()->depth
		, crop_img.getIplImage()->nChannels );
	cvSetZero(ref_scale_img);
	cvResize(crop_img.getIplImage(), ref_scale_img, opt_img_.transformfilter_.interpolation_);
	crop_img = ref_scale_img;
	cvReleaseImage(&ref_scale_img);
	bin_img_scaled_size_ = BORA::Position(crop_img.getWidth(), crop_img.getHeight());


	// Step 3. Expand Background
	BORA::Image ref_img;
	BORA::ImageUtility::ExpandImage(crop_img, ref_img, bin_img_start_pos_);


	// Step 4. Rotation
	IplImage *ref_rotation_img = cvCreateImage( cvGetSize(ref_img.getIplImage())
		, ref_img.getIplImage()->depth
		, ref_img.getIplImage()->nChannels );
	cvSetZero(ref_rotation_img);

	cv2DRotationMatrix(ref_img.getCenterPosition(), bin_rotation_, 1.0, cano_rule_.H_);
	cv2DRotationMatrix(ref_img.getCenterPosition(), -bin_rotation_, 1.0, cano_rule_.invH_);

	cvWarpAffine(ref_img.getIplImage(), ref_rotation_img, cano_rule_.H_,
		opt_img_.transformfilter_.interpolation_ + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));

	ref_img = ref_rotation_img;
	cvReleaseImage(&ref_rotation_img);


	// Step 5. Calc Rule
	// !! NOTICE !!
	// 'PERSPECTIVE' IGNORED
	cano_rule_.perspective_ = false;
	cano_rule_.blur_level_  = 0;
	cano_rule_.noise_level_ = 0;


	cano_img_ = ref_img;

	/*
	// Dec. 2011, Min-Hyuk Sung
	// ---- //
	BORA::Image ref_img;
	if(opt_img_.spacepartitioning_.use_space_partitioning_)
	{
		BORA::Image crop_img;
		BORA::ImageUtility::CropImage(bw_img_, crop_img,
			bin_space_partition_start_pos_, bin_space_partition_end_pos_);

		// SET 'bin_img_start_pos_'
		BORA::ImageUtility::ExpandImage(crop_img, ref_img, bin_img_start_pos_);
	}
	else
	{
		BORA::ImageUtility::ExpandImage(bw_img_, ref_img, bin_img_start_pos_);
	}

	CalcCannoTrans(ref_img);
	BORA::ImageUtility_TrainingEngine::ImageTransform(ref_img, cano_rule_, cano_img_,
		opt_img_.transformfilter_.interpolation_);

	// OPTIONAL
	// Remove unnecessarily expanded background...
	if( opt_img_.spacepartitioning_.use_space_partitioning_ )
	{
		assert( ref_img.getWidth() == ref_img.getHeight() );
		unsigned int old_image_size = ref_img.getWidth();
		unsigned int new_image_size = ceil(old_image_size * bin_scale_);
		unsigned int offset = (old_image_size - new_image_size) / 2;

		cvSetImageROI(cano_img_.getIplImage(), cvRect(offset, offset, new_image_size, new_image_size));

		IplImage *crop_cano_img = cvCreateImage( cvSize(new_image_size, new_image_size)
			, cano_img_.getIplImage()->depth
			, cano_img_.getIplImage()->nChannels );
		cvSetZero(crop_cano_img);

		cvCopy(cano_img_.getIplImage(), crop_cano_img);
		cano_img_ = crop_cano_img;
		
		bin_crop_offset_ = BORA::Position(offset, offset);
		cvReleaseImage(&crop_cano_img);
	}
	//
	*/

	indexMap_ = BORA::IndexMap(cano_img_);
	// ---- //

	//BORA::DebugFunc::Show(_ref_img);
	//BORA::DebugFunc::Show(cano_img_);
}

void BORA::Bin::CalcCannoTrans(const BORA::Image &_ref_img)
{
	// 단순히 회전과 확대인자만 들어가므로 나머지 사항은 제외처리
	cano_rule_.blur_level_  = 0;
	cano_rule_.noise_level_ = 0;
	
	//std::cout << ref_img_.getCenterPosition().x << ", " << ref_img_.getCenterPosition().y << std::endl;
	//std::cout << "bin_rotation_ : " << bin_rotation_ << std::endl;
	//std::cout << "bin_scale_ : " << bin_scale_ << std::endl;
	//std::cout << "-bin_rotation_ : " << -bin_rotation_ << std::endl;
	//std::cout << "1/bin_scale_ : " << 1/bin_scale_ << std::endl << std::endl;

	cv2DRotationMatrix(_ref_img.getCenterPosition(),
						bin_rotation_, bin_scale_,
						cano_rule_.H_);

	cv2DRotationMatrix(_ref_img.getCenterPosition(),
						-bin_rotation_, 1/bin_scale_,
						cano_rule_.invH_);

	if(bin_perspective_)
	{
		CvPoint2D32f *src = new CvPoint2D32f[4];
		CvPoint2D32f *dst = new CvPoint2D32f[4];

		int width	= static_cast<int>(_ref_img.getWidth());
		int height	= static_cast<int>(_ref_img.getHeight());

		double width_ratio = 0.15;
		double height_ratio = 0.3;

		src[0].x = 0;						src[0].y = 0;
		dst[0].x = width * width_ratio;		dst[0].y = height * height_ratio;

		src[1].x = width;					src[1].y = 0;
		dst[1].x = width-width*width_ratio;	dst[1].y = height * height_ratio;

		src[2].x = 0;						src[2].y = height;
		dst[2].x = 0;						dst[2].y = height - height * height_ratio;

		src[3].x = width;					src[3].y = height;
		dst[3].x = width;					dst[3].y = height - height * height_ratio;

		cvGetPerspectiveTransform(src, dst, cano_rule_.pH_);
		cvGetPerspectiveTransform(dst, src, cano_rule_.invpH_);

		cano_rule_.perspective_ = true;
		delete src, dst;
	}
	else
		cano_rule_.perspective_ = false;
}

bool BORA::Bin::hasNextTrnImg( BORA::BinDebug &_bin_debug, BORA::TrnImageDebug &_trn_debug )
{
	// training 이미지 생성 횟수 증가
	++createdTrnImg_number_;

	// 회전각 확인
	if( rotation_flag_ < rotation_max_ )
	{
		rotation_value_ = opt_img_.rotation_.detail_rotation_min_ + (rotation_flag_ * opt_img_.rotation_.detail_rotation_);
		CalcTrnTrans(_bin_debug, _trn_debug);
		++rotation_flag_;
		return true;
	}
	else
	{
		rotation_flag_ = 0;
		rotation_value_ = opt_img_.rotation_.detail_rotation_min_ + (rotation_flag_ * opt_img_.rotation_.detail_rotation_);
		++rotation_flag_;
	}

	// 확대인자 확인
	if( ++scale_flag_ < opt_img_scale_.detail_scale_num_ )
	{
		scale_value_ *= opt_img_scale_.detail_scale_;
		CalcTrnTrans(_bin_debug, _trn_debug);
		return true;
	}
	else
	{
		scale_flag_ = 0;
		scale_value_ = opt_img_scale_.scale_max_;
	}

	// 블러 체크
	if(opt_img_blur_.use_blur_)
	{
		if( ++blur_flag_ < opt_img_blur_.level_.size() )
		{
			CalcTrnTrans(_bin_debug, _trn_debug);
			return true;
		}
		else
		{
			blur_flag_ = 0;
		}
	}

	// 노이즈 체크
	if(opt_img_noise_.use_noise_)
	{
		if( ++noise_flag_ < opt_img_noise_.level_.size() )
		{
			CalcTrnTrans(_bin_debug, _trn_debug);
			return true;
		}
		else
		{
			noise_flag_ = 0;
		}
	}

	// affine 체크
	if( opt_img_affine_.use_affine_ )
	{
		// 랜덤 추출
		if(++affine_random_flag_ < opt_img_affine_factor_.random_value_extract_time_)
		{
			if(opt_img_affine_random_rotation_.theta_random_)
				affine_theta_value_ = BORA::Rand(opt_img_affine_factor_.theta_min_, opt_img_affine_factor_.theta_max_);

			if(opt_img_affine_random_rotation_.phi_random_)
				affine_phi_value_ = BORA::Rand(opt_img_affine_factor_.phi_min_, opt_img_affine_factor_.phi_max_);

			if(opt_img_affine_random_scale_.axis_x_random_)
				affine_scaleX_value_ = BORA::Rand(opt_img_affine_factor_.scale_axis_x_min_, opt_img_affine_factor_.scale_axis_x_max_);

			if(opt_img_affine_random_scale_.axis_y_random_)
				affine_scaleY_value_ =  BORA::Rand(opt_img_affine_factor_.scale_axis_y_min_, opt_img_affine_factor_.scale_axis_y_max_);


			CalcTrnTrans(_bin_debug, _trn_debug);
			return true;
		}
		else
		{
			affine_random_flag_ = 0;
		}

		// 선형 추출
		if(opt_img_affine_random_rotation_.theta_random_ == false)
		{
			if((affine_theta_value_ += opt_img_affine_random_rotation_.theta_increase_level_)< opt_img_affine_factor_.theta_max_)
			{
				CalcTrnTrans(_bin_debug, _trn_debug);
				return true;
			}
			else
			{
				affine_theta_value_ = opt_img_affine_factor_.theta_min_;
			}

			if((affine_phi_value_ += opt_img_affine_random_rotation_.phi_increase_level_)< opt_img_affine_factor_.phi_max_)
			{
				CalcTrnTrans(_bin_debug, _trn_debug);
				return true;
			}
			else
				affine_phi_value_ = opt_img_affine_factor_.phi_min_;

			if((affine_scaleX_value_ += opt_img_affine_random_scale_.axis_x_increase_level_)< opt_img_affine_factor_.scale_axis_x_max_)
			{
				CalcTrnTrans(_bin_debug, _trn_debug);
				return true;
			}
			else
				affine_scaleX_value_ = opt_img_affine_factor_.scale_axis_x_min_;

			if((affine_scaleY_value_ += opt_img_affine_random_scale_.axis_y_increase_level_) < opt_img_affine_factor_.scale_axis_y_max_)
			{
				CalcTrnTrans(_bin_debug, _trn_debug);
				return true;
			}
			else
				affine_scaleY_value_ = opt_img_affine_factor_.scale_axis_y_min_;
		}

	}

	return false;
}

void BORA::Bin::CalcTrnTrans( BORA::BinDebug &_bin_debug, BORA::TrnImageDebug &_trn_debug )
{
	/* trn 디버깅 시작 */
	if(options_.debugfileoptions_.isDump_)
	{
		_trn_debug.StartDebug(_bin_debug.getRootDir(), bin_index_, createdTrnImg_number_);
	}

	// 블러 체크
	if(opt_img_blur_.use_blur_)
		trn_rule_.blur_level_ = opt_img_blur_.level_[blur_flag_];

	// 랜덤 블러 체크
	if(opt_img_.random_blur_.use_random_blur_)
	{
		const unsigned int &maxValue = opt_img_.random_blur_.max_mask_level_;
		const unsigned int &minValue = opt_img_.random_blur_.min_mask_level_;
		unsigned int value;
		
		do{ 
			value = minValue + ( rand() % (maxValue + 1 - minValue) );
		}while(value != 0 && value %2 == 0);

		trn_rule_.blur_level_ = value;
	}

	// 노이즈 체크
	if(opt_img_noise_.use_noise_)
		trn_rule_.noise_level_ = opt_img_noise_.level_[noise_flag_];

	// 랜덤 노이즈 체크
	if(opt_img_.random_noise_.use_random_noise_)
	{
		// 최소값 -1 ~ 최대값 랜덤 값 추출
		// 최소값 -1 이 나온다면 적용을 안시키는 옵션
		trn_rule_.noise_level_ = rand() % (opt_img_.random_noise_.max_intensity_diff_ + 2 - opt_img_.random_noise_.min_intensity_diff_) + opt_img_.random_noise_.min_intensity_diff_ -1;
		if(trn_rule_.noise_level_ < opt_img_.random_noise_.min_intensity_diff_)
			trn_rule_.noise_level_ = 0;
	}

	// 어파인 체크
	if(opt_img_affine_.use_affine_)
	{
		CalcTrnAffineTrans();
	}
	else
	{
		cv2DRotationMatrix(cano_img_.getCenterPosition(),
			rotation_value_, scale_value_,
			trn_rule_.H_);

		cv2DRotationMatrix(cano_img_.getCenterPosition(),
			-rotation_value_, 1/scale_value_,
			trn_rule_.invH_);
	}

	if(opt_debug_.isDump_)
	{
		_bin_debug.WriteTrnOneElement(createdTrnImg_number_, rotation_value_, scale_value_, trn_rule_.blur_level_, trn_rule_.noise_level_, affine_theta_value_, affine_phi_value_, affine_scaleX_value_, affine_scaleY_value_, opt_img_affine_.use_affine_);
		_trn_debug.WriteAffineInfo(rotation_value_, scale_value_, trn_rule_.blur_level_, trn_rule_.noise_level_, affine_theta_value_, affine_phi_value_, affine_scaleX_value_, affine_scaleY_value_, opt_img_affine_.use_affine_);
	}
}

void BORA::Bin::CalcTrnAffineTrans()
{
	double theta = rotation_value_ + affine_theta_value_;
	double phi = affine_phi_value_;
	double scale_x = scale_value_ * affine_scaleX_value_;
	double scale_y = scale_value_ * affine_scaleY_value_;


	//회전각 라디안으로 환산
	theta	= theta*BORA::PI/180.0;
	phi		= phi*BORA::PI/180.0;

	//역으로 확대 비율 구하기
	double scale_x_inv = 1 / scale_x;
	double scale_y_inv = 1 / scale_y;

	const CvPoint2D32f &center = cano_img_.getCenterPosition();

	double t_alpha = cos(theta);
	double t_beta = sin(theta);

	double t_alpha_inv = cos(-theta);
	double t_beta_inv = sin(-theta);

	double t3col[2] = {(((1-t_alpha)*center.x) - (t_beta*center.y)), (((t_beta*center.x) - ((1-t_alpha)*center.y)))};
	double t3col_inv[2] = {(((1-t_alpha_inv)*center.x) - (t_beta_inv*center.y)), (((t_beta_inv*center.x) - ((1-t_alpha_inv)*center.y)))};

	double p_alpha = cos(phi);
	double p_beta = sin(phi);

	double p_alpha_inv = cos(-phi);
	double p_beta_inv = sin(-phi);

	double p3col[2] = {(((1-p_alpha)*center.x) - (p_beta*center.y)), (((p_beta*center.x) - ((1-p_alpha)*center.y)))};
	double p3col_inv[2] = {(((1-p_alpha_inv)*center.x) - (p_beta_inv*center.y)), (((p_beta_inv*center.x) - ((1-p_alpha_inv)*center.y)))};

	double scaleX_inv = 1 / scale_x;
	double scaleY_inv = 1 / scale_y;

	//--정변환
	double A00 = -p_beta * scale_y * (p_beta_inv * t_alpha + p_alpha_inv * t_beta) + p_alpha * scale_x * (p_alpha_inv * t_alpha - p_beta_inv * t_beta);
	double A01 = p_alpha * scale_y * (p_beta_inv * t_alpha + p_alpha_inv * t_beta) + p_beta * scale_x * (p_alpha_inv * t_alpha - p_beta_inv * t_beta);
	double A02 = (-A00*center.x) + (-A01*center.y) + center.x;
	double A10 = p_alpha * scale_x * (-p_beta_inv * t_alpha - p_alpha_inv * t_beta) - p_beta * scale_y * (p_alpha_inv * t_alpha - p_beta_inv * t_beta);
	double A11 = p_beta * scale_x * (-p_beta_inv * t_alpha - p_alpha_inv * t_beta) + p_alpha * scale_y * (p_alpha_inv * t_alpha - p_beta_inv * t_beta);
	double A12 = (-A10*center.x) + (-A11*center.y) + center.y;

	CvMat *H = trn_rule_.H_;

	cvmSet(H, 0, 0, static_cast<double>(A00) );
	cvmSet(H, 0, 1, static_cast<double>(A01) );
	cvmSet(H, 1, 0, static_cast<double>(A10) );
	cvmSet(H, 1, 1, static_cast<double>(A11) );
	cvmSet(H, 0, 2, static_cast<double>(A02) );
	cvmSet(H, 1, 2, static_cast<double>(A12) );

	//--역변환
	double A_inv00 = (p_alpha * p_alpha_inv * scale_x_inv - p_beta * p_beta_inv * scale_y_inv) * t_alpha_inv - (p_alpha_inv * p_beta * scale_x_inv + p_alpha * p_beta_inv * scale_y_inv) * t_beta_inv;
	double A_inv01 = (p_alpha_inv * p_beta * scale_x_inv + p_alpha * p_beta_inv * scale_y_inv) * t_alpha_inv + (p_alpha * p_alpha_inv * scale_x_inv - p_beta * p_beta_inv * scale_y_inv) * t_beta_inv;
	double A_inv02 = -A_inv00*center.x - A_inv01*center.y + center.x;
	double A_inv10 = (-p_alpha * p_beta_inv * scale_x_inv - p_alpha_inv * p_beta * scale_y_inv) * t_alpha_inv - (-p_beta * p_beta_inv * scale_x_inv + p_alpha * p_alpha_inv * scale_y_inv) * t_beta_inv;
	double A_inv11 = (-p_beta * p_beta_inv * scale_x_inv + p_alpha * p_alpha_inv * scale_y_inv) * t_alpha_inv + (-p_alpha * p_beta_inv * scale_x_inv - p_alpha_inv * p_beta * scale_y_inv) * t_beta_inv;
	double A_inv12 = -A_inv10*center.x - A_inv11*center.y + center.y;

	CvMat *invH = trn_rule_.invH_;

	cvmSet(invH, 0, 0, static_cast<double>(A_inv00) );
	cvmSet(invH, 0, 1, static_cast<double>(A_inv01) );
	cvmSet(invH, 1, 0, static_cast<double>(A_inv10) );
	cvmSet(invH, 1, 1, static_cast<double>(A_inv11) );
	cvmSet(invH, 0, 2, static_cast<double>(A_inv02) );
	cvmSet(invH, 1, 2, static_cast<double>(A_inv12) );
}

// 1. FAST9에 의해 corners를 검색한다.
//    1. 검색된 corners가 target 이미지로 부터 patch를 추출 할 수 있는지 본다.
//    2. patch를 추출 할 수 있으면, Patch를 만든다.
//    3. 이때 저장되는 position은 cannonical image에서의 좌표이다.
//    4. 수집한 patches을 Feature로 학습시킨다.
void BORA::Bin::LearningFeatures( const BORA::Image &_target, BORA::FEATURES &_features )
{
	// 1. FAST9에 의해 corners를 검색한다.
	BORA::POSITIONS corners;
	BORA::FAST9::FindCorners(_target, opt_corner_.threshold_, corners);

	//    1. 검색된 corners가 target 이미지로 부터 patch를 추출 할 수 있는지 본다.
	// 현재 좌표에서의 모서리를 찾아서 해당 좌표가 
	const unsigned int IMAGE_OFFSET = 7;
	//const BORA::Position shift_corner_pos[4] = { BORA::Position(-7, -7), BORA::Position(7, -7), BORA::Position(-7, 7), BORA::Position(7, 7)};

	unsigned int patch_index(0);
	for(unsigned int i = 0 ; i < corners.size() ; ++i)
	{
		//    2. patch를 추출 할 수 있으면, Patch를 만든다.
		// training에서 발견된 좌표의 각 모서리를 
		// reference(원본좌표)로 갔을 때 본래 이미지의 범위를 벗어나지 않는다면 가능한 거.
		const BORA::Position &trn_pos = corners[i];
		bool isClear(true);

		// Dec. 2011, Min-Hyuk Sung
		// Modified
		// Check whether given features points are out of boundary (with offset) or not
		// 1. Check position on the given 'training image'
		if( trn_pos.x < IMAGE_OFFSET || trn_pos.x >= _target.getWidth() - IMAGE_OFFSET ||
			trn_pos.y < IMAGE_OFFSET || trn_pos.y >= _target.getHeight() - IMAGE_OFFSET )
		{
			isClear = false;
		}

		// 2. Check position on the 'canonical image' by transformation
		// (It is required since 'index map' is created with the 'canonical image' size)
		// (Position on the 'original image' can be ignored)
		else
		{
			BORA::Position ref_pos = trn_rule_.invH_ * trn_pos;

			if(bin_perspective_)
				ref_pos = (cano_rule_.invH_ * (cano_rule_.invpH_ * ref_pos)) - bin_img_start_pos_;
			else
				ref_pos = (cano_rule_.invH_ * ref_pos) - bin_img_start_pos_;


			if( ref_pos.x < IMAGE_OFFSET || ref_pos.x >= bin_img_scaled_size_.x - IMAGE_OFFSET ||
				ref_pos.y < IMAGE_OFFSET || ref_pos.y >= bin_img_scaled_size_.y - IMAGE_OFFSET )
			{
				isClear = false;
			}
		}
		/*
		// Should be modified
		for(unsigned int j = 0 ; j < 4 && isClear ; ++j)
		{
			BORA::Position trn_corner_pos = trn_pos + shift_corner_pos[j];
			BORA::Position ref_corner_pos;

			if(bin_perspective_)
			{
				ref_corner_pos = (cano_rule_.invH_ * (cano_rule_.invpH_ * (trn_rule_.invH_ * trn_corner_pos))) - opt_img_.original_img_start_position_;
			}
			else
			{
				ref_corner_pos = (cano_rule_.invH_ * (trn_rule_.invH_ * trn_corner_pos)) - opt_img_.original_img_start_position_;
			}

			// 문제 발생 지역!!!
			if( ref_corner_pos.x < 0 || ref_corner_pos.x >= opt_img_.original_img_size_.x ||
				ref_corner_pos.y < 0 || ref_corner_pos.y >= opt_img_.original_img_size_.y ||
				trn_corner_pos.x < 0 || trn_corner_pos.x >= _target.getWidth() ||
				trn_corner_pos.y < 0 || trn_corner_pos.y >= _target.getHeight())
			{
				isClear = false;
			}
		}
		*/

		if(isClear == false)
			continue;

		//    3. 여기서 저장되는 좌표는 trn_img에서의 좌표이다.
		//       추후에 Bin에 포함될때 좌표가 변경된다.
		// training image에서 발견된 좌표를 cannonical image로 돌려야 한다.
		BORA::Position cano_pos = trn_rule_.invH_ * trn_pos;

		// 패치를 만든다.
#if DIRECT_QUAN == 0
		BORA::Patch crnt_patch;
		BORA::INDEX crnt_index;
		
		BORA::ImageUtility::GetPatchAndIndex(_target, trn_pos, crnt_patch, crnt_index);
		crnt_patch.setPosition(trn_pos);
		crnt_patch.setIndexInfo(bin_index_, createdTrnImg_number_, patch_index++);

		BORA::Feature crnt_feature;
		crnt_feature.setPosition(trn_pos);
		crnt_feature.AddPatchAndIndex(crnt_patch, crnt_index);
#elif DIRECT_QUAN == 1
		BORA::Patch crnt_qpatch;
		BORA::INDEX crnt_index;

		BORA::ImageUtility::GetQPatchAndIndex(_target, trn_pos, crnt_qpatch, crnt_index);
		crnt_qpatch.setPosition(trn_pos);
		crnt_qpatch.setIndexInfo(bin_index_, createdTrnImg_number_, patch_index++);

		BORA::Feature crnt_feature;
		crnt_feature.setPosition(trn_pos);
		crnt_feature.AddQPatchAndIndex(crnt_qpatch, crnt_index);
#endif

		//    4. 수집한 patches을 Feature로 학습시킨다.
		crnt_feature.setParentBin(static_cast<int>(bin_index_));
		_features.push_back(crnt_feature);
	}
	
}

/* indexMap(==feature map)에 배웠던 features를 등록한다. */
// 1. 발견되었던 features를 모두 순회 한다.
//    1. indexMap의 해당 좌표 -> 2px 이내에 있는지 검사 한다.
//       1. 있으면 해당 좌표에 j th feature가 가지고 있는 patch와 index를 등록한다.
//       2. 없으면 새로 등록한다.
void BORA::Bin::RegistFeatures( BORA::FEATURES &_found_features )
{
	const BORA::Position shift_2px_pos[13] = { BORA::Position(  0,  0)
											 , BORA::Position(  0, -1), BORA::Position( -1,  0), BORA::Position(  0,  1), BORA::Position(  1,  0)
											 , BORA::Position(  1, -1), BORA::Position(  0, -2), BORA::Position( -1, -1), BORA::Position( -2,  0)
											 , BORA::Position( -1,  1), BORA::Position(  0,  2), BORA::Position(  1,  1), BORA::Position(  2,  0)};

	// 1. 발견되었던 features를 모두 순회 한다.
	for(unsigned int i = 0 ; i < _found_features.size() ; ++i)
	{
		bool isRegisted(false);

		BORA::Feature &regist_feature =	_found_features[i];
		BORA::Position &cano_pos = _found_features[i].getPosition_ref();
		cano_pos = trn_rule_.invH_ * cano_pos;
		
		BORA::Position search_pos;
		int existFeatureIndex(-1);

		//    1. indexMap의 해당 좌표 -> 2px 이내에 있는지 검사 한다.
		for(unsigned int j = 0 ; (j < 13) && (isRegisted == false) ; ++j)
		{
			//       1. 있으면 해당 좌표에 i th feature가 가지고 있는 patch와 index를 등록한다.
			search_pos = cano_pos + shift_2px_pos[j];

			// Dec. 2011, Min-Hyuk Sung
			// size(trn_img) == size(cano_img_)
			assert(search_pos.x >= 0 && search_pos.x < cano_img_.getWidth());
			assert(search_pos.y >= 0 && search_pos.y < cano_img_.getHeight());
			
			// TODO : 전에 search_pos의 값이 인덱스 맵 밖으로 벗어나는 경우도 발생하는데 이부분 주의
			if((existFeatureIndex = indexMap_(search_pos.x, search_pos.y)) != -1)
			{
				features_[existFeatureIndex].AddFeatureData(regist_feature);
				isRegisted = true;
			}
		}
		
		if(isRegisted)
			continue;

		// 2. 없으면 새로 등록한다.
		existFeatureIndex = features_.size();
		
		// Dec. 2011, Min-Hyuk Sung
		if( existFeatureIndex > MAX_NUM_BIN_FEATURES )
		{
			// Since memory allocation is NOT guaranteed when # of features exceeds the given limit,
			// Prevent to add generated features
			std::cerr << "Warning: [RegistFeatures] Too many feature are generated in a bin: ("
				<< MAX_NUM_BIN_FEATURES << ")" << std::endl
				<< "Additionally generated features are ignored." << std::endl;
			system("pause");
		}
		else
		{
			features_.push_back(regist_feature);
			indexMap_(cano_pos.x, cano_pos.y) = existFeatureIndex;
		}
	}
}

/* 추출한 features_ 개수가 많을 때 정렬해서 잘라 낸다. */
void BORA::Bin::SortNTruncation( BORA::BinDebug &_bin_debug )
{
	// 정렬
	std::sort(features_.begin(), features_.end(), SortPatchNumber);

	// 50개 까지 잘라서 디스크립터의 대상으로만 한다.
	/*
	const unsigned int truncation_amount(50);
	if(features_.size() < truncation_amount)
		return ;

	features_.swap(BORA::FEATURES(features_.begin(), features_.begin()+truncation_amount));
	//*/


	// opt_patch_.enumTruncationType_에 따른 truncation은 삭제
	//*
	// Dec. 2011, Min-Hyuk Sung
	// ---- //
	if( opt_patch_.enumTruncationType_ != BORA::Options::PatchTruncation::TRUNCATION_NONE
		&& bin_truncation_amount_ > 0)
	{
		// 'truncation_amount' 이후에 같은 patch 수집 개수가 있으면 모두 디스크립터의 대상으로 올리는 구조
		if(features_.size() < bin_truncation_amount_)
			return ;

		const unsigned int last_patches_count(features_[bin_truncation_amount_-1].getHIP_ref().getPatchesSize());
		//unsigned int amount(features_.size());
		unsigned int amount = bin_truncation_amount_;

		if( !opt_patch_.truncationExactCut )
		{
			for(unsigned int fi = bin_truncation_amount_-1 ; fi < features_.size() ; ++fi)
			{
				// truncation_amount뒤에 있는 feature의 patch수집 개수가 줄어들지 않았다면 같이 수집하게 한다.
				if(features_[fi].getHIP_ref().getPatchesSize() < last_patches_count)
				{
					amount = fi;
					break;
				}
			}
		}

		// Feb. 2012, Min-Hyuk Sung
		//features_.swap(BORA::FEATURES(features_.begin(), features_.begin()+amount));
		features_.resize(amount);
	}
	// ---- //

	//*/
	
	/*
	// 기존 truncation
	switch(opt_patch_.enumTruncationType_)
	{
	case BORA::Options::PatchTruncation::TRUNCATION_NONE:
		break;
	case BORA::Options::PatchTruncation::TRUNCATION_BY_AMOUNT:
		// 개수 만큼 살린다.
		// 옵션값보다 특징점 개수가 많은지 확인한다.
		{
			const unsigned int amount = static_cast<unsigned int>(opt_patch_.unionTruncationValue_.amount_);

			if(features_.size() > amount)
			{
				//많으면 차이 만큼 지운다.            
				features_.swap(BORA::FEATURES(features_.begin(), features_.begin()+amount));
			}
		}

		break;

	case BORA::Options::PatchTruncation::TRUNCATION_BY_RATIO:
		// %만큼 살린다.
		// 전체개수*(1-옵션값) 개수 만큼 지운다.
		{
			const unsigned int remain_number = static_cast<unsigned int>(features_.size() * ( 1.0 - opt_patch_.unionTruncationValue_.ratio_ ));
			features_.swap(BORA::FEATURES(features_.begin(), features_.begin()+remain_number));
		}
		break;
	}
	//*/
}

/* 추출한 features_를 descriptor로 표현한다. */
// 1. features를 순회한다.
//    1. feature가 가지고 있는 HIP를 노말라이즈 시킨다.
//    2. HIP의 각 자리가 Descriptor의 각 자리에 대응되는데 5%이하 이면 1, 아니면 0으로 세팅한다.
void BORA::Bin::FeaturesRepresent( BORA::TDESCRIPTORS &_descriptors )
{
	// 1. features를 순회한다.
	for(unsigned int i = 0 ; i < features_.size() ; ++i)
	{
		//    1. feature를 descriptor로 만들 계산을 한다.
		features_[i].Calc();

		//    2. 해당 Feature를 descriptor로 만든다.
		// Assign feature 'positions'

		BORA::Position ref_pos = features_[i].getPosition();

		//ref_pos = ref_pos + bin_crop_offset_;

		// Dec. 2011, Min-Hyuk Sung
		// 'opt_img_.original_img_start_position_' -> 'bin_img_start_pos_'
		if(bin_perspective_)
			ref_pos = (cano_rule_.invH_ * (cano_rule_.invpH_ * ref_pos)) - bin_img_start_pos_;
		else
			ref_pos = (cano_rule_.invH_ * ref_pos) - bin_img_start_pos_;

		// Jac. 2012, Min-Hyuk Sung
		// Scaling
		assert(bin_scale_ > 0);
		ref_pos.x = ref_pos.x / bin_scale_;
		ref_pos.y = ref_pos.y / bin_scale_;

		// Dec. 2011, Min-Hyuk Sung
		// When it comes to space partitions,
		// The starting point of space partitions should be considered
		if(opt_img_.spacepartitioning_.use_space_partitioning_)
		{
			ref_pos = ref_pos + bin_space_partition_start_pos_;
		}

		BORA::TDescriptor newDescriptor(features_[i], ref_pos);
		//BORA::TDescriptor newDescriptor(features_[i], cano_rule_.invH_, opt_img_.original_img_start_position_);

		// Dec. 2011, Min-Hyuk Sung
		// Since a descriptor and indexes are computed,
		// image patches are no longer required...
		features_[i].getHIP().clear();

		newDescriptor.setFeature(features_[i]);

		// Dec. 2011, Min-Hyuk Sung
		// Set bin index
		newDescriptor.setParentBin(bin_index_);

		// Dec. 2011, Min-Hyuk Sung
		try
		{
			_descriptors.push_back(newDescriptor);
		}
		catch (std::bad_alloc& ba)
		{
			std::cerr << "Error: [Bin::FeaturesRepresent] bad_alloc caught: " << ba.what() << std::endl;
			system("pause");
		}
	}
}