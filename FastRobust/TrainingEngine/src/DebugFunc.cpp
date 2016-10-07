#include "StdAfx.h"
#include "DebugFunc.h"


void BORA::DebugFunc::Show( const BORA::Image &_src, int _delay /*= 0*/ )
{
	ShowIplImage(_src.getIplImage(), _delay);
}

void BORA::DebugFunc::Show( const IplImage *_src, int _delay /*= 0*/ )
{
	ShowIplImage(_src, _delay);
}

void BORA::DebugFunc::ShowIplImage( const IplImage *_src, const int _delay )
{
	const std::string windowName = "Debug Image Show Window";
	cvNamedWindow(windowName.c_str());
	cvShowImage(windowName.c_str(), _src);
	cvWaitKey(_delay);
}

void BORA::DebugFunc::DescriptosPoints( const BORA::DESCRIPTORS &_descs, BORA::POSITIONS &_positions )
{
	_positions.clear();
	_positions.reserve(_descs.size());

	for(unsigned int i = 0 ; i < _descs.size() ; ++i)
		_positions.push_back(_descs[i].getPosition());
}

void BORA::DebugFunc::FeaturesPoints( const BORA::FEATURES &_features, BORA::POSITIONS &_positions )
{
	_positions.clear();
	_positions.reserve(_features.size());

	for(unsigned int i = 0 ; i < _features.size() ; ++i)
		_positions.push_back(_features[i].getPosition_ref());
}

bool BORA::DebugFunc::isExistDir( const std::string &_path )
{
	// Feb. 2012, Min-Hyuk Sung
#ifdef WIN32
	_finddata_t fd;
	long handle;
	handle = _findfirst(_path.c_str(), &fd);
	
	bool result;
	if(handle == -1)
		result = false;
	else
		result = true;
	
	_findclose(handle);

	return result;
//#else
	//boost::filesystem::path p (_path.c_str());
	//return (boost::filesystem::exists(p) && boost::filesystem::is_directory(p));
#endif
}

bool BORA::DebugFunc::CreateDir( const std::string &_path )
{
	// Feb. 2012, Min-Hyuk Sung
#ifdef WIN32
	return _mkdir(_path.c_str()) < 0 ? false : true;
//#else
	//boost::filesystem::path p (_path.c_str());
	//return boost::filesystem::create_directory(p);
#endif
}

void BORA::DebugFunc::WriteDatabase( const std::string &_path, const BORA::Database &_database )
{
	std::ofstream ofile(_path.c_str());

	// 디스크립터의 내용을 먼저 쓴다.
	BORA::DESCRIPTORS descriptors;
	BORA::IndexTable indexTable;
	
	_database.getDatabase(descriptors, indexTable);
	
	ofile << descriptors.size() << std::endl;
	for(unsigned int i = 0 ; i < descriptors.size() ; ++i)
	{
		const BORA::Position &crnt_pos = descriptors[i].getPosition();
		ofile << crnt_pos.x << " " << crnt_pos.y << std::endl;
		
		const BORA::INDICES &crnt_indice = descriptors[i].getIndices();
		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = crnt_indice.begin(); it != crnt_indice.end(); ++it)
			ofile << (*it) << " ";
		/*
		for(unsigned int j = 0 ; j < crnt_indice.size() ; ++j)
		{
			ofile << crnt_indice[j] << " ";
		}
		*/
		ofile << std::endl;

		const BORA::DESC &crnt_data = descriptors[i].getDesc();
		for(unsigned int j = 0 ; j < 5 ; ++j)
		{
			ofile << crnt_data[j] << std::endl;
		}
	}

	// 인덱스 테이블을 쓴다.
	const BORA::INDEXTABLE &crnt_indice = indexTable.getData();
	for(unsigned int i = 0 ; i < BORA::INDEX_13_MAX ; ++i)
	{
		const BORA::INDICES &crnt_indices = crnt_indice[i];
		if(crnt_indices.size() != 0)
			ofile << i << " : ";

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = crnt_indices.begin(); it != crnt_indices.end(); ++it)
			ofile << (*it) << " ";
		/*
		for(unsigned int j = 0 ; j < crnt_indices.size() ; ++j)
		{
			ofile << crnt_indices[j] << " ";
		}
		*/
		ofile << std::endl;
	}

	ofile.close();
	
}

void BORA::DebugFunc::WriteDescriptorsToCSV( const std::string &_path, const BORA::DESCRIPTORS &_descriptors )
{
	std::stringstream info;
	std::ofstream csv(_path.c_str());

	info << "Index, "
		 << "디스크립터 순번"
		 << "좌표, , "
		 << "data, "
		 << std::endl;

	for(unsigned int i = 0 ; i < _descriptors.size() ; ++i)
	{
		const BORA::Descriptor &desc = _descriptors[i];
		const BORA::DESC &data = desc.getDesc();
		const BORA::Position &crnt_position = desc.getPosition();
		const BORA::INDICES &crnt_indices = desc.getIndices();

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = crnt_indices.begin(); it != crnt_indices.end(); ++it)
		{
			info << (*it) << ", "
				<< i << ", "
				<< crnt_position.x << ", " << crnt_position.y << ","
				<< BORA::getBinaryString(data[0]) << ", "
				<< BORA::getBinaryString(data[1]) << ", "
				<< BORA::getBinaryString(data[2]) << ", "
				<< BORA::getBinaryString(data[3]) << ", "
				<< BORA::getBinaryString(data[4]) << std::endl;
		}
		/*
		for(unsigned int j = 0 ; j < crnt_indices.size() ; ++j)
		{
			info << crnt_indices[j] << ", "
				 << i << ", "
				 << crnt_position.x << ", " << crnt_position.y << ","
				 << BORA::getBinaryString(data[0]) << ", "
				 << BORA::getBinaryString(data[1]) << ", "
				 << BORA::getBinaryString(data[2]) << ", "
				 << BORA::getBinaryString(data[3]) << ", "
				 << BORA::getBinaryString(data[4]) << std::endl;
		}
		*/
	}

	csv << info.str();
	csv.close();
}

BORA::TrainingDebug::TrainingDebug( void )
{

}

BORA::TrainingDebug::~TrainingDebug( void )
{
	if(debug_.is_open())
		debug_.close();
}

