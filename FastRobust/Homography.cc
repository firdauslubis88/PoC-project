// Copyright 2008 Isis Innovation Limited
#include "Homography.h"
#include <TooN/wls.h>
#include "MEstimator.h"

using namespace std;
void Homography::GenerateInlierSet(Matrix<3> m3Homography,
	unsigned int &num_matches, double *x_pos_list, double *y_pos_list, int *bin_idx_list)
{
	double *pixel_error_sq_list	= new double[num_matches];
	double *threshold_list		= new double[num_matches];

	for(unsigned int i = 0; i < num_matches; i++)
	{
		// ---- //
		const double *x_pos = x_pos_list+(2*i);
		const double *y_pos = y_pos_list+(2*i);


		double y_pos_est[3];
		y_pos_est[0] = m3Homography[0][2];
		y_pos_est[1] = m3Homography[1][2];
		y_pos_est[2] = m3Homography[2][2];

		y_pos_est[0] += m3Homography[0][0] * x_pos[0];
		y_pos_est[1] += m3Homography[1][0] * x_pos[0];
		y_pos_est[2] += m3Homography[2][0] * x_pos[0];

		y_pos_est[0] += m3Homography[0][1] * x_pos[1];
		y_pos_est[1] += m3Homography[1][1] * x_pos[1];
		y_pos_est[2] += m3Homography[2][1] * x_pos[1];


		double error[2], pixel_error[2];
		/*
		error[0] = (y_pos[0] * y_pos_est[2]) - y_pos_est[0];
		error[1] = (y_pos[1] * y_pos_est[2]) - y_pos_est[1];

		pixel_error[0] = m2PixelProjectionJac[0][0] * error[0];
		pixel_error[0] += m2PixelProjectionJac[0][1] * error[1];

		pixel_error[1] = m2PixelProjectionJac[1][0] * error[0];
		pixel_error[1] += m2PixelProjectionJac[1][1] * error[1];
		*/
		pixel_error[0] = (y_pos[0] * y_pos_est[2]) - y_pos_est[0];
		pixel_error[1] = (y_pos[1] * y_pos_est[2]) - y_pos_est[1];


		double pixel_error_sq = (pixel_error[0] * pixel_error[0]);
		pixel_error_sq += (pixel_error[1] * pixel_error[1]);

		double scale_sq = (y_pos_est[2] * y_pos_est[2]);
		// ---- //

		pixel_error_sq_list[i] = pixel_error_sq;
		threshold_list[i] = mdMaxPixelErrorSquared * scale_sq;

	}

	unsigned int num_inlier_matches = 0;

	for(unsigned int i = 0; i < num_matches; i++)
	{
		bool inlier = true;
		if(pixel_error_sq_list[i] > threshold_list[i])
			inlier = false;

		if(inlier)
		{
			assert(num_inlier_matches <= i);
			memcpy(x_pos_list+(2*num_inlier_matches), x_pos_list+(2*i), 2*sizeof(double));
			memcpy(y_pos_list+(2*num_inlier_matches), y_pos_list+(2*i), 2*sizeof(double));
			// Dec. 2011, Min-Hyuk Sung
			//
			memcpy(bin_idx_list+(num_inlier_matches), bin_idx_list+(i), sizeof(int));
			//
			num_inlier_matches++;
		}
	}

	// Update
	num_matches = num_inlier_matches;

	delete [] pixel_error_sq_list;
	delete [] threshold_list;
}

