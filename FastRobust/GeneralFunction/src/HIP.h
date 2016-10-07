/*******************************************************************
 Histogram Intensity Patches Class

 Patches�� Intensity ������׷� Ŭ����.
 ������ Patches�� Intensity ������׷��� �����Ѵ�.
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "ImageUtility.h"
#include "Patch.h"

namespace BORA
{
	typedef std::tr1::array<double, 64u> HIST_Q_PATCH;				// Histogram of quantiazed patch
	typedef std::tr1::array<BORA::HIST_Q_PATCH, 5u> HISTS_Q_PATCH;	// Histograms of quantized patch
	
	class HIP
	{
	private:
		BORA::PATCHES			patches_;

		// Training �� Bin���ο��� Patches�� �װ�, ���������� HIP�� ������ �ִ� Feature�� 
		// Descriptor�� �ٲ𶧿��� ��ȿ������ ������
		BORA::HISTS_Q_PATCH		data_;

	public:
		HIP(void);
		~HIP(void);

		// Dec. 2011, Min-Hyuk Sung
		void clear();

#if DIRECT_QUAN == 0
		// ���������� ���� ��
		void addPatch(const BORA::Patch &_patch);
		void addPATCHES(const BORA::HIP &_hip);
		void Normalize();

#elif DIRECT_QUAN == 1
		// �ӵ��� ���ؼ� ��ġ ������ �ٷ� Quantized�� ��ġ�� ������ �ٷ� Histogram�� �����Ѵ�.
		void addQPatch(const BORA::Patch &_qpatch);
		void addQPATCHES(const BORA::HIP &_hip);
		void Normalize_without_quantize();
#endif	

		
		const BORA::HIST_Q_PATCH	&getData(const unsigned int _idx) const;
		BORA::HIST_Q_PATCH			getData(const unsigned int _idx);

		BORA::HISTS_Q_PATCH			getDatas();
		const BORA::HISTS_Q_PATCH	getDatas() const;

		BORA::HISTS_Q_PATCH			&getDatas_ref();
		const BORA::HISTS_Q_PATCH	&getDatas_ref() const;

		const BORA::PATCHES			&getPATCHES_ref() const;
		BORA::PATCHES				&getPATCHES_ref();
		BORA::PATCHES				getPATCHES();

		unsigned int						getPatchesSize();
		const unsigned int				getPatchesSize() const;
	};
}

