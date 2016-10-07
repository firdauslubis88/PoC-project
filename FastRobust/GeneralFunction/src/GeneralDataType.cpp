#include "StdAfx.h"
#include "GeneralDataType.h"

BORA::Position::Position( const int _x, const int _y )
	: x(_x)
	, y(_y)
{

}

BORA::Position::Position( void )
	: x(0)
	, y(0)
{

}

void BORA::Position::operator()( const int _x, const int _y )
{
	x = _x;
	y = _y;
}

void BORA::Position::operator=( const Position &_other )
{
	x = _other.x;
	y = _other.y;
}


double BORA::Rand( const double &_min, const double &_max )
{
	if(_min == _max)
		return _min;

	double result;
	while(true)
	{
		int randmin(0), randmax(1000);
		int value = rand() % randmax + randmin;

		result = (static_cast<double>(value)/static_cast<double>(randmax - randmin))*(_max - _min) + _min;

		if(_min <= result && result <= _max)
			return result;
	}
}

BORA::Position operator+( const BORA::Position &_A, const BORA::Position &_B )
{
	return BORA::Position(_A.x + _B.x, _A.y + _B.y);
}

BORA::Position operator-( const BORA::Position &_A, const BORA::Position &_B )
{
	return BORA::Position(_A.x - _B.x, _A.y - _B.y);
}


/* 2x3행의 매트릭스와 Position의 곱 오버로딩 */
BORA::Position operator*( const CvMat *_mat, const BORA::Position &pos)
{
	BORA::Position result(0, 0);
	double posx = static_cast<double>(pos.x);
	double posy = static_cast<double>(pos.y);
	if(_mat->rows == 2)
	{
		double mat[6] = { 
			cvmGet(_mat, 0, 0), 
			cvmGet(_mat, 0, 1), 
			cvmGet(_mat, 0, 2),
			cvmGet(_mat, 1, 0),
			cvmGet(_mat, 1, 1),
			cvmGet(_mat, 1, 2),
		};

		double x = mat[0]*posx + mat[1]*posy + mat[2];
		double y = mat[3]*posx + mat[4]*posy + mat[5];

		result.x = static_cast<int>(x);
		result.y = static_cast<int>(y);
	}
	else if(_mat->rows == 3)
	{
		double mat[9] = {
			cvmGet(_mat, 0, 0),
			cvmGet(_mat, 0, 1),
			cvmGet(_mat, 0, 2),
			cvmGet(_mat, 1, 0),
			cvmGet(_mat, 1, 1),
			cvmGet(_mat, 1, 2),
			cvmGet(_mat, 2, 0),
			cvmGet(_mat, 2, 1),
			cvmGet(_mat, 2, 2)
		};
		
		double x = mat[0]*posx + mat[1]*posy + mat[2];
		double y = mat[3]*posx + mat[4]*posy + mat[5];
		double z = mat[6]*posx + mat[7]*posy + mat[8];
		
		result.x = static_cast<int>(x/z);
		result.y = static_cast<int>(y/z);
	}
	return result;
}

bool operator==( const BORA::Position &_A, const BORA::Position &_B )
{
	return _A.x == _B.x && _A.y == _B.y;
}

bool operator!=( const BORA::Position &_A, const BORA::Position &_B )
{
	return _A.x != _B.x || _A.y != _B.y;
}

bool BORA::operator<(const BORA::Position &_A, const BORA::Position &_B){
	if(_A.x==_B.x)
		return _A.y == _B.y;
	else
		return _A.x == _B.x;
}
