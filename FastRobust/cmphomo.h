#ifndef CMPHOMO_H
#define CMPHOMO_H

void homography_transform(const double a[2], const double H[3][3], double r[2]);

/* computes the homography sending [0,0] , [0,1], [1,1] and [1,0]
 * to x,y,z and w.
 */
void homography_from_4pt(const double *x, const double *y, const double *z, const double *w, double cgret[8]);

/*
 * computes the homography sending a,b,c,d to x,y,z,w
 */
void homography_from_4corresp(
                const double *a, const double *b, const double *c, const double *d,
                const double *x, const double *y, const double *z, const double *w, double R[3][3]);

#endif
