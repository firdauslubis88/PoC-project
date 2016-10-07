#include "StdAfx.h"
#include "IndexMap.h"


BORA::IndexMap::IndexMap( const BORA::Image &_image )
	: width_(_image.getWidth())
	, height_(_image.getHeight())
{
	reSize();
	Clear();
}

BORA::IndexMap::IndexMap( const unsigned int &_width, const unsigned int &_height )
	: width_(_width)
	, height_(_height)
{
	reSize();
	Clear();
}

BORA::IndexMap::~IndexMap( void )
{
	
}

void BORA::IndexMap::Clear( const int _data /*= -1*/ )
{
	for(unsigned int x = 0 ; x < width_ ; ++x)
		for(unsigned int y = 0 ; y < height_ ; ++y)
			data_[y][x] = _data;
}

/* rhs */
const int & BORA::IndexMap::operator()( const unsigned int &_x, const unsigned int &_y ) const
{
	return data_[_y][_x];
}

/* lhs */
int & BORA::IndexMap::operator()( const unsigned int &_x, const unsigned int &_y )
{
	return data_[_y][_x];
}

void BORA::IndexMap::reSize()
{
	data_.resize(height_);
	for(unsigned int i = 0 ; i < height_ ; ++i)
		data_[i].resize(width_);
}