void BORA::TrainingDebug::StartDebug( const std::string &_path, const std::string &_image_file_name )
{
	// _path			= original image의 path(C:/data/)
	// _image_file_name = original image의 확장자를 제외한 파일명(cars)
	const std::stringstream first_name(_path + _image_file_name);
	std::stringstream check_path(first_name.str());
	unsigned int file_idx(0);

	while(BORA::DebugFunc::isExistDir(check_path.str()))	// 폴더가 존재 하면 뒤에 '(#)'를 붙인다.
	{
		check_path.str("");
		check_path.clear();
		check_path << first_name.str() << "(" << ++file_idx << ")";
	}

	root_dir_.clear();
	root_dir_.append(check_path.str());
	
	BORA::DebugFunc::CreateDir(root_dir_);			// 폴더를 생성한다
	root_dir_.append("/");							// 마지막에 '/'를 붙여준다.

	if(debug_.is_open())
		debug_.close();

	debug_.open(std::string(root_dir_+"home.html").c_str());				// home.html 파일 생성
	
	// root/home.html
	std::stringstream info;
	info << "<html>\n"
		 << "<head>\n"
		 << "<title>Fast Matching 2.3ns Training Debugging</title>\n"
		 << "<link type=\"text/css\" rel=\"stylesheet\" href=\"./style.css\" />\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1>HOME</h1>\n"
		 << "<hr/>\n"
		 << "<a href='./bins/b_home.html'>BINS</a> <a href='./descriptors/d_home.html'>DESCRIPTORS</a> <a href='./index table/i_home.html'>Index Table</a>\n"
		 << "<hr/>\n";

	debug_ << info.str();

	// root/style.css
	std::ofstream cssfile(std::string(root_dir_+"style.css").c_str());
	std::stringstream css;
	
	css << "table\n"
		<< "{\n"
		<< "margin: 0px 0px 0px 0px;\n"
		<< "padding: 0px 0px 0px 0px;\n"
		<< "border: 1px solid #555;\n"
		<< "border-collapse:collapse;\n"
		<< "}\n"
		<< "tr\n"
		<< "{\n"
		<< "text-align:center;\n"
		<< "margin: 0px 0px 0px 0px;\n"
		<< "padding: 0px 0px 0px 0px;\n"
		<< "}\n"
		<< "td\n"
		<< "{\n"
		<< "margin: 0px 0px 0px 0px;\n"
		<< "padding: 2px 5px 2px 5px;\n"
		<< "}\n"
		<< ".Over:hover\n"
		<< "{\n"
		<< "background-color:#00dd99;\n"
		<< "cursor:pointer;"
		<< "}\n"
		<< ".Thum\n"
		<< "{\n"
		<< "width:60px;\n"
		<< "}\n"
		<< ".DescTD\n"
		<< "{\n"
		<< "width:50px;\n"
		<< "height:50px;\n"
		<< "vertical-align:bottom;\n"
		<< "letter-spacing:0px;\n"
		<< "position:relative;\n"
		<< "background-color:#ff8;\n"
		<< "}\n"
		<< "div\n"
		<< "{\n"
		<< "padding:0px;\n"
		<< "margin:0px;\n"
		<< "position:absolute;\n"
		<< "width:20%;\n"
		<< "overflow:hidden;\n"
		<< "bottom:0px;\n"
		<< "}\n"
		<< ".intensityTD\n"
		<< "{\n"
		<< "padding:5px;\n"
		<< "}\n";

	cssfile << css.str();
	cssfile.close();
}

void BORA::TrainingDebug::SaveImages( const BORA::Image &_ref_img, const BORA::Image &_ref_descriptors_img )
{
	std::stringstream ref_img_save_file_name;
	std::stringstream dref_img_save_file_name;

	ref_img_save_file_name	<< root_dir_ << "ref_img.jpg";
	dref_img_save_file_name	<< root_dir_ << "dref_img.jpg";

	BORA::ImageUtility::SaveImage(_ref_img, ref_img_save_file_name.str());
	BORA::ImageUtility::SaveImage(_ref_descriptors_img, dref_img_save_file_name.str());

	std::stringstream info;
	info << "<p>\n"
		 << "<img src='./ref_img.jpg' onmouseover='src=\"./dref_img.jpg\"' onmouseout='src=\"./ref_img.jpg'/>\n"
		 << "</p>\n";

	info << "<p>\n"
		 << "Image Size : " << setw(9) << _ref_img.getWidth() << " x " << setw(9) << _ref_img.getHeight() << "\n"
		 << "</p>\n";

	debug_ << info.str();
}

void BORA::TrainingDebug::WriteDescriptorsSize( const unsigned int &_descriptorsSize )
{
	std::stringstream info;
	info << "<p>\n"
		<< "Descriptors Size : " << setw(20) << _descriptorsSize << "\n"
		<< "</p>\n";

	debug_ << info.str();
}

void BORA::TrainingDebug::WriteIndexTable( const BORA::IndexTable &_indexTable )
{
	const BORA::INDEXTABLE &data = _indexTable.getDataConst();
	std::stringstream info;
	info << "<p>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>key</td>\n"
		 << "<td>size</td>\n"
		 << "</tr>\n";

	const BORA::INDEXTABLE &idxTable = _indexTable.getDataConst();
	for(unsigned int i = 0 ; i < idxTable.size() ; ++i)
	{
		if(idxTable[i].size() == 0)
			continue;
		info << "<tr class='Over'>\n"
			 << "<td>" << i << "</td>\n"
			 << "<td>" << idxTable[i].size() << "</td>\n"
			 << "</tr>\n";
	}

	info << "</table>\n"
		 << "</p>\n";


	debug_ << info.str();
}

void BORA::TrainingDebug::FinishDebug( void )
{
	std::stringstream info;
	info << "</body>\n"
		 << "</html>";
	
	debug_ << info.str();
	debug_.close();
}

std::string BORA::TrainingDebug::getRootDir( void )
{
	return root_dir_;
}
const std::string & BORA::TrainingDebug::getRootDir( void ) const
{
	return root_dir_;
}



