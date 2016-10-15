#include "StdAfx.h"
#include "Training.h"


BORA::Training::Training(void)
	// Dec. 2011, Min-Hyuk Sung
	// Large Scale Image - Space Partitioning
	// ---- //
	: ori_img_width_(0)
	, ori_img_height_(0)
	, num_total_bins_(0)
	// ---- //
{
}


BORA::Training::~Training(void)
{
	bin_info_list_.clear();
}

/* 맴버변수 options_를 초기화 한다. */
bool BORA::Training::Init( const std::string &_path )
{
	std::cout << "Training Init() start...\n";
	if(BORA::Parser::ParseTrainingOptions(_path, options_) == false)
		return false;

	if(options_.debugfileoptions_.isDump_)
	{
		training_debug_.StartDebug(options_.img_path_, options_.img_file_name_);
	}

	if(LoadReferenceImage(ori_img_, bw_img_) == false)
	{
		std::cerr << "error: [LoadReferenceImage] loading file is failed." << std::endl;
		return false;
	}

	//database_.setImageSize(options_.image_.original_img_size_);
	
	SetImageSize(ori_img_);
	InitializeBin();

	std::cout << "Training Init() completed...\n";
	return true;
}

/* 계산을 시작한다. */
// 1. reference image를 불러온다.
// 2. 이미지를 학습 시켜 Database를 만든다.
// 3. 파일로 기록한다.
bool BORA::Training::Calc(char _output_file[])
{
	clock_t time_start;
	clock_t time_end;

	std::cout << "Training Calc() Start...\n";

	// Dec, 2011, Min-Hyuk Sung
	// 
	for(unsigned int BIN_IDX = START_BIN_IDX, SEQ_IDX = (START_BIN_IDX/NUM_BINS_IN_SEQ);
		BIN_IDX < num_total_bins_;
		BIN_IDX += NUM_BINS_IN_SEQ, SEQ_IDX++)
	{

		// 1. 이미지를 학습 시켜 Database를 만든다.
		BORA::Database	database_;
		database_.setImageSize(options_.image_.original_img_size_);

		time_start = clock();
		CreateDatabase(database_, BIN_IDX, NUM_BINS_IN_SEQ, SEQ_IDX);
		time_end = clock();
		printf("Generation Time\t: %lfs\n", (time_end - time_start)/1000.0);

		// 2. 파일로 기록한다.
		std::stringstream database_file_name;
		std::stringstream option_info_name;

		database_file_name << options_.img_path_ + options_.img_file_name_ << ".dat";
		option_info_name << options_.img_path_ + options_.img_file_name_ << " training option.txt";

		sprintf(_output_file, "%s", database_file_name.str().c_str());

		// Dec. 2011, Min-Hyuk Sung
		// More than one sequences
		if( NUM_BINS_IN_SEQ < num_total_bins_ )
		{
			database_file_name << "." << std::setfill('0') << std::setw(3) << SEQ_IDX;
			option_info_name << "." << std::setfill('0') << std::setw(3) << SEQ_IDX;
		}
		std::cout << "Save to '" << database_file_name.str() << "'..." << std::endl;

		database_.WriteToFile(database_file_name.str(), options_.saveType);

		BORA::Parser::WriteTrainingOptions(option_info_name.str(), options_, database_.getDescsRef().size());

		// Training 디버깅 끝
		if(options_.debugfileoptions_.isDump_)
		{
			training_debug_.FinishDebug();
		}

		// 테스트
		BORA::Database testDatabase;
		testDatabase.ReadFromFile(database_file_name.str());

		if(database_ == testDatabase)
		{
			std::cout << "Read/Write\t: Success" << std::endl;
		}
		else
		{
			std::cout << "database_ and testDatabase either different. check please" << std::endl;
		}
	}

	
	std::cout << "Training Calc() completed...\n";
	return true;
}

