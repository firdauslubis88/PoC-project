#include "StdAfx.h"
#include "Database.h"



BORA::Database::~Database(void)
{
}

BORA::Database::Database( void )
{
}

BORA::Database::Database( const BORA::DESCRIPTORS &_descriptors, const BORA::IndexTable &_indexTable )
	: descriptors_(_descriptors)
	, indexTable_(_indexTable)
{
}

void BORA::Database::setDatabase( const BORA::DESCRIPTORS &_descriptors, const BORA::IndexTable &_indexTable )
{
	if(descriptors_.empty() == false)
	{
		descriptors_.clear();
	}

	if(indexTable_.empty() == false)
	{
		indexTable_.Clear();
	}

	descriptors_	= _descriptors;
	indexTable_		= _indexTable;
}

void BORA::Database::getDatabase( BORA::DESCRIPTORS &_descriptors, BORA::IndexTable &_indexTable )
{
	// ������ �ڷ� �ʱ�ȭ
	_descriptors.clear();
	_indexTable.Clear();

	_descriptors = descriptors_;
	_indexTable = indexTable_;
}

void BORA::Database::setBinInfos( const BORA::BININFOS &_bin_infos )
{
	binInfos_ = _bin_infos;
}

const void BORA::Database::getDatabase( BORA::DESCRIPTORS &_descriptors, BORA::IndexTable &_indexTable ) const
{
	// ������ �ڷ� �ʱ�ȭ
	_descriptors.clear();
	_indexTable.Clear();

	_descriptors = descriptors_;
	_indexTable = indexTable_;
}

BORA::IndexTable & BORA::Database::getIndexTableRef()
{
	return indexTable_;
}

const BORA::IndexTable & BORA::Database::getIndexTableRef() const
{
	return indexTable_;
}

BORA::DESCRIPTORS & BORA::Database::getDescsRef()
{
	return descriptors_;
}

const BORA::DESCRIPTORS & BORA::Database::getDescsRef() const
{
	return descriptors_;
}

const BORA::BININFOS	&BORA::Database::getBinInfos_ref() const
{
	return binInfos_;
}

BORA::BININFOS		&BORA::Database::getBinInfos_ref()
{
	return binInfos_;
}

const BORA::BININFOS	BORA::Database::getBinInfos() const
{
	return binInfos_;
}

BORA::BININFOS		BORA::Database::getBinInfos()
{
	return binInfos_;
}

bool BORA::operator==( const BORA::Database &_A, const BORA::Database &_B )
{
	if(_A.image_size_.x != _B.image_size_.x || _A.image_size_.y != _B.image_size_.y)
		return false;

	return _A.descriptors_ == _B.descriptors_ && _A.indexTable_ == _B.indexTable_;
}

BORA::Position BORA::Database::getImageSize()
{
	return image_size_;
}

const BORA::Position &BORA::Database::getImageSize() const
{
	return image_size_;
}


void BORA::Database::setImageSize( const BORA::Position &_image_size )
{
	image_size_ = _image_size;
}

bool BORA::Database::WriteToFile( const std::string &_path, const unsigned int &_type )
{
	std::ofstream saveStream(_path.c_str(), std::ios::binary);
	if( !saveStream.is_open() )
		return false;

	// �⺻ ������ ����.
	saveStream.write((char*)&_type, sizeof(_type));
	saveStream.write((char*)&image_size_, sizeof(image_size_));

	bool result(false);
	switch(_type)
	{
	case BORA::DBTYPE::VER1:
		result = WriteTYPE0(saveStream);
		break;
	case BORA::DBTYPE::KIST:
		result = WriteTYPE10(saveStream);
		break;
	case BORA::DBTYPE::DEBUG1:
		result = WriteTYPE100(saveStream);
		break;
	}

	saveStream.close();
	return result;
}