double Homography::MLESACScore(Matrix<3> m3Homography,
	unsigned int num_matches, double *x_pos_list, double *y_pos_list)
{
	double score = 0.0;
	double *pixel_error_sq_list	= new double[num_matches];
	double *scale_sq_list		= new double[num_matches];

	for(unsigned int i = 0; i < num_matches; i++)
	{
		// ---- //
		const double *x_pos = x_pos_list+(2*i);
		const double *y_pos = y_pos_list+(2*i);


		double y_pos_est[3];
		y_pos_est[0] = m3Homography[0][2];
		y_pos_est[1] = m3Homography[1][2];
		y_pos_est[2] = m3Homography[2][2];

		y_pos_est[0] += m3Homography[0][0] * x_pos[0];
		y_pos_est[1] += m3Homography[1][0] * x_pos[0];
		y_pos_est[2] += m3Homography[2][0] * x_pos[0];

		y_pos_est[0] += m3Homography[0][1] * x_pos[1];
		y_pos_est[1] += m3Homography[1][1] * x_pos[1];
		y_pos_est[2] += m3Homography[2][1] * x_pos[1];


		double error[2], pixel_error[2];
		/*
		error[0] = (y_pos[0] * y_pos_est[2]) - y_pos_est[0];
		error[1] = (y_pos[1] * y_pos_est[2]) - y_pos_est[1];

		pixel_error[0] = m2PixelProjectionJac[0][0] * error[0];
		pixel_error[0] += m2PixelProjectionJac[0][1] * error[1];

		pixel_error[1] = m2PixelProjectionJac[1][0] * error[0];
		pixel_error[1] += m2PixelProjectionJac[1][1] * error[1];
		*/
		pixel_error[0] = (y_pos[0] * y_pos_est[2]) - y_pos_est[0];
		pixel_error[1] = (y_pos[1] * y_pos_est[2]) - y_pos_est[1];


		double pixel_error_sq = (pixel_error[0] * pixel_error[0]);
		pixel_error_sq += (pixel_error[1] * pixel_error[1]);

		double scale_sq = (y_pos_est[2] * y_pos_est[2]);
		// ---- //

		pixel_error_sq_list[i] = pixel_error_sq;
		scale_sq_list[i] = scale_sq;
	}

	for(unsigned int i = 0; i < num_matches; i++)
	{
		if(abs(scale_sq_list[i] < 1.0E-12))	score += mdMaxPixelErrorSquared;
		double dSquaredError = pixel_error_sq_list[i] / scale_sq_list[i];

		if(dSquaredError > mdMaxPixelErrorSquared)
			score += mdMaxPixelErrorSquared;
		else 
			score += dSquaredError;
	}

	delete[] pixel_error_sq_list;
	delete[] scale_sq_list;
	return score;
}

bool Homography::Compute(Matrix<3> &m3BestHomography,
	unsigned int &num_matches, double *x_pos_list, double *y_pos_list, int *bin_idx_list)
{
	mdMaxPixelErrorSquared = MAX_PIXEL_ERROR * MAX_PIXEL_ERROR;

	// Jul. 2011, Min-Hyuk Sung
#if PROSAC_INIT_NUM >= 0
	// Sort
#endif

	// Jul. 2011, Min-Hyuk Sung
	bool isChanged = true;

	// Find best homography from minimal sets of image matches
	BestHomographyFromMatches_MLESAC(m3BestHomography,
		num_matches, x_pos_list, y_pos_list, isChanged);

	// Generate the inlier set, and refine the best estimate using this
	GenerateInlierSet(m3BestHomography, num_matches, x_pos_list, y_pos_list, bin_idx_list);

	// Jul. 2011, Min-Hyuk Sung
	// Refine homography only when it is changed
	if(isChanged)
	{
		for(int iteration = 0; iteration < NUM_REFINE_HOMOGRAPHY; iteration++)
			RefineHomographyWithInliers(m3BestHomography,
				num_matches, x_pos_list, y_pos_list);
	}

	return true;
}

// Throughout the whole thing,
// SecondView = Homography * FirstView

