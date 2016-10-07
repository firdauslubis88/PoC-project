/*******************************************************************
 Database 클래스

 Training과 Matching에서 사용하는 Database클래스.
 Descriptors와 IndexLookupTable을 가지고 있다.
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "Descriptor.h"
#include "IndexTable.h"
#include "BinInfo.h"
#include <iostream>
#include <iomanip>

namespace BORA
{
	namespace DBTYPE
	{
		const unsigned int VER1		= 0;
		const unsigned int KIST		= 10;
		const unsigned int DEBUG1		= 100;
	}

	class Database
	{
	private:
		BORA::Position		image_size_;
		BORA::IndexTable	indexTable_;
		BORA::DESCRIPTORS	descriptors_;
		
		/* for debugging */
		BORA::BININFOS		binInfos_;

	public:
		Database(void);
		Database(const BORA::DESCRIPTORS &_descriptors, const BORA::IndexTable &_indexTable);
		~Database(void);

		BORA::IndexTable &getIndexTableRef();
		const BORA::IndexTable &getIndexTableRef() const;
		BORA::DESCRIPTORS &getDescsRef();
		const BORA::DESCRIPTORS &getDescsRef() const;
		BORA::Position getImageSize();
		const BORA::Position &getImageSize() const;
		
		const BORA::BININFOS	&getBinInfos_ref() const;
		BORA::BININFOS		&getBinInfos_ref();
		const BORA::BININFOS	getBinInfos() const;
		BORA::BININFOS		getBinInfos();

		void setImageSize(const BORA::Position &_image_size);
		void setBinInfos(const BORA::BININFOS &_bin_infos);
		void getDatabase(BORA::DESCRIPTORS &_descriptors, BORA::IndexTable &_indexTable);
		const void getDatabase(BORA::DESCRIPTORS &_descriptors, BORA::IndexTable &_indexTable) const;
		void setDatabase(const BORA::DESCRIPTORS &_descriptors, const BORA::IndexTable &_indexTable);

		bool WriteToFile(const std::string &_path, const unsigned int &_type);
		bool ReadFromFile(const std::string &_path);
		bool ReadFromFile(const std::string &_path, int &width, int &height, unsigned int &type);

		friend bool operator==(const BORA::Database &_A, const BORA::Database &_B);

	private:
		bool WriteTYPE0(std::ofstream &saveStream);
		bool ReadTYPE0(std::ifstream &readStream);

		bool WriteTYPE10(std::ofstream &saveStream);
		bool ReadTYPE10(std::ifstream &readStream);

		bool WriteTYPE100(std::ofstream &saveStream);
		bool ReadTYPE100(std::ifstream &readStream);

		bool WriteTYPECompact(std::ofstream &saveStream);
		bool ReadTYPECompact(std::ifstream &readStream);

	// Dec. 2011, Min-Hyuk Sung
	public:
		bool MergeFiles(const std::string &_path, int &width, int &height, unsigned int &type);
	private:
		bool AppendTYPE0(std::ifstream &readStream);
		bool AppendTYPE100(std::ifstream &readStream);
	};
}