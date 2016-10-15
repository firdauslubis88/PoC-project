#ifndef _MATCHER_H_
#define _MATCHER_H_

#include <cstdio>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>

#include "ImageUtility.h"
#include "FAST9.h"
#include "MatchOption.h"
#include "MatchDescriptor.h"
#include "Descriptor.h"
#include "DataBase.h"
#include "ImagePyramid.h"

#include "PTAM/ATANCamera.h"
#include "Homography.h"
#include "CameraPose.h"
#include "CameraPoseARToolKit.h"
// Eigen
//#include "EigenTooNBase.h"
//#include "EigenHomography.h"

#define MIN_NUM_PAIRS	10
#define ARTOOLKIT_LIB
#define PRINT_STATISTICS
#define DB_FILE_TYPE	1	// If 100, Bin info is loaded and analyzed...
//#define MERGE_DB_FILE		// If defined, multiple DB files are loaded and merged...

namespace BORA
{
	class Match
	{
	public:
		Match();
		//Match(const char *imageFileName);
		~Match();

		//bool loadData(const char *dataFileName);
		bool loadData(const char *dataFileName, int &width, int &height);
		bool match(BORA::Image gray, ATANCamera &database_cam, ATANCamera &query_cam,
			Matrix<3> &homography, SE3<> &camera_pose, 
			std::vector<HomographyMatch> &m_pairs, std::vector<HomographyMatch> &m_inlier_pairs,
			MatchOption opt = MatchOption());
		//for test
		//bool match(BORA::Image gray, Matrix<3> &homography, MatchOption opt = MatchOption(), vector<int> *scoreVec = NULL);

		void analysis(int dissimilar_score, const char *filename);

	private:
		void inline init();
		unsigned int inline findPairs(int dissimilar_score,
			std::vector<HomographyMatch> &m_pairs, vector<int> *scoreVec = NULL);
		int inline bitCount(unsigned __int64 data);

		

		//bool inline removeOutlier();
		//bool inline removeOutlierOpenCV();

		bool m_is_data_loaded;

		ImagePyramid m_pyramid;

		INDEXTABLE m_index_table;
		DESCRIPTORS m_database;
		vector<MatchDescriptor> m_query;

		// Dec. 2011, Min-Hyuk Sung
#if DB_FILE_TYPE == 100
		#include <algorithm>

	private:
		BININFOS m_bin_info;

		typedef struct BinAdjInfo_
		{
			std::list<unsigned int> adj_space_partitions;
				 
			std::list<unsigned int> direct_adj_rotations;
			std::list<unsigned int> direct_adj_scales;
				 
			std::list<unsigned int> adj_rotations;
			std::list<unsigned int> adj_scales;

		} BinAdjInfo;

		std::vector<BinAdjInfo> m_bin_adj_info_;

	public:
		void initBinInfo();

		bool isAdjSpacePartitionBins( BORA::BinInfo &i_bin, BORA::BinInfo &j_bin, int range = 1 );
		bool isAdjRotationBins( BORA::BinInfo &i_bin, BORA::BinInfo &j_bin, int range = 1 );
		bool isAdjScaleBins( BORA::BinInfo &i_bin, BORA::BinInfo &j_bin, int range = 1 );

		unsigned int numAdjBins( BinAdjInfo &bin );

		void printFeatureBinInfo(std::vector<HomographyMatch> &pairs,
			int &max_count_bin);

		void computeCorrectBin(Matrix<3> &homography);
#endif

		// August 2011, Jihyun You (for test)
	public:

		// July 2011, Min-Hyuk Sung
		//std::vector<HomographyMatch> m_pairs;
		//std::vector<HomographyMatch> m_inlier_pairs;
	};
};

#endif