BORA::BinsDebug::BinsDebug( void )
{

}
BORA::BinsDebug::~BinsDebug( void )
{
	if(debug_.is_open())
		debug_.close();
}
std::string BORA::BinsDebug::getRootDir( void )
{
	return root_dir_;
}
const std::string & BORA::BinsDebug::getRootDir( void ) const
{
	return root_dir_;
}

void BORA::BinsDebug::StartDebug( const std::string &_path )
{
	// _path 는 이전에 training에서 넘어온 것이므로
	// 이미지path/이미지명/bins 로 넘어 왔다.
	root_dir_.clear();
	root_dir_.append(_path);
	if(BORA::DebugFunc::isExistDir(root_dir_) == false)
		BORA::DebugFunc::CreateDir(root_dir_);
	root_dir_.append("/");

	if(debug_.is_open())
		debug_.close();

	debug_.open(std::string(root_dir_+"b_home.html").c_str());

	std::stringstream info;
	info << "<html>\n"
		 << "<head>\n"
		 << "<title>Fast Matching 2.3ns Training Debugging</title>\n"
		 << "<link type=\"text/css\" rel=\"stylesheet\" href=\"../style.css\" />\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1>BINS</h1>\n"
		 << "<hr/>\n"
		 << "<a href='../home.html'>HOME</a> <a href='../descriptors/d_home.html'>DESCRIPTORS</a> <a href='../index table/i_home.html'>Index Table</a>\n"
		 << "<hr/>\n"
		 << "<!-- 여기부터 시작 -->\n";

	debug_ << info.str();
}

void BORA::BinsDebug::StartTable()
{
	std::stringstream info;

	info << "<p>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>index</td>\n"
		 << "<td>thumnail</td>\n"
		 << "<td>rotation</td>\n"
		 << "<td>scale</td>\n"
		 << "</tr>\n";

	debug_ << info.str();
}

void BORA::BinsDebug::WriteOneElem( const unsigned int &_bin_index, const double &_bin_rotation, const double &_bin_scale )
{
	// 여기에 올라가는 썸네일 이미지는 cannonical image만 보여주도록 한다.
	std::stringstream info;
	info.setf(ios::fixed);
	info << setprecision(4)
		 << "<tr class=\"Over\" onclick=\"window.location='./" << _bin_index << "/" << _bin_index << "_th_bin.html'\">\n"
		 << "<td>" << _bin_index << "</td>\n"
		 << "<td><img src='./" << _bin_index << "/cano_img.jpg' class='Thum'/></td>\n"
		 << "<td>" << setw(9) << _bin_rotation << "</td>\n"
		 << "<td>" << setw(9) << _bin_scale << "</td>\n"
		 << "</tr>\n";

	debug_ << info.str();
}

void BORA::BinsDebug::FinishTable()
{
	std::stringstream info;
	info << "</table>\n"
		 << "</p>\n";

	debug_ << info.str();
}

void BORA::BinsDebug::FinishDebug( void )
{
	std::stringstream info;
	info << "</body>\n"
		 << "</html>";

	debug_ << info.str();
}



BORA::BinDebug::BinDebug( void )
{

}
BORA::BinDebug::~BinDebug( void )
{
	if(debug_.is_open())
		debug_.close();
}
std::string BORA::BinDebug::getRootDir( void )
{
	return root_dir_;
}
const std::string & BORA::BinDebug::getRootDir( void ) const
{
	return root_dir_;
}

void BORA::BinDebug::StartDebug( const std::string &_path, const unsigned int &_bin_index )
{
	root_dir_.clear();

	// 위쪽에서 BinsDebug에서 폴더명이 내려왔으므로
	// _path : 이미지폴더/이미지명/bins/
	std::stringstream folder_name;
	folder_name << _path << _bin_index;	// folder_name = 이미지폴더/이미지명/bins/i
	root_dir_.append(folder_name.str());

	if(BORA::DebugFunc::isExistDir(root_dir_) == false)
		BORA::DebugFunc::CreateDir(root_dir_);

	root_dir_.append("/");				// root_dir_ = 이미지폴더/이미지명/bins/i/
	
	if(debug_.is_open())
		debug_.close();

	std::stringstream file_name;
	file_name << root_dir_ << _bin_index << "_th_bin.html";		// file_name = 이미지폴더/이미지명/bins/i/i_th_bin.html
	
	debug_.open(file_name.str().c_str());

	std::stringstream info;
	info << "<html>\n"
		 << "<head>\n"
		 << "<title>Fast Matching 2.3ns Training Debugging</title>\n"
		 << "<link type=\"text/css\" rel=\"stylesheet\" href=\"../../style.css\" />\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1><a href='../b_home.html'>BINS</a> > " << _bin_index << " th Bin</h1>\n"
		 << "<hr/>\n"
		 << "<a href='../../home.html'>HOME</a> <a href='../../descriptors/d_home.html'>DESCRIPTORS</a> <a href='../../index table/i_home.html'>Index Table</a>\n"
		 << "<hr/>\n";

	// 이미지 정보를 쓴다.
	info << "<!--여기부터 시작-->\n"
		 << "<p>\n"
		 << "<img src=\"./cano_img.jpg\" onmouseover=\"src='./fcano_img.jpg'\" onmouseout=\"src='./cano_img.jpg'\" />\n"
		 << "</p>\n";

	debug_ << info.str();
}

void BORA::BinDebug::FinishDebug( void )
{
	std::stringstream info;
	info << "</body>\n"
		 << "</html>";

	debug_ << info.str();

	debug_.close();
}


void BORA::BinDebug::SaveImages( const BORA::Image &_cano_img, const BORA::Image &_fcano_img )
{
	std::string cano_name, fcano_name;
	cano_name.append(root_dir_+"cano_img.jpg");
	fcano_name.append(root_dir_+"fcano_img.jpg");

	BORA::ImageUtility::SaveImage(_cano_img, cano_name);
	BORA::ImageUtility::SaveImage(_fcano_img, fcano_name);
}

