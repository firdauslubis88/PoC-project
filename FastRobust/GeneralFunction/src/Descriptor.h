/*******************************************************************
 Descriptor 클래스

 Training과 Matching에서 사용하는 Feature를 만든 클래스
 각기 Feature에 의해서 Descriptor를 만드는 과정은 다르기 때문에 virtual화.
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "Feature.h"

namespace BORA
{
	class Descriptor
	{
	protected:
		BORA::Position		position_;
		BORA::INDICES		indices_;
		BORA::DESC			data_;

		/* for debugging */
		int					parent_bin_;

	public:
		/* trained Feature로 부터 Descriptor를 만들때 */
		Descriptor(const BORA::Feature &_trained_feature, const BORA::Position &_ref_position);
		Descriptor(const BORA::Feature &_trained_feature, const CvMat *_invH, const BORA::Position &_shifted_pos);
		Descriptor(void);
		~Descriptor(void);

		/* 다른 디스크립터와 비교후 1의 개수를 반환 */
		virtual unsigned __int64 compare(const Descriptor &_other);

		void Clear();

		/* 기본 set&get 함수 */
		void setDesc(const BORA::DESC &_desc);
		void setPosition(const BORA::Position &_pos);
		void setIndices(const BORA::INDICES &_indices);
		void setParentBin(const int &_bin_idx);
		void addIndex(const BORA::INDEX &_index);

		BORA::Position getPosition();
		const BORA::Position &getPosition() const;
		BORA::INDICES getIndices();
		const BORA::INDICES &getIndices() const;

		BORA::DESC			getDesc();
		const BORA::DESC	&getDesc() const;

		const int &getParentBin_ref() const;
		int &getParentBin_ref();
		const int getParentBin() const;
		int getParentBin();

		friend bool operator==(const BORA::Descriptor &_A, const BORA::Descriptor &_B);
	};

	typedef std::vector<Descriptor>		DESCRIPTORS;

	// 디스크립터를 0과 1으로 이루어진 문자열로 가져온다.
	std::string getBinaryString( unsigned __int64 num );
}

bool operator==(const BORA::DESCRIPTORS &_A, const BORA::DESCRIPTORS &_B);