/*******************************************************************
 Patch class

 이미지의 특정좌표에 있는 8x8 이미지 정보
*******************************************************************/
#pragma once
#include "GeneralDataType.h"

namespace BORA
{
	typedef std::tr1::array<unsigned int, 64>		PATCH;
	class Patch
	{
	private:
		BORA::Position	pos_;
		BORA::PATCH		patch_;

		// 디버깅 용으로 만든 변수
		unsigned int bin_idx_;
		unsigned int trn_idx_;
		unsigned int my_idx_;

	public:
		Patch(void);
		~Patch(void);

		void Clear();
		
		void setIndexInfo(const unsigned int &_bin_idx, const unsigned int &_trn_idx, const unsigned int &_my_idx);
		void setPosition(const BORA::Position &_pos);
		void setPATCH(const BORA::PATCH &_patch);

		void getIndexInfo(unsigned int &_bin_idx, unsigned int &_trn_idx, unsigned int &_patch_idx);
		const void getIndexInfo(unsigned int &_bin_idx, unsigned int &_trn_idx, unsigned int &_patch_idx) const;

		BORA::Position getPosition();
		BORA::Position &getPosition_ref();
		const BORA::Position &getPosition_ref() const;

		BORA::PATCH getPATCH();
		BORA::PATCH &getPATCH_ref();
		const BORA::PATCH &getPATCH_ref() const;
	};

	typedef std::vector<BORA::Patch>			PATCHES;
}