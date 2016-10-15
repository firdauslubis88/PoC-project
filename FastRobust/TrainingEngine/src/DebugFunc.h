/*******************************************************************
 DebugFunc 클래스

 Training Engine에서 사용하게 되는 디버깅에 대한 여러가지 함수
*******************************************************************/
#pragma once
#include <sstream>
#include <iomanip>

#include "GeneralDataType.h"
#include "Image.h"
#include "ImageUtility.h"
#include "IndexTable.h"
#include "Descriptor.h"
#include "TrainingDescriptor.h"
#include "Database.h"
// Feb. 2012, Min-Hyuk Sung
#ifdef WIN32
#include <io.h>
#include <direct.h>
//#else
//#include <boost/filesystem.hpp>
#endif

using namespace std;

namespace BORA
{
	class DebugFunc
	{
	public:
		// OpenCV를 이용해서 이미지를 창에 띄워서 보여준다.
		static void Show(const BORA::Image &_src, int _delay = 0);
		static void Show(const IplImage *_src, int _delay = 0);

		// Directory가 존재하는지 확인
		static bool isExistDir(const std::string &_path);

		// Directoory 생성
		static bool CreateDir(const std::string &_path);

		// 디스크립터로부터 좌표의 집합만 받아온다.
		static void DescriptosPoints(const BORA::DESCRIPTORS &_descs, BORA::POSITIONS &_positions);

		// 특징점으로 부터 좌표의 집합만 받아온다.
		static void FeaturesPoints(const BORA::FEATURES &_features, BORA::POSITIONS &_positions);

		// 데이터베이스의 내용을 파일에 쓴다.
		static void WriteDatabase(const std::string &_path, const BORA::Database &_database);

		// 디스크립터의 내용을 CSV파일로 쓴다.
		static void WriteDescriptorsToCSV(const std::string &_path, const BORA::DESCRIPTORS &_descriptors);

	private:
		static void ShowIplImage(const IplImage *_src, const int _delay);
	};

	class TrainingDebug
	{
	private:
		std::string		root_dir_;	// 이미지폴더/이미지명/
		std::ofstream	debug_;		// home.html

	public:
		TrainingDebug( void );
		~TrainingDebug( void );

		void StartDebug( const std::string &_path, const std::string &_image_file_name );
		void FinishDebug( void );

		void SaveImages( const BORA::Image &_ref_img, const BORA::Image &_ref_descriptors_img );
		void WriteDescriptorsSize( const unsigned int &_descriptorsSize );
		void WriteIndexTable( const BORA::IndexTable &_indexTable );

		std::string getRootDir(void);
		const std::string &getRootDir(void) const;

	private:
	};

	class IndexTableDebug
	{
	private:
		std::ofstream	debug_;
		std::string		root_dir_;

	public:
		IndexTableDebug(void);
		~IndexTableDebug(void);

		void StartDebug(const std::string &_path);
		void FinishDebug();

		void WriteIndexTableInfo(const BORA::IndexTable &_table);

		std::string getRootDir(void);
		const std::string &getRootDir(void) const;

	private:
	};

	class BinsDebug
	{
	private:
		std::string		root_dir_;	// 이미지폴더/이미지명/bins/
		std::ofstream	debug_;		// b_home.html
		
	public:
		BinsDebug( void );
		~BinsDebug( void );

		void StartDebug( const std::string &_path );
		void FinishDebug(void);

		void StartTable();
		void WriteOneElem(const unsigned int &_bin_index, const double &_bin_rotation, const double &_bin_scale);
		void FinishTable();

		std::string getRootDir(void);
		const std::string &getRootDir(void) const;
	};
	
	class BinDebug
	{
	private:
		std::string		root_dir_;	// 이미지폴더/이미지명/bins/i/
		std::ofstream	debug_;		// i_th_bin.html

	public:
		BinDebug(void);
		~BinDebug(void);

		void StartDebug(const std::string &_path, const unsigned int &_bin_index);
		void FinishDebug(void);

		void SaveImages(const BORA::Image &_cano_img, const BORA::Image &_fcano_img);

		void StartTrnTable();
		void WriteTrnOneElement(const unsigned int &_trn_index, const double &_rotation, const double &_scale, const unsigned int &_blur, const unsigned int &_noise, const double &_theta, const double &_phi, const double &_sx, const double &_sy, const bool &_apply_affine);
		void FinishTrnTable();

		/* before & after features sorting */

		/* 확정된 n개의 features만 기록 */
		void WirteFeaturesInfo(const BORA::FEATURES &_features);

		std::string getRootDir(void);
		const std::string &getRootDir(void) const;

	private:
		void WriteFeatureInfo(const BORA::Feature &_feature, const unsigned int &_t);
	};

	class TrnImageDebug
	{
	private:
		std::ofstream	debug_;
		std::string		root_dir_;

	public:
		TrnImageDebug(void);
		~TrnImageDebug(void);

		void StartDebug(const std::string &_path, const unsigned int &_bin_idx, const unsigned int &_trn_idx);
		void FinishDebug(void);
		
		void SaveImages(const BORA::Image &_trn_img, const BORA::Image &_ftrn_img);

		void WriteAffineInfo(const double &_rotation, const double &_scale, const unsigned int &_blur, const unsigned int &_noise, const double &_theta, const double &_phi, const double &_sx, const double &_sy, const bool &_apply_affine);

		void WritePatches(const BORA::FEATURES &_features);
		void SavePatchesImage(const BORA::FEATURES &_features);

		std::string getRootDir(void);
		const std::string &getRootDir(void) const;

	private:
	};

	class DescriptorsDebug
	{
	private:
		std::ofstream	debug_;
		std::string		root_dir_;

	public:
		DescriptorsDebug();
		~DescriptorsDebug();

		void StartDebug(const std::string &_path);
		void FinishDebug();

		void WriteDescriptorsTable(const BORA::TDESCRIPTORS &_descs);
		
		std::string getRootDir();
		const std::string &getRootDir() const;

	private:
		void WriteDescriptor(const BORA::TDescriptor &_desc, const unsigned int &_desc_idx);

		// 64비트 바이너리중 _i번째의 바이너리를 가져온다.
		unsigned int getValueFromDesc(const unsigned __int64 _data, const unsigned int &_i);
	};
}

