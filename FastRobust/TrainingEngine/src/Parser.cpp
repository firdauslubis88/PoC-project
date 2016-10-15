#include "StdAfx.h"
#include "Parser.h"


bool BORA::Parser::ParseTrainingOptions( const std::string &_path, BORA::Options &_opt )
{
    std::ifstream fs(_path.c_str());

    if(fs.fail())
    {
        std::cout << "FnParser::ParseTrainingOption(..) error\n";
        std::cout << "file open error : " << _path << std::endl;
        return false;
    }

    _opt.Init();		//***** 옵션을 초기화 한다.

    while(!fs.eof())
    {
        std::string while_key;
        fs >> while_key;

        // 한 줄 주석 처리
        if(while_key[0] == '#')
        {
            char ch;
            while(fs.get(ch) && ch != '\n');

            continue;
        }
        /*
        // 이미지 경로
        if(while_key == "ImagePath")
        {
            //ImagePath					Path
            char ch;
            std::string value_string;

            while(fs.get(ch) && isspace(ch));

            // 공백이 포함된 경로는 반드시 " " 로 감싼다.
            if( ch == '\"')
            {
                while( fs.get(ch) && ch != '\"')
                    value_string.push_back(ch);
            }
            // 일반적인(공백 없는) 경로의 경우
            else
            {
                std::string rest_value;
                fs >> rest_value;
                value_string.push_back(ch);    
                value_string += rest_value;
            }
            
            for(unsigned int i = 0; i < value_string.size(); ++i)
                if(value_string[i] == '\\')
                    value_string[i] = '/';

            if( *value_string.rbegin() != '/')
                value_string.push_back('/');

			_opt.img_path_ = value_string;
        }
        
        else if(while_key == "ImageName")
        {
            //ImageName					Name include extionsion
			char ch;
            std::string value_string;

			while(fs.get(ch) && isspace(ch));

			// 공백이 포함된 경로는 반드시 " " 로 감싼다.
			if( ch == '\"')
			{
				while( fs.get(ch) && ch != '\"')
					value_string.push_back(ch);
			}
			// 일반적인(공백 없는) 경로의 경우
			else
			{
				std::string rest_value;
				fs >> rest_value;
				value_string.push_back(ch);
				value_string += rest_value;
			}

            _opt.img_file_name_extantion_	= value_string;
            _opt.img_file_name_				= value_string.substr(0, value_string.rfind('.'));
        }
		*/
		else if(while_key == "FileSaveType")
		{
			std::string value_string;
			fs >> value_string;
			if(value_string == "VER1")
				_opt.saveType = BORA::DBTYPE::VER1;
			else if(value_string == "KIST")
				_opt.saveType = BORA::DBTYPE::KIST;
			else if(value_string == "DEBUG1")
				_opt.saveType = BORA::DBTYPE::DEBUG1;
			else
				_opt.saveType = BORA::DBTYPE::VER1;
		}

		else if(while_key == "debug?")
		{
			//debug?						#[0 | 1]
			int value_int;
			fs >> value_int;

			_opt.debugfileoptions_.isDump_ = value_int == 1 ? true : false;

		}
        
        else if(while_key == "dumpEachTraingingImage")
        {
            //dumpEachTraingingImage		#[0 | 1]
            int value_int;
            fs >> value_int;
			
            _opt.debugfileoptions_.dumpEachTrainingImage_ = value_int == 1 ? true : false;
        }
        
        else if(while_key == "dumpFeatureData")
        {
            //dumpFeatureData				#[0 | 1]
            int value_int;
            fs >> value_int;
            _opt.debugfileoptions_.dumpFeatureData_ = value_int == 1 ? true : false;
        }
        
        else if(while_key == "dumpDescriptionAsCSV")
        {
            //dumDescriptoionAsCSV		#[0 | 1]
            int value_int;
            fs >> value_int;
            _opt.debugfileoptions_.dumpDescriptionAsCSV_ = value_int == 1 ? true : false;
        }
        
        else if(while_key == "wishFeatureNumber")
        {
            //wishFeatureNumber			#[(∞, 0] | [1, ∞]]
            int value_int;
            fs >> value_int;
            _opt.cornerdetection_.wishFeatureNumber_ = value_int;
            if(value_int <= 0)
            {
                //threshold					#[1, 254] //위의 wishFeatureNumber가 첫번째 조건 일 때만 사용됨
                fs >> while_key >> value_int;
                _opt.cornerdetection_.threshold_ = value_int;
            }
            else
            {
                fs >> while_key >> value_int;
            }
    
        }else if(while_key == "indexType"){
            //indexType					#[5 | 13]
            unsigned int value_int;
            fs >> value_int;
            _opt.indexing_.enumIndexing_ = value_int == 5 ? BORA::Options::Indexing::INDEX_5 : BORA::Options::Indexing::INDEX_13;
        }else if(while_key == "TruncationType")
        {
            typedef BORA::Options::PatchTruncation PatchTruncation;

            //TruncationType				[NONE | AMOUNT | RATIO]			#[0 | [1, ∞] | [0.0, 1.0]]
            std::string value_string;
            fs >> value_string;
			
            if(value_string == "NONE")
            {
                _opt.patchtruncation_.enumTruncationType_ = PatchTruncation::TRUNCATION_NONE;

                int value_dummy;
                fs >> value_dummy;
                _opt.patchtruncation_.unionTruncationValue_.amount_ = 0;
            }
            
            else if(value_string == "AMOUNT")
            {
                _opt.patchtruncation_.enumTruncationType_ = PatchTruncation::TRUNCATION_BY_AMOUNT;
				
                unsigned int value_int;
                fs >> value_int;
                _opt.patchtruncation_.unionTruncationValue_.amount_ = value_int;
            }
            /*
            else if(value_string == "RATIO")
            {
                _opt.patchtruncation_.enumTruncationType_ = PatchTruncation::TRUNCATION_BY_RATIO;
                
                double value_Scalar;
                fs >> value_Scalar;
                _opt.patchtruncation_.unionTruncationValue_.ratio_ = value_Scalar;
            }
			*/

        }

		// Dec. 2011, Min-Hyuk Sung
		else if(while_key == "TruncationExactCut")
		{
			//truncationExactCut				#[0 | 1]
			int value_int;
			fs >> value_int;
			_opt.patchtruncation_.truncationExactCut = value_int == 1 ? true : false;
		}
        
        else if(while_key == "basicRotation")
        {
            //basicRotation				#(0.0, 350.0] //default(10.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.rotation_.basic_rotation_ = value_Scalar;
        }
        
        else if(while_key == "basicRotationMin")
        {
            //basicRotationMin			#[0.0, 350.0] //default(0.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.rotation_.basic_rotation_min_ = value_Scalar;
        }
        
        else if(while_key == "basicRotationMax")
        {
            //basicRotationMax			#(basicRoationMin, basicRotationMin+360.0) //default(360.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.rotation_.basic_rotation_max_ = value_Scalar;
        }
        
        else if(while_key == "detailRotation")
        {
            //detailRotation				#(0.0, 359.0] //default(1.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.rotation_.detail_rotation_ = value_Scalar;
        }
        
        else if(while_key == "detailRotationMin")
        {
            //detailRotationMin			#[-359.0, 359.0] //default(-5.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.rotation_.detail_rotation_min_ = value_Scalar;
        }
        
        else if(while_key == "detailRotationMax")
        {
            //detailRotationMax			#(detailRotaionMin, detailRotationMin+360.0) //default(5.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.rotation_.detail_rotation_max_ = value_Scalar;
        }
        
        else if(while_key == "scaleMax")
        {
            //scaleMax					#(0.0, 1.0] //default(1.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.scale_.scale_max_ = value_Scalar;
        }
        
        else if(while_key == "basicScale")
        {
            //basicScale					#(0.0, 1.0] //default(0.8)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.scale_.basic_scale_ = value_Scalar;
        }
        
        else if(while_key == "basicScaleNum")
        {
            //basicScaleNum				#(0, ∞] //default(7)
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.scale_.basic_scale_num_ = value_int;
        }

        else if(while_key == "detailScale")
        {
            //detailScale					#(0.0, 1.0] //default(0.9)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.scale_.detail_scale_ = value_Scalar;
        }

        else if(while_key == "detaileScaleNum")
        {
            //detaileScaleNum				#(0, ∞] //default(2)
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.scale_.detail_scale_num_ = value_int;
        }
        
        else if(while_key == "useBlur")
        {
            //useBlur						#[0 | 1] //default(0)
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.blur_.use_blur_ = value_int == 1 ? true : false;
        }

		else if(while_key == "randomBlur")
		{
			//randomBlur				#[0 | 1] //default(1)
			unsigned int value_int;
			fs >> value_int;
			_opt.image_.random_blur_.use_random_blur_ = value_int == 1 ? true : false;
		}

		else if(while_key == "randomBlurMin")
		{
			// randomBlurMin			#odd[0, ∞]
			unsigned int value_int;
			fs >> value_int;
			_opt.image_.random_blur_.min_mask_level_ = value_int;
		}
		
		else if(while_key == "randomBlurMax")
		{
			// randomBlurMax			#odd[randomBlurMin, ∞]
			unsigned int value_int;
			fs >> value_int;
			_opt.image_.random_blur_.max_mask_level_ = value_int;
		}
        
		else if(while_key == "randomNoise")
		{
			// randomNoise				#[0 | 1]
			unsigned int value_int;
			fs >> value_int;
			_opt.image_.random_noise_.use_random_noise_ = value_int == 1 ? true : false;
		}
		
		else if(while_key == "randomNoiseMin")
		{
			// randomNoiseMin			#[10, randomNoiseMax]
			unsigned int value_int;
			fs >> value_int;
			_opt.image_.random_noise_.min_intensity_diff_ = value_int;
		}

		else if(while_key == "randomNoiseMax")
		{
			// randomNoiseMax			#[randomNoiseMin, 254]
			unsigned int value_int;
			fs >> value_int;
			_opt.image_.random_noise_.max_intensity_diff_ = value_int;
		}



        else if(while_key == "useApplyBlurFirst")
        {
            //useApplyBlurFirst			#[0 | 1] //default(1)
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.blur_.apply_blur_first_ = value_int == 1 ? true : false;
        }
        
        else if(while_key == "blurLevel")
        {
            //blurLevel					#[odd Number] # ... #[-1] //-1을 받으면 끝낸다.
            
            _opt.image_.blur_.level_.push_back(0);//처음에 한번도 안한 깨끗한 이미지를 위한 값을 넣는다.

            for(;;)
            {
                int value_int;
                fs >> value_int;
                if(value_int <= -1)
                    break;
                _opt.image_.blur_.level_.push_back(value_int);
            }
            
        }
        
        else if(while_key == "useNoise")
        {
            //useNoise					#[0 | 1] //default(0)
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.noise_.use_noise_ = value_int == 1 ? true : false;
        }
        
        else if(while_key == "noiseLevel")
        {
            //noiseLevel					#(0, 245] # ... #[-1] //-1을 받으면 끝낸다.

            _opt.image_.noise_.level_.push_back(0);//처음에 한번도 안한 깨끗한 이미지를 위한 값을 넣는다.

            for(;;)
            {
                int value_int;
                fs >> value_int;
                if(value_int == -1)
                    break;
                _opt.image_.noise_.level_.push_back(value_int);
            }
        }

		else if(while_key == "usePerspective")
		{
			unsigned int value_int;
			fs >> value_int;
			_opt.image_.perspective_.use_perspective_ = value_int == 1 ? true : false;
		}
        
        else if(while_key == "useAffine")
        {
            //useAffine					#[0 | 1] //default(0)
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.affine_.use_affine_ = value_int == 1 ? true : false;
        }
        
        else if(while_key == "thetaMin")
        {
            //thetaMin					#[-359.0, 359.0] //default(-5.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.affine_.factor_.theta_min_ = value_Scalar;
        }
        
        else if(while_key == "thetaMax")
        {
            //thetaMax					#(thetaMin, thetaMax+359.0] //default(5.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.affine_.factor_.theta_max_ = value_Scalar;
        }
        
        else if(while_key == "phiMin")
        {
            //phiMin						#[-359.0, 359.0] //default(-180.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.affine_.factor_.phi_min_ = value_Scalar;
        }
        
        else if(while_key == "phiMax")
        {
            //phiMax						#(phiMin, phiMin+359.0] //default(180.0)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.affine_.factor_.phi_max_ = value_Scalar;
        }
        
        else if(while_key == "scaleAxisXmin")
        {
            //scaleAxisXmin				#(0.0, ∞] //default(0.6)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.affine_.factor_.scale_axis_x_min_ = value_Scalar;
        }
        
        else if(while_key == "scaleAxisXmax")
        {
            //scaleAxisXmax				#(scaleAxisXmin, scaleAxisXmin+∞] //default(1.2)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.affine_.factor_.scale_axis_x_max_ = value_Scalar;
        }
        
        else if(while_key == "scaleAxisYmin")
        {
            //scaleAxisYmin				#(0.0, ∞] //default(0.6)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.affine_.factor_.scale_axis_y_min_ = value_Scalar;
        }
        
        else if(while_key == "scaleAxisYmax")
        {
            //scaleAxisYmax				#(scaleAxisYmin, scaleAxisYmin+∞] //default(1.2)
            double value_Scalar;
            fs >> value_Scalar;
            _opt.image_.affine_.factor_.scale_axis_y_max_ = value_Scalar;
        }
        
        else if(while_key == "thetaRandom")
        {
            //thetaRandom					#[0 | 1]	#(0.0, ∞]	//0으로 세팅했으면 Scalar 값 default(1.0)
            //								#[1, ∞]		//1로 세팅했으면 int 값 default(10)
            unsigned int value;
            fs >> value;
            _opt.image_.affine_.randomrotation_.theta_random_ = value == 1 ? true : false;

            if(value == 0)
            {
                double value_Scalar;
                fs >> value_Scalar;
                _opt.image_.affine_.randomrotation_.theta_increase_level_ = value_Scalar;
            }
        }
        
        else if(while_key == "phiRandom")
        {
            //phiRandom					#[0 | 1]	#(0.0, ∞]	//0으로 세팅했으면 Scalar 값 default(180.0)
            //							#[1, ∞]		//1로 세팅했으면 int 값 default(1)
            unsigned int value;
            fs >> value;
            _opt.image_.affine_.randomrotation_.phi_random_ = value == 1 ? true : false;

            if(value == 0)
            {
                double value_Scalar;
                fs >> value_Scalar;
                _opt.image_.affine_.randomrotation_.phi_increase_level_ = value_Scalar;
            }
        }
        
        else if(while_key == "axisXrandom")
        {
            //axisXrandom					#[0 | 1]	#(0.0, ∞]	//0으로 세팅했으면 Scalar 값 default(0.5)
            //								#[1, ∞]		//1로 세팅했으면 int 값 default(3)
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.affine_.randomscale_.axis_x_random_ = value_int == 1 ? true : false;
            if(value_int == 0)
            {
                double value_Scalar;
                fs >> value_Scalar;
                _opt.image_.affine_.randomscale_.axis_x_increase_level_ = value_Scalar;
            }
        }
        
        else if(while_key == "axisYrandom")
        {
            //axisYrandom					#[0 | 1]	#(0.0, ∞]	//default(0.5)
            //								#[1, ∞]		//default(3)
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.affine_.randomscale_.axis_y_random_ = value_int == 1 ? true : false;
            if(value_int == 0)
            {
                double value_Scalar;
                fs >> value_Scalar;
                _opt.image_.affine_.randomscale_.axis_y_increase_level_ = value_Scalar;
            }
        }

        else if(while_key == "randomValueExtractTime")
        {
            unsigned int value_int;
            fs >> value_int;
            _opt.image_.affine_.factor_.random_value_extract_time_ = value_int > 0 ? value_int : 0;
        }

        else if(while_key == "TransformFilter")
        {
            std::string interpolation;
            std::string warping_flag;

            fs >> interpolation >> warping_flag;
            if(interpolation == "CV_INTER_NN")
            {
                _opt.image_.transformfilter_.interpolation_ = CV_INTER_NN;
            }
            else if(interpolation == "CV_INTER_LINEAR")
            {
                _opt.image_.transformfilter_.interpolation_ = CV_INTER_LINEAR;
            }
            else if(interpolation == "CV_INTER_CUBIC")
            {
                _opt.image_.transformfilter_.interpolation_ = CV_INTER_CUBIC;
            }
            else if(interpolation == "CV_INTER_AREA")
            {
                _opt.image_.transformfilter_.interpolation_ = CV_INTER_AREA;
            }
#ifdef OPENCV_VERSION_220
			// OpenCV 2.2 or later
            else if(interpolation == "CV_INTER_LANCZOS4")
            {
                _opt.image_.transformfilter_.interpolation_ = CV_INTER_LANCZOS4;
            }
#endif

            if(warping_flag == "CV_WARP_FILL_OUTLIERS")
            {
                _opt.image_.transformfilter_.wraping_falg_ = CV_WARP_FILL_OUTLIERS;
            }
            else if(warping_flag == "CV_WARP_INVERSE_MAP")
            {
                _opt.image_.transformfilter_.wraping_falg_ = CV_WARP_INVERSE_MAP;
            }
            else
            {
                //default 값으로 측정한다.
                _opt.image_.transformfilter_.wraping_falg_ = CV_WARP_FILL_OUTLIERS;
            }
        }

		// Dec. 2011, Min-Hyuk Sung
		// Large Scale Image - Space Partitioning
		else if(while_key == "spatialPartioning")
		{
			// spacepartitioning	//default(0)
			unsigned int value_int;
			fs >> value_int;
			if(value_int > 1)
			{
				_opt.image_.spacepartitioning_.use_space_partitioning_ = true;
				_opt.image_.spacepartitioning_.space_partion_basic_num_ = value_int;
			}
			else
			{
				_opt.image_.spacepartitioning_.use_space_partitioning_ = false;
				_opt.image_.spacepartitioning_.space_partion_basic_num_ = 1;
			}
		}
    }

    /********** 옵션의 최소, 최대치 유효성을 검사 한다 **********/
    if(_opt.CheckOptionsRule() == false)
    {
        std::cout << "FnParser::ParseTrainingOption(..) error\n";
        return false;
    }
    return true;
}

