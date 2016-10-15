// Copyright 2008 Isis Innovation Limited
#include "CameraPose.h"
#include "PTAM/SmallMatrixOpts.h"
#include <TooN/se3.h>
#include <TooN/SVD.h>
#include <TooN/wls.h>
#include "MEstimator.h"

using namespace std;
bool CameraPose::Compute(ATANCamera &cameraFirst, ATANCamera &cameraSecond,
							 vector<HomographyMatch> &vMatchesInliers,
							 Matrix<3> &m3BestHomography,
							 SE3<> &se3SecondFromFirst)
{
  // Decompose the best homography into a set of possible decompositions
  DecomposeHomography(m3BestHomography);

  // At this stage should have eight decomposition options, if all went according to plan
  if(mvDecompositions.size() != 8)
    return false;
  
  // And choose the best one based on visibility constraints
  ChooseBestDecomposition(cameraFirst, vMatchesInliers, m3BestHomography, se3SecondFromFirst);
  se3SecondFromFirst = mvDecompositions[0].se3SecondFromFirst;

  return true;
}

void CameraPose::DecomposeHomography(Matrix<3> &m3BestHomography)
{
  mvDecompositions.clear();
  SVD<3> svd(m3BestHomography);
  Vector<3> v3Diag = svd.get_diagonal();
  double d1 = fabs(v3Diag[0]); // The paper suggests the square of these (e.g. the evalues of AAT)
  double d2 = fabs(v3Diag[1]); // should be used, but this is wrong. c.f. Faugeras' book.
  double d3 = fabs(v3Diag[2]);
  
  Matrix<3> U = svd.get_U();
  Matrix<3> V = svd.get_VT().T();
  
  double s = M3Det(U) * M3Det(V);
  
  double dPrime_PM = d2;
  
  int nCase;
  if(d1 != d2 && d2 != d3)
    nCase = 1;
  else if( d1 == d2 && d2 == d3)
    nCase = 3;
  else
    nCase = 2;
  
  if(nCase != 1)
    {
      cout << "  CameraPosition: This motion case is not implemented or is degenerate. Try again. " << endl;
      return;
    }
  
  double x1_PM;
  double x2;
  double x3_PM;

  // All below deals with the case = 1 case.
  // Case 1 implies (d1 != d3) 
  { // Eq. 12
    x1_PM = sqrt((d1*d1 - d2*d2) / (d1*d1 - d3*d3));
    x2    = 0;
    x3_PM = sqrt((d2*d2 - d3*d3) / (d1*d1 - d3*d3));
  };
  
  double e1[4] = {1.0,-1.0,1.0,-1.0};
  double e3[4] = {1.0, 1.0, -1.0,-1.0};
    
  Vector<3> v3np;
  HomographyDecomposition decomposition;

  // Case 1, d' > 0:
  decomposition.d = s * dPrime_PM;
  for(int signs=0; signs<4; signs++)
    {
      // Eq 13
      decomposition.m3Rp = Identity;
      double dSinTheta = (d1 - d3) * x1_PM * x3_PM * e1[signs] * e3[signs] / d2;
      double dCosTheta = (d1 * x3_PM * x3_PM + d3 * x1_PM * x1_PM) / d2;
      decomposition.m3Rp[0][0] = dCosTheta;                
      decomposition.m3Rp[0][2] = -dSinTheta;
      decomposition.m3Rp[2][0] = dSinTheta;                
      decomposition.m3Rp[2][2] = dCosTheta;
      
      // Eq 14
      decomposition.v3Tp[0] = (d1 - d3) * x1_PM * e1[signs];
      decomposition.v3Tp[1] = 0.0;
      decomposition.v3Tp[2] = (d1 - d3) * -x3_PM * e3[signs];
  
      v3np[0] = x1_PM * e1[signs];
      v3np[1] = x2;
      v3np[2] = x3_PM * e3[signs];
      decomposition.v3n = V * v3np;
      
      mvDecompositions.push_back(decomposition);
    }
  // Case 1, d' < 0:
  decomposition.d = s * -dPrime_PM;
  for(int signs=0; signs<4; signs++)
    { 
      // Eq 15
      decomposition.m3Rp = -1 * Identity;
      double dSinPhi = (d1 + d3) * x1_PM * x3_PM * e1[signs] * e3[signs] / d2;
      double dCosPhi = (d3 * x1_PM * x1_PM - d1 * x3_PM * x3_PM) / d2;
      decomposition.m3Rp[0][0] = dCosPhi;                
      decomposition.m3Rp[0][2] = dSinPhi;
      decomposition.m3Rp[2][0] = dSinPhi;                
      decomposition.m3Rp[2][2] = -dCosPhi;
      
      // Eq 16
      decomposition.v3Tp[0] = (d1 + d3) * x1_PM * e1[signs];
      decomposition.v3Tp[1] = 0.0;
      decomposition.v3Tp[2] = (d1 + d3) * x3_PM * e3[signs];

      v3np[0] = x1_PM * e1[signs];
      v3np[1] = x2;
      v3np[2] = x3_PM * e3[signs];
      decomposition.v3n = V * v3np;
      
      mvDecompositions.push_back(decomposition);
    }
  
  // While we have the SVD results calculated here, store the decomposition R and t results as well..
  for(unsigned int i=0; i<mvDecompositions.size(); i++)
    {
      mvDecompositions[i].se3SecondFromFirst.get_rotation() = 
	s * U * mvDecompositions[i].m3Rp * V.T();
      mvDecompositions[i].se3SecondFromFirst.get_translation() = 
	U * mvDecompositions[i].v3Tp;
    }
}

