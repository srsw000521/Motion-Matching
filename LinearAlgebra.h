#if !defined(_LINEARALGEBRA_H_INCLUDED_)
#define _LINEARALGEBRA_H_INCLUDED_

#include "vector3d.h"
#include "Quat.h"	


float ** matMatMult(float **A, float **B, int arow, int acol, int bcol);
float * matVecMult(float **A, float *B, int arow, int acol);
float * matVecMult1DDCol(double *A, double *B, int arow, int acol);
float * matVecMultD(float **A, double *B, int arow, int acol);
float * vecMatMult(int arow,int acol,float *v,float **A);
float ** matTrans(float **A, int arow, int acol);




bool ImInverseMatrixGJ(float ** A, float ** invA, int size); 
bool ImGaussj(float **a, int n, float **b, int m);
																					// [m*n] [n*1] = [m*1]
bool ImPseudoSolution(int m, int n, float ** A, float *x, float *b);				//   A  *  x   =   b
																					// [m*n] [n*t] = [m*t]
bool ImLinearRegression(int m, int n, int t, float ** A, float ** X, float ** B);	//   A  *  X   =   B

																					// [m*n] [n*t] = [m*t]
bool ImPseudoSolutionMat(int m, int n, int t, float **A, float **X, float **B);		//   A  *  x   =   b



void svdcmp(float** a, int m, int n, float* w, float** v);
float ImGetDeterminant3by3(float ** a);
void ImGetLeastSquareRotationForPointSet(int num, Vector3f * a, Vector3f * b, float ** R);

void ImLfit(Vector3f *x, float *y, float *sig, int ndat, float *a, int *ia, int ma, float **covar);
void ImLinearFunc(Vector3f x, float *afunc, int ma);
void ImLfit(float ** x, float *y, float *sig, int ndat, float *a, int *ia, int ma, float **covar);
void ImLinearFunc(float * x, float *afunc, int ma);

float ImCubicBSpline(float r);
float ImQuadraticBSpline(float r);



Quat ImGetLeastSquareQuaternionForPointSet(int num, Vector3f * source, Vector3f * target);
void ImGetCovariantMatrixForTwoPointSet(int num, Vector3f * source, Vector3f * target, float ** K);

Quat ImGetLeastSquareQuaternionForSkewedPointSet(int num, Vector3f* source, Vector3f* target);
void ImGetCovariantMatrixForTwoSkewedPointSet(int num, Vector3f* source, Vector3f* target, float** K);



																					// [m*n] [n*1] = [m*1]
void ImMatVecMult(int m, int n, float ** A, float * x, float * b);					//   A  *  x   =   b


void tred2( float **a, int n, float d[], float e[] );
void tqli( float d[], float e[], int n, float **z );



#endif // !defined(_LINEARALGEBRA_H_INCLUDED_)
