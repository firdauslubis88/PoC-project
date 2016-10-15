// Copyright 2008 Isis Innovation Limited
#include "CameraPoseARToolKit.h"
#include <TooN/wls.h>
#include "MEstimator.h"

using namespace std;
bool CameraPoseARToolKit::Compute(ATANCamera &cameraFirst, ATANCamera &cameraSecond,
							 vector<HomographyMatch> &vMatchesInliers,
							 Matrix<3> &m3BestHomography,
							 SE3<> &se3SecondFromFirst)
{
	// Assume that 'cameraFirst' is the given image (as a marker)
	// and 'cameraSecond' is the captured image from a camera

	ParamInitialize(cameraSecond);

	//double center[2] = {0, 0};
	//double width = 80;
	double center[2] = {320, 240};
	double width = 480;
	double conv[3][4];
	double prev_conv[3][4];

	ARMarkerInfo marker_info;
	GetMarkerInfo(cameraFirst, cameraSecond, m3BestHomography, marker_info);

	/*
	for(unsigned int i = 0; i < 3; i++)
		for(unsigned int j = 0; j < 3; j++)
			prev_conv[i][j] = se3SecondFromFirst.get_rotation().get_matrix()[i][j];

	for(unsigned int i = 0; i < 3; i++)
		prev_conv[i][3] = se3SecondFromFirst.get_translation()[i];
		*/

	// Get camera pose error
	double avg_error = 0.0;
	unsigned int num_inlier_matches = vMatchesInliers.size();
	if( num_inlier_matches > 0 )
	{
		for(unsigned int i = 0; i < num_inlier_matches; i++)
		{
			Vector<3> up = unproject( vMatchesInliers[i].v2CamPlaneFirst );
			Vector<2> p = cameraSecond.Project( project(se3SecondFromFirst * up) );
			Vector<2> d = (vMatchesInliers[i].v2CamPlaneSecond - p);
			double error = sqrt(d * d);
			avg_error += error;
		}
		avg_error /= num_inlier_matches;
	}

	if(avg_error < 2 * MAX_PIXEL_ERROR)
	{
		for(int iteration = 0; iteration < NUM_REFINE_CAMERA_POSE; iteration++)
		{
			double prior = 1.0;
			RefineCameraPoseWithInliers(cameraFirst, cameraSecond,
			vMatchesInliers, se3SecondFromFirst, prior);
		}
	}
	else
	{
		//arGetTransMatCont( &marker_info, prev_conv, center, width, conv );
		arGetTransMat( &marker_info, center, width, conv );

		Matrix<3> m3Rp;
		for(unsigned int i = 0; i < 3; i++)
			for(unsigned int j = 0; j < 3; j++)
				m3Rp[i][j] = conv[i][j];
		se3SecondFromFirst.get_rotation() = m3Rp;

		for(unsigned int i = 0; i < 3; i++)
			se3SecondFromFirst.get_translation()[i] = conv[i][3];
	}

	return true;
}

void CameraPoseARToolKit::ParamInitialize(ATANCamera &camera)
{
	ARParam param;
	param.xsize = (int)camera.GetImageSize()[0];
	param.ysize = (int)camera.GetImageSize()[1];

	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 4; j++)
			param.mat[i][j] = 0;

	param.mat[0][0] = camera.GetFocalLength()[0];
	param.mat[1][1] = camera.GetFocalLength()[1];
	param.mat[0][2] = camera.GetImageCenter()[0];
	param.mat[1][2] = camera.GetImageCenter()[1];
	param.mat[0][1] = 0;
	param.mat[2][2] = 1;

	param.dist_factor[0] = camera.GetImageCenter()[0];
	param.dist_factor[1] = camera.GetImageCenter()[1];
	param.dist_factor[2] = 0;	// Assume no distortion
	param.dist_factor[3] = 1;

	arInitCparam( &param );
}