void BORA::BinDebug::StartTrnTable()
{
	std::stringstream info;
	info << "<p>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>index</td><td>thumnail</td><td>rotation</td><td>scale</td><td>blur</td><td>noise</td><td>theta</td><td>phi</td><td>x scale</td><td>y scale</td>\n"
		 << "</tr>\n";

	debug_ << info.str();
}
void BORA::BinDebug::WriteTrnOneElement( const unsigned int &_trn_index, const double &_rotation, const double &_scale, const unsigned int &_blur, const unsigned int &_noise, const double &_theta, const double &_phi, const double &_sx, const double &_sy, const bool &_apply_affine )
{
	std::stringstream  info;
	info.setf(ios::fixed);
	info << setprecision(4)
		 << "<tr class='Over' onclick=\"window.location='./" << _trn_index << "/" << _trn_index << "_th_trn.html'\">\n"
		 << "<td>" << setw(9) << _trn_index << "</td>\n"
		 << "<td><img src='./" << _trn_index << "/trn_img.jpg' class='Thum' /></td>\n"
		 << "<td>" << setw(9) << _rotation << "</td>\n"
		 << "<td>" << setw(9) << _scale << "</td>\n";
	
	info << "<td>";
	if(_blur == 0)
		info << "x";
	else
		info << "o" << "(" << _blur << ")";
	info << "</td>\n";

	info << "<td>\n";
	if(_noise == 0)
		info << "x";
	else
		info << "o" << "(" << _noise << ")";
	info << "</td>\n";

	info << "<td>" << setw(9) << _theta << "</td>\n"
		 << "<td>" << setw(9) << _phi << "</td>\n"
		 << "<td>" << setw(9) << _sx << "</td>\n"
		 << "<td>" << setw(9) << _sy << "</td>\n"
		 << "</tr>\n";

	debug_ << info.str();
}
void BORA::BinDebug::FinishTrnTable()
{
	std::stringstream info;
	info << "</table>\n"
		<< "</p>\n";

	debug_ << info.str();
}

void BORA::BinDebug::WirteFeaturesInfo( const BORA::FEATURES &_features )
{
	// 해당 빈에서 발견된 features가 저장되는 경로는
	// 이미지폴더/이미지명/bins/i/features/t_th_feature.html
	// i : bin index
	// t : feature index
	std::stringstream features_folder;
	features_folder << root_dir_ << "features";		// features_folder : 이미지폴더/이미지명/bins/i/features
	if(BORA::DebugFunc::isExistDir(features_folder.str()) == false)
		BORA::DebugFunc::CreateDir(features_folder.str());


	std::stringstream info;
	info.setf(ios::fixed);
	info << "<p>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>index</td><td>position</td><td>patches size</td><td>indices</td>\n"
		 << "</tr>\n";

	for(unsigned int t = 0 ; t < _features.size() ; ++t)
	{
		WriteFeatureInfo(_features[t], t);

		const BORA::Position	&crnt_pos = _features[t].getPosition_ref();
		const BORA::HIP			&crnt_hip = _features[t].getHIP_ref();

		info << setprecision(4)
			 << "<tr class='Over' onclick=\"window.location='./features/" << t << "_th_feature.html'\">\n"
			 << "<td>" << setw(9) << t << "</td>\n"
			 << "<td>" << setw(5) << crnt_pos.x << " x " << setw(5) << crnt_pos.y << "</td>\n"
			 << "<td>" << setw(13) << crnt_hip.getPatchesSize() << "</td>\n";

		info << "<td>\n";
		const BORA::VOTEINDICES &crnt_vote = _features[t].getVoteIndice_ref();
		BORA::VOTEINDICES::const_iterator itr = crnt_vote.begin();
		while(itr != crnt_vote.end())
		{
			info << setw(9) << itr->first << "(" << setw(4) << itr->second << ") ";
			std::advance(itr, 1);
		}
		
		info << "</td>\n";
	}

	info << "</table>\n"
		 << "</p>\n";

	debug_ << info.str();

}

void BORA::BinDebug::WriteFeatureInfo(const BORA::Feature &_feature, const unsigned int &_t)
{
	// feature의 세부정보 쓰기
	std::stringstream file_name;
	file_name << root_dir_ << "features/" << _t << "_th_feature.html";
	std::ofstream feature_debug;
	feature_debug.open(file_name.str().c_str());

	std::stringstream info;
	info.setf(ios::fixed);
	info << "<html>\n"
		 << "<head>\n"
		 << "<title>Fast Matching 2.3ns Training Debugging</title>\n"
		 << "<link type=\"text/css\" rel=\"stylesheet\" href=\"../../../style.css\" />\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1><a href='../../b_home.html'>BINS</a> > i th Bin > " << _t << " th Feature</h1>\n"
		 << "<hr/>\n"
		 << "<a href='../../../home.html'>HOME</a> <a href='../../../descriptors/d_home.html'>DESCRIPTORS</a> <a href='../../../index table/i_home.html'>Index Table</a>\n"
		 << "<hr/>\n"
		 << "<!-- 여기부터 시작 -->\n";


	const BORA::Position &pos = _feature.getPosition_ref();
	info << "<p>\n"
		 << "position : " << setw(6) << pos.x << ", " << setw(6) << pos.y << "<br/>\n";
	
	const BORA::VOTEINDICES &voteidx = _feature.getVoteIndice_ref();
	BORA::VOTEINDICES::const_iterator itr = voteidx.begin();
	info << "indices : ";
	while(itr != voteidx.end())
	{
		info << itr->first << "(" << itr->second << ") ";
		std::advance(itr, 1);
	}
	info << "<br/>\n"
		 << "</p>\n";
	
	
	info << "<p>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>index</td>\n"
		 << "<td>training image thumnail</td>\n"
		 << "<td>patch position</td>\n"
		 << "<td>patch data</td>\n"
		 << "<td>quantized patch data</td>\n"
		 << "</tr>\n";

	const BORA::PATCHES &patches = _feature.getHIP_ref().getPATCHES_ref();
	for(unsigned int tp = 0 ; tp < patches.size() ; ++tp)
	{
		unsigned int bin_idx, trn_idx, patch_idx;
		const BORA::Position &patch_position = patches[tp].getPosition_ref();
		patches[tp].getIndexInfo(bin_idx, trn_idx, patch_idx);

		info << "<tr class='Over'>\n"
			 << "<td>" << setw(6) << tp << "</td>\n"
			 << "<td onclick=\"window.location='../" << trn_idx << "/" << trn_idx << "_th_trn.html'\"><img src=\"../" << trn_idx << "/trn_img.jpg\" class='thum'/></td>\n"
			 << "<td>" << setw(6) << patch_position.x << ", " << setw(6) << patch_position.y << "</td>\n"
			 << "<td><img src='../" << trn_idx << "/patches/" << patch_idx << ".jpg' /></td>\n"
			 << "<td><img src='../" << trn_idx << "/patches/q" << patch_idx << ".jpg' /></td>\n"
			 << "</tr>\n";
	}
	info << "</table>\n"
		 << "</p>\n";
	
	info << "</body>\n"
		 << "</html>\n";
	
	feature_debug << info.str();
	feature_debug.close();
}