bool BORA::Training::LoadReferenceImage( BORA::Image &_ori_img, BORA::Image &_bw_img )
{
	const std::string img_full_path	= options_.img_path_ + options_.img_file_name_ext_;
	std::cout << "Loading '" << img_full_path << "'..." << std::endl;
	
	// 이미지를 불러온다.
	if(_ori_img.LoadFromFile(img_full_path) == false)
		return false;
	
	// 이미지의 정보를 옵션에 기록한다.
	options_.image_.original_img_size_(_ori_img.getWidth(), _ori_img.getHeight());

	//BORA::DebugFunc::Show(_ori_img);
	
	// 레퍼런스 이미지는 흑백화 시켜버린다.
	//BORA::Image _bw_img;
	BORA::ImageUtility::ColorImageToGray(_ori_img, _bw_img);

	// Dec. 2011, Min-Hyuk Sung
	// Moved to BORAL::Bin
	/*
	BORA::Position &_shift_pos = options_.image_.original_img_start_position_;
	BORA::ImageUtility::ExpandImage(_bw_img, _ref_img, _shift_pos);
	*/

	//BORA::DebugFunc::Show(_ori_img);
	return true;
}

/* ref_img를 학습시켜 데이터베이스를 구축하여 databse_에 저장 */
// 1. 이미지로부터 디스크립터들을 만든다.
// 2. 생성된 디스크립터들을 가지고 인덱스 테이블을 만든다.
void BORA::Training::CreateDatabase( BORA::Database &_database, unsigned int start_bin_idx, int num_bins, int seq_idx )
{
	BORA::TDESCRIPTORS	tdescriptors;
	BORA::DESCRIPTORS	descriptors;
	BORA::IndexTable	indexTable;

	BORA::BININFOS		&bin_infos = _database.getBinInfos_ref();

	//clock_t time_start;

	//time_start = clock();
	// 1. 이미지로부터 디스크립터들을 만든다.
	CreateDescriptors(tdescriptors,  bin_infos, start_bin_idx, num_bins);
	//printf("CreateDescriptors in CreateDatabase : %lfs\n", (clock() - time_start)/1000.0);
	
	/* descriptor 디버깅 시작 & 끝 */
	if(options_.debugfileoptions_.isDump_)
	{
		BORA::DescriptorsDebug desc_debug;
		desc_debug.StartDebug(training_debug_.getRootDir());
		desc_debug.WriteDescriptorsTable(tdescriptors);
		desc_debug.FinishDebug();
	}
	
	// Dec. 2011, Min-Hyuk Sung
	/*
	descriptors.resize(tdescriptors.size());
	for(unsigned int i = 0 ; i < tdescriptors.size() ; ++i)
		tdescriptors[i].ConvertToDescirptor(descriptors[i]);
	*/
	// Vector -> List ([] operator does not work)
	// Each element removes right after conversion
	for(BORA::TDESCRIPTORS::iterator it = tdescriptors.begin(); it != tdescriptors.end();
		it = tdescriptors.erase(it))
	{
		BORA::Descriptor desc;
		it->ConvertToDescirptor(desc);
		descriptors.push_back(desc);
	}

	// 2. 생성된 디스크립터들을 가지고 인덱스 테이블을 만든다.
	CreateIndexTable(descriptors, indexTable);

	/* index table 디버깅 시작 & 끝, training 디버깅 */
	if(options_.debugfileoptions_.isDump_)
	{
		// 원본이미지와 흑백 원본이미지에 descriptors의 좌표를 칠한 이미지를 저장한다.
		BORA::Image gray_original_img;
		BORA::POSITIONS positions;
		BORA::ImageUtility::ColorImageToGray(ori_img_, gray_original_img);
		BORA::ImageUtility::GrayImageToColor(gray_original_img, gray_original_img);
		BORA::DebugFunc::DescriptosPoints(descriptors, positions);

		BORA::ImageUtility::DrawPoints(gray_original_img, positions, gray_original_img);
		
		training_debug_.SaveImages(ori_img_, gray_original_img);

		// 디스크립터 크기를 기록한다.
		training_debug_.WriteDescriptorsSize(descriptors.size());

		// 인덱스 테이블에 대한 개요를 기록한다.
		training_debug_.WriteIndexTable(indexTable);

		// 인덱스 테이블에 대한 페이지를 기록한다.
		BORA::IndexTableDebug table_debug;
		table_debug.StartDebug(training_debug_.getRootDir());
		table_debug.WriteIndexTableInfo(indexTable);
		table_debug.FinishDebug();

		// 디스크립터정보를 CSV 포멧으로 기록한다.
		BORA::DebugFunc::WriteDescriptorsToCSV(options_.img_path_+options_.img_file_name_+".csv", descriptors);
	}

	// 마지막에 디스크립터 담긴 위치를 그린 이미지 쓰기
	BORA::Image img_with_desc;
	BORA::ImageUtility::ColorImageToGray(ori_img_, img_with_desc);
	BORA::ImageUtility::GrayImageToColor(img_with_desc, img_with_desc);
	BORA::POSITIONS descriptors_positions;
	BORA::DebugFunc::DescriptosPoints(descriptors, descriptors_positions);
	BORA::ImageUtility::DrawPoints(img_with_desc, descriptors_positions, img_with_desc);

	// Dec. 2011, Min-Hyuk Sung
	if(seq_idx < 0)
	{
		BORA::ImageUtility::SaveImage(img_with_desc, options_.img_path_ + options_.img_file_name_ + "_desc.jpg");	
	}
	else
	{
		std::stringstream img_with_desc_path;
		img_with_desc_path << options_.img_path_ + options_.img_file_name_ + "_desc_" << std::setfill('0') << std::setw(3) << seq_idx << ".jpg";
		BORA::ImageUtility::SaveImage(img_with_desc, img_with_desc_path.str());
	}

	_database.setDatabase(descriptors, indexTable);

	std::cout << "# of Descriptor\t: " << descriptors.size() << std::endl;
}

