#include "stdafx.h"
#include "Match.h"
#include <time.h>

namespace BORA
{
	Match::Match()
	{
		init();
	}

	/*
	Match::Match(const char *dataFileName)
	{
		init();
		loadData(dataFileName);
	}
	*/

	Match::~Match()
	{
	}

	void Match::init()
	{
		m_query.clear();
		m_database.clear();
		//m_pairs.clear();
		//m_index_table.fill(INDICES());
		m_index_table.assign(INDICES());
	}

	/*
	bool Match::loadData(const char *dataFileName)
	{
		init();

		Database db;
		m_is_data_loaded = db.ReadFromFile(dataFileName);
		if( !m_is_data_loaded )	return false;

		IndexTable it;
		db.getDatabase(m_database, it);
		m_index_table = it.getData();

		return m_is_data_loaded;
	}
	*/

	bool Match::loadData(const char *dataFileName, int &width, int &height)
	{
		init();
		
		Database db;
		unsigned int file_type;
#ifndef MERGE_DB_FILE
		m_is_data_loaded = db.ReadFromFile(dataFileName, width, height, file_type);
#else
		m_is_data_loaded = db.MergeFiles(dataFileName, width, height, file_type);
#endif
		if( !m_is_data_loaded )	return false;

		IndexTable it;
		db.getDatabase(m_database, it);
		m_index_table = it.getData();

		// Dec. 2011, Min-Hyuk Sung
#if DB_FILE_TYPE == 100
		m_bin_info = db.getBinInfos();
		initBinInfo();
#endif

		return m_is_data_loaded;
	}