BORA::TrnImageDebug::TrnImageDebug( void )
{

}
BORA::TrnImageDebug::~TrnImageDebug( void )
{
	if(debug_.is_open())
		debug_.close();
}
std::string BORA::TrnImageDebug::getRootDir( void )
{
	return root_dir_;
}
const std::string & BORA::TrnImageDebug::getRootDir( void ) const
{
	return root_dir_;
}

void BORA::TrnImageDebug::StartDebug( const std::string &_path, const unsigned int &_bin_idx, const unsigned int &_trn_idx )
{
	// _path는 i번째 bin이 주기 때문에
	// 이미지폴더/이미지명/bins/i/가 된다.
	std::stringstream filename;
	filename << _path << _trn_idx;	// filename : 이미지폴더/이미지명/bins/i/j

	root_dir_.clear();
	root_dir_.append(filename.str());

	if(BORA::DebugFunc::isExistDir(root_dir_) == false)
		BORA::DebugFunc::CreateDir(root_dir_);

	root_dir_.append("/");			// root_dir_ : 이미지폴더/이미지명/bins/i/j/
	
	if(debug_.is_open())
		debug_.close();

	std::stringstream trn_file_name;	// trn_file_name : 이미지폴더/이미지명/bins/i/j/j_th_trn.html
	trn_file_name << root_dir_
				  << _trn_idx
				  << "_th_trn.html";

	debug_.open(trn_file_name.str().c_str());

	std::stringstream info;
	info << "<html>\n"
		 << "<head>\n"
		 << "<title>Fast Matching 2.3ns Training Debugging</title>\n"
		 << "<link type=\"text/css\" rel=\"stylesheet\" href=\"../../../style.css\" />\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1><a href='../../b_home.html'>BINS</a> > <a href='../" << _bin_idx << "_th_bin.html'>" << _bin_idx << " th Bin</a> > " << _trn_idx << " th Training Image</h1>\n"
		 << "<hr/>\n"
		 << "<a href='../../home.html'>HOME</a> <a href='../../descriptors/d_home.html'>DESCRIPTORS</a> <a href='../../index table/i_home.html'>Index Table</a>\n"
		 << "<hr/>\n"
		 << "<!-- 여기부터 시작 -->\n";

	// training 이미지와 feature의 좌표가 찍힌 이미지를 기입
	info << "<p>\n"
		 << "<img src=\"./trn_img.jpg\" onmouseover=\"src='./ftrn_img.jpg'\" onmouseout=\"src='./trn_img.jpg'\" />\n"
		 << "</p>\n";

	debug_ << info.str();
}

void BORA::TrnImageDebug::FinishDebug( void )
{
	std::stringstream info;
	info << "</body>\n"
		 << "</table>\n";

	debug_ << info.str();
	debug_.close();
}

void BORA::TrnImageDebug::SaveImages( const BORA::Image &_trn_img, const BORA::Image &_ftrn_img )
{
	std::string trn_name, ftrn_name;
	trn_name = root_dir_ + "trn_img.jpg";
	ftrn_name = root_dir_ + "ftrn_img.jpg";

	BORA::ImageUtility::SaveImage(_trn_img, trn_name);
	BORA::ImageUtility::SaveImage(_ftrn_img, ftrn_name);
}

void BORA::TrnImageDebug::WriteAffineInfo( const double &_rotation, const double &_scale, const unsigned int &_blur, const unsigned int &_noise, const double &_theta, const double &_phi, const double &_sx, const double &_sy, const bool &_apply_affine )
{
	std::stringstream info;
	info.setf(ios::fixed);
	info << setprecision(4)
		 << "<p>\n"
		 << "rotation : " << setw(9) << _rotation << "</br>\n"
		 << "scale : " << setw(9) << _scale << "</br>\n"
		 << "blur : ";
	if(_blur == 0)
		info << "not apply";
	else
		info << _blur;
	
	info << "</br>\n"
		 << "noise : ";

	if(_noise == 0)
		info << "not apply";
	else
		info << _noise;

	info << "</br>\n"
		 << "Affine : ";

	if(_apply_affine == false)
		info << "not apply";
	else
	{
		info << setprecision(4) << "theta(" << setw(9) << _theta << ") phi(" << setw(9) << _phi << ") scaleX(" << setw(9) << _sx << ") scaleY(" << setw(9) << _sy << ")";
	}
	info << "</br>\n"
		 << "</p>\n";

	debug_ << info.str();
}

void BORA::TrnImageDebug::WritePatches( const BORA::FEATURES &_features )
{
	std::stringstream info;
	info << "<p>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>index</td><td>position</td><td>patch data</td><td>quantized patch data</td><td>INDEX</td>\n"
		 << "</tr>\n";
	for(unsigned int k = 0 ; k < _features.size() ; ++k)
	{
		const BORA::Patch &crnt_patch = _features[k].getHIP_ref().getPATCHES_ref()[0];
		const BORA::Position &crnt_position = crnt_patch.getPosition_ref();
		
		unsigned int bin_idx, trn_idx, patch_idx;
		crnt_patch.getIndexInfo(bin_idx, trn_idx, patch_idx);

		const BORA::VOTEINDICES &crnt_indice = _features[k].getVoteIndice_ref();
		BORA::VOTEINDICES::const_iterator itr(crnt_indice.begin());
		info << "<tr class='Over'>\n"
			 << "<td>" << patch_idx << "</td><td>" << setw(5) << crnt_position.x << ", " << setw(5) << crnt_position.y << "</td>"
			 << "<td><img src='./patches/" << patch_idx << ".jpg'></td>"
			 << "<td><img src='./patches/q" << patch_idx << ".jpg'></td>"
			 << "<td>" << setw(9) << itr->first << "</td>\n"
			 << "</tr>\n";
	}

	info << "</table>\n"
		 << "</p>\n";

	debug_ << info.str();

	if(BORA::DebugFunc::isExistDir(root_dir_+"patches") == false)
		BORA::DebugFunc::CreateDir(root_dir_+"patches");
	SavePatchesImage(_features);
}