/* 이미지를 가지고 디스크립터들을 만든다. */
void BORA::Training::CreateDescriptors( BORA::TDESCRIPTORS &descriptors, BORA::BININFOS &_bin_infos,
	 unsigned int start_bin_idx, int num_bins)
{
	if( start_bin_idx >= num_total_bins_ )
	{
		std::cerr << "warning: [Training::CreateDescriptors] Wrong bin range: ("
			<< start_bin_idx << " >= " << num_total_bins_ << ")" << std::endl
			<< "The bin range is reset as the whole bin list..." << std::endl;
		system("pause");

		start_bin_idx = 0;
		num_bins = num_total_bins_;
	}

	// 'num_bin_idx < 0' indicates at the end of the bin list
	if( num_bins < 0 || (start_bin_idx + num_bins) > num_total_bins_ )
		num_bins = num_total_bins_ - start_bin_idx;



	/* bins 디버깅 시작 */
	BORA::BinsDebug bins_debug;
	if(options_.debugfileoptions_.isDump_)
	{
		bins_debug.StartDebug(training_debug_.getRootDir() + "Bins");
		bins_debug.StartTable();
	}

	// 1. 다음 생성할 빈이 있으면 빈을 생성한다.
	for(unsigned int bin_index = start_bin_idx; bin_index < (start_bin_idx + num_bins); bin_index++)
	{
		// Dec. 2011, Min-Hyuk Sung
		BORA::BinInfo &bin_info = bin_info_list_[bin_index];

		// Jac. 2012, Min-Hyuk Sung
		unsigned int local_bin_index = _bin_infos.size();
		BORA::Bin bin = BORA::Bin(local_bin_index, bw_img_, bin_info, options_);
		
		if(options_.debugfileoptions_.isDump_)
		{
			bins_debug.WriteOneElem(bin_index, bin_info.bin_rotation_, bin_info.bin_scale_);
		}
		
		// 2. 해당 빈에서 생성되는 디스크립터들을 쌓아 놓는다.
		bin.Calc(descriptors, bins_debug.getRootDir());
		_bin_infos.push_back(bin_info);

		// Dec. 2011, Min-Hyuk Sung
#ifdef SAVE_CANONICAL_IMAGE
		std::stringstream cano_img_path;
		cano_img_path << options_.img_path_ << "bin/"
			<< options_.img_file_name_ + "_bin_"
			<< std::setfill('0') << std::setw(6) << bin_index << ".bmp";
		BORA::ImageUtility::SaveImage(bin.cano_img_, cano_img_path.str());
#endif

		std::cout << "bin : " << setw(6) << bin_index+1 << " / " << num_total_bins_ << " (" << num_bins << ") "
			<< "  (# of descriptors: " << descriptors.size() << ")" << std::endl;
	}

	/* bins 디버깅 끝 */
	if(options_.debugfileoptions_.isDump_)
	{
		bins_debug.FinishTable();
		bins_debug.FinishDebug();
	}
}

