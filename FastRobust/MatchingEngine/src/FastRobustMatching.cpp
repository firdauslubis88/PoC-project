#include "FastRobustMatching.h"
#include <time.h>
#include <vector>

#include <TooN/TooN.h>
#include <TooN/se3.h>
#include "PTAM/ATANCamera.h"
#include "Match.h"
#include "MatchOption.h"


static bool					g_initizlize = false;

static ATANCamera			g_target_camera;
static ATANCamera			g_query_camera; 
static Matrix<3>			g_homography;
static SE3<>				g_camera_pose;

std::vector<HomographyMatch> g_pairs;
std::vector<HomographyMatch> g_inlier_pairs;

// Dec. 2011, Min-Hyuk Sung
static double				g_modelview_matrix[16];
static double				g_frustrum_matrix[16];

static BORA::Match			g_match;
static BORA::MatchOption	g_options;


bool FastRobustMatching::initialize( const char* _feature_file, int _query_width, int _query_height )
{
	if( !_feature_file )	return false;

	g_target_camera = ATANCamera();
	g_query_camera = ATANCamera();

	int target_width = 0, target_height = 0;
	if( !g_match.loadData(_feature_file, target_width, target_height) )
	{
		std::cerr << "error: [FastRobust::Initialize] Loading file failed: "
			<< _feature_file << std::endl;
		return false;
	}
	else
	{
		g_target_camera.SetImageSize(makeVector<double>(target_width, target_height));
	}

	g_query_camera.SetImageSize(makeVector<double>((double)_query_width, (double)_query_height));

	Matrix<4> mm4frustum = g_query_camera.MakeUFBLinearFrustumMatrix(10, 5000);
	//Matrix<4> mm4frustum = g_query_camera.MakeUFBLinearFrustumMatrix(0.5, 100);

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			g_frustrum_matrix[i*4+j] = mm4frustum[j][i];

	g_options.dissimilar_score = DISSIMILAR_SCORE;
	g_options.threshold = FAST_CORNER_THRESHOLD;

	g_initizlize = true;
	return g_initizlize;
}

bool FastRobustMatching::matching( IplImage *_query )
{
	if(!g_initizlize)	return false;
//	printf("after g_initizliz\n");

	if(!_query
		|| _query->width != g_query_camera.GetImageSize()[0]
	|| _query->height != g_query_camera.GetImageSize()[1])
		return false;
//	printf("after g_query_camera.GetImageSize\n");

	BORA::Image query_image = _query;
	BORA::Image gray_image;
	BORA::ImageUtility::ColorImageToGray(query_image, gray_image);

//	printf("after ColorImageToGray\n");
	// Assume that the target image size(width, height) is the same with the source image size
	bool ret = g_match.match(gray_image, g_target_camera, g_query_camera,
		g_homography, g_camera_pose, g_pairs, g_inlier_pairs, g_options);
//	printf("after g_match.match\n");
	if( !ret )
	{
		return false;
	}


#ifndef USE_GLU_PERSPECTIVE
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			g_modelview_matrix[i*4+j] = g_camera_pose.get_rotation().get_matrix()[j][i];

	for(int i = 0; i < 3; i++)
	{
		g_modelview_matrix[3*4+i] = g_camera_pose.get_translation()[i];
		g_modelview_matrix[i*4+3] = 0;
	}

	g_modelview_matrix[3*4+3] = 1;
#else
	// Transform T: (x, y, z) -> (-x, -y, -z)
	// In the graphics pipeline (Modelview Matrix -> Projection Matrix),
	// PTAM's ATANCamera use its own projection(frustum) matrix,
	// which changes y, z axes to -y, -z, repectively
	// (Due to the difference between computer graphics and vision coordinates system).
	// In order to use OpenGL function (gluPerspective),
	// apply this transform next to the 'Modelview Matrix'.
	Vector<3> x_axis = makeVector<double>(1, 0, 0);
	Vector<3> y_axis = makeVector<double>(0, -1, 0);
	Vector<3> z_axis = makeVector<double>(0, 0, -1);

	Matrix<3> trans_rot;
	trans_rot[0] = x_axis;
	trans_rot[1] = y_axis;
	trans_rot[2] = z_axis;
	trans_rot = trans_rot.T();

	TooN::SE3<> trans;
	trans.get_rotation() = trans_rot;
	trans.get_translation() = makeVector<double>(0, 0, 0);

	trans = trans * g_camera_pose;

	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			g_modelview_matrix[i*4+j] = trans.get_rotation().get_matrix()[j][i];

	for(int i = 0; i < 3; i++)
	{
		g_modelview_matrix[3*4+i] = trans.get_translation()[i];
		g_modelview_matrix[i*4+3] = 0;
	}

	g_modelview_matrix[3*4+3] = 1;
#endif

	return true;
}