void BORA::TrnImageDebug::SavePatchesImage(const BORA::FEATURES &_features)
{
	for(unsigned int i = 0 ; i < _features.size() ; ++i)
	{
		unsigned int bin_idx, trn_idx, patch_idx;
		const BORA::Patch &patch = _features[i].getHIP_ref().getPATCHES_ref()[0];
		BORA::Patch qpatch;
		patch.getIndexInfo(bin_idx, trn_idx, patch_idx);

		BORA::ImageUtility::Quantization(patch, qpatch);

		// root_dir_ : 이미지폴더/이미지명/bins/i/j/
		std::stringstream patch_name, qpatch_name;
		patch_name << root_dir_ << "patches/" << patch_idx << ".jpg";
		qpatch_name << root_dir_ << "patches/q" << patch_idx << ".jpg";
		
		BORA::Image patch_img, qpatch_img;
		BORA::ImageUtility::PatchToImage(patch, patch_img, 10);
		BORA::ImageUtility::QPatchToImage(qpatch, qpatch_img, 10);

		//BORA::DebugFunc::Show(patch_img);
		//BORA::DebugFunc::Show(qpatch_img);

		BORA::ImageUtility::SaveImage(patch_img, patch_name.str(), BORA::SAVETYPE_PATCH);
		BORA::ImageUtility::SaveImage(qpatch_img, qpatch_name.str(), BORA::SAVETYPE_PATCH);
	}
}







BORA::IndexTableDebug::IndexTableDebug( void )
{

}
BORA::IndexTableDebug::~IndexTableDebug( void )
{
	if(debug_.is_open())
		debug_.close();
}
std::string BORA::IndexTableDebug::getRootDir( void )
{
	return root_dir_;
}
const std::string & BORA::IndexTableDebug::getRootDir( void ) const
{
	return root_dir_;
}


void BORA::IndexTableDebug::StartDebug( const std::string &_path )
{
	root_dir_.clear();

	std::stringstream file_name;
	file_name << _path << "index table";		// file_name : 이미지폴더/이미지명/index table
	
	root_dir_.append(file_name.str());
	if(BORA::DebugFunc::isExistDir(root_dir_) == false)
		BORA::DebugFunc::CreateDir(root_dir_);

	root_dir_.append("/");

	if(debug_.is_open())
		debug_.close();

	debug_.open(std::string(root_dir_+"i_home.html").c_str());

	std::stringstream info;
	info << "<html>\n"
		 << "<head>\n"
		 << "<title>Fast Matching 2.3ns Training Debugging</title>\n"
		 << "<link type=\"text/css\" rel=\"stylesheet\" href=\"../style.css\" />\n"
		 << "<script type=\"text/javascript\">\n"
		 << "function getSelectValue(id){\n"
		 << "	var select = document.getElementById(id);\n"
		 << "	return select.options[select.selectedIndex].value;\n"
		 << "}\n"
		 << "</script>\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1>INDEX TABLE</h1>\n"
		 << "<hr/>\n"
		 << "<a href='../home.html'>HOME</a> <a href='../bins/b_home.html'>BINS</a> <a href='../descriptors/d_home.html'>DESCRIPTORS</a>\n"
		 << "<hr/>\n"
		 << "<!-- 여기부터 시작 -->\n";



	debug_ << info.str();
}
void BORA::IndexTableDebug::FinishDebug()
{
	std::stringstream info;
	info << "</body>\n"
		 << "</html>";

	debug_ << info.str();
}

void BORA::IndexTableDebug::WriteIndexTableInfo( const BORA::IndexTable &_table )
{
	std::stringstream info;
	info << "<p>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>key</td>\n"
		 << "<td>descriptors</td>\n"
		 << "<td>link</td>\n"
		 << "</tr>\n";

	const BORA::INDEXTABLE &table = _table.getDataConst();
	for(unsigned int i = 0 ; i < table.size() ; ++i)
	{
		const BORA::INDICES &crnt_indices = table[i];
		if(crnt_indices.size() == 0)
			continue;

		info << "<tr class='Over'>\n"
			 << "<td>" << i << "</td>\n"
			 << "<td>\n"
			 << "<select id='select_" << i << "'>\n";

		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = crnt_indices.begin(); it != crnt_indices.end(); ++it)
		{
			const BORA::INDEX &crnt_index = (*it);
			info << "<option value='../descriptors/" << crnt_index << "/" << crnt_index << "_th_desc.html'>" << crnt_index << "</option>\n";
		}
		/*
		for(unsigned int ti = 0 ; ti < crnt_indices.size() ; ++ti)
		{
			const BORA::INDEX &crnt_index = crnt_indices[ti];
			info << "<option value='../descriptors/" << crnt_index << "/" << crnt_index << "_th_desc.html'>" << crnt_index << "</option>\n";
		}
		*/

		info << "</select>\n"
			 << "</td>\n"
			 << "<td>\n"
			 << "<input type='button' value='open descriptor info' onclick='window.location=getSelectValue(\"select_" << i << "\")' />\n"
			 << "</td>\n";

	}


	info << "</table>\n"
		 << "</p>\n";

	debug_ << info.str();
}




BORA::DescriptorsDebug::DescriptorsDebug()
{
	//
}
BORA::DescriptorsDebug::~DescriptorsDebug()
{

}
std::string BORA::DescriptorsDebug::getRootDir()
{
	return root_dir_;
}
const std::string &BORA::DescriptorsDebug::getRootDir() const
{
	return root_dir_;
}