bool BORA::Database::ReadFromFile(const std::string &_path)
{
	std::ifstream readStream(_path.c_str(), std::ios::binary);
	if( !readStream.is_open() )
		return false;

	// �⺻ ������ �д´�.
	unsigned int type(0);
	readStream.read((char*)&type, sizeof(type));
	readStream.read((char*)&image_size_, sizeof(image_size_));

	bool result(false);
	switch(type)
	{
	case BORA::DBTYPE::VER1:
		result = ReadTYPE0(readStream);
		break;
	case BORA::DBTYPE::KIST:
		result = ReadTYPE10(readStream);
		break;
	case BORA::DBTYPE::DEBUG1:
		result = ReadTYPE100(readStream);
		break;
	}

	return result;
}

bool BORA::Database::ReadFromFile(const std::string &_path, int &width, int &height, unsigned int &type)
{
	std::cout << "Loading database " << _path << "..." << std::endl;

	std::ifstream readStream(_path.c_str(), std::ios::binary);
	if( !readStream.is_open() )
	{
		std::cerr << "error: [Database::ReadFromFile] File is not found: " << _path << std::endl;
		return false;
	}


	// Dec. 2011, Min-Hyuk Sung
	// �� �ʱ�ȭ
	indexTable_.Clear();
	for(unsigned int i = 0 ; i < descriptors_.size() ; ++i)
		descriptors_[i].Clear();


	// �⺻ ������ �д´�.
	readStream.read((char*)&type, sizeof(type));
	readStream.read((char*)&image_size_, sizeof(image_size_));

	width  = image_size_.x;
	height = image_size_.y;

	bool result(false);
	switch(type)
	{
	case BORA::DBTYPE::VER1:
		result = ReadTYPE0(readStream);
		break;
	case BORA::DBTYPE::KIST:
		result = ReadTYPE10(readStream);
		break;
	case BORA::DBTYPE::DEBUG1:
		result = ReadTYPE100(readStream);
		break;
	}

	// Dec. 2011, Min-Hyuk Sung
	// DEBUG
	//std::cout << "Save to debug.dat... \n";
	//WriteToFile( "debug.dat", type );

	return result;
}


