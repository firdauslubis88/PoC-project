#include "StdAfx.h"
#include "Descriptor.h"


BORA::Descriptor::Descriptor(void)
	: position_(0, 0)
	, parent_bin_(-1)
{
	indices_.clear();
	for(unsigned int i = 0 ; i < BORA::QUANTIZATION_LEVEL ; ++i)
	{
		data_[i] = 0;
	}
}

BORA::Descriptor::Descriptor(const BORA::Feature &_trained_feature, const BORA::Position &_ref_position)
	: position_(_ref_position)
{
	// 초기화
	indices_.clear();
	for(unsigned int i = 0 ; i < BORA::QUANTIZATION_LEVEL ; ++i)
	{
		data_[i] = 0;
	}
	parent_bin_ = -1;
	
	// 디스크립터 데이터 정보
	const BORA::HIP &crnt_hip = _trained_feature.getHIP_ref();
	for(unsigned int i = 0 ; i < 5 ; ++i)
	{
		const BORA::HIST_Q_PATCH &patch_of_hip = crnt_hip.getData(i);
		for(unsigned int j = 0 ; j < 64 ; ++j)
		{
			if(patch_of_hip[j] < 0.05)
				data_[i] |= BORA::shift64[63-j];
		}
	}

	// 인덱스들 정보
	indices_.clear();
	const BORA::VOTEINDICES &voteIndice = _trained_feature.getVoteIndice_ref();
	BORA::VOTEINDICES::const_iterator itr_voteIndice = voteIndice.begin();
	while(itr_voteIndice != voteIndice.end())
	{
		indices_.push_back(itr_voteIndice->first);
		std::advance(itr_voteIndice, 1);
	}

	// 디버깅 정보
	parent_bin_ = _trained_feature.getParentBin();
}


BORA::Descriptor::Descriptor( const BORA::Feature &_trained_feature, const CvMat *_invH, const BORA::Position &_shifted_pos)
{
	// 초기화
	indices_.clear();
	for(unsigned int i = 0 ; i < BORA::QUANTIZATION_LEVEL ; ++i)
	{
		data_[i] = 0;
	}

	// 위치 정보
	position_ = (_invH * _trained_feature.getPosition_ref()) - _shifted_pos;

	// 디스크립터 데이터 정보
	const BORA::HIP &crnt_hip = _trained_feature.getHIP_ref();
	for(unsigned int i = 0 ; i < 5 ; ++i)
	{
		const BORA::HIST_Q_PATCH &patch_of_hip = crnt_hip.getData(i);
		for(unsigned int j = 0 ; j < 64 ; ++j)
		{
			if(patch_of_hip[j] < 0.05)
				data_[i] |= BORA::shift64[63-j];
		}
	}
	
	// 인덱스들 정보
	indices_.clear();
	const BORA::VOTEINDICES &voteIndice = _trained_feature.getVoteIndice_ref();
	BORA::VOTEINDICES::const_iterator itr_voteIndice = voteIndice.begin();
	while(itr_voteIndice != voteIndice.end())
	{
		indices_.push_back(itr_voteIndice->first);
		std::advance(itr_voteIndice, 1);
	}

	// 디버깅 정보
	parent_bin_ = _trained_feature.getParentBin();
}


BORA::Descriptor::~Descriptor(void)
{
}

unsigned __int64 BORA::Descriptor::compare( const Descriptor &_other )
{
	return 0;
}

void BORA::Descriptor::setPosition( const BORA::Position &_pos )
{
	position_ = _pos;
}

void BORA::Descriptor::setIndices( const BORA::INDICES &_indices )
{
	indices_ = _indices;
}

void BORA::Descriptor::addIndex( const BORA::INDEX &_index )
{
	indices_.push_back(_index);
}

BORA::Position BORA::Descriptor::getPosition()
{
	return position_;
}

const BORA::Position & BORA::Descriptor::getPosition() const
{
	return position_;
}

BORA::INDICES BORA::Descriptor::getIndices()
{
	return indices_;
}

const BORA::INDICES & BORA::Descriptor::getIndices() const
{
	return indices_;
}

BORA::DESC BORA::Descriptor::getDesc()
{
	return data_;
}

const BORA::DESC	& BORA::Descriptor::getDesc() const
{
	return data_;
}

void BORA::Descriptor::Clear()
{
	position_(0, 0);
	indices_.clear();
	//data_.fill(0);
	data_.assign(0);
}

void BORA::Descriptor::setDesc( const BORA::DESC &_desc )
{
	data_ = _desc;
}

void BORA::Descriptor::setParentBin( const int &_bin_idx )
{
	parent_bin_ = _bin_idx;
}

const int & BORA::Descriptor::getParentBin_ref() const
{
	return parent_bin_;
}

int & BORA::Descriptor::getParentBin_ref()
{
	return parent_bin_;
}

const int BORA::Descriptor::getParentBin() const
{
	return parent_bin_;
}

int BORA::Descriptor::getParentBin()
{
	return parent_bin_;
}

std::string BORA::getBinaryString( unsigned __int64 num )
{
	std::string result;
	for(unsigned int i = 0 ; i < 64 ; ++i)
	{
		int b = num >> 63;

		if(b == 0)
			result += "0 ";
		else
			result += "1 ";

		if((i+1)%4 == 0)
			result += ", ";

		num = num << 1;
	}

	return result;
}

namespace BORA {
//bool BORA::operator==( const BORA::Descriptor &_A, const BORA::Descriptor &_B )
bool operator==( const Descriptor &_A, const Descriptor &_B )
{
	//if(_A.indices_.size() != _B.indices_.size())
	//	return false;

	//const unsigned int indice_size = _A.indices_.size();
	//for(unsigned int i = 0 ; i < indice_size ; ++i)
	//{
	//	if(_A.indices_[i] != _B.indices_[i])
	//		return false;
	//}

	if(_A.position_ != _B.position_)
	{
		printf("Descirptor position not same.\n");
		return false;
	}

	if(_A.data_ != _B.data_)
	{
		printf("Descriptor data not same.\n");
		return false;
	}

	return true;
}
}

bool operator==( const BORA::DESCRIPTORS &_A, const BORA::DESCRIPTORS &_B )
{
	if(_A.size() != _B.size())
	{
		printf("between descriptors not same.\n");
		return false;
	}

	const unsigned int descriptors_size = _A.size();

	for(unsigned int i = 0 ; i < descriptors_size ; ++i)
	{
		if((_A[i] == _B[i]) == false)
		{
			printf("%u th desscriptor not same.\n", i);
			return false;
		}
	}
	return true;
}
