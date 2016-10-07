#include "StdAfx.h"
#include "HIP.h"


BORA::HIP::HIP(void)
{
	for(unsigned int i = 0 ; i < 5 ; ++i)
		//data_[i].fill(0.0);
		data_[i].assign(0.0);
}


BORA::HIP::~HIP(void)
{
}

// Dec. 2011, Min-Hyuk Sung
void BORA::HIP::clear()
{
	patches_.clear();
}

const BORA::PATCHES	& BORA::HIP::getPATCHES_ref() const
{
	return patches_;
}
BORA::PATCHES & BORA::HIP::getPATCHES_ref()
{
	return patches_;
}

BORA::PATCHES BORA::HIP::getPATCHES()
{
	return patches_;
}

const BORA::HIST_Q_PATCH	& BORA::HIP::getData( const unsigned int _idx ) const
{
	return data_[_idx];
}

BORA::HIST_Q_PATCH BORA::HIP::getData( const unsigned int _idx )
{
	return data_[_idx];
}


#if DIRECT_QUAN == 0

void BORA::HIP::addPatch( const BORA::Patch &_patch )
{
	patches_.push_back(_patch);
}

void BORA::HIP::addPATCHES( const BORA::HIP &_hip )
{
	const BORA::PATCHES &other_patches = _hip.getPATCHES_ref();

	for(unsigned int i = 0 ; i < other_patches.size() ; ++i)
		patches_.push_back(other_patches[i]);
}

void BORA::HIP::Normalize()
{
	// data_�� �ʱ�ȭ ��Ų��.
	for(unsigned int i = 0 ; i < 5 ; ++i)
	{
		//data_[i].fill(0.0);
		data_[i].assign(0.0);
	}

	// pathces_�� ��Ÿ�����̼� ��Ų��.
	const unsigned int patchesSize(patches_.size());
	BORA::PATCHES quantied_patches;
	quantied_patches.resize(patchesSize);
	for(unsigned int i = 0 ; i < patchesSize ; ++i)
	{
		BORA::ImageUtility::Quantization(patches_[i], quantied_patches[i]);
	}

	// ��Ÿ�����̼� ��Ų patches�� ������ ������ �Է��Ѵ�.
	for(unsigned int i = 0 ; i < patchesSize ; ++i)
	{
		const BORA::PATCH &crnt_patch = quantied_patches[i].getPATCH_ref();
		for(unsigned int j = 0 ; j < 64 ; ++j)
		{
			data_[crnt_patch[j]][j] += 1.0;
		}
	}

	// 64���� �ȼ��� ��ȸ �Ѵ�.
	for(unsigned int i = 0 ; i < 64 ; ++i)
	{
		// �� �ȼ����� ���� ��� 0~4 ������ ������ ���Ѵ�.
		double sum_all_size_a_pixel = data_[0][i] + data_[1][i] + data_[2][i] + data_[3][i] + data_[4][i];

		// �� ������ �Ѱ����� ������.
		data_[0][i] /= sum_all_size_a_pixel;
		data_[1][i] /= sum_all_size_a_pixel;
		data_[2][i] /= sum_all_size_a_pixel;
		data_[3][i] /= sum_all_size_a_pixel;
		data_[4][i] /= sum_all_size_a_pixel;
	}
}

#elif DIRECT_QUAN == 1
void BORA::HIP::addQPatch(const BORA::Patch &_qpatch)
{
	const BORA::PATCH &patch = _qpatch.getPATCH_ref();
	for(unsigned int pix = 0 ; pix < 64 ; ++pix)
		++data_[ patch[pix] ][pix];
}

void BORA::HIP::addQPATCHES(const BORA::HIP &_hip)
{
	const BORA::HISTS_Q_PATCH &other_hip = _hip.getDatas_ref();
	for(unsigned int ilevel = 0 ; ilevel < 5 ; ++ilevel)
	{
		for(unsigned int pixel = 0 ; pixel < 64 ; ++pixel)
		{
			data_[ilevel][pixel] += other_hip[ilevel][pixel];
		}
	}
}

void BORA::HIP::Normalize_without_quantize()
{
	// 64���� �ȼ��� ��ȸ �Ѵ�.
	for(unsigned int i = 0 ; i < 64 ; ++i)
	{
		// �� �ȼ����� ���� ��� 0~4 ������ ������ ���Ѵ�.
		double sum_all_size_a_pixel = data_[0][i] + data_[1][i] + data_[2][i] + data_[3][i] + data_[4][i];

		// �� ������ �Ѱ����� ������.
		data_[0][i] /= sum_all_size_a_pixel;
		data_[1][i] /= sum_all_size_a_pixel;
		data_[2][i] /= sum_all_size_a_pixel;
		data_[3][i] /= sum_all_size_a_pixel;
		data_[4][i] /= sum_all_size_a_pixel;
	}
}

#endif







unsigned int BORA::HIP::getPatchesSize()
{
	return patches_.size();
}

const unsigned int BORA::HIP::getPatchesSize() const
{
	return patches_.size();
}

std::tr1::array<BORA::HIST_Q_PATCH, 5u> BORA::HIP::getDatas()
{
	return data_;
}

const std::tr1::array<BORA::HIST_Q_PATCH, 5u> BORA::HIP::getDatas() const
{
	return data_;
}

BORA::HISTS_Q_PATCH & BORA::HIP::getDatas_ref()
{
	return data_;
}

const BORA::HISTS_Q_PATCH & BORA::HIP::getDatas_ref() const
{
	return data_;
}
