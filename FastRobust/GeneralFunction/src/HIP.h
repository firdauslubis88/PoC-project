/*******************************************************************
 Histogram Intensity Patches Class

 Patches의 Intensity 히스토그램 클래스.
 수집된 Patches의 Intensity 히스토그램을 관리한다.
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

		// Training 시 Bin내부에서 Patches를 쌓고, 최종적으로 HIP를 가지고 있는 Feature가 
		// Descriptor로 바뀔때에만 유효해지는 데이터
		BORA::HISTS_Q_PATCH		data_;

	public:
		HIP(void);
		~HIP(void);

		// Dec. 2011, Min-Hyuk Sung
		void clear();

#if DIRECT_QUAN == 0
		// 디버기용으로 사용될 때
		void addPatch(const BORA::Patch &_patch);
		void addPATCHES(const BORA::HIP &_hip);
		void Normalize();

#elif DIRECT_QUAN == 1
		// 속도를 위해서 패치 수집시 바로 Quantized된 패치를 수집해 바로 Histogram에 축적한다.
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