	bool Match::match(BORA::Image gray, ATANCamera &database_cam, ATANCamera &query_cam,
		Matrix<3> &homography, SE3<> &camera_pose,
		std::vector<HomographyMatch> &m_pairs, std::vector<HomographyMatch> &m_inlier_pairs, MatchOption opt)
	{
		m_pairs.clear();
		m_inlier_pairs.clear();

		if( !m_is_data_loaded )
		{
			std::cerr << "[Match::match] Error: Data file not loaded" << std::endl;
			return false;
		}
		clock_t time;
		
		// 1. Make pyramids and create descriptors
		//time = clock();

		m_query.clear();
		m_pyramid.init(gray.getWidth(), gray.getHeight());
		m_pyramid.setImage(gray);
		unsigned int num_corners = m_pyramid.getDescriptor(m_query, opt);
		m_pyramid.clear();
		if(num_corners < MIN_NUM_PAIRS)	return false;

		//time = clock() - time;
		//printf(">>> [1] %lf\n", ((float)time)/(float)CLOCKS_PER_SEC );


		// 2. Find matching pairs
		//time = clock();

		findPairs(opt.dissimilar_score, m_pairs);
		unsigned int num_pairs = m_pairs.size();
		if(num_pairs < MIN_NUM_PAIRS)	return false;

		//time = clock() - time;
		//printf(">>> [2] %lf\n", ((float)time)/(float)CLOCKS_PER_SEC );


		// 3. Compute homography and camera pose
		//time = clock();

		/*
		for(unsigned int i = 0; i < num_pairs; i++)
		{
			// First <-- Target
			// Second <-- Query
			m_pairs[i].v2CamPlaneFirst = database_cam.UnProject(m_pairs[i].v2CamPlaneFirst);
			m_pairs[i].v2CamPlaneSecond = query_cam.UnProject(m_pairs[i].v2CamPlaneSecond);
		}
		*/

		// Jul. 2011, Min-Hyuk Sung
		Homography hm;

#ifdef ARTOOLKIT_LIB
		CameraPoseARToolKit cp;
#else
		CameraPose cp;
#endif
		// Eigen
		// Mar. 2011, Min-Hyuk Sung
		// The error is measured using 'm2PixelProjectionJac'
		// on the 'second (query)' plane (m2 = Hm1)
		//hm.m2PixelProjectionJac = query_cam.GetProjectionDerivs();

		//hm.Compute(m_pairs, m_inlier_pairs, homography);
		// -- TEST -- //
		unsigned int num_matches = m_pairs.size();
		double *x_pos_list = new double[2*num_matches];
		double *y_pos_list = new double[2*num_matches];
		// Dec. 2011, Min-Hyuk Sung
		//
		int *bin_idx_list = new int[num_matches];
		//
		for(unsigned int i = 0; i < num_matches; i++)
		{
			x_pos_list[2*i+0] = m_pairs[i].v2CamPlaneFirst[0];
			x_pos_list[2*i+1] = m_pairs[i].v2CamPlaneFirst[1];
			y_pos_list[2*i+0] = m_pairs[i].v2CamPlaneSecond[0];
			y_pos_list[2*i+1] = m_pairs[i].v2CamPlaneSecond[1];
			// Dec. 2011, Min-Hyuk Sung
			//
			bin_idx_list[i] = m_pairs[i].binIdx;
			//
		}

		hm.Compute(homography, num_matches, x_pos_list, y_pos_list, bin_idx_list);

		// 'num_matches' is updated
		m_inlier_pairs.resize(num_matches);
		for(unsigned int i = 0; i < num_matches; i++)
		{
			m_inlier_pairs[i].v2CamPlaneFirst[0] = x_pos_list[2*i+0];
			m_inlier_pairs[i].v2CamPlaneFirst[1] = x_pos_list[2*i+1];
			m_inlier_pairs[i].v2CamPlaneSecond[0] = y_pos_list[2*i+0];
			m_inlier_pairs[i].v2CamPlaneSecond[1] = y_pos_list[2*i+1];
			// Dec. 2011, Min-Hyuk Sung
			//
			m_inlier_pairs[i].binIdx = bin_idx_list[i];
			//
		}
		delete [] x_pos_list;
		delete [] y_pos_list;
		// Dec. 2011, Min-Hyuk Sung
		//
		delete [] bin_idx_list;
		//
		// ---- //

		/*
		// ---- //
		EigenHomography eigen_hm;
		for(unsigned i = 0; i < 2; i++)
			for(unsigned j = 0; j < 2; j++)
				eigen_hm.m2PixelProjectionJac(i, j) = query_cam.GetProjectionDerivs()[i][j];

		Eigen::Matrix3 eigen_homography;
		std::vector<EigenHomographyMatch, Eigen::aligned_allocator<EigenHomographyMatch>> eigen_pairs;
		std::vector<EigenHomographyMatch, Eigen::aligned_allocator<EigenHomographyMatch>> eigen_inlier_pairs;

		eigen_pairs.resize(m_pairs.size());
		for(unsigned int i = 0; i < m_pairs.size(); i++)
		{
			for(unsigned j = 0; j < 2; j++)
			{
				eigen_pairs[i].v2CamPlaneFirst[j] = m_pairs[i].v2CamPlaneFirst[j];
				eigen_pairs[i].v2CamPlaneSecond[j] = m_pairs[i].v2CamPlaneSecond[j];
			}
			eigen_pairs[i].nScore = m_pairs[i].nScore;
		}

		eigen_hm.Compute(eigen_pairs, eigen_inlier_pairs, eigen_homography);

		m_inlier_pairs.resize(eigen_inlier_pairs.size());
		for(unsigned int i = 0; i < m_inlier_pairs.size(); i++)
		{
			for(unsigned j = 0; j < 2; j++)
			{
				m_inlier_pairs[i].v2CamPlaneFirst[j] = eigen_inlier_pairs[i].v2CamPlaneFirst[j];
				m_inlier_pairs[i].v2CamPlaneSecond[j] = eigen_inlier_pairs[i].v2CamPlaneSecond[j];
			}
			m_inlier_pairs[i].nScore = eigen_inlier_pairs[i].nScore;
		}

		for(unsigned i = 0; i < 3; i++)
			for(unsigned j = 0; j < 3; j++)
				homography[i][j] = eigen_homography(i, j);
		// ---- //
		*/
		//unsigned int num_inlier_matches = m_inlier_pairs.size();
		unsigned int num_inlier_matches = num_matches;

		
		if(num_inlier_matches < MIN_NUM_PAIRS)	return false;

#if DB_FILE_TYPE == 100
		printf(">>> [AFTER ]\n");
		int max_count_bin = -1;
		printFeatureBinInfo( m_inlier_pairs, max_count_bin );

		printf(">>> [BEFORE]\n");
		printFeatureBinInfo( m_pairs, max_count_bin );
#endif

#ifdef PRINT_STATISTICS
		printf( "%3d/%3d (%2.2lf%%)\n", num_inlier_matches, m_pairs.size(),
			(float)num_inlier_matches/m_pairs.size()*100 );
#endif

		cp.Compute(database_cam, query_cam, m_inlier_pairs, homography, camera_pose);

		//time = clock() - time;
		//printf(">>> [3] %lf\n", ((float)time)/(float)CLOCKS_PER_SEC );
		//printf("\n");

		return true;
	}