void CameraPoseARToolKit::GetMarkerInfo(
	ATANCamera &cameraFirst, ATANCamera &cameraSecond,
	Matrix<3> &m3BestHomography, ARMarkerInfo &marker_info )
{
	marker_info.dir = 0;

	Vector<2> marker_corners[4];
	Vector<2> image_corners[4];
	//Vector<3> image_edge_centers[4];

	double marker_width = (int)cameraFirst.GetImageSize()[0];
	double marker_height = (int)cameraFirst.GetImageSize()[1];
	/*
	marker_corners[0][0] = 0;				marker_corners[0][1] = 0;
	marker_corners[1][0] = marker_width;	marker_corners[1][1] = 0;
	marker_corners[2][0] = marker_width;	marker_corners[2][1] = marker_height;
	marker_corners[3][0] = 0;				marker_corners[3][1] = marker_height;
	*/
	double marker_size = std::min(marker_width, marker_height);
	double marker_x_offset = (marker_width - marker_size) / 2;
	double marker_y_offset = (marker_height - marker_size) / 2;
	marker_corners[0][0] = marker_x_offset;						marker_corners[0][1] = marker_y_offset + marker_size;
	marker_corners[1][0] = marker_x_offset + marker_size;		marker_corners[1][1] = marker_y_offset + marker_size;
	marker_corners[2][0] = marker_x_offset + marker_size;		marker_corners[2][1] = marker_y_offset;
	marker_corners[3][0] = marker_x_offset;						marker_corners[3][1] = marker_y_offset;

	// vertex
	for(unsigned int i = 0; i < 4; i++)
	{
		//Vector<3> up = unproject(cameraFirst.UnProject(marker_corners[i]));
		//image_corners[i] = cameraSecond.Project( project( m3BestHomography * up ) );
		Vector<3> up = unproject(marker_corners[i]);
		image_corners[i] = project( m3BestHomography * up );
		marker_info.vertex[i][0] = image_corners[i][0];
		marker_info.vertex[i][1] = image_corners[i][1];
	}

	// line
	for(unsigned int i = 0; i < 4; i++)
	{
		Vector<3> p1 = unproject(image_corners[i]);
		Vector<3> p2 = unproject(image_corners[(i+1)%4]);
		//Vector<3> p1 = unproject(cameraSecond.UnProject(image_corners[i]));
		//Vector<3> p2 = unproject(cameraSecond.UnProject(image_corners[(i+1)%4]));
		//image_edge_centers[i] = (p1 + p2)/2;
		Vector<3> l = p1 ^ p2;
		marker_info.line[i][0] = l[0];
		marker_info.line[i][1] = l[1];
		marker_info.line[i][2] = l[2];
	}

	/*
	// pos(center)
	Vector<3> cl1 = (image_edge_centers[0] ^ image_edge_centers[2]);
	Vector<3> cl2 = (image_edge_centers[1] ^ image_edge_centers[3]);
	Vector<2> query_center = cameraSecond.Project( project(cl1 ^ cl2) );
	marker_info.pos[0] = query_center[0];
	marker_info.pos[1] = query_center[1];
	*/
}

void CameraPoseARToolKit::RefineCameraPoseWithInliers(ATANCamera &cameraFirst, ATANCamera &cameraSecond,
	std::vector<HomographyMatch> &vMatchesInliers,
	SE3<> &se3SecondFromFirst, double prior)
{
	WLS<6> wls;
	wls.add_prior(prior);

	vector<Matrix<2,6> > vmJacobians;
	vector<Vector<2> > vvErrors;
	vector<double> vdErrorSquared;

	Matrix<2> m2PixelProjectionJac = cameraSecond.GetProjectionDerivs();

	for(unsigned int i=0; i<vMatchesInliers.size(); i++)
	{
		// First, find error.
		Vector<2> v2First = vMatchesInliers[i].v2CamPlaneFirst;
		Vector<3> v3Second = se3SecondFromFirst * unproject(v2First);
		Vector<2> v2Second = cameraSecond.Project( project(v3Second) );
		Vector<2> v2Second_real = vMatchesInliers[i].v2CamPlaneSecond;
		Vector<2> v2Error = (v2Second_real - v2Second);

		vdErrorSquared.push_back(v2Error* v2Error);
		vvErrors.push_back(v2Error);

		Matrix<2,6> m26Jacobian;

		double dOneOverCameraZ = 1.0 / v3Second[2];
		for(int m=0; m<6; m++)
		{
			const Vector<4> v4Motion = SE3<>::generator_field(m, unproject(v3Second));
			Vector<2> v2CamFrameMotion;
			v2CamFrameMotion[0] = (v4Motion[0] - v3Second[0] * v4Motion[2] * dOneOverCameraZ) * dOneOverCameraZ;
			v2CamFrameMotion[1] = (v4Motion[1] - v3Second[1] * v4Motion[2] * dOneOverCameraZ) * dOneOverCameraZ;
			m26Jacobian.T()[m] = m2PixelProjectionJac * v2CamFrameMotion;
		};

		vmJacobians.push_back(m26Jacobian);
	}

	// Calculate robust sigma:
	vector<double> vdd = vdErrorSquared;
	double dSigmaSquared = Tukey::FindSigmaSquared(vdd);

	// Add re-weighted measurements to WLS:
	for(unsigned int i=0; i<vMatchesInliers.size(); i++)
	{
		double dWeight = Tukey::Weight(vdErrorSquared[i], dSigmaSquared);
		wls.add_mJ(vvErrors[i][0], vmJacobians[i][0], dWeight);
		wls.add_mJ(vvErrors[i][1], vmJacobians[i][1], dWeight);
	}
	wls.compute();
	Vector<6> v6Update = wls.get_mu();
	se3SecondFromFirst = SE3<>::exp(v6Update) * se3SecondFromFirst;
}