/* DBTYPE : VER1 */
bool BORA::Database::ReadTYPE0( std::ifstream &readStream )
{
	// Assume that all previous data are cleared

	// ��ũ����s �б�
	unsigned int descriptorSize(0);
	readStream.read((char*)&descriptorSize, sizeof(descriptorSize));
	descriptors_.reserve(descriptorSize);

	for(unsigned int i = 0 ; i < descriptorSize ; ++i)
	{
		BORA::DESC		desc;
		BORA::Position	position;
		unsigned int			idx;

		readStream.read((char*)&desc, sizeof(desc));
		readStream.read((char*)&position, sizeof(position));

		unsigned int			indices_size;
		BORA::INDICES	indices;
		readStream.read((char*)&indices_size, sizeof(indices_size));
		for(unsigned int j = 0 ; j < indices_size ; ++j)
		{
			BORA::INDEX crnt_index;
			readStream.read((char*)&crnt_index, sizeof(crnt_index));
			indices.push_back(crnt_index);
		}

		readStream.read((char*)&idx, sizeof(idx));

		// �ε��� üũ
		if( i != idx )
			return false;

		BORA::Descriptor readDescriptor;
		readDescriptor.setDesc(desc);
		readDescriptor.setPosition(position);
		readDescriptor.setIndices(indices);

		// �ϳ��� �ҷ��� ��ũ���� ����
		descriptors_.push_back(readDescriptor);
	}

	// �ε��� ���̺� �ҷ�����
	unsigned int indexTableSize(0);
	readStream.read((char*)&indexTableSize, sizeof(indexTableSize));
	BORA::INDEXTABLE &indexTableData = indexTable_.getData();

	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		unsigned int i_th_indexTable_size(0);
		readStream.read((char*)&i_th_indexTable_size, sizeof(i_th_indexTable_size));

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		//indexTableData[i].reserve(i_th_indexTable_size);
		for(unsigned int j = 0 ; j < i_th_indexTable_size ; ++j)
		{
			BORA::INDEX value(0);
			readStream.read((char*)&value, sizeof(value));

			indexTableData[i].push_back(value);
		}
	}

	readStream.close();
	return true;
}
bool BORA::Database::WriteTYPE0( std::ofstream &saveStream )
{
	//��ũ���͸� ����.
	unsigned int descriptorSize = descriptors_.size();
	saveStream.write((char *)&descriptorSize, sizeof(descriptorSize));

	for(unsigned int i = 0 ; i < descriptorSize ; ++i)
	{
		const BORA::DESC		&desc		= descriptors_[i].getDesc();
		const BORA::Position	&position	= descriptors_[i].getPosition();
		const BORA::INDICES		&indices	= descriptors_[i].getIndices();
		const unsigned int			indices_size = indices.size();

		saveStream.write((char *)&desc,		sizeof(desc));
		saveStream.write((char *)&position, sizeof(position));

		saveStream.write((char *)&indices_size, sizeof(indices_size));
		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = indices.begin(); it != indices.end(); ++it)
		{
			const BORA::INDEX &crnt_index = (*it);
			saveStream.write((char *)&crnt_index, sizeof(crnt_index));
		}
		/*
		for(unsigned int j = 0 ; j < indices_size ; ++j)
		{
			const BORA::INDEX &crnt_index = indices[j];
			saveStream.write((char *)&crnt_index, sizeof(crnt_index));
		}
		*/

		saveStream.write((char *)&i,		sizeof(i));
	}

	// �ε��� ���̺��� ����.
	// �ϴ��� 13�ε����� ������ �ϹǷ� ���� �ڵ��� 13�ε����� ������.
	unsigned int indexTableSize(BORA::INDEX_13_MAX);
	saveStream.write((char*)&indexTableSize, sizeof(indexTableSize));
	const BORA::INDEXTABLE &indexTableData = indexTable_.getData();

	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		unsigned int i_th_indexTable_size = indexTableData[i].size();
		saveStream.write((char*)&i_th_indexTable_size, sizeof(i_th_indexTable_size));

		const BORA::INDICES &i_th_indice = indexTableData[i];
		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = i_th_indice.begin(); it != i_th_indice.end(); ++it)
		{
			const BORA::INDEX &value = (*it);
			saveStream.write((char*)&value, sizeof(value));
		}
		/*
		for(unsigned int j = 0 ; j < i_th_indice.size() ; ++j)
		{
			const BORA::INDEX &value = i_th_indice[j];
			saveStream.write((char*)&value, sizeof(value));
		}
		*/
	}

	saveStream.close();
	return true;
}

