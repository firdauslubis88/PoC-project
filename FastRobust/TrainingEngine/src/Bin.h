/*******************************************************************
 Bin Ŭ����

 ���������δ� �ش� viewport������ ��ũ���͵��� �����ϴ� Ŭ����.
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
		const BORA::Options		&options_;		// ���� �ɼǿ� ���� ����

		unsigned int			bin_index_;		// �� ��ü �ε���
		BORA::Image				cano_img_;		// cannonical image
		BORA::ATransformRule	cano_rule_;		// reference image -> cannonical image�� ����� �߻��Ǵ� ��Ģ

		BORA::FEATURES			features_;		// features�� ����
		BORA::IndexMap			indexMap_;		// features�� ��

	public:
		Bin(unsigned int _bin_index, 
			const BORA::Image &_bw_img, 
			const BORA::BinInfo &_bin_info,
			const BORA::Options &_options);
		~Bin(void);

		void Calc(BORA::TDESCRIPTORS &_descriptors, const std::string &_debug_path);

	public:
		/* hasNext�� ���� falg ���� �ʱ�ȭ */
		void InitImageTransformVariation();
		
		/* ���� training image�� ���� �Ҽ� ������ �ش� �������� ����. */
		bool hasNextTrnImg(BORA::BinDebug &_bin_debug, BORA::TrnImageDebug &_trn_debug);

		/* target image(==training image)�κ��� feature�� ����. */
		void LearningFeatures(const BORA::Image &_target, BORA::FEATURES &_features);

		/* indexMap(==feature map)�� ����� features�� ����Ѵ�. */
		void RegistFeatures(BORA::FEATURES &_found_features);

		/* ������ features_ ������ ���� �� �����ؼ� �߶� ����. */
		void SortNTruncation(BORA::BinDebug &_bin_debug);

		/* ������ features_�� descriptor�� ǥ���Ѵ�. */
		void FeaturesRepresent(BORA::TDESCRIPTORS &_descriptors);

		/* reference image <-> cannonical image �� ATransfromRule�� ���Ѵ�. */
		void CalcCannoTrans(const BORA::Image &_ref_img);

		/* cannonical image <-> training image �� ATransformRule�� �Ϲݿ� ���� ���Ѵ�. */
		void CalcTrnTrans(BORA::BinDebug &_bin_debug, BORA::TrnImageDebug &_trn_debug);

		/* cannonical image <-> training image �� ATransformRule�� affine�� ���� ���Ѵ�. */
		void CalcTrnAffineTrans();

	private:
		/* hasNextTrn�� ���� flags */
		unsigned int			rotation_flag_;
		unsigned int			rotation_max_;
		unsigned int			scale_flag_;

		unsigned int			blur_flag_;
		unsigned int			noise_flag_;

		// affine ���� ���� flags
		unsigned int			affine_random_flag_;

		/* ������ ��ȯ�� ����� ������ */
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

		/* ����� ������ ���̴� ������ */
		unsigned int			createdTrnImg_number_;		// �����ߴ� training image�� ����


		/* ��뿡 ���� ���� ���۷��� ��� */
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