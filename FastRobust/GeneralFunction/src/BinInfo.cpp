#include "StdAfx.h"
#include "BinInfo.h"

BORA::BinInfo::BinInfo()
	: bin_rotation_(0)
	, bin_scale_(1)
	, bin_perspective_(false)
	, bin_space_partition_start_pos_(0, 0)
	, bin_space_partition_end_pos_(0, 0)
	, bin_rotation_flag_(0)
	, bin_scale_flag_(0)
	, bin_perspec_flag_(0)
	, bin_X_space_partition_flag_(0)
	, bin_Y_space_partition_flag_(0)
	, bin_truncation_amount_(0)
{
}

BORA::BinInfo::BinInfo( const BinInfo &Other )
	: bin_rotation_(Other.bin_rotation_)
	, bin_scale_(Other.bin_scale_)
	, bin_perspective_(Other.bin_perspective_)
	, bin_space_partition_start_pos_(Other.bin_space_partition_start_pos_)
	, bin_space_partition_end_pos_(Other.bin_space_partition_end_pos_)
	, bin_rotation_flag_(Other.bin_rotation_flag_)
	, bin_scale_flag_(Other.bin_scale_flag_)
	, bin_perspec_flag_(Other.bin_perspec_flag_)
	, bin_X_space_partition_flag_(Other.bin_X_space_partition_flag_)
	, bin_Y_space_partition_flag_(Other.bin_Y_space_partition_flag_)
	, bin_truncation_amount_(Other.bin_truncation_amount_)
{

}

BORA::BinInfo::BinInfo(
	double _rotation_value,
	double _scale_value,
	bool _perspective,
	BORA::Position _space_partition_start_pos,
	BORA::Position _space_partition_end_pos,
	unsigned int _rotation_flag,
	unsigned int _scale_flag,
	unsigned int _perspec_flag,
	unsigned int _X_space_partition_flag,
	unsigned int _Y_space_partition_flag,
	unsigned int _truncation_amount )

	: bin_rotation_(_rotation_value)
	, bin_scale_(_scale_value)
	, bin_perspective_(_perspective)
	, bin_space_partition_start_pos_(_space_partition_start_pos)
	, bin_space_partition_end_pos_(_space_partition_end_pos)
	, bin_rotation_flag_(_rotation_flag)
	, bin_scale_flag_(_scale_flag)
	, bin_perspec_flag_(_perspec_flag)
	, bin_X_space_partition_flag_(_X_space_partition_flag)
	, bin_Y_space_partition_flag_(_Y_space_partition_flag)
	, bin_truncation_amount_(_truncation_amount)
{

}

/*
BORA::BinInfo::BinInfo(
double			_rotation_value,
double			_scale_value,
bool			_perspective,
BORA::Position	_space_partition_start_pos,
BORA::Position	_space_partition_end_pos,
unsigned int	_truncation_amount)

: bin_rotation_(_rotation_value)
, bin_scale_(_scale_value)
, bin_perspective_(_perspective)
, bin_space_partition_start_pos_(_space_partition_start_pos)
, bin_space_partition_end_pos_(_space_partition_end_pos)
, bin_truncation_amount_(_truncation_amount)
, bin_rotation_flag_(0)
, bin_scale_flag_(0)
, bin_perspec_flag_(0)
, bin_X_space_partition_flag_(0)
, bin_Y_space_partition_flag_(0)
{
}
*/

