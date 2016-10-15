/*******************************************************************
 Feature Ŭ����

 �߰ߵ� Ư¡���� �� ��ü ����
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "HIP.h"

namespace BORA
{
	class Feature
	{
	private:
		BORA::Position		position_;
		BORA::VOTEINDICES	indices_;
		BORA::HIP			hip_;

		/* for debugging */
		int					parent_bin_;

		/* for multiple target */
		double				orientation_;

	public:
		Feature(void);
		~Feature(void);

		void setPosition(const BORA::Position &_pos);
		void setOrientation(const double &_orientation);
		void setParentBin(const int &_bin_idx);

#if DIRECT_QUAN == 0

		void AddPatchAndIndex(const BORA::Patch &_patch, const BORA::INDEX &_index);

#elif DIRECT_QUAN == 1

		void AddQPatchAndIndex(const BORA::Patch &_qpatch, const BORA::INDEX &_index);

#endif
		void AddFeatureData(const BORA::Feature &_other);
		void Calc();		//***** Descriptor�� ����� ���� ���������� ���� �Ϸ��� �ൿ

		const BORA::Position	&getPosition_ref() const;
		BORA::Position			&getPosition_ref();
		BORA::Position			getPosition();

		const BORA::HIP			&getHIP_ref() const;
		BORA::HIP				&getHIP_ref();
		BORA::HIP				getHIP();

		const BORA::VOTEINDICES &getVoteIndice_ref() const;
		BORA::VOTEINDICES		&getVoteIndice_ref();
		BORA::VOTEINDICES		getVoteIndice();

		const double			&getOrientation_ref() const;
		double					&getOrientation_ref();
		double					getOrientation();

		const int				&getParentBin_ref() const;
		int						&getParentBin_ref();
		const int				getParentBin() const;
		int						getParentBin();

		void operator=(const BORA::Feature &_other);

		void PrintMe();
	protected:
		void TruncateIndex();	//***** 80%�� ��ǥ���� ���� �ε����� �����.
	};
	
	typedef std::pair<BORA::INDEX, unsigned int>	VOTEelem;
	typedef std::vector<BORA::Feature>		FEATURES;
}


bool SortPatchNumber(const BORA::Feature & _A, const BORA::Feature & _B);
bool SortVoteElem(const BORA::VOTEelem &_A, const BORA::VOTEelem & _B);