void BORA::DescriptorsDebug::StartDebug(const std::string &_path)
{
	root_dir_.clear();
	std::stringstream file_name;
	
	file_name << _path << "descriptors";		// file_name : 이미지폴더/이미지명/descriptors
	root_dir_.append(file_name.str());

	if(BORA::DebugFunc::isExistDir(root_dir_) == false)
		BORA::DebugFunc::CreateDir(root_dir_);

	root_dir_.append("/");

	if(debug_.is_open())
		debug_.close();

	debug_.open(std::string(root_dir_+"d_home.html").c_str());

	std::stringstream info;
	info << "<html>\n"
		 << "<head>\n"
		 << "<title>Fast Matching 2.3ns Training Debugging</title>\n"
		 << "<link type=\"text/css\" rel=\"stylesheet\" href=\"../style.css\" />\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1>DESCRIPTORS</h1>\n"
		 << "<hr/>\n"
		 << "<a href='../home.html'>HOME</a> <a href='../bins/b_home.html'>BINS</a> <a href='../index table/i_home.html'>Index Table</a>\n"
		 << "<hr/>\n"
		 << "<!-- 여기부터 시작 -->\n";

	debug_ << info.str();
}

void BORA::DescriptorsDebug::FinishDebug()
{
	std::stringstream info;
	info << "</body>\n"
		 << "</table>\n";

	debug_ << info.str();
}

void BORA::DescriptorsDebug::WriteDescriptorsTable(const BORA::TDESCRIPTORS &_descs)
{
	std::stringstream info;
	info << "<p>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>index</td>\n"
		 << "<td>position</td>\n"
		 << "<td>descriptor data</td>\n"
		 << "<td>indices</td>\n"
		 << "</tr>\n";

	// Dec. 2011, Min-Hyuk Sung
	//for(unsigned int c = 0 ; c < _descs.size() ; ++c)
	//{
	//	const BORA::TDescriptor &desc = _descs[c];
	unsigned int c = 0;
	for(BORA::TDESCRIPTORS::const_iterator it = _descs.begin(); it != _descs.end(); ++it, ++c)
	{
		const BORA::TDescriptor &desc =(*it);
		info << "<tr class='Over' onclick=\"window.location='./" << c << "/" << c << "_th_desc.html'\">\n"
			 << "<td>" << c << "</td>\n";
		
		const BORA::Position &pos = desc.getPosition();
		info << "<td>\n"
			 << setw(5) << pos.x << ", " << setw(5) << pos.y
			 << "</td>\n";
		
		
		const BORA::DESC &data = desc.getDesc();
		info << "<td>\n";		
		info << BORA::getBinaryString(data[0]) << "<br/>\n"
			 << BORA::getBinaryString(data[1]) << "<br/>\n"
			 << BORA::getBinaryString(data[2]) << "<br/>\n"
			 << BORA::getBinaryString(data[3]) << "<br/>\n"
			 << BORA::getBinaryString(data[4]) << "<br/>\n";

		info << "</td>\n";
		
		const BORA::INDICES &indices = desc.getIndices();
		info << "<td>\n";
		// Dec. 2011, Min-Hyuk Sung
		// INDICES: vector -> list
		for(BORA::INDICES::const_iterator it = indices.begin(); it != indices.end(); ++it)
			info << (*it) << ",  ";
		/*
		for(unsigned int ci = 0 ; ci < indices.size() ; ++ci)
		{
			info << indices[ci] << ",  ";
		}
		*/
		info << "</td>\n"
			 << "</tr>\n";

		// descriptor 하나의 세부적인 내용을 적은 파일을 생성해야 한다.
		WriteDescriptor(desc, c);
	}
	
	info << "</table>\n"
		 << "</p>\n";


	debug_ << info.str();
}