	/*
	bool Match::match(BORA::Image gray, Matrix<3> &homography, MatchOption opt, vector<int> *scoreVec)
	{
		m_pairs.clear();
		m_inlier_pairs.clear();

		if( !m_is_data_loaded )
		{
			std::cerr << "[Match::match] Error: Data file not loaded" << std::endl;
			return false;
		}
		
		// 1. Make pyramids and create descriptors
		m_query.clear();
		m_pyramid.init(gray.getWidth(), gray.getHeight());
		m_pyramid.setImage(gray);
		unsigned int num_corners = m_pyramid.getDescriptor(m_query, opt);
		if(num_corners < MIN_NUM_PAIRS)	return false;

		// 2. Find matching pairs
		findPairs(opt.dissimilar_score, scoreVec);
		unsigned int num_pairs = m_pairs.size();
		if(num_pairs < MIN_NUM_PAIRS)	return false;

		// 3. Compute homography and camera pose
//		for(unsigned int i = 0; i < num_pairs; i++)
//		{
//			// First <-- Target
//			// Second <-- Query
//			m_pairs[i].v2CamPlaneFirst = database_cam.UnProject(m_pairs[i].v2CamPlaneFirst);
//			m_pairs[i].v2CamPlaneSecond = query_cam.UnProject(m_pairs[i].v2CamPlaneSecond);
//		}

		// Jul. 2011, Min-Hyuk Sung
		Homography hm;

//#ifdef ARTOOLKIT_LIB
//		CameraPoseARToolKit cp;
//#else
//		CameraPose cp;
//#endif

		// Mar. 2011, Min-Hyuk Sung
		// The error is measured using 'm2PixelProjectionJac'
		// on the 'second (query)' plane (m2 = Hm1)
//		hm.m2PixelProjectionJac = query_cam.GetProjectionDerivs();
		hm.m2PixelProjectionJac(0,0) = 1;
		hm.m2PixelProjectionJac(0,1) = 0;
		hm.m2PixelProjectionJac(1,0) = 0;
		hm.m2PixelProjectionJac(1,1) = 1;

		hm.Compute(m_pairs, m_inlier_pairs, homography);
		if(m_inlier_pairs.size() < MIN_NUM_PAIRS)	return false;

		//printf( "%3d/%3d (%2.2lf%%)\n", m_inlier_pairs.size(), m_pairs.size(),
		//	(float)m_inlier_pairs.size()/m_pairs.size()*100 );

//		cp.Compute(database_cam, query_cam, m_inlier_pairs, homography, camera_pose);


		return true;
	}
	*/

