// -*- c++ -*- 
// CameraPoseARToolKit.h 
// Implementation according to ARToolKit

#ifndef __CAMERA_POSE_ARTOOLKIT_H
#define __CAMERA_POSE_ARTOOLKIT_H

#include <AR/ar.h>
#include "CameraPose.h"
#include "ATANCamera.h"

#define NUM_REFINE_CAMERA_POSE	100


class CameraPoseARToolKit : public CameraPose
{
public:
	// Jul. 2011, Min-Hyuk Sung
	// X2 = RX1 + t
	virtual bool Compute(ATANCamera &cameraFirst, ATANCamera &cameraSecond,
		std::vector<HomographyMatch> &vMatchesInliers,
		Matrix<3> &m3BestHomography,
		SE3<> &se3SecondFromFirst);

	void RefineCameraPoseWithInliers(ATANCamera &cameraFirst, ATANCamera &cameraSecond,
		std::vector<HomographyMatch> &vMatchesInliers,
		SE3<> &se3SecondFromFirst, double prior = 1.0);

private:
	void ParamInitialize(ATANCamera &camera);

	void GetMarkerInfo(ATANCamera &cameraFirst, ATANCamera &cameraSecond,
		Matrix<3> &m3BestHomography, ARMarkerInfo &marker_info);
};

#endif
