/*******************************************************************
 Trainging 클래스

 이미지 학습을 위한 최상위 클래스
 GeneralFunction 라이브러리에 디펜던시가 있음.
*******************************************************************/
#pragma once
#include "Database.h"
#include "ImageUtility.h"
#include "Image.h"
#include "GeneralDataType.h"
#include "Options.h"
#include "Parser.h"
#include "Bin.h"
#include "ImageUtility_TrainingEngine.h"
#include "DebugFunc.h"

#include <ctime>

// Dec. 2011, Min-Hyuk Sung
// Every 'NUM_BINS_IN_SEQ' bins are separately trained and saved into a file
#define START_BIN_IDX		0
#define NUM_BINS_IN_SEQ		360
//#define SAVE_CANONICAL_IMAGE

namespace BORA
{
	class Training
	{
	public:
		BORA::Options	options_;

	public:
		Training(void);
		~Training(void);

		bool Init(const std::string &_path);

		bool Calc(char _output_file[]);		//***** 학습 시작부분

	private:
		bool LoadReferenceImage( BORA::Image &_ori_img, BORA::Image &_bw_img );

		void CreateDatabase( BORA::Database	&_database, unsigned int start_bin_idx = 0, int num_bins = -1, int seq_idx = -1 );
		
		void CreateDescriptors( BORA::TDESCRIPTORS &descriptors, BORA::BININFOS &_bin_infos, unsigned int start_bin_idx = 0, int num_bins = -1 );

		void CreateIndexTable( const BORA::DESCRIPTORS &_descriptors, BORA::IndexTable &_indexTable);
		
	private:
		BORA::Image			ori_img_;
		BORA::Image			bw_img_;
		BORA::TrainingDebug training_debug_;

		unsigned int	ori_img_width_;
		unsigned int	ori_img_height_;

		void SetImageSize(const BORA::Image &_ori_img);

		inline unsigned int getSpacePartitionsForScale(
			double bin_scale_value,
			double bin_space_partition_basic_ratio);

		void UpdateSpacePartitioningInterval(
			double bin_scale_value,								// in
			double bin_space_partition_basic_ratio,			// in
			unsigned int &bin_space_partition_num,			// out
			BORA::Position &bin_space_partition_interval,	// out
			unsigned int &bin_truncation_amount,			// out
			bool verbose = false);

		void InitializeBin();
		BININFOS				bin_info_list_;
		unsigned int			num_total_bins_;
	};
}