/* DBTYPE : KIST */
bool BORA::Database::ReadTYPE10( std::ifstream &readStream)
{
	// Assume that all previous data are cleared

	unsigned int desc_size(0);
	readStream.read((char*)&desc_size, sizeof(desc_size));

	descriptors_.reserve(desc_size);
	for(unsigned int i = 0 ; i < desc_size ; ++i)
	{
		BORA::Descriptor descriptor;
		BORA::Position pos(0, 0);
		BORA::DESC desc;

		readStream.read((char*)&pos.x, sizeof(pos.x));
		readStream.read((char*)&pos.y, sizeof(pos.y));
		readStream.read((char*)&desc, sizeof(desc));

		descriptor.setDesc(desc);
		descriptor.setPosition(pos);

		descriptors_.push_back(descriptor);
	}
	
	BORA::INDEXTABLE &indexTableData = indexTable_.getData();
	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		unsigned int idx_size(0);
		readStream.read((char*)&idx_size, sizeof(idx_size));

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		//table[i].reserve(idx_size);
		for(unsigned int j = 0 ; j < idx_size ; ++j)
		{
			int idx_value(0);
			readStream.read((char*)&idx_value, sizeof(idx_value));

			indexTableData[i].push_back(idx_value);
		}
	}

	return true;
}
bool BORA::Database::WriteTYPE10(std::ofstream &saveStream)
{
	unsigned int desc_size = descriptors_.size();
	saveStream.write((char*)&desc_size, sizeof(desc_size));

	for(unsigned int i = 0 ; i < desc_size ; ++i)
	{
		const BORA::Position &pos = descriptors_[i].getPosition();
		const BORA::DESC &desc = descriptors_[i].getDesc();

		saveStream.write((char*)&pos.x, sizeof(pos.x));
		saveStream.write((char*)&pos.y, sizeof(pos.y));
		saveStream.write((char*)&desc, sizeof(desc));
	}

	BORA::INDEXTABLE &table = indexTable_.getData();
	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		unsigned int idx_size = table[i].size();
		saveStream.write((char*)&idx_size, sizeof(idx_size));

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = table[i].begin(); it != table[i].end(); ++it)
		{
			int idx_value = (*it);
			saveStream.write((char*)&idx_value, sizeof(idx_value));
		}
		/*
		for(unsigned int j = 0 ; j < idx_size ; ++j)
		{
			int idx_value = table[i][j];
			saveStream.write((char*)&idx_value, sizeof(idx_value));
		}
		*/
	}

	return true;
}