	unsigned int Match::findPairs(int dissimilar_score,
		std::vector<HomographyMatch> &m_pairs, vector<int> *scoreVec)
	{
		//test
		if(scoreVec && !scoreVec->empty())
			scoreVec->clear();

		m_pairs.clear();
		m_pairs.reserve(m_query.size());

		// For every feature point in the query image
		for(unsigned int iQuery = 0; iQuery < m_query.size(); iQuery++)
		{
			int iBestData = -1;
			int max_score = 320;
			int equalScore=0; //test

			MatchDescriptor query = m_query[iQuery];
			INDICES &indices = m_index_table[query.index];
			
			// For every feature in the given index entry
			for(INDICES::iterator iterIdx = indices.begin(); iterIdx != indices.end(); iterIdx++)
			{

				int score = bitCount( query.compare(m_database[*iterIdx]) );
				
				if(score <= dissimilar_score && score < max_score)
				{
					iBestData = *iterIdx;
					max_score = score;
					equalScore = 1; //test
				}else if(score == max_score){
					equalScore++;
				}
			}

			if( max_score <= dissimilar_score )
			{
				assert(iBestData >= 0);

				/*
				MatchPair pair;
				pair.iData = iBestData;
				pair.iQuery = iQuery;
				pair.score = max_score;
				pair.pData.x = m_database[iBestData].getPosition().x;
				pair.pData.y = m_database[iBestData].getPosition().y;
				pair.pQuery = m_query[iQuery].point;
				pair.inlier = 0;
				m_pairs.push_back(pair);
				*/

				// July 2011, Min-Hyuk Sung
				// First <-- Database
				// Second <-- Query
				HomographyMatch pair;
				//pair.id1 = iBestData;
				//pair.id2 = iQuery;
				//pair.equalScore = equalScore;

				// Dec. 2011, Min-Hyuk Sung
#if DB_FILE_TYPE == 100
				pair.binIdx = m_database[iBestData].getParentBin();
#endif
				//

				pair.v2CamPlaneFirst[0] = m_database[iBestData].getPosition().x;
				pair.v2CamPlaneFirst[1] = m_database[iBestData].getPosition().y;
				pair.v2CamPlaneSecond[0] = m_query[iQuery].point.x;
				pair.v2CamPlaneSecond[1] = m_query[iQuery].point.y;
				pair.nScore = max_score;
				m_pairs.push_back(pair);

				if(scoreVec)//test
					scoreVec->push_back(equalScore);
			}
		}

		return m_pairs.size();
	}

	// Jul. 2012, Min-Hyuk Sung
	void Match::analysis(int dissimilar_score, const char *filename)
	{
		FILE *f = fopen(filename, "w");

		// For every feature point in the query image
		for(INDEX index = 0; index < INDEX_13_MAX; index++)
		{
			INDICES &indices = m_index_table[index];
			unsigned int count_features = indices.size();

			unsigned int count_similar_feature_pairs = 0;
			unsigned int count_feature_pairs = ( count_features * (count_features - 1) ) / 2;

			if(count_features > 1)
			{
				// For every feature in the given index entry
				for(INDICES::iterator desc_it = indices.begin(); desc_it != indices.end(); desc_it++)
				{
					for(INDICES::iterator desc_jt = desc_it; desc_jt != indices.end(); desc_jt++)
					{
						if( desc_it == desc_jt )
							continue;

						MatchDescriptor desc_i( m_database[*desc_it] );
						Descriptor &desc_j = m_database[*desc_jt];
						int score = bitCount( desc_i.compare(desc_j) );

						if(score <= dissimilar_score)
							count_similar_feature_pairs++;
					}

					count_features++;
				}

				printf("[%d] %d, %d, %3.2lf\n",
					index, count_similar_feature_pairs, count_feature_pairs,
					float(count_similar_feature_pairs)/count_feature_pairs);

				fprintf(f, "%d, %d, %d, %3.2lf\n",
					index, count_similar_feature_pairs, count_feature_pairs,
					float(count_similar_feature_pairs)/count_feature_pairs);
			}
		}

		fclose(f);
	}