/* 디스크립터들을 가지고 인덱스 테이블을 만들어서 반환한다. */
void BORA::Training::CreateIndexTable( const BORA::DESCRIPTORS &_descriptors, BORA::IndexTable &_indexTable )
{
	_indexTable.Calc(_descriptors);
}

// Dec. 2011, Min-Hyuk Sung
// Large Scale Image - Space Partitioning
// ---- //
void BORA::Training::SetImageSize(const BORA::Image &_ori_img)
{
	ori_img_width_  = _ori_img.getWidth();
	ori_img_height_ = _ori_img.getHeight();
}

unsigned int BORA::Training::getSpacePartitionsForScale(
	double bin_scale_value,
	double bin_space_partition_basic_ratio )
{
	if( !options_.image_.spacepartitioning_.use_space_partitioning_ 
		|| options_.image_.spacepartitioning_.space_partion_basic_num_ <= 1)
	{
		return 1;
	}

	assert(bin_space_partition_basic_ratio > 0
		&& bin_space_partition_basic_ratio <= 1);

	unsigned int num_space_partitions = 1;

	// Only when current scale is bigger than one partition scale
	if( bin_scale_value > bin_space_partition_basic_ratio )
		num_space_partitions = (unsigned int)((bin_scale_value / bin_space_partition_basic_ratio) + 0.5);

	return num_space_partitions;
}

void BORA::Training::UpdateSpacePartitioningInterval(
	double bin_scale_value,							// in
	double bin_space_partition_basic_ratio,			// in
	unsigned int &bin_space_partition_num,			// out
	BORA::Position &bin_space_partition_interval,	// out
	unsigned int &bin_truncation_amount,			// out
	bool verbose)
{
	if( !options_.image_.spacepartitioning_.use_space_partitioning_ 
		|| options_.image_.spacepartitioning_.space_partion_basic_num_ <= 1)
	{
		return;
	}

	// Set 'bin_space_partitioning_num_' for current scale level (Rounding)
	bin_space_partition_num = getSpacePartitionsForScale(bin_scale_value, bin_space_partition_basic_ratio);

	// A => B <=> not A or B
	assert( bin_scale_value != options_.image_.scale_.scale_max_
		|| bin_space_partition_num == options_.image_.spacepartitioning_.space_partion_basic_num_ );

	// !! NOTICE !! //
	// A partition interval is obtained with the 'original' image size, not the 'scaled' size
	// since a space partition will be obtained on the original image in BORA::Bin
	// Use 'ceil' to make sure that the set of partitions cover the whole image
	unsigned int x_intv = (unsigned int)ceil((double)ori_img_width_  / bin_space_partition_num);
	unsigned int y_intv = (unsigned int)ceil((double)ori_img_height_ / bin_space_partition_num);

	assert(x_intv * bin_space_partition_num >= ori_img_width_ );
	assert(y_intv * bin_space_partition_num >= ori_img_height_);

	bin_space_partition_interval = BORA::Position(x_intv, y_intv);

	// # of features of each bin should be modified
	double bin_space_partition_curr_ratio = (bin_scale_value / bin_space_partition_num);

	if(options_.patchtruncation_.enumTruncationType_
		!= BORA::Options::PatchTruncation::TRUNCATION_NONE)
	{
		double bin_scale_ratio = bin_space_partition_curr_ratio / bin_space_partition_basic_ratio;
		unsigned int basic_truncation_amount = options_.patchtruncation_.unionTruncationValue_.amount_;
		// Rounding
		bin_truncation_amount = (unsigned int)((basic_truncation_amount * bin_scale_ratio) + 0.5);
	}
	// ...

	// VERBOSE
	if(verbose)
	{
		printf("----\n");
		printf("Scale Factor:\t%lf\n", bin_scale_value);
		printf("Partition Ratio(Default):\t%lf\n", bin_space_partition_basic_ratio);
		printf("Partition Ratio(Current):\t%lf\n", bin_space_partition_curr_ratio);
		printf("\n");

		printf("# of Features:\t\t%d\n", bin_truncation_amount);
		printf("# of Partitions:\t%dx%d\n", bin_space_partition_num, bin_space_partition_num);
		printf("Partition Size:\t\t(%d, %d)\n", x_intv, y_intv);
		printf("----\n\n");
	}
}

