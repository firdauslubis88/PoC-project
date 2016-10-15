/*******************************************************************
 Options ����ü

 ���� �ɼǿ� ���� �� �� ������
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "Database.h"

// Dec. 2011, Min-Hyuk Sung
//#define OPENCV_VERSION_220	// OpenCV 2.2.0 or above

// -- DEFINES -- //
// Fast Corner
#define DEFAULT_CORNERDETECTION_THRESHOLD	80

// Truncation
#define DEFAULT_PATCHTRUNCATION_AMOUNT		50
#define DEFAULT_PATCHTRUNCATION_EXACT_CUT	false

// Rotation
#define DEFAULT_IMAGE_BASIC_ROTATION		10.0
#define DEFAULT_IMAGE_BASIC_ROTATION_MIN	0.0
#define DEFAULT_IMAGE_BASIC_ROTATION_MAX	360.0

#define DEFAULT_IMAGE_DETAIL_ROTATION		1.0
#define DEFAULT_IMAGE_DETAIL_ROTATION_MIN	-5.0
#define DEFAULT_IMAGE_DETAIL_ROTATION_MAX	5.0

// Scale
#define DEFAULT_IMAGE_SCALE_MAX				1.0
#define DEFAULT_IMAGE_BASIC_SCALE			0.8
#define DEFAULT_IMAGE_BASIC_SCALE_NUM		7

#define DEFAULT_IMAGE_DETAIL_SCALE			0.9
#define DEFAULT_IMAGE_DETAIL_SCALE_NUM		2
// ---- //


namespace BORA
{
	// Dec. 2011, Min-Hyuk Sung
	// struct -> class, Make a constructor
	//struct Options
	class Options
	{
	public:
		std::string		img_path_;					// end with '\\'
		std::string		img_file_name_ext_;	// �̹��� ���ϸ�(Ȯ���� ����)
		std::string		img_file_name_;				// �̹��� ���ϸ�(Ȯ���� �� ����)
		unsigned int	saveType;					// ��� ���������� ���õ� ����
		
		// ����� ����
		struct DebugFileOptions
		{
			bool isDump_;
			bool dumpEachTrainingImage_;
			bool dumpFeatureData_;
			bool dumpDescriptionAsCSV_;
		} debugfileoptions_;

		// FAST9 ����
		struct CornerDetection
		{
			int wishFeatureNumber_;
			unsigned int threshold_; // corner detection�� �� ���ڰ�
		} cornerdetection_;

		// INDEXING ����
		struct Indexing
		{
			enum IndexType {
				INDEX_5		= 5,
				INDEX_13	= 13,
			};

			IndexType enumIndexing_;
		} indexing_;

		// PatchTruncation ����
		struct PatchTruncation
		{
			enum TruncationType
			{
				TRUNCATION_NONE			= 0,
				TRUNCATION_BY_AMOUNT	= 1,
				//TRUNCATION_BY_RATIO	= 2,
				TRUNCATION_NUM			= 3
			};

			TruncationType enumTruncationType_;

			union TruncationValueType
			{
				unsigned int	amount_;
				double	ratio_;
			};

			TruncationValueType unionTruncationValue_;

			// Dec. 2011, Min-Hyuk Sung
			bool truncationExactCut;

		} patchtruncation_;

		// �̹��� ��ȯ ����
		struct Image
        {
			BORA::Position original_img_size_;
			// Dec. 2011, Min-Hyuk Sung
			// Removed
			//BORA::Position original_img_start_position_;	// ExpandImage �� �� �����ؾ� �Ѵ�.

            struct TransformFilter
            {
                //cvWarpAffine�Լ� ���� ����
                //default CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS
                unsigned int interpolation_;	//sub-pixel interpolation method[0, 4]
                unsigned int wraping_falg_;		//warping flag[8 | 16]

            } transformfilter_;

            struct Rotation
            {
                double basic_rotation_;     
                double basic_rotation_min_; 
                double basic_rotation_max_; 

                double detail_rotation_;    
                double detail_rotation_min_;
                double detail_rotation_max_;
            } rotation_;

            struct Scale
            {
                double			scale_max_;  

                double			basic_scale_;
                unsigned int    basic_scale_num_; 

                double			detail_scale_;    
                unsigned int    detail_scale_num_;
            } scale_;

			struct RandomBlur
			{
				bool			use_random_blur_;
				unsigned int	min_mask_level_;
				unsigned int	max_mask_level_;
			} random_blur_;

            struct Blur
            {     
                bool						use_blur_;
                bool						apply_blur_first_;
                std::vector<unsigned int>	level_;
            } blur_;

            struct Noise
            {        
                bool use_noise_;
                std::vector<unsigned int> level_;
            } noise_;

			struct RandomNoise
			{
				bool			use_random_noise_;
				unsigned int	min_intensity_diff_;
				unsigned int	max_intensity_diff_;
			} random_noise_;

			struct Perspective
			{
				bool use_perspective_;
			} perspective_;

            struct Affine
            {
				bool use_affine_;

                struct Factor
                {
                    double theta_min_;
                    double theta_max_;

                    double phi_min_;
                    double phi_max_;

                    double scale_axis_x_min_;
                    double scale_axis_x_max_;

                    double scale_axis_y_min_;
                    double scale_axis_y_max_;

                    /********** �߰��� �κ� **********/
                    // 0 ���ϸ� �������� ������� �� ���ڸ� �����Ѵ�.
                    // 1 �̻��̸� �������� ����� ���ڵ��� �������� �ؼ� ���� �����Ѵ�.
                    unsigned int random_value_extract_time_;
                } factor_;

                struct RandomRotation
                {
                    bool			theta_random_;
                    //unsigned int	theta_times;	//***** ���� ���� ���
                    double			theta_increase_level_;

                    bool			phi_random_;
                    //unsigned int	phi_times;		//***** ���� ���� ���
                    double			phi_increase_level_;
                } randomrotation_;

                struct RandomScale
                {
                    bool			axis_x_random_;
                    //unsigned int	axis_x_times;	//***** ���� ���� ���
                    double			axis_x_increase_level_;

                    bool			axis_y_random_;
                    //unsigned int	axis_y_times;	//***** ���� ���� ���
                    double			axis_y_increase_level_;
                } randomscale_;
            } affine_;

			// Dec. 2011, Min-Hyuk Sung
			// Large Scale Image - Space Partitioning
			struct SpatialParitioning
			{
				bool				use_space_partitioning_;
				unsigned int		space_partion_basic_num_;
			} spacepartitioning_;

            
        } image_;

		// Dec. 2011, Min-Hyuk Sung
		// struct -> class, Make a constructor
		Options();

		void Init();
		bool CheckOptionsRule();
	};

}

std::ostream &operator<<(std::ostream &_out, const BORA::Options &_opts);