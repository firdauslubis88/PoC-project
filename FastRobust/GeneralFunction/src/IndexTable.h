/*******************************************************************
 IndexLookupTable 클래스

 Training과 Matching에서 공통으로 사용되는 인덱스 룩업 테이블.
 디스크립터들을 이용해서 인덱스 테이블을 만든다.
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "Descriptor.h"

namespace BORA
{
	class IndexTable
	{
	private:
		BORA::INDEXTABLE	data_;

	public:
		IndexTable(void);
		~IndexTable(void);

		/* 디스크립터들을 인자로 받아서 Table의 data를 갱신한다. */
		void Calc(const BORA::DESCRIPTORS &_descriptors);

		void setTable(const BORA::INDEXTABLE &_data);

		void Clear();
		bool empty(){ return data_.empty(); }

		// Dec. 2011, Min-Hyuk Sung
		BORA::INDEXTABLE		&getData();
		BORA::INDEXTABLE		getDataClone();
		const BORA::INDEXTABLE	&getDataConst() const;

		friend bool operator==(const BORA::IndexTable &_A, const BORA::IndexTable &_B);
	};
}