void BORA::Training::InitializeBin()
{
	bin_info_list_.clear();
	num_total_bins_  = 0;


	// -- Initialize -- //

	bool bin_perspective_value	= false;
	double bin_scale_value		= options_.image_.scale_.scale_max_;
	double bin_rotation_value	= 0;

	unsigned int bin_perspec_num = ( options_.image_.perspective_.use_perspective_ ) ? (2) : (1);
	unsigned int bin_scale_num = options_.image_.scale_.basic_scale_num_;
	unsigned int bin_rotation_num =
		(unsigned int)((options_.image_.rotation_.basic_rotation_max_
		- options_.image_.rotation_.basic_rotation_min_)
		/ options_.image_.rotation_.basic_rotation_);


	// Space Partitioning
	// !! NOTICE !! //
	// The start and end position and interval of space partition is obtained
	// with the 'original' image size, not the 'scaled' size
	// since a space partition will be obtained on the original image in BORA::Bin
	BORA::Position bin_space_partition_interval(0, 0);
	BORA::Position bin_space_partition_start_pos(0, 0);
	BORA::Position bin_space_partition_end_pos(0, 0);

	double bin_space_partition_basic_ratio = 1.0;
	unsigned int bin_space_partition_num = 1;

	if( options_.image_.spacepartitioning_.use_space_partitioning_
		&& options_.image_.spacepartitioning_.space_partion_basic_num_ >= 1 )
	{
		bin_space_partition_basic_ratio
			= options_.image_.scale_.scale_max_
			/ options_.image_.spacepartitioning_.space_partion_basic_num_;

		// Should be changed for each scale
		bin_space_partition_num = options_.image_.spacepartitioning_.space_partion_basic_num_;
	}
	else
	{
		// For safety...
		options_.image_.spacepartitioning_.space_partion_basic_num_ = 1;
	}

	// ZERO - Unlimited
	unsigned int bin_truncation_amount = 0;

	if(options_.patchtruncation_.enumTruncationType_
		== BORA::Options::PatchTruncation::TRUNCATION_BY_AMOUNT)
	{
		bin_truncation_amount = options_.patchtruncation_.unionTruncationValue_.amount_;
	}
	// ---- //


	// Perspective
	for(unsigned int bin_perspec_flag = 0; bin_perspec_flag < bin_perspec_num; bin_perspec_flag++)
	{
		bin_perspective_value = (bin_perspec_flag > 0);

		// Scale
		for(unsigned int bin_scale_flag = 0; bin_scale_flag < bin_scale_num; bin_scale_flag++)
		{
			bin_scale_value = options_.image_.scale_.scale_max_ * pow(options_.image_.scale_.basic_scale_, ((double)bin_scale_flag));

			if( options_.image_.spacepartitioning_.use_space_partitioning_ )
			{
				UpdateSpacePartitioningInterval(
					bin_scale_value,
					bin_space_partition_basic_ratio,
					bin_space_partition_num,		// Updated
					bin_space_partition_interval,	// Updated
					bin_truncation_amount,			// Updated
					false);
			}

			// Rotation
			for(unsigned int bin_rotation_flag = 0; bin_rotation_flag < bin_rotation_num; bin_rotation_flag++)
			{
				bin_rotation_value = options_.image_.rotation_.basic_rotation_min_ + (bin_rotation_flag * options_.image_.rotation_.basic_rotation_);

				// Space Partition
				if( !options_.image_.spacepartitioning_.use_space_partitioning_ )
				{
					bin_space_partition_start_pos = BORA::Position(0, 0);
					bin_space_partition_end_pos = BORA::Position(ori_img_width_, ori_img_height_);

					// -- Add Bin Info -- //
					BORA::BinInfo bin_info = BORA::BinInfo(
						bin_rotation_value, bin_scale_value, bin_perspective_value,
						bin_space_partition_start_pos, bin_space_partition_end_pos,
						bin_rotation_flag, bin_scale_flag, bin_perspec_flag, 0, 0,
						bin_truncation_amount);
					bin_info_list_.push_back(bin_info);
					// ---- //
				}
				else
				{
					// Space Partition X
					for(unsigned int bin_X_space_partition_flag = 0; bin_X_space_partition_flag < bin_space_partition_num; bin_X_space_partition_flag++)
					{

						// Space Partition Y
						for(unsigned int bin_Y_space_partition_flag = 0; bin_Y_space_partition_flag < bin_space_partition_num; bin_Y_space_partition_flag++)
						{
							// !! NOTICE !! //
							// Actually, each partition has OVERLAPPED region with adjacent ones
							// in order to consider feature points on the partition boundary
							const int PARTITION_OFFSET = 8;

							assert(ori_img_width_ > 0);
							assert(ori_img_height_ > 0);
							assert(bin_space_partition_interval.x > 0);
							assert(bin_space_partition_interval.y > 0);

							bin_space_partition_start_pos = BORA::Position(
								(bin_X_space_partition_flag * bin_space_partition_interval.x) - PARTITION_OFFSET,
								(bin_Y_space_partition_flag * bin_space_partition_interval.y) - PARTITION_OFFSET );

							bin_space_partition_end_pos = BORA::Position(
								((bin_X_space_partition_flag + 1) * bin_space_partition_interval.x) + PARTITION_OFFSET,
								((bin_Y_space_partition_flag + 1) * bin_space_partition_interval.y) + PARTITION_OFFSET );

							bin_space_partition_start_pos.x = max(0, bin_space_partition_start_pos.x);
							bin_space_partition_start_pos.y = max(0, bin_space_partition_start_pos.y);

							bin_space_partition_end_pos.x = min((int)ori_img_width_ , bin_space_partition_end_pos.x);
							bin_space_partition_end_pos.y = min((int)ori_img_height_, bin_space_partition_end_pos.y);

							if( bin_space_partition_end_pos.x <= bin_space_partition_start_pos.x
								|| bin_space_partition_end_pos.y <= bin_space_partition_start_pos.y )
							{
								std::cerr << "warning: [ImageUtility::CropImage] Wrong range: ("
									<< bin_space_partition_start_pos.x << ",  " << bin_space_partition_start_pos.y << ") -> ("
									<< bin_space_partition_end_pos.x << ",  " << bin_space_partition_end_pos.y << ")" << std::endl;
								return;
							}


							// -- Add Bin Info -- //
							BORA::BinInfo bin_info = BORA::BinInfo(
								bin_rotation_value, bin_scale_value, bin_perspective_value,
								bin_space_partition_start_pos, bin_space_partition_end_pos,
								bin_rotation_flag, bin_scale_flag, bin_perspec_flag,
								bin_X_space_partition_flag, bin_Y_space_partition_flag,
								bin_truncation_amount);
							bin_info_list_.push_back(bin_info);
							// ---- //
						}
						// Space Partition Y [End]

					}
					// Space Partition X [End]
				}

			}
			// Rotation [End]

		}
		// Scale [End]
		
	}
	// Perspective [End]

	num_total_bins_ = bin_info_list_.size();
	std::cout << "# of total bins: " << num_total_bins_ << std::endl;

	return;
}

// ---- //