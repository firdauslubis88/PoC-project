#include "stdafx.h"
#include "Match.h"

namespace BORA
{
#if DB_FILE_TYPE == 100
	bool Match::isAdjSpacePartitionBins( BORA::BinInfo &i_bin, BORA::BinInfo &j_bin, int range )
	{
		/*
		bool ret = i_bin.bin_scale_ == j_bin.bin_scale_
			&& i_bin.bin_rotation_ == j_bin.bin_rotation_

			// Adjacent (Overlapped or Border)
			&& i_bin.bin_space_partition_start_pos_.x <= j_bin.bin_space_partition_end_pos_.x
			&& i_bin.bin_space_partition_end_pos_.x >= j_bin.bin_space_partition_start_pos_.x
			&& i_bin.bin_space_partition_start_pos_.y <= j_bin.bin_space_partition_end_pos_.y
			&& i_bin.bin_space_partition_end_pos_.y >= j_bin.bin_space_partition_start_pos_.y;
		*/

		int X_space_partition_flag_diff = abs(
			((int)i_bin.bin_X_space_partition_flag_)
			- ((int)j_bin.bin_X_space_partition_flag_) );

		int Y_space_partition_flag_diff = abs(
			((int)i_bin.bin_Y_space_partition_flag_)
			- ((int)j_bin.bin_Y_space_partition_flag_) );

		bool ret = i_bin.bin_scale_flag_ == j_bin.bin_scale_flag_
			&& i_bin.bin_rotation_flag_ == j_bin.bin_rotation_flag_

			// Adjacent
			&& X_space_partition_flag_diff <= range
			&& Y_space_partition_flag_diff <= range;

		return ret;
	}

	bool Match::isAdjRotationBins( BORA::BinInfo &i_bin, BORA::BinInfo &j_bin, int range )
	{
		/*
		double rotation_diff = abs(i_bin.bin_rotation_ - j_bin.bin_rotation_);

		bool ret = i_bin.bin_scale_ == j_bin.bin_scale_
			&& i_bin.bin_space_partition_start_pos_.x == j_bin.bin_space_partition_start_pos_.x
			&& i_bin.bin_space_partition_start_pos_.y == j_bin.bin_space_partition_start_pos_.y
			&& i_bin.bin_space_partition_end_pos_.x == j_bin.bin_space_partition_end_pos_.x
			&& i_bin.bin_space_partition_end_pos_.y == j_bin.bin_space_partition_end_pos_.y

			// Adjacent
			&& rotation_diff <= range;
		*/

		int rotation_flag_diff = abs(
			((int)i_bin.bin_rotation_flag_) - ((int)j_bin.bin_rotation_flag_) );

		bool ret = i_bin.bin_scale_flag_ == j_bin.bin_scale_flag_
			&& i_bin.bin_X_space_partition_flag_ == j_bin.bin_X_space_partition_flag_
			&& i_bin.bin_Y_space_partition_flag_ == j_bin.bin_Y_space_partition_flag_

			// Adjacent
			&& rotation_flag_diff != 0 && rotation_flag_diff <= range;

		return ret;
	}

	bool Match::isAdjScaleBins( BORA::BinInfo &i_bin, BORA::BinInfo &j_bin, int range )
	{
		/*
		assert(range < 1);
		double scale_diff = (i_bin.bin_scale_ / j_bin.bin_scale_);

		bool ret = i_bin.bin_rotation_ == j_bin.bin_rotation_

			// Overlapped
			&& i_bin.bin_space_partition_start_pos_.x < j_bin.bin_space_partition_end_pos_.x
			&& i_bin.bin_space_partition_end_pos_.x > j_bin.bin_space_partition_start_pos_.x
			&& i_bin.bin_space_partition_start_pos_.y < j_bin.bin_space_partition_end_pos_.y
			&& i_bin.bin_space_partition_end_pos_.y > j_bin.bin_space_partition_start_pos_.y

			// Adjacent
			&& scale_diff >= range && scale_diff <= 1 / range;
		*/

		int scale_flag_diff = abs(
			((int)i_bin.bin_scale_flag_) - ((int)j_bin.bin_scale_flag_) );

		bool ret = i_bin.bin_rotation_flag_ == j_bin.bin_rotation_flag_

			// Overlapped
			&& i_bin.bin_space_partition_start_pos_.x < j_bin.bin_space_partition_end_pos_.x
			&& i_bin.bin_space_partition_end_pos_.x > j_bin.bin_space_partition_start_pos_.x
			&& i_bin.bin_space_partition_start_pos_.y < j_bin.bin_space_partition_end_pos_.y
			&& i_bin.bin_space_partition_end_pos_.y > j_bin.bin_space_partition_start_pos_.y

			// Adjacent
			&& scale_flag_diff != 0 && scale_flag_diff <= range;

		return ret;
	}