bool operator<(const HomographyDecomposition lhs, const HomographyDecomposition rhs)
{
  return lhs.nScore < rhs.nScore;
}

static double SampsonusError(Vector<2> &v2Dash, const Matrix<3> &m3Essential, Vector<2> &v2)
{
  Vector<3> v3Dash = unproject(v2Dash);
  Vector<3> v3 = unproject(v2);  
  
  double dError = v3Dash * m3Essential * v3;
  
  Vector<3> fv3 = m3Essential * v3;
  Vector<3> fTv3Dash = m3Essential.T() * v3Dash;
  
  Vector<2> fv3Slice = fv3.slice<0,2>();
  Vector<2> fTv3DashSlice = fTv3Dash.slice<0,2>();
  
  return (dError * dError / (fv3Slice * fv3Slice + fTv3DashSlice * fTv3DashSlice));
}


void CameraPose::ChooseBestDecomposition(ATANCamera &cameraFirst,
	std::vector<HomographyMatch> &vMatchesInliers,
	Matrix<3> &m3BestHomography, SE3<> &se3SecondFromFirst)
{
  assert(mvDecompositions.size() == 8);
  for(unsigned int i=0; i<mvDecompositions.size(); i++)
    {
      HomographyDecomposition &decom = mvDecompositions[i];
      int nPositive = 0;
      for(unsigned int m=0; m<vMatchesInliers.size(); m++)
	{
	  Vector<2> &v2 = vMatchesInliers[m].v2CamPlaneFirst;
	  double dVisibilityTest = (m3BestHomography[2][0] * v2[0] + m3BestHomography[2][1] * v2[1] + m3BestHomography[2][2]) / decom.d;
	  if(dVisibilityTest > 0.0)
	    nPositive++;
	};
      decom.nScore = -nPositive;
    }
  
  sort(mvDecompositions.begin(), mvDecompositions.end());
  mvDecompositions.resize(4);
  
  for(unsigned int i=0; i<mvDecompositions.size(); i++)
    {
      HomographyDecomposition &decom = mvDecompositions[i];
      int nPositive = 0;
      for(unsigned int m=0; m<vMatchesInliers.size(); m++)
	{
	  Vector<3> v3 = unproject(vMatchesInliers[m].v2CamPlaneFirst);
	  double dVisibilityTest = v3 * decom.v3n / decom.d;
	  if(dVisibilityTest > 0.0)
	    nPositive++;
	};
      decom.nScore = -nPositive;
    }
  
  sort(mvDecompositions.begin(), mvDecompositions.end());
  mvDecompositions.resize(2);

  // Jul. 2011, Min-Hyuk Sung
  // ---- //
  // Angle(dot product) with z-axis
  double score_0 = abs(mvDecompositions[0].v3n[2]);
  double score_1 = abs(mvDecompositions[1].v3n[2]);

  if(abs(score_0 - score_1) < 0.1)
  {
	  CalcPlaneAligner(cameraFirst, m3BestHomography, mvDecompositions[0]);
	  CalcPlaneAligner(cameraFirst, m3BestHomography, mvDecompositions[1]);

	  Vector<3> z_axis_0 = mvDecompositions[0].se3SecondFromFirst.get_rotation().get_matrix()[2];
	  Vector<3> z_axis_1 = mvDecompositions[1].se3SecondFromFirst.get_rotation().get_matrix()[2];

	  Vector<3> z_axis_prev = se3SecondFromFirst.get_rotation().get_matrix()[2];

	  double prev_diff_0 = abs(z_axis_0 * z_axis_prev);
	  double prev_diff_1 = abs(z_axis_1 * z_axis_prev);

	  if(prev_diff_0 > prev_diff_1)
		  mvDecompositions.erase(mvDecompositions.begin() + 1);
	  else
		  mvDecompositions.erase(mvDecompositions.begin());
  }
  else
  {
	  if(score_0 > score_1)
		  mvDecompositions.erase(mvDecompositions.begin() + 1);
	  else
		  mvDecompositions.erase(mvDecompositions.begin());

	  // X2 = RX1 + t
	  // Find the transform from plane M to camera C1
	  CalcPlaneAligner(cameraFirst, m3BestHomography, mvDecompositions[0]);
  }
  
  // According to Faugeras and Lustman, ambiguity exists if the two scores are equal
  // but in practive, better to look at the ratio!
  //double dRatio = (double) mvDecompositions[1].nScore / (double) mvDecompositions[0].nScore;

  //if(dRatio < 0.9) // no ambiguity!
  //  mvDecompositions.erase(mvDecompositions.begin() + 1);

  // Jan. 2010, Min-Hyuk Sung
  /*
  if(!mvIsFirstHomography)
  {
	  const double PI = 3.14159265;
	  SO3<> current_0(mvDecompositions[0].se3SecondFromFirst.get_rotation().get_matrix());
	  SO3<> current_1(mvDecompositions[1].se3SecondFromFirst.get_rotation().get_matrix());
	  SO3<> previous(mvPrevBestDecomposition.se3SecondFromFirst.get_rotation().get_matrix());
	  Vector<3> ln_0 = (current_0 * previous.inverse()).ln();
	  Vector<3> ln_1 = (current_1 * previous.inverse()).ln();
	  double angle_0 = sqrt(ln_0[0] * ln_0[0] + ln_0[1] * ln_0[1] + ln_0[2] * ln_0[2]);
	  double angle_1 = sqrt(ln_1[0] * ln_1[0] + ln_1[1] * ln_1[1] + ln_1[2] * ln_1[2]);
	  assert(angle_0 >= 0 && angle_0 <= 2 * PI);
	  assert(angle_1 >= 0 && angle_1 <= 2 * PI);
	  if(angle_0 > PI/2)	angle_0 = 2*PI - angle_0;
	  if(angle_1 > PI/2)	angle_1 = 2*PI - angle_1;

	  if(abs(angle_0) < abs(angle_1))
		  mvDecompositions.erase(mvDecompositions.begin() + 1);
	  else
		  mvDecompositions.erase(mvDecompositions.begin());
  }
  else             // two-way ambiguity. Resolve by sampsonus score of all points.
    {
      double dErrorSquaredLimit  = mdMaxPixelErrorSquared * 4;
      double adSampsonusScores[2];
      for(int i=0; i<2; i++)
	{
	  SE3<> se3 = mvDecompositions[i].se3SecondFromFirst;
	  Matrix<3> m3Essential;
	  for(int j=0; j<3; j++)
	    m3Essential.T()[j] = se3.get_translation() ^ se3.get_rotation().get_matrix().T()[j];
	  
	  double dSumError = 0;
	  for(unsigned int m=0; m < mvMatches.size(); m++ )
	    {
	      double d = SampsonusError(mvMatches[m].v2CamPlaneSecond, m3Essential, mvMatches[m].v2CamPlaneFirst);
	      if(d > dErrorSquaredLimit)
		d = dErrorSquaredLimit;
	      dSumError += d;
	    }
	  
	  adSampsonusScores[i] = dSumError;
	}

      if(adSampsonusScores[0] <= adSampsonusScores[1])
	mvDecompositions.erase(mvDecompositions.begin() + 1);
      else
	mvDecompositions.erase(mvDecompositions.begin());
    }
  */
  
  // Jan. 2010, Min-Hyuk Sung
  //if(mvIsFirstHomography)	mvIsFirstHomography = false;
  //mvPrevBestDecomposition = mvDecompositions[0];
}

