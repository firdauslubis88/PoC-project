/*******************************************************************
 Bin 클래스

 최종적으로는 해당 viewport에서의 디스크립터들을 생성하는 클래스.
*******************************************************************/
#pragma once
#include "FAST9.h"
#include "Image.h"
#include "Feature.h"
#include "TrainingDescriptor.h"
#include "GeneralDataType.h"
#include "Options.h"
#include "ImageUtility_TrainingEngine.h"
#include "IndexMap.h"
#include "DebugFunc.h"
#include "BinInfo.h"

#include <algorithm>
#include <ctime>

// Dec. 2011, Min-Hyuk Sung
// Set the maximum number of generated features for each bin
// After generating features, some of them features can be pruned...
// ('opt_patch_.unionTruncationValue_.amount_')
#define MAX_NUM_BIN_FEATURES	10000

namespace BORA
{
	class Bin : public BinInfo
	{
		// Dec. 2011, Min-Hyuk Sung
		friend class Training;

	private:
		const BORA::Image		&bw_img_;		// reference image
		const BORA::Options		&options_;		// 각종 옵션에 관련 정보

		unsigned int			bin_index_;		// 빈 자체 인덱스
		BORA::Image				cano_img_;		// cannonical image
		BORA::ATransformRule	cano_rule_;		// reference image -> cannonical image로 변경시 발생되는 규칙

		BORA::FEATURES			features_;		// features의 정보
		BORA::IndexMap			indexMap_;		// features의 맵

	public:
		Bin(unsigned int _bin_index, 
			const BORA::Image &_bw_img, 
			const BORA::BinInfo &_bin_info,
			const BORA::Options &_options);
		~Bin(void);

		void Calc(BORA::TDESCRIPTORS &_descriptors, const std::string &_debug_path);

	public:
		/* hasNext를 위한 falg 변수 초기화 */
		void InitImageTransformVariation();
		
		/* 다음 training image를 생성 할수 있으면 해당 변수들을 세팅. */
		bool hasNextTrnImg(BORA::BinDebug &_bin_debug, BORA::TrnImageDebug &_trn_debug);

		/* target image(==training image)로부터 feature를 배운다. */
		void LearningFeatures(const BORA::Image &_target, BORA::FEATURES &_features);

		/* indexMap(==feature map)에 배웠던 features를 등록한다. */
		void RegistFeatures(BORA::FEATURES &_found_features);

		/* 추출한 features_ 개수가 많을 때 정렬해서 잘라 낸다. */
		void SortNTruncation(BORA::BinDebug &_bin_debug);

		/* 추출한 features_를 descriptor로 표현한다. */
		void FeaturesRepresent(BORA::TDESCRIPTORS &_descriptors);

		/* reference image <-> cannonical image 의 ATransfromRule을 구한다. */
		void CalcCannoTrans(const BORA::Image &_ref_img);

		/* cannonical image <-> training image 의 ATransformRule을 일반에 의해 구한다. */
		void CalcTrnTrans(BORA::BinDebug &_bin_debug, BORA::TrnImageDebug &_trn_debug);

		/* cannonical image <-> training image 의 ATransformRule을 affine에 의해 구한다. */
		void CalcTrnAffineTrans();

	private:
		/* hasNextTrn을 위한 flags */
		unsigned int			rotation_flag_;
		unsigned int			rotation_max_;
		unsigned int			scale_flag_;

		unsigned int			blur_flag_;
		unsigned int			noise_flag_;

		// affine 랜덤 추출 flags
		unsigned int			affine_random_flag_;

		/* 실제로 변환에 적용될 데이터 */
		double			rotation_value_;
		double			scale_value_;

		unsigned int	blur_value_;
		unsigned int	noise_value_;

		double			affine_theta_value_;
		double			affine_phi_value_;
		double			affine_scaleX_value_;
		double			affine_scaleY_value_;

		BORA::ATransformRule		trn_rule_;

		// Dec. 2011, Min-Hyuk Sung
		// Large Scale Image - Space Partitioning
		// ---- //
		// Starting position and image size of given image partition
		// The given image partition is enclosed with expanded background (canonical image)
		// Thus, 'bin_img_start_pos_' MAY NOT be (0, 0)
		BORA::Position	bin_img_start_pos_;
		BORA::Position	bin_img_scaled_size_;

		// Background can be reduced after scaling...
		// The offset of croped background is recorded...
		//BORA::Position	bin_crop_offset_;
		// ---- //

		/* 디버깅 용으로 쓰이는 데이터 */
		unsigned int			createdTrnImg_number_;		// 생성했던 training image의 개수


		/* 사용에 편리를 위한 레퍼런스 등록 */
		const BORA::Options::Image							&opt_img_;
		const BORA::Options::Image::Rotation				&opt_img_rotation_;
		const BORA::Options::Image::Scale					&opt_img_scale_;
		const BORA::Options::Image::Blur					&opt_img_blur_;
		const BORA::Options::Image::Noise					&opt_img_noise_;
		const BORA::Options::Image::Affine					&opt_img_affine_;
		const BORA::Options::Image::Affine::Factor			&opt_img_affine_factor_;
		const BORA::Options::Image::Affine::RandomRotation	&opt_img_affine_random_rotation_;
		const BORA::Options::Image::Affine::RandomScale		&opt_img_affine_random_scale_;

		const BORA::Options::CornerDetection				&opt_corner_;
		const BORA::Options::DebugFileOptions				&opt_debug_;
		const BORA::Options::Indexing						&opt_indexing_;
		const BORA::Options::PatchTruncation				&opt_patch_;
	};
}