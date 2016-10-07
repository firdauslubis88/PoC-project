/*******************************************************************
 BinInfo Ŭ����

 �������� ���õ� ������ ������ �ִ� Ŭ����
 �� ���� �ڽ��� � ����Ʈ�� ��ǥ�ϴ����� ���� ��ȯ ������ ������ �ִ�
 rotation, scale�� ���� ȸ���� ȸ���� ���� �����̸�
 perpective�� ���� �ܼ��� �߳� ���߳ĸ� ������ �ִ� �����̴�.

 ����ü�� ����Ѵ�.
*******************************************************************/
#pragma once
#include <vector>
#include "GeneralDataType.h"

namespace BORA
{
	// Dec. 2011, Min-Hyuk Sung
	// BINFO -> BinInfo, struct -> class
	class BinInfo
	{
	public:
		double		bin_rotation_;
		double		bin_scale_;
		bool		bin_perspective_;

		// Dec. 2011, Min-Hyuk Sung
		// Large Scale Image - Space Partitioning
		// ---- //
		BORA::Position	bin_space_partition_start_pos_;
		BORA::Position	bin_space_partition_end_pos_;
		// ---- //

		// Jan. 2012, Min-Hyuk Sung
		unsigned int bin_rotation_flag_;
		unsigned int bin_scale_flag_;
		unsigned int bin_perspec_flag_;
		unsigned int bin_X_space_partition_flag_;
		unsigned int bin_Y_space_partition_flag_;


		unsigned int	bin_truncation_amount_;
		

	public:
		BinInfo();

		BinInfo(const BinInfo &Other);

		/*
		BinInfo(
			double			_rotation_value,
			double			_scale_value,
			bool			_perspective, 
			BORA::Position	_space_partition_start_pos,
			BORA::Position	_space_partition_end_pos,
			unsigned int	_truncation_amount);
		*/

		BinInfo(
			double			_rotation_value,
			double			_scale_value,
			bool			_perspective, 
			BORA::Position	_space_partition_start_pos,
			BORA::Position	_space_partition_end_pos,
			unsigned int	_rotation_flag,
			unsigned int	_scale_flag,
			unsigned int	_perspec_flag,
			unsigned int	_X_space_partition_flag,
			unsigned int	_Y_space_partition_flag,
			unsigned int	_truncation_amount);
	};

	typedef std::vector<BinInfo>		BININFOS;

}
