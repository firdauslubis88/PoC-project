/*******************************************************************
 IndexLookupTable Ŭ����

 Training�� Matching���� �������� ���Ǵ� �ε��� ��� ���̺�.
 ��ũ���͵��� �̿��ؼ� �ε��� ���̺��� �����.
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

		/* ��ũ���͵��� ���ڷ� �޾Ƽ� Table�� data�� �����Ѵ�. */
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