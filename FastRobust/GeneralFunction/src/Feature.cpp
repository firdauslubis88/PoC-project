#include "StdAfx.h"
#include "Feature.h"


BORA::Feature::Feature(void)
	: position_(0, 0)
	, orientation_(0.0)
	, parent_bin_(-1)
{
	indices_.clear();
}


BORA::Feature::~Feature(void)
{
}

#if DIRECT_QUAN == 0

void BORA::Feature::AddPatchAndIndex( const BORA::Patch &_patch, const BORA::INDEX &_index )
{
	// �ش� �ε����� ��ǥ�� �߰� �Ѵ�.
	++indices_[_index];

	// HIP �� ���� ��ġ�� �߰��Ѵ�.
	hip_.addPatch(_patch);
}

#elif DIRECT_QUAN == 1

void BORA::Feature::AddQPatchAndIndex(const BORA::Patch &_qpatch, const BORA::INDEX &_index)
{
	// �ش� �ε����� ��ǥ�� �߰� �Ѵ�.
	++indices_[_index];

	// HIP�� ���� ��ġ�� �߰��Ѵ�.
	hip_.addQPatch(_qpatch);
}

#endif


void BORA::Feature::AddFeatureData( const BORA::Feature &_other )
{
	// ���� ��ϵǴ� Feature�� �ƴ϶�, other�� hip������ indice�� ������ �������� �ȴ�.
	
#if DIRECT_QUAN == 0
	
	// HIP������ ���Ѵ�.
	hip_.addPATCHES(_other.hip_);

#elif DIRECT_QUAN == 1
	
	hip_.addQPATCHES(_other.hip_);

#endif
	
	// �ε������� �� �ε����� ��ǥ���� ���Ѵ�.
	BORA::VOTEINDICES::const_iterator itr = _other.indices_.begin();
	while(itr != _other.indices_.end())
	{
		indices_[itr->first] += itr->second;
		std::advance(itr, 1);
	}
}

/* Descriptor�� ����� ���� ���������� ���� �Ϸ��� �ൿ */
void BORA::Feature::Calc()
{
#if DIRECT_QUAN == 0
	// HIP������ �븻������ ��Ų��.
	hip_.Normalize();
#else
	hip_.Normalize_without_quantize();
#endif

	// �ε��� ����
	TruncateIndex();
}

void BORA::Feature::TruncateIndex()
{
	unsigned int total_vote_size(0);
	std::vector<BORA::VOTEelem> votes;
	votes.clear();

	BORA::VOTEINDICES::iterator itr_voteIndice = indices_.begin();
	unsigned int debug_count(0);
	while(itr_voteIndice != indices_.end())
	{
		total_vote_size += itr_voteIndice->second;
		votes.push_back(VOTEelem(itr_voteIndice->first, itr_voteIndice->second));
		std::advance(itr_voteIndice, 1);
	}
	
	// �������� ����
	std::sort(votes.begin(), votes.end(), SortVoteElem);

	const unsigned int truncation_count(static_cast<unsigned int>(total_vote_size * 0.8));
	unsigned int left_count(0);		// �������Ե� indices�� ����
	unsigned int crnt_vote_count(0);	// ������ ��ǥ �����
	unsigned int last_vote_count(0);
	
	// 80% ��ǥ�� ����
	while(crnt_vote_count <= truncation_count)
	{
		crnt_vote_count += last_vote_count = votes[left_count].second;
		++left_count;
	}
	
	//*
	// 80% ���Ŀ� ���� ��ǥ���� ���� ���ҵ� index�� ����� ��ƾ
	for(; left_count < votes.size() ; ++left_count)
	{
		if(last_vote_count > votes[left_count].second)
			break;
	}
	//*/
	
	// �������� üũ�ߴ� �ε���(left_count)�� �߰� �Ǹ� �ȵǹǷ� left_count �� ��ü�� ���� ������ �ȴ�.
	if(votes.size() > left_count)
		// Feb. 2012, Min-Hyuk Sung
		//votes.swap(std::vector<BORA::VOTEelem>(votes.begin(), votes.begin() + left_count));
		votes.resize(left_count);
	
	// ������ map Ÿ���� indices_�� ��ü�Ѵ�.
	indices_.clear();
	for(unsigned int i = 0 ; i < votes.size() ; ++i)
	{
		indices_[votes[i].first] = votes[i].second;
	}
	

}

const BORA::Position	& BORA::Feature::getPosition_ref() const
{
	return position_;
}
BORA::Position			& BORA::Feature::getPosition_ref()
{
	return position_;
}

BORA::Position BORA::Feature::getPosition()
{
	return position_;
}

const BORA::HIP			& BORA::Feature::getHIP_ref() const
{
	return hip_;
}
BORA::HIP				& BORA::Feature::getHIP_ref()
{
	return hip_;
}

BORA::HIP BORA::Feature::getHIP()
{
	return hip_;
}

const BORA::VOTEINDICES & BORA::Feature::getVoteIndice_ref() const
{
	return indices_;
}
BORA::VOTEINDICES &BORA::Feature::getVoteIndice_ref()
{
	return indices_;
}

BORA::VOTEINDICES BORA::Feature::getVoteIndice()
{
	return indices_;
}

void BORA::Feature::setPosition( const BORA::Position &_pos )
{
	position_ = _pos;
}

void BORA::Feature::operator=( const BORA::Feature &_other )
{
	position_ = _other.position_;
	indices_  = _other.indices_;
	hip_	  = _other.hip_;
}

void BORA::Feature::PrintMe()
{
	std::cout << "position : " << position_.x << ", " << position_.y << std::endl;

	std::cout << "indices\n";
	BORA::VOTEINDICES::iterator itr = indices_.begin();
	while(itr != indices_.end())
	{
		std::cout << itr->first << "(" << itr->second << ") ";
		std::advance(itr, 1);
	}
	std::cout << std::endl;

	
	const BORA::PATCHES &patches = hip_.getPATCHES_ref();
	std::cout << "patches(" << patches.size() << ")\n";
	for(unsigned int i = 0 ; i < patches.size() ; ++i)
	{
		const BORA::Patch &patch = patches[i];
		std::cout << patch.getPosition_ref().x << ", " << patch.getPosition_ref().y << std::endl;
	}
	std::cout << std::endl;
}

void BORA::Feature::setOrientation( const double &_orientation )
{
	orientation_ = _orientation;
}

void BORA::Feature::setParentBin( const int &_bin_idx )
{
	parent_bin_ = _bin_idx;
}

const double			& BORA::Feature::getOrientation_ref() const
{
	return orientation_;
}

double					& BORA::Feature::getOrientation_ref()
{
	return orientation_;
}

double BORA::Feature::getOrientation()
{
	return orientation_;
}

const int				& BORA::Feature::getParentBin_ref() const
{
	return parent_bin_;
}

int						& BORA::Feature::getParentBin_ref()
{
	return parent_bin_;
}

int BORA::Feature::getParentBin()
{
	return parent_bin_;
}

const int BORA::Feature::getParentBin() const
{
	return parent_bin_;
}

bool SortPatchNumber( const BORA::Feature & _A, const BORA::Feature & _B )
{
	return _A.getHIP_ref().getPATCHES_ref().size() > _B.getHIP_ref().getPATCHES_ref().size();
}

bool SortVoteElem( const BORA::VOTEelem &_A, const BORA::VOTEelem & _B )
{
	return _A.second > _B.second;
}