/* DBTYPE : DEBUG1 */
bool BORA::Database::ReadTYPE100(std::ifstream &readStream)
{
	// Assume that all previous data are cleared

	// ��ũ����s �б�
	unsigned int descriptorSize(0);
	readStream.read((char*)&descriptorSize, sizeof(descriptorSize));
	descriptors_.reserve(descriptorSize);

	for(unsigned int i = 0 ; i < descriptorSize ; ++i)
	{
		BORA::DESC		desc;
		BORA::Position	position;
		int				bin_idx;	//***** �߰�
		unsigned int			idx;

		readStream.read((char*)&desc, sizeof(desc));
		readStream.read((char*)&position, sizeof(position));
		readStream.read((char*)&bin_idx, sizeof(bin_idx));	//***** �߰�

		unsigned int			indices_size;
		BORA::INDICES	indices;
		readStream.read((char*)&indices_size, sizeof(indices_size));
		for(unsigned int j = 0 ; j < indices_size ; ++j)
		{
			BORA::INDEX crnt_index;
			readStream.read((char*)&crnt_index, sizeof(crnt_index));
			indices.push_back(crnt_index);
		}

		readStream.read((char*)&idx, sizeof(idx));

		// �ε��� üũ
		if( i != idx )
			return false;

		BORA::Descriptor readDescriptor;
		readDescriptor.setDesc(desc);
		readDescriptor.setPosition(position);
		readDescriptor.setIndices(indices);
		readDescriptor.setParentBin(bin_idx);	//***** �߰�

		// �ϳ��� �ҷ��� ��ũ���� ����
		descriptors_.push_back(readDescriptor);
	}

	// �ε��� ���̺� �ҷ�����
	unsigned int indexTableSize(0);
	readStream.read((char*)&indexTableSize, sizeof(indexTableSize));
	BORA::INDEXTABLE &indexTableData = indexTable_.getData();

	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		unsigned int i_th_indexTable_size(0);
		readStream.read((char*)&i_th_indexTable_size, sizeof(i_th_indexTable_size));

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		//indexTableData[i].reserve(i_th_indexTable_size);
		for(unsigned int j = 0 ; j < i_th_indexTable_size ; ++j)
		{
			BORA::INDEX value(0);
			readStream.read((char*)&value, sizeof(value));

			indexTableData[i].push_back(value);
		}
	}

	// bin ���� �ҷ�����
	unsigned int bin_info_size(0);
	readStream.read((char*)&bin_info_size, sizeof(bin_info_size));
	for(unsigned int i = 0 ; i < bin_info_size ; ++i)
	{
		BORA::BinInfo readBinInfo;
		readStream.read((char*)&readBinInfo.bin_rotation_,					sizeof(readBinInfo.bin_rotation_));
		readStream.read((char*)&readBinInfo.bin_scale_,						sizeof(readBinInfo.bin_scale_));
		readStream.read((char*)&readBinInfo.bin_perspective_,				sizeof(readBinInfo.bin_perspective_));
		readStream.read((char*)&readBinInfo.bin_space_partition_start_pos_, sizeof(readBinInfo.bin_space_partition_start_pos_));
		readStream.read((char*)&readBinInfo.bin_space_partition_end_pos_,	sizeof(readBinInfo.bin_space_partition_end_pos_));

		readStream.read((char*)&readBinInfo.bin_rotation_flag_,				sizeof(readBinInfo.bin_rotation_flag_));
		readStream.read((char*)&readBinInfo.bin_scale_flag_,				sizeof(readBinInfo.bin_scale_flag_));
		readStream.read((char*)&readBinInfo.bin_perspec_flag_,				sizeof(readBinInfo.bin_perspec_flag_));
		readStream.read((char*)&readBinInfo.bin_X_space_partition_flag_,	sizeof(readBinInfo.bin_X_space_partition_flag_));
		readStream.read((char*)&readBinInfo.bin_Y_space_partition_flag_,	sizeof(readBinInfo.bin_Y_space_partition_flag_));

		readStream.read((char*)&readBinInfo.bin_truncation_amount_,	sizeof(readBinInfo.bin_truncation_amount_));

		binInfos_.push_back(readBinInfo);
	}

	//***** �߰�
	for(unsigned int i = 0 ; i < descriptorSize ; ++i)
	{
		const int bin_idx = descriptors_[i].getParentBin_ref();
		if(bin_idx < 0 || bin_idx >= (int)binInfos_.size())
		{
			printf("error: [ReadTYPE100] Bin index of (%d) = %d\n", i, bin_idx);
		}
	}
	
	readStream.close();
	return true;
}
bool BORA::Database::WriteTYPE100(std::ofstream &saveStream)
{
	//��ũ���͸� ����.
	unsigned int descriptorSize = descriptors_.size();
	saveStream.write((char *)&descriptorSize, sizeof(descriptorSize));

	for(unsigned int i = 0 ; i < descriptorSize ; ++i)
	{
		const BORA::DESC		&desc		= descriptors_[i].getDesc();
		const BORA::Position	&position	= descriptors_[i].getPosition();
		const int				&bin_idx	= descriptors_[i].getParentBin_ref();	//***** �߰�
		const BORA::INDICES		&indices	= descriptors_[i].getIndices();
		const unsigned int			indices_size = indices.size();

		saveStream.write((char *)&desc,		sizeof(desc));
		saveStream.write((char *)&position, sizeof(position));
		saveStream.write((char *)&bin_idx,	sizeof(bin_idx));	//***** �߰�
		//***** �߰�
		if(bin_idx < 0 || bin_idx >= (int)binInfos_.size())
		{
			printf("error: [WriteTYPE100] Bin index of (%d) = %d\n", i, bin_idx);
		}

		saveStream.write((char *)&indices_size, sizeof(indices_size));
		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = indices.begin(); it != indices.end(); ++it)
		{
			const BORA::INDEX &crnt_index = (*it);
			saveStream.write((char *)&crnt_index, sizeof(crnt_index));
		}
		/*
		for(unsigned int j = 0 ; j < indices_size ; ++j)
		{
			const BORA::INDEX &crnt_index = indices[j];
			saveStream.write((char *)&crnt_index, sizeof(crnt_index));
		}
		*/

		saveStream.write((char *)&i,		sizeof(i));
	}

	// �ε��� ���̺��� ����.
	// �ϴ��� 13�ε����� ������ �ϹǷ� ���� �ڵ��� 13�ε����� ������.
	unsigned int indexTableSize(BORA::INDEX_13_MAX);
	saveStream.write((char*)&indexTableSize, sizeof(indexTableSize));
	const BORA::INDEXTABLE &indexTableData = indexTable_.getData();

	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		unsigned int i_th_indexTable_size = indexTableData[i].size();
		saveStream.write((char*)&i_th_indexTable_size, sizeof(i_th_indexTable_size));

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		const BORA::INDICES &i_th_indice = indexTableData[i];
		for(BORA::INDICES::const_iterator it = i_th_indice.begin(); it != i_th_indice.end(); ++it)
		{
			const BORA::INDEX &value = (*it);
			saveStream.write((char*)&value, sizeof(value));
		}
		/*
		for(unsigned int j = 0 ; j < i_th_indice.size() ; ++j)
		{
			const BORA::INDEX &value = i_th_indice[j];
			saveStream.write((char*)&value, sizeof(value));
		}
		*/
	}

	// ������
	unsigned int bin_info_size (binInfos_.size());
	saveStream.write((char*)&bin_info_size, sizeof(bin_info_size));
	for(unsigned int i = 0 ; i < bin_info_size ; ++i)
	{
		saveStream.write((char*)&binInfos_[i].bin_rotation_,					sizeof(binInfos_[i].bin_rotation_));
		saveStream.write((char*)&binInfos_[i].bin_scale_,						sizeof(binInfos_[i].bin_scale_));
		saveStream.write((char*)&binInfos_[i].bin_perspective_,					sizeof(binInfos_[i].bin_perspective_));
		saveStream.write((char*)&binInfos_[i].bin_space_partition_start_pos_,	sizeof(binInfos_[i].bin_space_partition_start_pos_));
		saveStream.write((char*)&binInfos_[i].bin_space_partition_end_pos_,		sizeof(binInfos_[i].bin_space_partition_end_pos_));

		saveStream.write((char*)&binInfos_[i].bin_rotation_flag_,			sizeof(binInfos_[i].bin_rotation_flag_));
		saveStream.write((char*)&binInfos_[i].bin_scale_flag_,				sizeof(binInfos_[i].bin_scale_flag_));
		saveStream.write((char*)&binInfos_[i].bin_perspec_flag_,			sizeof(binInfos_[i].bin_perspec_flag_));
		saveStream.write((char*)&binInfos_[i].bin_X_space_partition_flag_,	sizeof(binInfos_[i].bin_X_space_partition_flag_));
		saveStream.write((char*)&binInfos_[i].bin_Y_space_partition_flag_,	sizeof(binInfos_[i].bin_Y_space_partition_flag_));

		saveStream.write((char*)&binInfos_[i].bin_truncation_amount_,	sizeof(binInfos_[i].bin_truncation_amount_));
	}

	saveStream.close();
	return true;
}

