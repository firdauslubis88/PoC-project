#include "StdAfx.h"
#include "TrainingDescriptor.h"



BORA::TDescriptor::TDescriptor( void )
{
	
}

BORA::TDescriptor::TDescriptor(const BORA::Feature &_trainded_feature, const BORA::Position &_ref_pos)
	: Descriptor(_trainded_feature, _ref_pos)
{

}

BORA::TDescriptor::TDescriptor( const BORA::Feature &_trainded_feature, const CvMat *_invH, const BORA::Position &_shifted_pos )
	: Descriptor(_trainded_feature, _invH, _shifted_pos)
{
	
}

BORA::TDescriptor::~TDescriptor( void )
{

}

BORA::Feature BORA::TDescriptor::getFeature()
{
	return feature_;
}

const BORA::Feature & BORA::TDescriptor::getFeature() const
{
	return feature_;
}

void BORA::TDescriptor::ConvertToDescirptor( BORA::Descriptor &_desc )
{
	_desc.Clear();

	_desc.setPosition(position_);
	_desc.setIndices(indices_);
	_desc.setDesc(data_);

	// Dec. 2011, Min-Hyuk Sung
	_desc.setParentBin(parent_bin_);
}

void BORA::TDescriptor::setFeature( const BORA::Feature &_feature )
{
	feature_ = _feature;
}
