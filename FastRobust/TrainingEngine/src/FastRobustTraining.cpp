#include "StdAfx.h"
#include "FastRobustTraining.h"
#include "Training.h"
#include <string>

bool FastRobustTraining::training_start(
	const char *_image_file,
	const char *_option_file,
	char _output_file[])
{

	BORA::Training	training_data;

	std::string image_file	= _image_file;
	std::string option_file = _option_file;


	std::string::size_type idx1 = std::string(image_file).rfind("\/");
	if(idx1 == std::string::npos) idx1 = 0;

	std::string::size_type idx2 = std::string(image_file).rfind(".");
	if(idx2 == std::string::npos) idx2 = image_file.size();


	training_data.options_.img_path_			= image_file.substr(0, idx1 + 1);
	training_data.options_.img_file_name_ext_	= image_file.substr(idx1 + 1, image_file.size() - (idx1 + 1));
	training_data.options_.img_file_name_		= image_file.substr(idx1 + 1, idx2 - (idx1 + 1));


	if( !training_data.Init(option_file) )
	{
		std::cout << "error: [FastRobustTraining::training_start] Initialization Failed" << std::endl;
		return false;
	}

	if( !training_data.Calc(_output_file) )
	{
		std::cout << "error: [FastRobustTraining::training_start] Generation Failed" << std::endl;
		return false;
	}

	return true;
}