	/*
	bool inline Match::removeOutlier(){
		if(m_pairs.size() < 10)
			return false;

		std::vector<HomographyMatch> matches, matchesInliers;
		Matrix<3> m3Homography;
		matches.resize(m_pairs.size(), HomographyMatch());
		for(unsigned int i=0; i<m_pairs.size(); i++){
			matches[i].v2CamPlaneFirst[0] = m_pairs[i].pData.x;
			matches[i].v2CamPlaneFirst[1] = m_pairs[i].pData.y;

			matches[i].v2CamPlaneSecond[0] = m_pairs[i].pQuery.x;
			matches[i].v2CamPlaneSecond[1] = m_pairs[i].pQuery.y;

			matches[i].nScore = m_pairs[i].score;

			matches[i].m2PixelProjectionJac[0][0] = 1;
			matches[i].m2PixelProjectionJac[0][1] = 0;
			matches[i].m2PixelProjectionJac[1][0] = 0;
			matches[i].m2PixelProjectionJac[1][1] = 1;
		}

		Homography hm;
		hm.Compute(matches, matchesInliers, m3Homography);

		m_pairs.clear();
		if(matchesInliers.size() < 5)
			return false;

		for(unsigned int i=0; i<matchesInliers.size(); i++){
			MatchPair p;
			p.pData.x = (int)matchesInliers[i].v2CamPlaneFirst[0];
			p.pData.y = (int)matchesInliers[i].v2CamPlaneFirst[1];
			p.pQuery.x = (int)matchesInliers[i].v2CamPlaneSecond[0];
			p.pQuery.y = (int)matchesInliers[i].v2CamPlaneSecond[1];
			p.score = matchesInliers[i].nScore;
			p.inlier = 1;
			m_pairs.push_back(p);
		}

		//copy homography
		H->data.db[0] = m3Homography(0,0);
		H->data.db[1] = m3Homography(0,1);
		H->data.db[2] = m3Homography(0,2);
		H->data.db[3] = m3Homography(1,0);
		H->data.db[4] = m3Homography(1,1);
		H->data.db[5] = m3Homography(1,2);
		H->data.db[6] = m3Homography(2,0);
		H->data.db[7] = m3Homography(2,1);
		H->data.db[8] = m3Homography(2,2);

		return true;
	}
	*/

