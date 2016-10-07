// -*- c++ -*- 
// Copyright 2008 Isis Innovation Limited

// CameraPose.h 
// Implementation according to Faugeras and Lustman

#ifndef __CAMERA_POSE_H
#define __CAMERA_POSE_H
#include "../3rdparty/TooN/TooN.h"
using namespace TooN;
#include "../3rdparty/TooN/se3.h"
#include <vector>
#include "../3rdparty/PTAM/ATANCamera.h"
#include "Homography.h"


// Storage for each homography decomposition
struct HomographyDecomposition
{
  Vector<3> v3Tp;
  Matrix<3> m3Rp;
  double d;
  Vector<3> v3n;
  
  // The resolved composition..
  SE3<> se3SecondFromFirst;
  SE3<> se3Test;
  int nScore;
};

class CameraPose
{
public:
	// Jul. 2011, Min-Hyuk Sung
	// X2 = RX1 + t
	virtual bool Compute(ATANCamera &cameraFirst, ATANCamera &cameraSecond,
		std::vector<HomographyMatch> &vMatchesInliers,
		Matrix<3> &m3BestHomography,
		SE3<> &se3SecondFromFirst);

	//double PlaneFittingError(Matrix<3> &m3BestHomography, HomographyDecomposition &decomposition);

private:
  void DecomposeHomography(Matrix<3> &m3BestHomography);
  void ChooseBestDecomposition(ATANCamera &cameraFirst,
	  std::vector<HomographyMatch> &vMatchesInliers,
	  Matrix<3> &m3BestHomography, SE3<> &se3SecondFromFirst);

  // Mar. 2011, Min-Hyuk Sung
  void CalcPlaneAligner(ATANCamera &camera, Matrix<3> &m3BestHomography,
	  HomographyDecomposition &decomposition);
  
  std::vector<HomographyDecomposition> mvDecompositions;
};

#endif