// Dec. 2011 Min-Hyuk Sung
// ---- //
bool BORA::Database::MergeFiles(const std::string &_path, int &width, int &height, unsigned int &type)
{
	// �� �ʱ�ȭ
	indexTable_.Clear();
	for(unsigned int i = 0 ; i < descriptors_.size() ; ++i)
		descriptors_[i].Clear();

	for( unsigned int SEQ_IDX = 0; true; SEQ_IDX++ )
	{
		std::stringstream filepath;
		filepath << _path << "." << std::setfill('0') << std::setw(3) << SEQ_IDX;

		std::ifstream readStream( filepath.str().c_str(), std::ios::binary );
		if( !readStream.is_open() )
			break;

		std::cout << "Loading database " << filepath.str() << "..." << std::endl;

		// �⺻ ������ �д´�.
		readStream.read((char*)&type, sizeof(type));
		readStream.read((char*)&image_size_, sizeof(image_size_));

		width  = image_size_.x;
		height = image_size_.y;

		bool result(false);
		switch(type)
		{
		case BORA::DBTYPE::VER1:
			result = AppendTYPE0(readStream);
			break;
		case BORA::DBTYPE::DEBUG1:
			result = AppendTYPE100(readStream);
			break;
		default:
			readStream.close();
			return false;
		}

		if( !result )	return false;
	}

	// Dec. 2011, Min-Hyuk Sung
	// DEBUG
	std::cout << "Save to merge_debug.dat... \n";
	WriteToFile( "merge_debug.dat", type );

	return true;
}