	/*
	bool Match::removeOutlierOpenCV(){
		if(m_pairs.size() < 10)
			return false;

		int prosac_count = 10;
		int outlier_range = 3;

		// Homogeneous coordinates (Transposed for matrix multiplication)
		CvMat *A  = cvCreateMat(3, m_pairs.size(), CV_64F);
		CvMat *B  = cvCreateMat(3, m_pairs.size(), CV_64F);
		CvMat *TA = cvCreateMat(3, m_pairs.size(), CV_64F);

		for(int iMatching=0; iMatching<(int)m_pairs.size(); iMatching++)
		{
			A->data.db[iMatching + A->cols*0] = m_pairs[iMatching].pData.x;
			A->data.db[iMatching + A->cols*1] = m_pairs[iMatching].pData.y;
			A->data.db[iMatching + A->cols*2] = 1.0;

			B->data.db[iMatching + B->cols*0] = m_pairs[iMatching].pQuery.x;
			B->data.db[iMatching + B->cols*1] = m_pairs[iMatching].pQuery.y;
			B->data.db[iMatching + B->cols*2] = 1.0;

			m_pairs[iMatching].inlier = 0;
		}

		cvFindHomography(A, B, H, CV_RANSAC, outlier_range);
		cvmMul(H, A, TA);

		// matching 결과물 개수 만큼( == A의 row count) 다른 점들을 판별하는 부분
		int numOfInlier = 0;
		for(int i=0; i<A->cols; i++)
		{
			double zA = TA->data.db[i + A->cols*2];
			double xA = TA->data.db[i + A->cols*0] / zA;
			double yA = TA->data.db[i + A->cols*1] / zA;

			double xB = B->data.db[i + B->cols*0];
			double yB = B->data.db[i + B->cols*1];

			double d = ((xA - xB)*(xA - xB) + (yA - yB)*(yA - yB));
			int th = outlier_range*outlier_range;
			if(d<th)
			{
				m_pairs[i].inlier = 1;
				numOfInlier++;
			}
		}


		// best inlier 개수가 5개 미만이면 부적절한 매칭
		if(numOfInlier < 5)
			return false;

		// 아웃라이어 페어 제거
		std::sort(m_pairs.begin(), m_pairs.end(), ComparePairInlierGreater);
		m_pairs.erase(m_pairs.begin()+numOfInlier, m_pairs.end());

		cvReleaseMat(&A);
		cvReleaseMat(&B);
		cvReleaseMat(&TA);

		// 적절한 매치이므로 true를 반환
		return true;
	}
	*/
/*
	//MMX를 이용한 bit count
	int Match::bitCount(unsigned __int64 a)
	{
		__int64 mask1 = 0x5555555555555555u; // 01010101...
		__int64 mask2 = 0x3333333333333333u; // 00110011...
		__int64 mask3 = 0x0f0f0f0f0f0f0f0fu; // 00001111...
		__asm 
		{
			pxor mm6, mm6;             // mm6 = 0
			movq mm0, dword ptr a;     // mm0 = a

			//byte 이하의 범위
			//1단계(01010101)
			movq mm1, mm0;             // mm1 = mm0
			movq mm5, dword ptr mask1; // mm5 = mask1
			psrld mm1, 1;              // mm1 >> 1
			pand mm0, mm5;             // mm0에 mask1 적용
			pand mm1, mm5;             // mm1에 mask1 적용
			paddb mm0, mm1;            // mm0 += mm1

			//2단계(00110011)
			movq mm1, mm0;             // mm1 = mm0
			movq mm5, dword ptr mask2; // mm5 = mask2
			psrld mm1, 2;			   // mm1 >> 2
			pand mm0, mm5;			   // mm0에 mask2 적용
			pand mm1, mm5;			   // mm1에 mask2 적용
			paddb mm0, mm1;			   // mm0 += mm1

			//3단계(00001111)
			movq mm1, mm0;             // mm1 = mm0
			movq mm5, dword ptr mask3; // mm5 = mask3
			psrld mm1, 4;			   // mm1 >> 4
			pand mm0, mm5;			   // mm0에 mask3 적용
			pand mm1, mm5;			   // mm1에 mask3 적용
			paddb mm0, mm1;			   // mm0 += mm1

			//byte단계
			movq mm1, mm0;             // mm1 = mm0
			punpcklbw mm0, mm6;        // mm0 -> unpack high byte
			punpckhbw mm1, mm6;        // mm1 -> unpack low byte
			paddw mm0, mm1;            // mm0 += mm1

			//word단계
			movq mm1, mm0;             // mm1 = mm0
			punpcklwd mm0, mm6;		   // mm0 -> unpack high word
			punpckhwd mm1, mm6;		   // mm1 -> unpack low word
			paddd mm0, mm1;			   // mm0 += mm1

			//doubleword단계
			movq mm1, mm0;             // mm1 = mm0
			punpckldq mm0, mm6;		   // mm0 -> unpack high double word
			punpckhdq mm1, mm6;		   // mm1 -> unpack low double word
			paddq mm0, mm1;			   // mm0 += mm1

			//결과 리턴
			movd eax, mm0;

			emms; // empty mmx state
		}
	}
*/
	int Match::bitCount(unsigned __int64 n)
	{
		n = ((0xaaaaaaaaaaaaaaaa & n) >> 1) + (0x5555555555555555 & n);
		n = ((0xcccccccccccccccc & n) >> 2) + (0x3333333333333333 & n);
		n = ((0xf0f0f0f0f0f0f0f0 & n) >> 4) + (0x0f0f0f0f0f0f0f0f & n);
		n = ((0xff00ff00ff00ff00 & n) >> 8) + (0x00ff00ff00ff00ff & n);
		n = ((0xffff0000ffff0000 & n) >> 16) + (0x0000ffff0000ffff & n);
		n = ((0xffffffff00000000 & n) >> 32) + (0x00000000ffffffff & n);
		return n;
	}
}