/*
void Homography::RefineHomographyWithInliers(Matrix<3> &m3BestHomography,
	unsigned int num_matches, double *x_pos_list, double *y_pos_list)
{
	WLS<9> wls;
	wls.add_prior(1.0);

	vector<Matrix<2,9> > vmJacobians;
	vector<Vector<2> > vvErrors;
	vector<double> vdErrorSquared;

	for(unsigned int i=0; i<num_matches; i++)
	{
		// First, find error.
		Vector<2> v2First = makeVector(x_pos_list[2*i+0], x_pos_list[2*i+1]);
		Vector<2> v2Second_real = makeVector(y_pos_list[2*i+0], y_pos_list[2*i+1]);

		Vector<3> v3Second = m3BestHomography * unproject(v2First);
		Vector<2> v2Second = project(v3Second);
		
		//Jul. 2011, Min-Hyuk Sung
		//Vector<2> v2Error = vMatchesInliers[i].m2PixelProjectionJac * (v2Second_real - v2Second);
		Vector<2> v2Error = m2PixelProjectionJac * (v2Second_real - v2Second);

		vdErrorSquared.push_back(v2Error* v2Error);
		vvErrors.push_back(v2Error);

		Matrix<2,9> m29Jacobian;
		double dDenominator = v3Second[2];

		// Jacobians wrt to the elements of the homography:
		// For x:
		m29Jacobian[0].slice<0,3>() = unproject(v2First) / dDenominator;
		m29Jacobian[0].slice<3,3>() = Zeros;
		double dNumerator = v3Second[0];
		m29Jacobian[0].slice<6,3>() = -unproject(v2First) * dNumerator / (dDenominator * dDenominator);
		// For y:
		m29Jacobian[1].slice<0,3>() = Zeros;
		m29Jacobian[1].slice<3,3>()  = unproject(v2First) / dDenominator;;
		dNumerator = v3Second[1];
		m29Jacobian[1].slice<6,3>() = -unproject(v2First) * dNumerator / (dDenominator * dDenominator);

		//Jul. 2011, Min-Hyuk Sung
		//vmJacobians.push_back(vMatchesInliers[i].m2PixelProjectionJac * m29Jacobian);
		vmJacobians.push_back(m2PixelProjectionJac * m29Jacobian);
	}

	// Calculate robust sigma:
	vector<double> vdd = vdErrorSquared;
	double dSigmaSquared = Tukey::FindSigmaSquared(vdd);

	// Add re-weighted measurements to WLS:
	for(unsigned int i=0; i<num_matches; i++)
	{
		double dWeight = Tukey::Weight(vdErrorSquared[i], dSigmaSquared);
		wls.add_mJ(vvErrors[i][0], vmJacobians[i][0], dWeight);
		wls.add_mJ(vvErrors[i][1], vmJacobians[i][1], dWeight);
	}
	wls.compute();
	Vector<9> v9Update = wls.get_mu();
	Matrix<3> m3Update;
	m3Update[0] = v9Update.slice<0,3>();
	m3Update[1] = v9Update.slice<3,3>();
	m3Update[2] = v9Update.slice<6,3>();
	m3BestHomography += m3Update;
}
*/