// Jan. 2010, Min-Hyuk Sung
//bool CameraPosition::mvIsFirstHomography = true;
//HomographyDecomposition CameraPosition::mvPrevBestDecomposition;

void CameraPose::CalcPlaneAligner(ATANCamera &camera,  Matrix<3> &m3BestHomography,
	HomographyDecomposition &decomposition)
{
	// The given decomposition contains a rigid transform matrix from View1 to View2
	// The transform from plane coordinates to View1 coordinates is defined by
	// 'decomposition.v3n' and 'decomposition.d'
	// Please refer to the paper written by Faugeras and Lustman in 1992

	if(decomposition.v3n[2] > 0)
	{
		decomposition.v3n = -decomposition.v3n;
		decomposition.d = -decomposition.d;
	}

	double width = camera.GetImageSize()[0];
	double height = camera.GetImageSize()[1];
	double axis_length = 0.5 * min(width, height);

	Vector<3> origin = unproject( camera.UnProject(makeVector<double>(width/2, height/2)) );
	Vector<3> x_axis = unproject( camera.UnProject(makeVector<double>(width/2 + axis_length, height/2)) );
	Vector<3> y_axis = unproject( camera.UnProject(makeVector<double>(width/2, height/2 + axis_length)) );
	Vector<3> z_axis;

	// For given 'n' and 'd', nX = d (Faugeras and Lustman, 1992)
	// X = [x, y, z]T = h * [x', y', z']T
	// [x', y', z'] is obtained from image coordinates
	// h = d / (n * [x', y', z']T)
	origin = origin * (decomposition.d / (decomposition.v3n * origin + 1.0E-12));
	x_axis = x_axis * (decomposition.d / (decomposition.v3n * x_axis + 1.0E-12)) - origin;
	y_axis = y_axis * (decomposition.d / (decomposition.v3n * y_axis + 1.0E-12)) - origin;
	z_axis = decomposition.v3n;

	normalize(x_axis);
	normalize(y_axis);
	normalize(z_axis);

	assert(z_axis * x_axis < 1.0E-6);
	assert(z_axis * y_axis < 1.0E-6);

	x_axis = x_axis - (z_axis * (x_axis * z_axis));
	normalize(x_axis);
	y_axis = z_axis ^ x_axis;

	Matrix<3> rotWorldToCamera, id = Identity;
	rotWorldToCamera[0] = x_axis;
	rotWorldToCamera[1] = y_axis;
	rotWorldToCamera[2] = z_axis;
	rotWorldToCamera = rotWorldToCamera.T();

	SE3<> se3Aligner;
	se3Aligner.get_rotation() = rotWorldToCamera;
	se3Aligner.get_translation() = origin;

	decomposition.se3SecondFromFirst = decomposition.se3SecondFromFirst * se3Aligner;


	//// -- Test -- //
	//Vector<2> corners[3];
	//Vector<3> axis_homography[3];

	//corners[0][0] = mCamera.GetImageSize()[0]/2;	corners[0][1] = mCamera.GetImageSize()[1]/2;
	//corners[1][0] = mCamera.GetImageSize()[0];	corners[1][1] = mCamera.GetImageSize()[1]/2;
	//corners[2][0] = mCamera.GetImageSize()[0]/2;	corners[2][1] = mCamera.GetImageSize()[1];

	//for(unsigned int i = 0; i < 3; i++)
	//	axis_homography[i] = mm3 * unproject(mCamera.UnProject(corners[i]));

	//Vector<3> x_axis_homography = unit(axis_homography[1] - axis_homography[0]);
	//Vector<3> y_axis_homography = unit(axis_homography[2] - axis_homography[0]);
	//Vector<3> z_axis_homography = x_axis_homography ^ y_axis_homography;

	//double score = abs( z_axis_aligned * z_axis_homography );
	//// ---- //
}