void BORA::Parser::WriteTrainingOptions( const std::string &_path, const BORA::Options &_opt, const unsigned int &_descriptors_size )
{
	std::stringstream info;
	std::ofstream opt_file(_path.c_str());


	info << _opt.img_file_name_ext_ << "image file training options\n\n"
		 << "Image Size : " << _opt.image_.original_img_size_.x << " x " << _opt.image_.original_img_size_.y << "\n\n"
		 << "Bin Create Information\n"
		 << "rotation angle : " << _opt.image_.rotation_.basic_rotation_min_ << " ~ " << _opt.image_.rotation_.basic_rotation_max_ << " +(" << _opt.image_.rotation_.basic_rotation_ << ")\n"
		 << "scale factor : " << _opt.image_.scale_.basic_scale_ << " x " << _opt.image_.scale_.basic_scale_num_ << "times\n\n"
		 << "Training Infromation\n"
		 << "rotation angle : " << _opt.image_.rotation_.detail_rotation_min_ << " ~ " << _opt.image_.rotation_.detail_rotation_max_ << " +(" << _opt.image_.rotation_.detail_rotation_ << ")\n"
		 << "scale factor : " << _opt.image_.scale_.detail_scale_ << " x " << _opt.image_.scale_.detail_scale_num_ << "times\n";

	if( (_opt.image_.blur_.use_blur_ && (_opt.image_.noise_.use_noise_ || _opt.image_.random_noise_.use_random_noise_)) ||
		(_opt.image_.random_blur_.use_random_blur_ && (_opt.image_.noise_.use_noise_ || _opt.image_.random_noise_.use_random_noise_)) )
	{
		if(_opt.image_.blur_.apply_blur_first_)
			info << "apply blur first\n";
		else
			info << "aplly noise first\n";
	}

	if(_opt.image_.random_blur_.use_random_blur_)
	{
		info << "random blur : " << _opt.image_.random_blur_.min_mask_level_ << " ~ " << _opt.image_.random_blur_.max_mask_level_ << std::endl;
	}
	else if(_opt.image_.blur_.use_blur_)
	{
		info << "blur level : ";
		for(unsigned int i = 0 ; i < _opt.image_.blur_.level_.size() ; ++i)
			info << _opt.image_.blur_.level_[i] << " > ";
		info << _opt.image_.blur_.level_.size() << "개\n";
	}
	else
		info << "blur not apply\n";

	if(_opt.image_.random_noise_.use_random_noise_)
	{
		info << "random noise : "
			 << _opt.image_.random_noise_.min_intensity_diff_
			 << " ~ "
			 << _opt.image_.random_noise_.max_intensity_diff_
			 << std::endl;
	}
	else if(_opt.image_.noise_.use_noise_)
	{
		info << "noise level : ";
		for(unsigned int i = 0 ; i < _opt.image_.noise_.level_.size() ; ++i)
			info << _opt.image_.noise_.level_[i] << " > ";
		info << _opt.image_.noise_.level_.size() << "개\n";
	}
	else
		info << "noise not apply\n";

	if(_opt.image_.perspective_.use_perspective_)
		info << "perspective use\n";
	else
		info << "perspective not use\n";


	if(_opt.image_.affine_.use_affine_)
	{
		unsigned int linear_extract_count(0);
		info << "Affine factors\n";
		
		if(_opt.image_.affine_.randomrotation_.theta_random_ == false)
		{
			++linear_extract_count;
			info << "theta value extract by linear : "
				 << _opt.image_.affine_.factor_.theta_min_ << " ~ " << _opt.image_.affine_.factor_.theta_max_ << " +(" << _opt.image_.affine_.randomrotation_.theta_increase_level_ << ")\n";
		}

		if(_opt.image_.affine_.randomrotation_.phi_random_ == false)
		{
			++linear_extract_count;
			info << "phi value extract by linear : "
				 << _opt.image_.affine_.factor_.phi_min_ << " ~ " << _opt.image_.affine_.factor_.phi_max_ << " +(" << _opt.image_.affine_.randomrotation_.phi_increase_level_ << ")\n";
		}

		if(_opt.image_.affine_.randomscale_.axis_x_random_ == false)
		{
			++linear_extract_count;
			info << "scale x factor extract by linear : "
				 << _opt.image_.affine_.factor_.scale_axis_x_min_ << " ~ " << _opt.image_.affine_.factor_.scale_axis_x_max_ << " +(" << _opt.image_.affine_.randomscale_.axis_x_increase_level_ << ")\n";
		}

		if(_opt.image_.affine_.randomscale_.axis_y_random_ == false)
		{
			++linear_extract_count;
			info << "scale y factor extract by linear : "
				<< _opt.image_.affine_.factor_.scale_axis_y_min_ << " ~ " << _opt.image_.affine_.factor_.scale_axis_y_max_ << " +(" << _opt.image_.affine_.randomscale_.axis_y_increase_level_ << ")\n";
		}

		
		if(linear_extract_count < 4)
		{
			if(_opt.image_.affine_.randomrotation_.theta_random_)
				info << "theta, ";
			if(_opt.image_.affine_.randomrotation_.phi_random_)
				info << "phi, ";
			if(_opt.image_.affine_.randomscale_.axis_x_random_)
				info << "scale x, ";
			if(_opt.image_.affine_.randomscale_.axis_y_random_)
				info << "scale y, ";

			info << " factor(s) extract random count : " << _opt.image_.affine_.factor_.random_value_extract_time_ << "\n\n";
				
		}
	}
	else
		info << "affine not apply\n";

	info << "\n\n"
		 << "Descriptors size : "
		 << _descriptors_size
		 << "\n";

	opt_file << info.str();
	opt_file.close();
}
