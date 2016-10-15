/*******************************************************************
 Parser Ŭ����

 Ư���� Options�� ��ϵǾ��ִ� ������ �о���� Ŭ����
*******************************************************************/
#pragma once
#include "GeneralDataType.h"
#include "Options.h"

#include <fstream>

namespace BORA
{
	class Parser
	{
	public:
		static bool ParseTrainingOptions(const std::string &_path, BORA::Options &_opt);
		static void WriteTrainingOptions(const std::string &_path, const BORA::Options &_opt, const unsigned int &_descriptors_size);
	};
}