	unsigned int Match::numAdjBins( BinAdjInfo &bin )
	{
		unsigned int num_adj_bins =
			bin.adj_space_partitions.size()
			+ bin.adj_rotations.size()
			+ bin.adj_scales.size();

		return num_adj_bins;
	}

	void Match::initBinInfo()
	{
		unsigned int num_bins = m_bin_info.size();
		assert( num_bins > 0 );
		
		m_bin_adj_info_.resize( num_bins );
		for( unsigned int i = 0; i < num_bins; i++ )
		{
			m_bin_adj_info_[i].adj_space_partitions.clear();
			m_bin_adj_info_[i].direct_adj_rotations.clear();
			m_bin_adj_info_[i].direct_adj_scales.clear();
			m_bin_adj_info_[i].adj_rotations.clear();
			m_bin_adj_info_[i].adj_scales.clear();
		}

		const unsigned int NEIGHBOR_ROTATION = 1;
		const unsigned int NEIGHBOR_SCALE = 2;

		//const double ROTATION_INTERVAL = 10;
		//const double SCALE_INTERVAL = 0.9 - (1.0E-6);
		//assert( SCALE_INTERVAL < 1 );

		//const double rotation_range = (ROTATION_INTERVAL * NEIGHBOR_ROTATION);
		//const double scale_range = pow(SCALE_INTERVAL, (double)NEIGHBOR_SCALE);


		//printf("Neighbor Range:\n");
		//printf("Rotation: [%lf, %lf]\n", -rotation_range, rotation_range),
		//printf("Scale: [%lf, %lf]\n", scale_range, 1 / scale_range);


		unsigned int min_adj_bins = num_bins;
		unsigned int max_adj_bins = 0;
		float avg_adj_bins = 0.0f;


		// Step 1. Find,
		// 1) Adjacent space partitions with the same scale and rotation (Neighbor partitions)
		// 2) Adjacent rotation bin with the same space partition and scale
		// 3) Adjacent scale bin with the same rotation and OVERLAPPED space range
		for(unsigned int i = 0; i < num_bins; i++)
		{
			BORA::BinInfo &i_bin = m_bin_info[i];
			// !! NOTICE !!
			// Add itself to neighbor partitions
			m_bin_adj_info_[i].adj_space_partitions.push_back(i);

			for(unsigned int j = i+1; j < num_bins; j++)
			{
				BORA::BinInfo &j_bin = m_bin_info[j];

				// Space Partition
				if( isAdjSpacePartitionBins(i_bin, j_bin) )
				{
					m_bin_adj_info_[i].adj_space_partitions.push_back(j);
					m_bin_adj_info_[j].adj_space_partitions.push_back(i);
				}

				// Rotation
				else if ( isAdjRotationBins(i_bin, j_bin, NEIGHBOR_ROTATION) )
				{
					m_bin_adj_info_[i].direct_adj_rotations.push_back(j);
					m_bin_adj_info_[j].direct_adj_rotations.push_back(i);
				}

				// Scale
				else if ( isAdjScaleBins(i_bin, j_bin, NEIGHBOR_SCALE) )
				{
					m_bin_adj_info_[i].direct_adj_scales.push_back(j);
					m_bin_adj_info_[j].direct_adj_scales.push_back(i);
				}
			}
		}

		// Step 2. Add adjacent rotation and scale bins of NEIGHBOR PARTITIONS
		for(unsigned int i = 0; i < num_bins; i++)
		{
			BinAdjInfo &adj_bin_info = m_bin_adj_info_[i];

			assert(adj_bin_info.adj_space_partitions.size() <= 8);
			assert(adj_bin_info.direct_adj_rotations.size() <= (1 + 1) * NEIGHBOR_ROTATION);
			assert(adj_bin_info.direct_adj_scales.size() <= (4 + 9) * NEIGHBOR_SCALE);

			/*
			unsigned int prev_sp_size = adj_bin_info.adj_space_partitions.size();
			adj_bin_info.adj_space_partitions.unique();
			unsigned int curr_sp_size = adj_bin_info.adj_space_partitions.size();
			if(prev_sp_size != curr_sp_size)
			{
				std::cerr << "[error] Wrong neighbor partitions: (" << i << ")" << std::endl;
				system("pause");
			}
			*/

			adj_bin_info.adj_rotations.clear();
			// For all NEIGHBOR PARTITIONS
			for( std::list<unsigned int>::iterator it = adj_bin_info.adj_space_partitions.begin();
				it != adj_bin_info.adj_space_partitions.end(); ++it )
			{
				adj_bin_info.adj_rotations.insert(
					adj_bin_info.adj_rotations.end(),
					m_bin_adj_info_[ *it ].direct_adj_rotations.begin(), 
					m_bin_adj_info_[ *it ].direct_adj_rotations.end() );
			}

			// All added rotation bins are unique
			// adj_bin_info.adj_rotations.unique();
			/*
			unsigned int prev_rot_size = adj_bin_info.adj_rotations.size();
			adj_bin_info.adj_rotations.unique();
			unsigned int curr_rot_size = adj_bin_info.adj_rotations.size();
			if(prev_rot_size != curr_rot_size)
			{
				std::cerr << "[error] Wrong neighbor partitions: (" << i << ")" << std::endl;
				system("pause");
			}
			*/

			adj_bin_info.adj_scales.clear();
			for( std::list<unsigned int>::iterator it = adj_bin_info.adj_space_partitions.begin();
				it != adj_bin_info.adj_space_partitions.end(); ++it )
			{
				adj_bin_info.adj_scales.insert(
					adj_bin_info.adj_scales.end(),
					m_bin_adj_info_[ *it ].direct_adj_scales.begin(), 
					m_bin_adj_info_[ *it ].direct_adj_scales.end() );
			}

			// Make a unique list
			adj_bin_info.adj_scales.sort();
			adj_bin_info.adj_scales.unique();


			unsigned int num_adj_bins = numAdjBins( adj_bin_info );

			if( num_adj_bins < min_adj_bins )
				min_adj_bins = num_adj_bins;

			if( num_adj_bins > max_adj_bins )
				max_adj_bins = num_adj_bins;

			avg_adj_bins += (float)num_adj_bins;
		}
		avg_adj_bins /= (float)num_bins;


		// DEBUG
		// ---- //
		for(unsigned int i = 0; i < num_bins; i++)
		{
			std::vector<unsigned int> is_adj(num_bins, 0);
			BinAdjInfo &adj_bin_info = m_bin_adj_info_[ i ];

			// 1. Neighbor Partitions (including itself)
			for(std::list<unsigned int>::iterator it = adj_bin_info.adj_space_partitions.begin();
				it != adj_bin_info.adj_space_partitions.end(); ++it)
			{
				if( is_adj[ *it ] != 0 )
				{
					std::cerr << "warning: [Match::initBinInfo] Duplicated adjacent bins: ("
						<< i << ") - [" << is_adj[ *it ] << ", " << 1 << "]" << std::endl;
					system("pause");
				}
				else is_adj[ *it ] = 1;
			}

			// 2. Adjacent rotation bins of all neighbor partitions
			for(std::list<unsigned int>::iterator it = adj_bin_info.adj_rotations.begin();
				it != adj_bin_info.adj_rotations.end(); ++it)
			{
				if( is_adj[ *it ] != 0 )
				{
					std::cerr << "warning: [Match::initBinInfo] Duplicated adjacent bins: ("
						<< i << ") - [" << is_adj[ *it ] << ", " << 2 << "]" << std::endl;
					system("pause");
				}
				else is_adj[ *it ] = 2;
			}

			// 3. Adjacent scale bins of all neighbor partitions
			for(std::list<unsigned int>::iterator it = adj_bin_info.adj_scales.begin();
				it != adj_bin_info.adj_scales.end(); ++it)
			{
				if( is_adj[ *it ] != 0 )
				{
					std::cerr << "warning: [Match::initBinInfo] Duplicated adjacent bins: ("
						<< i << ") - [" << is_adj[ *it ] << ", " << 3 << "]" << std::endl;
					system("pause");
				}
				else is_adj[ *it ] = 3;
			}
		}
		printf("[Match::initBinInfo] Debuggin complete\n");
		// ---- //

		printf("# of Avg. Adj. Bins: %lf (%d, %d)\n", avg_adj_bins, min_adj_bins, max_adj_bins);
	}

