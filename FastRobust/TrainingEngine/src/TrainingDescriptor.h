/*******************************************************************
 TrainingDescriptor 클래스

 기존의 Descriptor를 상속 받아서 디버깅용으로 사용하기 위한 맴버 변수들을 가지고 있다.
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "Descriptor.h"
#include "Feature.h"
#include <list>

namespace BORA
{
	class TDescriptor : public BORA::Descriptor
	{
	private:
		BORA::Feature		feature_;

	public:
		TDescriptor(const BORA::Feature &_trainded_feature, const BORA::Position &_ref_pos);
		TDescriptor(const BORA::Feature &_trainded_feature, const CvMat *_invH, const BORA::Position &_shifted_pos);
		TDescriptor(void);
		~TDescriptor(void);

		void setFeature(const BORA::Feature &_feature);

		BORA::Feature getFeature();
		const BORA::Feature &getFeature()const;

		void ConvertToDescirptor(BORA::Descriptor &_desc);
	};

	//typedef std::vector<TDescriptor> TDESCRIPTORS;
	typedef std::list<TDescriptor> TDESCRIPTORS;
}