#ifndef USE_GLU_PERSPECTIVE
double *FastRobustMatching::get_frustum_matrix()
{
	return g_frustrum_matrix;
}
#endif

double *FastRobustMatching::get_modelview_matrix()
{
	return g_modelview_matrix;
}

void FastRobustMatching::get_box_corners( double _corners[4][2] )
{
	Vector<2> src_corners[4];
	src_corners[0][0] = 0;									src_corners[0][1] = g_target_camera.GetImageSize()[1];
	src_corners[1][0] = g_target_camera.GetImageSize()[0];	src_corners[1][1] = g_target_camera.GetImageSize()[1];
	src_corners[2][0] = g_target_camera.GetImageSize()[0];	src_corners[2][1] = 0;
	src_corners[3][0] = 0;									src_corners[3][1] = 0;

	for(unsigned int i = 0; i < 4; i++)
	{
		Vector<3> up = unproject(src_corners[i]);
		//Vector<2> p = project( g_homography * up );
		Vector<2> p = g_query_camera.Project( project(g_camera_pose * up) );
		_corners[i][0] = p[0];
		_corners[i][1] = p[1];
	}
}

void FastRobustMatching::get_box_corners_3D( double _corners[4][3] )
{
	Vector<2> src_corners[4];
	src_corners[0][0] = 0;									src_corners[0][1] = g_target_camera.GetImageSize()[1];
	src_corners[1][0] = g_target_camera.GetImageSize()[0];	src_corners[1][1] = g_target_camera.GetImageSize()[1];
	src_corners[2][0] = g_target_camera.GetImageSize()[0];	src_corners[2][1] = 0;
	src_corners[3][0] = 0;	

	for(unsigned int i = 0; i < 4; i++)
	{
		Vector<3> p = unproject(src_corners[i]);
		_corners[i][0] = p[0];
		_corners[i][1] = p[1];
		_corners[i][2] = p[2];
	}
}

/*
void CFastRobust::GetAxes2D( double axes[4][2] )
{
	Vector<3> src_axes[4];
	src_axes[0][0] = 0;	src_axes[0][1] = 0;	src_axes[0][2] = 0;
	src_axes[1][0] = 1;	src_axes[1][1] = 0;	src_axes[1][2] = 0;
	src_axes[2][0] = 0;	src_axes[2][1] = 1;	src_axes[2][2] = 0;
	src_axes[3][0] = 0;	src_axes[3][1] = 0;	src_axes[3][2] = 1;

	for(unsigned int i = 0; i < 4; i++)
	{
		Vector<2> p = m_query_camera.Project( project( m_camera_pose * src_axes[i] ) );
		axes[i][0] = p[0];
		axes[i][1] = p[1];
	}
}
*/

void FastRobustMatching::get_axes( double _axes[4][3] )
{
	Vector<3> src_axes[4];
	src_axes[0][0] = 0;	src_axes[0][1] = 0;	src_axes[0][2] = 0;
	src_axes[1][0] = 1;	src_axes[1][1] = 0;	src_axes[1][2] = 0;
	src_axes[2][0] = 0;	src_axes[2][1] = 1;	src_axes[2][2] = 0;
	src_axes[3][0] = 0;	src_axes[3][1] = 0;	src_axes[3][2] = 1;

	for(unsigned int i = 0; i < 4; i++)
	{
		Vector<3> p = g_camera_pose * src_axes[i];
		_axes[i][0] = p[0];
		_axes[i][1] = p[1];
		_axes[i][2] = p[2];
	}
}

// Dec. 2011, Min-Hyuk Sung
void FastRobustMatching::get_matched_pairs( MatchPair &_pairs )
{
	unsigned int num_pairs = g_pairs.size();
	_pairs.resize( num_pairs );
	for(unsigned int i = 0; i < num_pairs; i++)
	{
		CvPoint p1 = cvPoint( g_pairs[i].v2CamPlaneFirst[0], g_pairs[i].v2CamPlaneFirst[1] );
		CvPoint p2 = cvPoint( g_pairs[i].v2CamPlaneSecond[0], g_pairs[i].v2CamPlaneSecond[1] );
		_pairs[i] = std::make_pair(p1, p2);
	}
}

void FastRobustMatching::get_matched_inlier_pairs( MatchPair &_pairs )
{
	unsigned int num_pairs = g_inlier_pairs.size();
	_pairs.resize( num_pairs );
	for(unsigned int i = 0; i < num_pairs; i++)
	{
		CvPoint p1 = cvPoint( g_inlier_pairs[i].v2CamPlaneFirst[0], g_inlier_pairs[i].v2CamPlaneFirst[1] );
		CvPoint p2 = cvPoint( g_inlier_pairs[i].v2CamPlaneSecond[0], g_inlier_pairs[i].v2CamPlaneSecond[1] );
		_pairs[i] = std::make_pair(p1, p2);
	}
}