bool BORA::Database::AppendTYPE0( std::ifstream &readStream )
{
	// Append
	unsigned int descriptor_idx_offset = descriptors_.size();
	unsigned int bin_idx_offset = binInfos_.size();

	// ��ũ����s �б�
	unsigned int descriptorSize(0);
	readStream.read((char*)&descriptorSize, sizeof(descriptorSize));
	descriptors_.reserve(descriptorSize);

	for(unsigned int i = 0 ; i < descriptorSize ; ++i)
	{
		BORA::DESC		desc;
		BORA::Position	position;
		unsigned int			idx;

		readStream.read((char*)&desc, sizeof(desc));
		readStream.read((char*)&position, sizeof(position));

		unsigned int			indices_size;
		BORA::INDICES	indices;
		readStream.read((char*)&indices_size, sizeof(indices_size));
		for(unsigned int j = 0 ; j < indices_size ; ++j)
		{
			BORA::INDEX crnt_index;
			readStream.read((char*)&crnt_index, sizeof(crnt_index));
			indices.push_back(crnt_index);
		}

		readStream.read((char*)&idx, sizeof(idx));

		// �ε��� üũ
		if( i != idx )
			return false;

		BORA::Descriptor readDescriptor;
		readDescriptor.setDesc(desc);
		readDescriptor.setPosition(position);
		readDescriptor.setIndices(indices);

		// �ϳ��� �ҷ��� ��ũ���� ����
		descriptors_.push_back(readDescriptor);
	}

	// �ε��� ���̺� �ҷ�����
	unsigned int indexTableSize(0);
	readStream.read((char*)&indexTableSize, sizeof(indexTableSize));
	BORA::INDEXTABLE &indexTableData = indexTable_.getData();

	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		unsigned int i_th_indexTable_size(0);
		readStream.read((char*)&i_th_indexTable_size, sizeof(i_th_indexTable_size));

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		//indexTableData[i].reserve(i_th_indexTable_size);
		for(unsigned int j = 0 ; j < i_th_indexTable_size ; ++j)
		{
			BORA::INDEX value(0);
			readStream.read((char*)&value, sizeof(value));

			// Append
			// value += offset
			indexTableData[i].push_back(descriptor_idx_offset + value);
		}
	}

	readStream.close();
	return true;
}