/*
double CameraPose::PlaneFittingError(Matrix<3> &m3BestHomography, HomographyDecomposition &decomposition)
{
	double error = 0;

	Vector<2> src_corners[4];
	src_corners[0][0] = 0;							src_corners[0][1] = 0;
	src_corners[1][0] = mCamera.GetImageSize()[0];	src_corners[1][1] = 0;
	src_corners[2][0] = mCamera.GetImageSize()[0];	src_corners[2][1] = mCamera.GetImageSize()[1];
	src_corners[3][0] = 0;							src_corners[3][1] = mCamera.GetImageSize()[1];

	for(unsigned int i = 0; i < 4; i++)
	{
		//Vector<3> p = unproject(mCamera.UnProject(src_corners[i]));
		//Vector<3> v = unproject(project(m3BestHomography * p));

		//Matrix<3> a = v.as_col() * v.as_row();
		//double    b = v * v;
		//assert(abs(b) >= 1.0e-4);
		//a = -(a / b);
		//for(int i = 0; i < 3; i++)	a[i][i] += 1;	// For identity matrix

		//Vector<3> c = se3SecondFromFirst * p;
		//c = a * c;
		//error += c * c;

		Vector<3> p = unproject(mCamera.UnProject(src_corners[i]));
		Vector<2> v = project(m3BestHomography * p);

		p = p * (decomposition.d / (decomposition.v3n * p + 1.0E-12));
		Vector<2> c = project(decomposition.se3SecondFromFirst * p);

		error += (v - c) * (v - c);
	}
	return error;
}
*/