void BORA::DescriptorsDebug::WriteDescriptor(const BORA::TDescriptor &_desc, const unsigned int &_desc_idx)
{
	std::stringstream file_name;
	file_name << root_dir_ << _desc_idx;				// file_name : 이미지폴더/이미지명/_desc_idx

	BORA::DebugFunc::CreateDir(file_name.str());

	file_name << "/" << _desc_idx << "_th_desc.html";	// file_name : 이미지폴더/이미지명/_desc_idx/_desc_idx_th_desc.html

	std::ofstream descriptor_debug;
	descriptor_debug.open(file_name.str().c_str());

	std::stringstream info;
	info << "<html>\n"
		 << "<head>\n"
		 << "<title>Fast Matching 2.3ns Training Debugging</title>\n"
		 << "<link type=\"text/css\" rel=\"stylesheet\" href=\"../../style.css\" />\n"
		 << "</head>\n"
		 << "<body>\n"
		 << "<h1><a href=\"../d_home.html\">DESCRIPTROS</a> > " << _desc_idx << " th descirptor</h1>\n"
		 << "<hr/>\n"
		 << "<a href='../../home.html'>HOME</a> <a href='../../bins/b_home.html'>BINS</a> <a href='../../index table/i_home.html'>Index Table</a>\n"
		 << "<hr/>\n"
		 << "<!-- 여기부터 시작 -->\n"
		 << "<p>\n";

	const BORA::Position &pos = _desc.getPosition();
	info << "position : " << setw(5) << pos.x << ", " << setw(5) << pos.y << "<br/>\n"
		 << "descriptor data<br />\n";

	const BORA::DESC &data = _desc.getDesc();
	info << "0 : " << BORA::getBinaryString(data[0]) << "<br />\n"
		 << "1 : " << BORA::getBinaryString(data[1]) << "<br />\n"
		 << "2 : " << BORA::getBinaryString(data[2]) << "<br />\n"
		 << "3 : " << BORA::getBinaryString(data[3]) << "<br />\n"
		 << "4 : " << BORA::getBinaryString(data[4]) << "<br />\n";

	const BORA::INDICES &indices = _desc.getIndices();
	// Dec. 2011, Min-Hyuk Sung
	// INDICES: vector -> list
	for(BORA::INDICES::const_iterator it = indices.begin(); it != indices.end(); ++it)
		info << (*it) << ",  ";
	/*
	for(unsigned int i = 0 ; i < indices.size() ; ++i)
	{
		info << indices[i] << ",  ";
	}
	*/

	info << "<p>\n"
		 << "<table border='1'>\n";

	const BORA::HIP	&hip	= _desc.getFeature().getHIP_ref();
	const std::tr1::array<BORA::HIST_Q_PATCH, 5u> &hip_data = hip.getDatas_ref();
	
	// HIP 정보
	for(unsigned int row = 0 ; row < 8 ; ++row)
	{
		info << "<tr>\n";
		for(unsigned int col = 0 ; col < 8 ; ++col)
		{
			std::tr1::array<unsigned int, 5u> percent = {
				static_cast<unsigned int>(hip_data[0][row*8+col] * 100),
				static_cast<unsigned int>(hip_data[1][row*8+col] * 100),
				static_cast<unsigned int>(hip_data[2][row*8+col] * 100),
				static_cast<unsigned int>(hip_data[3][row*8+col] * 100),
				static_cast<unsigned int>(hip_data[4][row*8+col] * 100)
			};

			info << "<td class='descTD'>\n"
				 << "<div style='background-color:#000000; left: 0%; height:" << percent[0] << "&#37;' title='" << percent[0] << "'></div>\n"
				 << "<div style='background-color:#3f3f3f; left:20%; height:" << percent[1] << "&#37;' title='" << percent[1] << "'></div>\n"
				 << "<div style='background-color:#7e7e7e; left:40%; height:" << percent[2] << "&#37;' title='" << percent[2] << "'></div>\n"
				 << "<div style='background-color:#bdbdbd; left:60%; height:" << percent[3] << "&#37;' title='" << percent[3] << "'></div>\n"
				 << "<div style='background-color:#fcfcfc; left:80%; height:" << percent[4] << "&#37;' title='" << percent[4] << "'></div>\n"
				 << "</td>\n";
		}
		info << "</tr>\n";
	}

	info << "</table>\n"
		 << "</p>\n";


	// 과정
	BORA::PATCH accum[5];
	for(unsigned int i = 0 ; i < 5 ; ++i)
		//accum[i].fill(0);
		accum[i].assign(0);

	const BORA::PATCHES &patches = hip.getPATCHES_ref();
	BORA::PATCHES qpatches;
	qpatches.resize(patches.size());

	for(unsigned int i = 0 ; i < patches.size() ; ++i)
		BORA::ImageUtility::Quantization(patches[i], qpatches[i]);

	info << "<p>\n"
		 << "Process<br/>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>index</td>\n"
		 << "<td>quantized patch data</td>\n"
		 << "<td>level</td>\n"
		 << "<td>accum[0]</td>\n"
		 << "<td>accum[1]</td>\n"
		 << "<td>accum[2]</td>\n"
		 << "<td>accum[3]</td>\n"
		 << "<td>accum[4]</td>\n"
		 << "</tr>\n";

	for(unsigned int k = 0 ; k < qpatches.size() ; ++k)
	{
		unsigned int bin_idx, trn_idx, patch_idx;
		patches[k].getIndexInfo(bin_idx, trn_idx, patch_idx);

		info << "<tr class='Over'>\n"
			 << "<td>" << k << "</td>\n"
			 << "<td><img src='../../bins/" << bin_idx << "/" << trn_idx << "/patches/q" << patch_idx << ".jpg' /></td>";
		
		// one of quantized patches information
		info << "<td>\n"
			 << "<table border='1'>\n";
		for(unsigned int row = 0 ; row < 8 ; ++row)
		{
			info << "<tr>\n";
			for(unsigned int col = 0 ; col < 8 ; ++col)
			{
				switch(qpatches[k].getPATCH_ref()[row*8+col])
				{
				case 0: ++accum[0][row*8+col]; break;
				case 1: ++accum[1][row*8+col]; break;
				case 2:	++accum[2][row*8+col]; break;
				case 3:	++accum[3][row*8+col]; break;
				case 4:	++accum[4][row*8+col]; break;
				}
				info << "<td class='intensityTD'>" << qpatches[k].getPATCH_ref()[row*8+col] << "</td>\n";
			}
			info << "</tr>\n";
		}

		info << "</table>\n"
			 << "</td>\n";

		// acuum intensity level l patch information
		for(unsigned int l = 0 ; l < 5 ; ++l)
		{
			info << "<td>\n"
				 << "<table border='1'>\n";
			for(unsigned int row = 0 ; row < 8 ; ++row)
			{
				info << "<tr>\n";
				for(unsigned int col = 0; col < 8 ; ++col)
				{
					info << "<td class='intensityTD'>" << accum[l][row*8+col] << "</td>\n";
				}
				info << "</tr>\n";
			}
			info << "</table>\n"
				 << "</td>\n";
		}
	}

	info << "<p>\n"
		 << "Result<br/>\n"
		 << "<table border='1'>\n"
		 << "<tr>\n"
		 << "<td>desc</td>\n"
		 << "<td>[0]</td>\n"
		 << "<td>[1]</td>\n"
		 << "<td>[2]</td>\n"
		 << "<td>[3]</td>\n"
		 << "<td>[4]</td>\n"
		 << "</tr>\n";
	
	// five accumulated intensity number
	info << "<tr class='Over'>\n";
	info << "<td>accum</td>\n";
	
	for(unsigned int i = 0 ; i < 5 ; ++i)
	{	
		info << "<td>\n"
			 << "<table border='1'>\n";
		for(unsigned int row = 0 ; row < 8 ; ++row)
		{
			info << "<tr>\n";
			for(unsigned int col = 0; col < 8 ; ++col)
			{
				info << "<td class='intensityTD'>" << accum[i][row*8+col] << "</td>\n";
			}
			info << "</tr>\n";
		}

		info << "</table>\n"
			 << "</td>\n";
	}

	// result
	info << "<tr>\n"
		 << "<td>result</td>\n";

	for(unsigned int i = 0 ; i < 5 ; ++i)
	{
		info << "<td>\n"
			 << "<table border='1'>\n";
		for(unsigned int row = 0 ; row < 8 ; ++row)
		{
			info << "<tr>\n";
			for(unsigned int col = 0 ; col < 8 ; ++col)
			{
				info << "<td class='intensityTD'>" << getValueFromDesc(data[i], row*8+col) << "</td>\n";
			}
			info << "</tr>\n";
		}
		info << "</table>\n"
			 << "</td>\n";
	}

	info << "</tr>\n";


	descriptor_debug << info.str();
	descriptor_debug.close();
}

unsigned int BORA::DescriptorsDebug::getValueFromDesc( const unsigned __int64 _data, const unsigned int &_i )
{
	return (_data >> _i) & 1;
}
