/* ----------------------------------------
* Fast Robust Matching Algorithm
* version 1.0.2
* 
* Imaging Media Research Center @ KIST
* Min-Hyuk Sung (smh0816@imrc.kist.re.kr)
* Updated: Feb 22, 2012
* ---------------------------------------- */


#ifndef __FAST_ROBUST_TRAINING_H__
#define __FAST_ROBUST_TRAINING_H__


class FastRobustTraining
{
public:

	/**
	* This function is called once at the beginning of training
	* @param _image_file		Training target image file path
	* @param _option_file		Option file path
	* @param _output_file		Output data file path
	*/
	static bool training_start(
		const char *_image_file,
		const char *_option_file,
		char _output_file[]);
};



#endif