bool BORA::Database::AppendTYPE100( std::ifstream &readStream )
{
	// Append
	unsigned int descriptor_idx_offset = descriptors_.size();
	unsigned int bin_idx_offset = binInfos_.size();
	
	// ��ũ����s �б�
	unsigned int descriptorSize(0);
	readStream.read((char*)&descriptorSize, sizeof(descriptorSize));
	// �� �ʱ�ȭ
	// vector -> list
	descriptors_.reserve(descriptorSize);

	for(unsigned int i = 0 ; i < descriptorSize ; ++i)
	{
		BORA::DESC		desc;
		BORA::Position	position;
		int				bin_idx;	//***** �߰�
		unsigned int			idx;

		readStream.read((char*)&desc, sizeof(desc));
		readStream.read((char*)&position, sizeof(position));
		readStream.read((char*)&bin_idx, sizeof(bin_idx));	//***** �߰�

		unsigned int			indices_size;
		BORA::INDICES	indices;
		readStream.read((char*)&indices_size, sizeof(indices_size));
		for(unsigned int j = 0 ; j < indices_size ; ++j)
		{
			BORA::INDEX crnt_index;
			readStream.read((char*)&crnt_index, sizeof(crnt_index));
			indices.push_back(crnt_index);
		}

		readStream.read((char*)&idx, sizeof(idx));

		// �ε��� üũ
		if( i != idx )
			return false;

		BORA::Descriptor readDescriptor;
		readDescriptor.setDesc(desc);
		readDescriptor.setPosition(position);
		readDescriptor.setIndices(indices);
		// Jac. 2012, Min-Hyuk Sung
		readDescriptor.setParentBin(bin_idx_offset + bin_idx);	//***** �߰�

		// �ϳ��� �ҷ��� ��ũ���� ����
		descriptors_.push_back(readDescriptor);
	}

	// �ε��� ���̺� �ҷ�����
	unsigned int indexTableSize(0);
	readStream.read((char*)&indexTableSize, sizeof(indexTableSize));
	BORA::INDEXTABLE &indexTableData = indexTable_.getData();

	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		unsigned int i_th_indexTable_size(0);
		readStream.read((char*)&i_th_indexTable_size, sizeof(i_th_indexTable_size));

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		//indexTableData[i].reserve(i_th_indexTable_size);
		for(unsigned int j = 0 ; j < i_th_indexTable_size ; ++j)
		{
			BORA::INDEX value(0);
			readStream.read((char*)&value, sizeof(value));

			// Append
			// value += offset
			indexTableData[i].push_back(descriptor_idx_offset + value);
		}
	}

	// bin ���� �ҷ�����
	unsigned int bin_info_size(0);
	readStream.read((char*)&bin_info_size, sizeof(bin_info_size));
	for(unsigned int i = 0 ; i < bin_info_size ; ++i)
	{
		BORA::BinInfo readBinInfo;
		readStream.read((char*)&readBinInfo.bin_rotation_,					sizeof(readBinInfo.bin_rotation_));
		readStream.read((char*)&readBinInfo.bin_scale_,						sizeof(readBinInfo.bin_scale_));
		readStream.read((char*)&readBinInfo.bin_perspective_,				sizeof(readBinInfo.bin_perspective_));
		readStream.read((char*)&readBinInfo.bin_space_partition_start_pos_, sizeof(readBinInfo.bin_space_partition_start_pos_));
		readStream.read((char*)&readBinInfo.bin_space_partition_end_pos_,	sizeof(readBinInfo.bin_space_partition_end_pos_));

		readStream.read((char*)&readBinInfo.bin_rotation_flag_,				sizeof(readBinInfo.bin_rotation_flag_));
		readStream.read((char*)&readBinInfo.bin_scale_flag_,				sizeof(readBinInfo.bin_scale_flag_));
		readStream.read((char*)&readBinInfo.bin_perspec_flag_,				sizeof(readBinInfo.bin_perspec_flag_));
		readStream.read((char*)&readBinInfo.bin_X_space_partition_flag_,	sizeof(readBinInfo.bin_X_space_partition_flag_));
		readStream.read((char*)&readBinInfo.bin_Y_space_partition_flag_,	sizeof(readBinInfo.bin_Y_space_partition_flag_));

		readStream.read((char*)&readBinInfo.bin_truncation_amount_,	sizeof(readBinInfo.bin_truncation_amount_));

		binInfos_.push_back(readBinInfo);
	}

	//***** �߰�
	for(unsigned int i = 0 ; i < descriptorSize ; ++i)
	{
		const int bin_idx = descriptors_[i].getParentBin_ref();
		if(bin_idx < 0 || bin_idx >= (int)binInfos_.size())
		{
			printf("error: [AppendTYPE100] Bin index of (%d) = %d\n", i, bin_idx);
		}
	}
	
	readStream.close();
	return true;
}
// ---- //
