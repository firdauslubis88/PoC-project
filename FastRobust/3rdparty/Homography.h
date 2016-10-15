// -*- c++ -*- 
// Copyright 2008 Isis Innovation Limited

// Homography.h 
// Implementation according to Faugeras and Lustman

#ifndef __HOMOGRAPHY_H
#define __HOMOGRAPHY_H
#include "TooN/TooN.h"
using namespace TooN;
#include "TooN/se3.h"
#include <vector>
#include "cmphomo.h"

// If < 0, PROSAC is not used
#define PROSAC_INIT_NUM			-1
#define MAX_NUM_ITER			300
#define MAX_PIXEL_ERROR			5.0
#define NUM_REFINE_HOMOGRAPHY	10


// Homography matches are 2D-2D matches in a stereo pair, unprojected
// to the Z=1 plane.
struct HomographyMatch
{
	// To be filled in by MapMaker:
	Vector<2> v2CamPlaneFirst;
	Vector<2> v2CamPlaneSecond;
	//Matrix<2> m2PixelProjectionJac;
	// Jul. 2011, Min-Hyuk Sung
	int nScore;	// The lower the better

	// Dec. 2011, Min-Hyuk Sung
//#if DB_FILE_TYPE == 100
	int binIdx;
//#endif
};

class Homography
{
public:
	// y = Hx
	bool Compute(Matrix<3> &m3BestHomography,
		unsigned int &num_matches, double *x_pos_list, double *y_pos_list, int *bin_idx_list);

	//Matrix<2> m2PixelProjectionJac;

private:
  void BestHomographyFromMatches_MLESAC(Matrix<3> &m3BestHomography,
	  unsigned int num_matches, double *x_pos_list, double *y_pos_list,
	  bool &bIsChanged);

  void RefineHomographyWithInliers(Matrix<3> &m3BestHomography,
	  unsigned int num_matches, double *x_pos_list, double *y_pos_list);
  
  void GenerateInlierSet(Matrix<3> m3Homography,
	  unsigned int &num_matches, double *x_pos_list, double *y_pos_list, int *bin_idx_list);

  double MLESACScore(Matrix<3> m3Homography,
	  unsigned int num_matches, double *x_pos_list, double *y_pos_list);
  
  double mdMaxPixelErrorSquared;
};

#endif