	void Match::printFeatureBinInfo( std::vector<HomographyMatch> &pairs,
		int &max_count_bin)
	{
		unsigned int num_bins = m_bin_info.size();
		unsigned int num_pairs = pairs.size();

		if(num_pairs < MIN_NUM_PAIRS)
			return;

		std::vector<unsigned int> bin_count( num_bins, 0 );

		for(std::vector<HomographyMatch>::iterator it = pairs.begin();
			it != pairs.end(); ++it)
		{
			if(it->binIdx >= (int)num_bins)
			{
				printf("error: %d >= %d\n", it->binIdx, num_bins);
				return;
			}

			// Adjacency is symmetric
			BinAdjInfo &adj_bin_info = m_bin_adj_info_[ it->binIdx ];

			// 1. Neighbor Partitions (including itself)
			for(std::list<unsigned int>::iterator it = adj_bin_info.adj_space_partitions.begin();
				it != adj_bin_info.adj_space_partitions.end(); ++it)
				bin_count[ *it ]++;

			// 2. Adjacent rotation bins of all neighbor partitions
			for(std::list<unsigned int>::iterator it = adj_bin_info.adj_rotations.begin();
				it != adj_bin_info.adj_rotations.end(); ++it)
				bin_count[ *it ]++;

			// 3. Adjacent scale bins of all neighbor partitions
			for(std::list<unsigned int>::iterator it = adj_bin_info.adj_scales.begin();
				it != adj_bin_info.adj_scales.end(); ++it)
				bin_count[ *it ]++;
		}

		// PRINT
		if( max_count_bin < 0 )
		{
			// Find the maximum one
			printf("[MAX] ");
			for(unsigned int i = 0; i < num_bins; i++)
				if( max_count_bin < 0
					|| bin_count[i] > bin_count[max_count_bin])
						max_count_bin = i;
		}
		assert(max_count_bin > 0);

		printf("%3.2lf, %3.2lf, (%d, %d), (%d, %d) - %d/%d = %3.2lf%%\n",
			m_bin_info[max_count_bin].bin_rotation_, m_bin_info[max_count_bin].bin_scale_, 
			m_bin_info[max_count_bin].bin_space_partition_start_pos_.x, m_bin_info[max_count_bin].bin_space_partition_start_pos_.y, 
			m_bin_info[max_count_bin].bin_space_partition_end_pos_.x, m_bin_info[max_count_bin].bin_space_partition_end_pos_.y, 
			bin_count[max_count_bin], num_pairs,
			(float)(bin_count[max_count_bin]) / num_pairs * 100);
	}

