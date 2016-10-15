#include "StdAfx.h"
#include "Patch.h"


BORA::Patch::Patch(void)
{
	Clear();
}


BORA::Patch::~Patch(void)
{
}

void BORA::Patch::setPosition( const BORA::Position &_pos )
{
	pos_ = _pos;
}

void BORA::Patch::setPATCH( const BORA::PATCH &_patch )
{
	patch_ = _patch;
}

BORA::Position BORA::Patch::getPosition()
{
	return pos_;
}

BORA::Position &BORA::Patch::getPosition_ref()
{
	return pos_;
}

const BORA::Position & BORA::Patch::getPosition_ref() const
{
	return pos_;
}

BORA::PATCH BORA::Patch::getPATCH()
{
	return patch_;
}

BORA::PATCH &BORA::Patch::getPATCH_ref()
{
	return patch_;
}

const BORA::PATCH & BORA::Patch::getPATCH_ref() const
{
	return patch_;
}

void BORA::Patch::Clear()
{
	pos_(0, 0);
	for(unsigned int i = 0 ; i < 64 ; ++i)
		patch_[i] = 0;
}

void BORA::Patch::setIndexInfo( const unsigned int &_bin_idx, const unsigned int &_trn_idx, const unsigned int &_my_idx )
{
	bin_idx_ = _bin_idx;
	trn_idx_ = _trn_idx;
	my_idx_  = _my_idx;
}

void BORA::Patch::getIndexInfo( unsigned int &_bin_idx, unsigned int &_trn_idx, unsigned int &_patch_idx )
{
	_bin_idx = bin_idx_;
	_trn_idx = trn_idx_;
	_patch_idx = my_idx_;
}

const void BORA::Patch::getIndexInfo( unsigned int &_bin_idx, unsigned int &_trn_idx, unsigned int &_patch_idx ) const
{
	_bin_idx = bin_idx_;
	_trn_idx = trn_idx_;
	_patch_idx = my_idx_;
}
