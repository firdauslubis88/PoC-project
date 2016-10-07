#include "StdAfx.h"
#include "IndexTable.h"


BORA::IndexTable::IndexTable(void)
{
}

BORA::IndexTable::~IndexTable(void)
{
}

void BORA::IndexTable::Clear()
{
	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		BORA::INDICES	&crntIndices = data_[i];
		if(crntIndices.empty() == false)
		{
			crntIndices.clear();
		}
	}
}

/* 디스크립터들을 인자로 받아서 Table의 data를 갱신한다. */
void BORA::IndexTable::Calc( const BORA::DESCRIPTORS &_descriptors)
{
	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		data_[i].clear();
	}

	for(unsigned int descriptor_idx = 0 ; descriptor_idx < _descriptors.size() ; ++descriptor_idx)
	{
		const BORA::Descriptor	&crntDescriptor = _descriptors[descriptor_idx];
		const BORA::INDICES		&crntIndices = crntDescriptor.getIndices();
		
		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		//std::cout << i << " th descriotor's indices : ";
		for(BORA::INDICES::const_iterator it = crntIndices.begin(); it != crntIndices.end(); ++it)
		{
			//std::cout << crntIndices[j] << ", ";
			unsigned int index_from_descriptor (*it);
			data_[index_from_descriptor].push_back(descriptor_idx);
		}
		/*
		for(unsigned int j = 0 ; j < crntIndices.size() ; ++j)
		{
			//std::cout << crntIndices[j] << ", ";
			unsigned int index_from_descriptor (crntIndices[j]);
			data_[index_from_descriptor].push_back(descriptor_idx);
		}
		*/
		//std::cout << std::endl;
	}
}

BORA::INDEXTABLE	& BORA::IndexTable::getData()
{
	return data_;
}

BORA::INDEXTABLE BORA::IndexTable::getDataClone()
{
	return data_;
}

const BORA::INDEXTABLE	& BORA::IndexTable::getDataConst() const
{
	return data_;
}

void BORA::IndexTable::setTable( const BORA::INDEXTABLE &_data )
{
	data_ = _data;
}

bool BORA::operator==( const BORA::IndexTable &_A, const BORA::IndexTable &_B )
{
	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		if(_A.data_[i].size() != _B.data_[i].size())
		{
			printf("_A.data_[i].size() : %d\n_B.data_[i].size() : %d\n %d th indices size not same.\n",
				(int)_A.data_[i].size(), (int)_B.data_[i].size(), i);
			return false;
		}

		const unsigned int crnt_data_size = _A.data_[i].size();

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		const BORA::INDICES A_indices = _A.data_[i];
		const BORA::INDICES B_indices = _B.data_[i];
		BORA::INDICES::const_iterator a_it = A_indices.begin();
		BORA::INDICES::const_iterator b_it = B_indices.begin();
		unsigned int j = 0;
		while(a_it != A_indices.end() && b_it != B_indices.end())
		{
			if( (*a_it) != (*b_it) )
			{
				printf("%u th index data are not the same.\n", j);
				return false;
			}
			a_it++;
			b_it++;
			j++;
		}
		/*
		for(unsigned int j = 0 ; j < crnt_data_size ; ++j)
		{
			if(_A.data_[i][j] == _B.data_[i][j])
				continue;
			printf("%u th indices data not same.\n", j);
			return false;
		}
		*/
	}

	return true;
}