void Homography::RefineHomographyWithInliers(Matrix<3> &m3BestHomography,
	unsigned int num_matches, double *x_pos_list, double *y_pos_list)
{
	WLS<9> wls;
	wls.add_prior(1.0);

	vector<Matrix<2,9> > vmJacobians;

	double *y_pos_est_list		= new double[2*num_matches];
	double *pixel_error_vec_list= new double[2*num_matches];
	double *pixel_error_sq_list	= new double[num_matches];
	double *scale_list			= new double[num_matches];
	double *scale_sq_list		= new double[num_matches];
	
	for(unsigned int i=0; i<num_matches; i++)
	{
		// ---- //
		const double *x_pos = x_pos_list+(2*i);
		const double *y_pos = y_pos_list+(2*i);


		double y_pos_est[3];
		y_pos_est[0] = m3BestHomography[0][2];
		y_pos_est[1] = m3BestHomography[1][2];
		y_pos_est[2] = m3BestHomography[2][2];

		y_pos_est[0] += m3BestHomography[0][0] * x_pos[0];
		y_pos_est[1] += m3BestHomography[1][0] * x_pos[0];
		y_pos_est[2] += m3BestHomography[2][0] * x_pos[0];

		y_pos_est[0] += m3BestHomography[0][1] * x_pos[1];
		y_pos_est[1] += m3BestHomography[1][1] * x_pos[1];
		y_pos_est[2] += m3BestHomography[2][1] * x_pos[1];


		double error[2], pixel_error[2];
		/*
		error[0] = (y_pos[0] * y_pos_est[2]) - y_pos_est[0];
		error[1] = (y_pos[1] * y_pos_est[2]) - y_pos_est[1];

		pixel_error[0] = m2PixelProjectionJac[0][0] * error[0];
		pixel_error[0] += m2PixelProjectionJac[0][1] * error[1];

		pixel_error[1] = m2PixelProjectionJac[1][0] * error[0];
		pixel_error[1] += m2PixelProjectionJac[1][1] * error[1];
		*/
		pixel_error[0] = (y_pos[0] * y_pos_est[2]) - y_pos_est[0];
		pixel_error[1] = (y_pos[1] * y_pos_est[2]) - y_pos_est[1];


		double pixel_error_sq = (pixel_error[0] * pixel_error[0]);
		pixel_error_sq += (pixel_error[1] * pixel_error[1]);

		double scale_sq = (y_pos_est[2] * y_pos_est[2]);
		// ---- //

		memcpy(y_pos_est_list+(2*i), y_pos_est, 2*sizeof(double));
		memcpy(pixel_error_vec_list+(2*i), pixel_error, 2*sizeof(double));	// Need to be normalized with y_pos_est[2]
		pixel_error_sq_list[i] = pixel_error_sq;								// Need to be normalized with scale_sq
		scale_list[i] = y_pos_est[2];
		scale_sq_list[i] = scale_sq;
	}

	for(unsigned int i=0; i<num_matches; i++)
	{
		Vector<2> v2First = makeVector(x_pos_list[2*i+0], x_pos_list[2*i+1]);
		const double *v3Second = y_pos_est_list+(2*i);

		Matrix<2,9> m29Jacobian;
		double dDenominator = scale_list[i];

		// Jacobians wrt to the elements of the homography:
		// For x:
		m29Jacobian[0].slice<0,3>() = unproject(v2First) / dDenominator;
		m29Jacobian[0].slice<3,3>() = Zeros;
		double dNumerator = v3Second[0];
		m29Jacobian[0].slice<6,3>() = -unproject(v2First) * dNumerator / (dDenominator * dDenominator);
		// For y:
		m29Jacobian[1].slice<0,3>() = Zeros;
		m29Jacobian[1].slice<3,3>()  = unproject(v2First) / dDenominator;
		dNumerator = v3Second[1];
		m29Jacobian[1].slice<6,3>() = -unproject(v2First) * dNumerator / (dDenominator * dDenominator);

		//Jul. 2011, Min-Hyuk Sung
		//vmJacobians.push_back(vMatchesInliers[i].m2PixelProjectionJac * m29Jacobian);
		//vmJacobians.push_back(m2PixelProjectionJac * m29Jacobian);
		vmJacobians.push_back(m29Jacobian);
	}

	// Calculate robust sigma:
	vector<double> vdd(num_matches);
	for(unsigned int i=0; i<num_matches; i++)
	{
		double dErrorSquared;
		//if(abs(scale_sq_list[i] < 1.0E-12))	dErrorSquared = mdMaxPixelErrorSquared;
		dErrorSquared = pixel_error_sq_list[i] / scale_sq_list[i];
		vdd[i] = dErrorSquared;
	}

	double dSigmaSquared = Tukey::FindSigmaSquared(vdd);

	// Add re-weighted measurements to WLS:
	for(unsigned int i=0; i<num_matches; i++)
	{
		Vector<2> vvError = makeVector(pixel_error_vec_list[2*i+0], pixel_error_vec_list[2*i+1]);
		//if(abs(scale) < 1.0E-12)...
		vvError /= scale_list[i];

		double dErrorSquared;
		//if(abs(scale_sq_list[i] < 1.0E-12))	dErrorSquared = mdMaxPixelErrorSquared;
		dErrorSquared = pixel_error_sq_list[i] / scale_sq_list[i];

		double dWeight = Tukey::Weight(dErrorSquared, dSigmaSquared);
		wls.add_mJ(vvError[0], vmJacobians[i][0], dWeight);
		wls.add_mJ(vvError[1], vmJacobians[i][1], dWeight);
	}

	wls.compute();
	Vector<9> v9Update = wls.get_mu();
	Matrix<3> m3Update;
	m3Update[0] = v9Update.slice<0,3>();
	m3Update[1] = v9Update.slice<3,3>();
	m3Update[2] = v9Update.slice<6,3>();
	m3BestHomography += m3Update;

	delete [] y_pos_est_list;
	delete [] pixel_error_vec_list;
	delete [] pixel_error_sq_list;
	delete [] scale_list;
	delete [] scale_sq_list;
}

