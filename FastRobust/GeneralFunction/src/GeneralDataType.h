/*******************************************************************
 일반적 데이터 타입, 함수 및 헤더파일들
*******************************************************************/
#pragma once
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include <string>
#include <vector>
// Dec. 2011, Min-Hyuk Sung
//#include <array>
// VS2005
#include <boost/tr1/tr1/bcc32/array.h>
//
#include <list>
#include <map>
#include <ostream>
#include <iostream>
#include <fstream>
#include <cmath>

#include <cassert>

#ifndef __int64
#define __int64 long long int
#endif

// Patch를 수집 할 때 그냥 수집할지(0)
// 바로 퀀타이즈된 패치를 축적 할 지(1) 결정
#define DIRECT_QUAN 0

namespace BORA
{
	typedef unsigned char											BYTE;

	/* 기본 상수 */
	const unsigned int	INDEX_5_MAX			= 1u << 5;
	const unsigned int	INDEX_13_MAX		= 1u << 13;
	const unsigned int	QUANTIZATION_LEVEL	= 5;
	const float		PI					= 3.14159265358979323846f;
	
	/* 퀀타이즈 레벨에 대한 Intensity 상수 */ 
	const BORA::BYTE LV0_INT	= 0;
	const BORA::BYTE LV1_INT	= 51;
	const BORA::BYTE LV2_INT	= 102;
	const BORA::BYTE LV3_INT	= 153;
	const BORA::BYTE LV4_INT	= 204;

	typedef unsigned __int64										UINT64;
	typedef unsigned int											INDEX;
	typedef std::tr1::array<BORA::UINT64, 5>						DESC;
	// Dec. 2011, Min-Hyuk Sung
	// INDICES: vector -> list
	typedef std::list<BORA::INDEX>									INDICES;		//***** descriptor는 단순히 인덱스들의 집합만을 필요로 하므로
	typedef std::tr1::array<BORA::INDICES, BORA::INDEX_13_MAX >		INDEXTABLE;		//***** 추후에 5 <-> 13 교체 고려
	typedef std::map<BORA::INDEX, unsigned int>						VOTEINDICES;	//***** Feature가 vote받은 인덱스를 사용하기 위해서

	struct Position		//***** openCV에서의 함수에 국한되지 않기 위해 별도로 구성
	{
		int x, y;

		Position(void);
		Position(const int _x, const int _y);

		void		operator()(const int _x, const int _y);
		void		operator=(const BORA::Position &_other);
	};
	
	typedef std::vector<BORA::Position>			POSITIONS;
	
	double Rand(const double &_min, const double &_max);
	static unsigned __int64 shift64[]={
		0x0000000000000001u,
		0x0000000000000002u,
		0x0000000000000004u,
		0x0000000000000008u,
		0x0000000000000010u,
		0x0000000000000020u,
		0x0000000000000040u,
		0x0000000000000080u,
		0x0000000000000100u,
		0x0000000000000200u,
		0x0000000000000400u,
		0x0000000000000800u,
		0x0000000000001000u,
		0x0000000000002000u,
		0x0000000000004000u,
		0x0000000000008000u,
		0x0000000000010000u,
		0x0000000000020000u,
		0x0000000000040000u,
		0x0000000000080000u,
		0x0000000000100000u,
		0x0000000000200000u,
		0x0000000000400000u,
		0x0000000000800000u,
		0x0000000001000000u,
		0x0000000002000000u,
		0x0000000004000000u,
		0x0000000008000000u,
		0x0000000010000000u,
		0x0000000020000000u,
		0x0000000040000000u,
		0x0000000080000000u,
		0x0000000100000000u,
		0x0000000200000000u,
		0x0000000400000000u,
		0x0000000800000000u,
		0x0000001000000000u,
		0x0000002000000000u,
		0x0000004000000000u,
		0x0000008000000000u,
		0x0000010000000000u,
		0x0000020000000000u,
		0x0000040000000000u,
		0x0000080000000000u,
		0x0000100000000000u,
		0x0000200000000000u,
		0x0000400000000000u,
		0x0000800000000000u,
		0x0001000000000000u,
		0x0002000000000000u,
		0x0004000000000000u,
		0x0008000000000000u,
		0x0010000000000000u,
		0x0020000000000000u,
		0x0040000000000000u,
		0x0080000000000000u,
		0x0100000000000000u,
		0x0200000000000000u,
		0x0400000000000000u,
		0x0800000000000000u,
		0x1000000000000000u,
		0x2000000000000000u,
		0x4000000000000000u,
		0x8000000000000000u
	};

	/* OpenCV 변수 이름 재지정 */

	bool operator<(const BORA::Position &_A, const BORA::Position &_B);

	typedef CvPoint2D32f	CENTER;		//***** Image의 center에 대한 변수 매칭
}

BORA::Position operator+(const BORA::Position &_A, const BORA::Position &_B);
BORA::Position operator-(const BORA::Position &_A, const BORA::Position &_B);
BORA::Position operator*(const CvMat *mat, const BORA::Position &pos);
bool operator==(const BORA::Position &_A, const BORA::Position &_B);
bool operator!=(const BORA::Position &_A, const BORA::Position &_B);


/* 펑션 오버로딩 */
// CvPoint <=> BORA::Position
// BYTE <=> unsigned int