	void Match::computeCorrectBin( Matrix<3> &homography )
	{
		/*
		Vector<2> src_corners[4];
		src_corners[0][0] = 0;									src_corners[0][1] = g_target_camera.GetImageSize()[1];
		src_corners[1][0] = g_target_camera.GetImageSize()[0];	src_corners[1][1] = g_target_camera.GetImageSize()[1];
		src_corners[2][0] = g_target_camera.GetImageSize()[0];	src_corners[2][1] = 0;
		src_corners[3][0] = 0;									src_corners[3][1] = 0;

		// Edge Center
		Vector<2> edge_centers[4];
		edge_centers[0] = (src_corners[0] + src_corners[1]) / 2;	// Up
		edge_centers[1] = (src_corners[2] + src_corners[3]) / 2;	// Down
		edge_centers[2] = (src_corners[0] + src_corners[3]) / 2;	// Left
		edge_centers[3] = (src_corners[1] + src_corners[2]) / 2;	// Right

		// Center Line
		Vector<3> center_lines[4];
		center_lines[0] = unproject(edge_centers[0]) ^ unproject(edge_centers[1]);	// Up - Down
		center_lines[1] = unproject(edge_centers[2]) ^ unproject(edge_centers[3]);	// Left - Right

		// Center
		Vector<2> center = project( center_lines[0] ^ center_lines[1] );

		// Orientation Vector
		Vector<2> orientation[2];
		orientation[0] = (edge_centers[0] - center).normalize();
		orientation[1] = (edge_centers[3] - center).normalize();

		// Angle
		double angle_axis[2];
		angle_axis[0] = acos(orientation[0][0]);
		angle_axis[1] = acos(orientation[1][1]);
		dReal angle = (angle_axis[0] + angle_axis[1])/ 2;


		// Scale
		double scale_axis[4];
		scale_axis[0] = (src_corners[0] - src_corners[1]).length();	// Up
		scale_axis[1] = (src_corners[2] - src_corners[3]).length();	// Down
		scale_axis[2] = (src_corners[0] - src_corners[3]).length();	// Left
		scale_axis[3] = (src_corners[1] - src_corners[2]).length();	// Right
		*/
	}

#endif
}