void Homography::BestHomographyFromMatches_MLESAC(Matrix<3> &m3BestHomography,
	unsigned int num_matches, double *x_pos_list, double *y_pos_list,
	bool &bIsChanged)
{
	// assert(num_matches >= MIN_NUM_PAIRS);

	int anIndices[4];

	//m3BestHomography = Identity;
	// Jul. 2011, Min-Hyuk Sung
	Matrix<3> m3PrevHomography = m3BestHomography;
	bool isFirstIter = true;
	double dBestError = 999999999999999999.9;
	int max_num_matches = (int)num_matches;

#if PROSAC_INIT_NUM >= 0
	max_num_matches = PROSAC_INIT_NUM;
	max_num_matches = min(max_num_matches, (int)num_matches);
#endif

	// Do 'MAX_NUM_ITER' MLESAC trials.
	for(int nR = 0; nR < MAX_NUM_ITER ; nR++)
	{
		Matrix<3> m3Homography;
		if( isFirstIter )
		{
			m3Homography = m3PrevHomography;
		}
		else
		{
			// Find set of four unique matches
			for(int i=0; i<4; i++)
			{
				bool isUnique = false;
				int n;
				while(!isUnique)
				{
					n = rand() % max_num_matches;
					isUnique =true;
					for(int j=0; j<i && isUnique; j++)
						if(anIndices[j] == n)
							isUnique = false;
				};
				anIndices[i] = n;
			}

#if PROSAC_INIT_NUM >= 0
			max_num_matches++;
			max_num_matches = std::min(max_num_matches, (int)num_matches);
#endif

			// Find a homography from the minimal set..
			double R[3][3];
			homography_from_4corresp(
				x_pos_list+(2*anIndices[0]), x_pos_list+(2*anIndices[1]), x_pos_list+(2*anIndices[2]), x_pos_list+(2*anIndices[3]),
				y_pos_list+(2*anIndices[0]), y_pos_list+(2*anIndices[1]), y_pos_list+(2*anIndices[2]), y_pos_list+(2*anIndices[3]), R);

			for(int i = 0; i < 3; i++)
				for(int j = 0; j < 3; j++)
					m3Homography[i][j] = R[i][j];
		}

		//..and sum resulting MLESAC score
		double dError = MLESACScore(m3Homography, num_matches, x_pos_list, y_pos_list);

		if(dError < dBestError)
		{
			m3BestHomography = m3Homography;
			dBestError = dError;
		}

		// Jul. 2011, Min-Hyuk Sung
		// Turn off after first iteration
		isFirstIter = false;
	};
}
