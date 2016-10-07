/*******************************************************************
 Descriptor Ŭ����

 Training�� Matching���� ����ϴ� Feature�� ���� Ŭ����
 ���� Feature�� ���ؼ� Descriptor�� ����� ������ �ٸ��� ������ virtualȭ.
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
		/* trained Feature�� ���� Descriptor�� ���鶧 */
		Descriptor(const BORA::Feature &_trained_feature, const BORA::Position &_ref_position);
		Descriptor(const BORA::Feature &_trained_feature, const CvMat *_invH, const BORA::Position &_shifted_pos);
		Descriptor(void);
		~Descriptor(void);

		/* �ٸ� ��ũ���Ϳ� ���� 1�� ������ ��ȯ */
		virtual unsigned __int64 compare(const Descriptor &_other);

		void Clear();

		/* �⺻ set&get �Լ� */
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

	// ��ũ���͸� 0�� 1���� �̷���� ���ڿ��� �����´�.
	std::string getBinaryString( unsigned __int64 num );
}

bool operator==(const BORA::DESCRIPTORS &_A, const BORA::DESCRIPTORS &_B);