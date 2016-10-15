/*******************************************************************
 IndexMap Ŭ����

 nxm�� �ε����� ����ִ� ���� �����ϴ� Ŭ����
*******************************************************************/
#pragma once
#include "Image.h"
#include "GeneralDataType.h"

namespace BORA
{
	class IndexMap
	{
	private:
		std::vector<std::vector<int> > data_;
		
		unsigned int				width_;
		unsigned int				height_;

	public:
		IndexMap(const BORA::Image &_image);
		IndexMap(const unsigned int &_width, const unsigned int &_height);
		~IndexMap(void);

		void Clear(const int _data = -1);

		const int &operator()(const unsigned int &_x, const unsigned int &_y) const;	// rhs
		int &operator()(const unsigned int &_x, const unsigned int &_y);				// lhs

	private:
		void reSize();
	};
}