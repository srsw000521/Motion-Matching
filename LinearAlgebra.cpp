//#include "stdafx.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
//#include <nrutil.h>
#include "LinearAlgebra.h"
//#include "primitives.h"


float * matVecMult1DDCol(double *A, double *B, int arow, int acol) {
	float *C = (float*)malloc(arow*sizeof(float));
	for (int i=0;i<arow;i++) {
		C[i] = 0;
		for (int j=0;j<acol;j++) {
			C[i] += A[j*arow+i]*B[j];
		}
	}
	return C;
}

float ** matMatMult(float **A, float **B, int arow, int acol, int bcol) {
    float **C = (float**)malloc(arow*sizeof(float*));
    for (int i=0;i<arow;i++) {
        C[i] = (float*)malloc(bcol*sizeof(float));
        for (int j=0;j<bcol;j++) {
            C[i][j] = 0;
            for (int k=0;k<acol;k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

float * vecMatMult(int arow,int acol,float *v,float **A) {
	float *c = (float*)malloc(acol*sizeof(float));
	for (int i=0;i<acol;i++) {
		c[i] = 0;
		for (int j=0;j<arow;j++) {
			c[i] += A[j][i] * v[j];
		}
	}
	return c;
}

float * matVecMult(float **A, float *B, int arow, int acol) {
    float *C = (float*)malloc(arow*sizeof(float));
    for (int i=0;i<arow;i++) {
        C[i] = 0;
        for (int k=0;k<acol;k++) {
            C[i] += A[i][k] * B[k];
        }
    }
    return C;
}

float * matVecMultD(float **A, double *B, int arow, int acol) {
    float *C = (float*)malloc(arow*sizeof(float));
    for (int i=0;i<arow;i++) {
        C[i] = 0;
        for (int k=0;k<acol;k++) {
            C[i] += A[i][k] * B[k];
        }
    }
    return C;
}

float ** matTrans(float **A, int arow, int acol) {
	float ** AT = (float**)malloc(acol*sizeof(float*));

	for (int i=0;i<acol;i++) {
		AT[i] = (float*)malloc(arow*sizeof(float));
		for (int j=0;j<arow;j++) {
			AT[i][j] = A[j][i];
		}
	}

	return AT;
}


bool ImPseudoSolutionMat(int m, int n, int t, float **A, float **X, float **B) {
	float *BCol = (float*)calloc(m,sizeof(float));
	float *XCol = (float*)calloc(n,sizeof(float));
	bool res = true;
	for (int i=0;i<t;i++) {
		for (int j=0;j<m;j++) {
			BCol[j] = B[j][i];
		}
		res = res && ImPseudoSolution(m,n,A,XCol,BCol);
		for (int j=0;j<n;j++) {
			X[j][i] = XCol[j];
		}
	}
	return res;
}



bool ImInverseMatrixGJ(float ** A, float ** invA, int size)
{
	int i,j;
	int n_size = size;
	float ** n_m;
	float ** n_sol;

	n_m = new float * [n_size+1];
	for(i=0; i<n_size+1; i++)
		n_m[i] = new float [n_size+1];

	n_sol = new float * [n_size+1];
	for(i=0; i<n_size+1; i++)
		n_sol[i] = new float [2];

	for(i=0; i<=n_size; i++)
		n_sol[i][1] = 10.0f;				// DUMMY: AnyValue is okay

	for(i=0; i<n_size; i++)
		for(j=0; j<n_size; j++)
			n_m[i+1][j+1] = A[i][j];

	if(!ImGaussj(n_m, n_size, n_sol, 1))
		return false;

	for(i=0; i<n_size; i++)
		for(j=0; j<n_size; j++)
			invA[i][j] = n_m[i+1][j+1];

	for(i=0; i<n_size+1; i++)
		delete [] n_m[i];
	delete [] n_m;
	for(i=0; i<n_size+1; i++)
		delete [] n_sol[i];
	delete [] n_sol;
	return true;
}



bool ImGaussj(float **a, int n, float **b, int m)
{
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv;

	indxc = new int [n+1];
	indxr = new int [n+1];
	ipiv = new int [n+1]; 

	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++) 
	{
		big=0.0;
		for (j=1;j<=n;j++)
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) 
				{
					if (ipiv[k] == 0) 
					{
						if (fabs(double(a[j][k])) >= big) 
						{
							big=float(fabs(double(a[j][k])));
							irow=j;
							icol=k;
						}
					} 
					else if (ipiv[k] > 1){
//						AfxMessageBox("GAUSSJ: Singular Matrix-1");
						return false;
					};
				}
		++(ipiv[icol]);
		if (irow != icol) 
		{
			float temp;
			for (l=1;l<=n;l++) 
			{
				temp = a[irow][l];
				a[irow][l] = a[icol][l];
				a[icol][l] = temp;
			}
			for (l=1;l<=m;l++)
			{
				temp = b[irow][l];
				b[irow][l] = b[icol][l];
				b[icol][l] = temp;
//				SWAP(b[irow][l],b[icol][l])
			}
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) {
//			AfxMessageBox("GAUSSJ: Singular Matrix-2");
			return false;
		}
		pivinv=1.0f/a[icol][icol];
		a[icol][icol]=1.0;
		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
		for (ll=1;ll<=n;ll++)
			if (ll != icol) 
			{
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}
	for (l=n;l>=1;l--) 
	{
		if (indxr[l] != indxc[l])
		{
			float temp;
			for (k=1;k<=n;k++)
			{
//				SWAP(a[k][indxr[l]],a[k][indxc[l]]);k
				temp = a[k][indxr[l]];
				a[k][indxr[l]] = a[k][indxc[l]];
				a[k][indxc[l]] = temp;
			}
		}
	}
	
	delete [] ipiv;
	delete [] indxr;
	delete [] indxc;
	return true;
}


////////////////////////////////////////////////////////
//	SOLVE LINEAR SYSTEM BY USING SEUDO INVERSE
//	BASED on GAUSS JORDAN Ellimination
// [m*n] [n*1] = [m*1]
//   A  *  x   =   b
////////////////////////////////////////////////////////

bool ImPseudoSolution(int m, int n, float ** A, float *x, float *b)		
{
	float ** AT;					// n * m
	float ** ATA;					// n * n
	float ** InvATA;				// n * n
	float * ATb;					// n * 1
	
	AT = new float * [n];
	ATA = new float * [n];
	InvATA = new float * [n];
	ATb = new float [n];
	int i,j,k;
	for(i=0; i<n; i++)
	{
		AT[i] = new float [m];
		ATA[i] = new float [n];
		InvATA[i] = new float [n];
	}

	for(i=0; i<n; i++)
		for(j=0; j<m; j++)
			AT[i][j] = A[j][i];

		
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
		{
			ATA[i][j] = 0.0f;
			for(k=0; k<m; k++)
				ATA[i][j] += AT[i][k]*A[k][j];
		}

	for(i=0; i<n; i++)
	{
		ATb[i] = 0.0f;
		for(j=0; j<m; j++)
			ATb[i] +=AT[i][j]*b[j];
	}


	if(!ImInverseMatrixGJ(ATA, InvATA, n)) //ImInverseMatrixGJ(float ** A, float ** invA, int size)
	{
		for(i=0; i<n; i++)
		{
			delete [] AT[i];
			delete [] ATA[i];
			delete [] InvATA[i];
		}
		delete [] AT;
		delete [] ATA;
		delete [] InvATA;
		delete [] ATb;
		return false;
	}
			
	for(i=0; i<n; i++)
	{
		x[i] = 0.0f;
		for(j=0; j<n; j++)
			x[i] += InvATA[i][j]*ATb[j];
	}
	
	
	for(i=0; i<n; i++)
	{
		delete [] AT[i];
		delete [] ATA[i];
		delete [] InvATA[i];
	}
	delete [] AT;
	delete [] ATA;
	delete [] InvATA;
	delete [] ATb;
	
	return true;

}


////////////////////////////////////////////////////////
//	Linear Regression 
//	BASED on GAUSS JORDAN Ellimination
// [m*n] [n*t] = [m*t]
//   A  *  X   =   B
////////////////////////////////////////////////////////

bool ImLinearRegression(int m, int n, int t, float ** A, float ** X, float ** B)		
{

	float ** XT;					// t*n
	float ** XXT;					// n*n
	float ** InvXXT;				// n*n
	float ** BXT;					// m*n

	XT = new float * [t];
	XXT = new float * [n];
	InvXXT = new float * [n];
	BXT = new float * [m];
	int i,j,k,l;
	for(i=0; i<t; i++)
		XT[i] = new float [n];
	for(i=0; i<n; i++)
	{
		XXT[i] = new float [n];
		InvXXT[i] = new float [n];
	}
	for(i=0; i<m; i++)
		BXT[i] = new float [n];

	for(i=0; i<t; i++)
		for(j=0; j<n; j++)
			XT[i][j] = X[j][i];	
		
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
		{
			XXT[i][j] = 0.0f;
			for(k=0; k<t; k++)
				XXT[i][j] += X[i][k]*XT[k][j];
		}

	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
		{
			BXT[i][j] = 0.0f;
			for(k=0; k<t; k++)
				BXT[i][j] +=B[i][k]*XT[k][j];
		}

	
	if(!ImInverseMatrixGJ(XXT, InvXXT, n)) //ImInverseMatrixGJ(float ** A, float ** invA, int size)	
	{
		for(i=0; i<n; i++)
		{
			delete [] XXT[i];
			delete [] InvXXT[i];
		}
		for(i=0; i<m; i++) delete [] BXT[i];
		for(i=0; i<t; i++) delete [] XT[i];

		delete [] XT;
		delete [] XXT;
		delete [] InvXXT;
		delete [] BXT;
		return false;
	}


	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
		{
			A[i][j] = 0.0f;
			for(k=0; k<n; k++)
				A[i][j] += BXT[i][k]*InvXXT[k][j];
		}
			
	
	for(i=0; i<n; i++)
	{
		delete [] XXT[i];
		delete [] InvXXT[i];
	}
	for(i=0; i<m; i++) delete [] BXT[i];
	for(i=0; i<t; i++) delete [] XT[i];

	delete [] XT;
	delete [] XXT;
	delete [] InvXXT;
	delete [] BXT;

	
	return true;

}













static float at,bt,ct;
#define PYTHAG(a,b) ((at=fabs(a)) > (bt=fabs(b)) ? \
		     (ct=bt/at,at*sqrt(1.0+ct*ct)) : (bt ? (ct=at/bt,bt*sqrt(1.0+ct*ct)): 0.0))

static float maxarg1,maxarg2;
#define MAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
		  (maxarg1) : (maxarg2))
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

void ntrerror(const char *s)
{
  printf("%s\n",s);
  exit(1);
}

float *allocVect(int sz)
{
	float *ret;
	
	ret = (float*) calloc(sizeof(float), (size_t)sz);
	return ret;
}

void freeVect(float *ret)
{
	free(ret);
}

float **allocMatrix(int r,int c)
{
	float **ret;
		
	ret = (float **) calloc(sizeof(float), (size_t)(r*c));
	return ret;
}

void freeMatrix(float **ret,int r)
{
	free(ret);
}

void svdcmp(float** a, int m, int n, float* w, float** v)
{
  int flag,i,its,j,jj,k,l,nm;
  float c,f,h,s,x,y,z;
  float anorm=0.0,g=0.0,scale=0.0;
  float *rv1;
  void nrerror();

  if (m < n) ntrerror("SVDCMP: You must augment A with extra zero rows");
//  rv1=allocVect(n);
  rv1 = new float [n+1];
  for (i=1;i<=n;i++) {
    l=i+1;
    rv1[i]=scale*g;
    g=s=scale=0.0;
    if (i <= m) {
      for (k=i;k<=m;k++) scale += fabs(a[k][i]);
      if (scale) {
	for (k=i;k<=m;k++) {
	  a[k][i] /= scale;
	  s += a[k][i]*a[k][i];
	}
	f=a[i][i];
	g = -SIGN(sqrt(s),f);
	h=f*g-s;
	a[i][i]=f-g;
	if (i != n) {
	  for (j=l;j<=n;j++) {
	    for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
	    f=s/h;
	    for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
	  }
	}
	for (k=i;k<=m;k++) a[k][i] *= scale;
      }
    }
    w[i]=scale*g;
    g=s=scale=0.0;
    if (i <= m && i != n) {
      for (k=l;k<=n;k++) scale += fabs(a[i][k]);
      if (scale) {
	for (k=l;k<=n;k++) {
	  a[i][k] /= scale;
	  s += a[i][k]*a[i][k];
	}
	f=a[i][l];
	g = -SIGN(sqrt(s),f);
	h=f*g-s;
	a[i][l]=f-g;
	for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
	if (i != m) {
	  for (j=l;j<=m;j++) {
	    for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
	    for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
	  }
	}
	for (k=l;k<=n;k++) a[i][k] *= scale;
      }
    }
    anorm=MAX(anorm,(fabs(w[i])+fabs(rv1[i])));
  }
  for (i=n;i>=1;i--) {
    if (i < n) {
      if (g) {
	for (j=l;j<=n;j++)
	  v[j][i]=(a[i][j]/a[i][l])/g;
	for (j=l;j<=n;j++) {
	  for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
	  for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
	}
      }
      for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
    }
    v[i][i]=1.0;
    g=rv1[i];
    l=i;
  }
  for (i=n;i>=1;i--) {
    l=i+1;
    g=w[i];
    if (i < n)
      for (j=l;j<=n;j++) a[i][j]=0.0;
    if (g) {
      g=1.0/g;
      if (i != n) {
	for (j=l;j<=n;j++) {
	  for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
	  f=(s/a[i][i])*g;
	  for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
	}
      }
      for (j=i;j<=m;j++) a[j][i] *= g;
    } else {
      for (j=i;j<=m;j++) a[j][i]=0.0;
    }
    ++a[i][i];
  }
  for (k=n;k>=1;k--) {
    for (its=1;its<=30;its++) {
      flag=1;
      for (l=k;l>=1;l--) {
	nm=l-1;
	if (fabs(rv1[l])+anorm == anorm) {
	  flag=0;
	  break;
	}
	if (fabs(w[nm])+anorm == anorm) break;
      }
      if (flag) {
	c=0.0;
	s=1.0;
	for (i=l;i<=k;i++) {
	  f=s*rv1[i];
	  if (fabs(f)+anorm != anorm) {
	    g=w[i];
	    h=PYTHAG(f,g);
	    w[i]=h;
	    h=1.0/h;
	    c=g*h;
	    s=(-f*h);
	    for (j=1;j<=m;j++) {
	      y=a[j][nm];
	      z=a[j][i];
	      a[j][nm]=y*c+z*s;
	      a[j][i]=z*c-y*s;
	    }
	  }
	}
      }
      z=w[k];
      if (l == k) {
	if (z < 0.0) {
	  w[k] = -z;
	  for (j=1;j<=n;j++) v[j][k]=(-v[j][k]);
	}
	break;
      }
      if (its == 30) ntrerror("No convergence in 30 SVDCMP iterations");
      x=w[l];
      nm=k-1;
      y=w[nm];
      g=rv1[nm];
      h=rv1[k];
      f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      g=PYTHAG(f,1.0);
      f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
      c=s=1.0;
      for (j=l;j<=nm;j++) {
	i=j+1;
	g=rv1[i];
	y=w[i];
	h=s*g;
	g=c*g;
	z=PYTHAG(f,h);
	rv1[j]=z;
	c=f/z;
	s=h/z;
	f=x*c+g*s;
	g=g*c-x*s;
	h=y*s;
	y=y*c;
	for (jj=1;jj<=n;jj++) {
	  x=v[jj][j];
	  z=v[jj][i];
	  v[jj][j]=x*c+z*s;
	  v[jj][i]=z*c-x*s;
	}
	z=PYTHAG(f,h);
	w[j]=z;
	if (z) {
	  z=1.0/z;
	  c=f*z;
	  s=h*z;
	}
	f=(c*g)+(s*y);
	x=(c*y)-(s*g);
	for (jj=1;jj<=m;jj++) {
	  y=a[jj][j];
	  z=a[jj][i];
	  a[jj][j]=y*c+z*s;
	  a[jj][i]=z*c-y*s;
	}
      }
      rv1[l]=0.0;
      rv1[k]=f;
      w[k]=x;
    }
  }
  delete [] rv1;
//  freeVect(rv1);
}



void svbksb(float** u, float* w, float** v,int m, int n, float* b, float* x)
{  
	int jj,j,i; 
	float s,*tmp;
	tmp=allocVect(n);
	for (j=1;j<=n;j++) 
	{    
		s=0.0;    
		if (w[j]) 
		{
			for (i=1;i<=m;i++) 
				s += u[i][j]*b[i];   
			s /= w[j];   
		}   
		tmp[j]=s; 
	}
	for (j=1;j<=n;j++) 
	{   
		s=0.0;  
		for (jj=1;jj<=n;jj++) 
			s += v[j][jj]*tmp[jj];
		x[j]=s;
	} 
	freeVect(tmp);
}


float ImGetDeterminant3by3(float ** a)
{
	return   a[0][0]*a[1][1]*a[2][2] + a[0][2]*a[1][0]*a[2][1]
		   + a[0][1]*a[1][2]*a[2][0] - a[0][2]*a[1][1]*a[2][0]
		   - a[0][0]*a[1][2]*a[2][1] - a[0][1]*a[1][0]*a[2][2];

}

void ImGetCovariantMatrixForTwoPointSet(int num, Vector3f * source, Vector3f * target, float ** K)
{

	int i,j,k;
	
	// Reconstruct templete position from descrete coord;
	// and count current valid neighbor and get the position
	Vector3f center1 = Vector3f(0.0f,0.0f,0.0f), center2 = Vector3f(0.0f,0.0f,0.0f);

	for(i=0; i<num; i++)
	{
		center1 += source[i];
		center2 += target[i];
	}

	center1 /= (float) num;
	center2 /= (float) num;

	// constructing K matrix // 4 by 4 Matrix since index starts from 1;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			K[i][j] = 0.0f;
	
	for(k=0; k<num; k++)
		for(i=1; i<=3; i++)
			for(j=1; j<=3; j++)
				K[i][j] += (source[k][i-1]-center1[i-1])*(target[k][j-1]-center2[j-1]);		// (modified)*(target)
	
	return;
}


void ImGetLeastSquareRotationForPointSet(int num, Vector3f * source, Vector3f * target, float ** R)
{
	int i,j,k;

	// Finding least square fitting transformation to fit it: fitting templete to the given
	float ** U;				// 4 by 4 Matrix since index starts from 1;
	float ** V;
	float * W;
	U = new float * [4];
	V = new float * [4];
	W = new float [4];
	for (i=0; i<4; i++) U[i] = new float[4];
	for (i=0; i<4; i++) V[i] = new float[4];

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
		{
			U[i][j] = 0.0f;
			V[i][j] = 0.0f;
			W[i] = 0.0f;
		}

	ImGetCovariantMatrixForTwoPointSet(num, source, target, U);
			
	// void svdcmp(float** a, int m, int n, float* w, float** v);
	svdcmp(U, 3, 3, W, V);

	// Rotation matrix R = VUt
	for(i=0; i<3; i++) for(j=0; j<3; j++) 
	for(i=1; i<=3; i++)
		for(j=1; j<=3; j++)
		{
			R[i-1][j-1] = 0.0f;
			for(k=1; k<=3; k++)
				R[i-1][j-1] += V[i][k]*U[j][k];
		}

	// Checking for reflection
	int determinant = ImGetDeterminant3by3(R);
	if(determinant<0) 
	{
		int ind = 1;
		float min = W[1];
		for(k=1; k<=3; k++)
			if(W[k]>min){ min = W[k]; ind = k;}

		for(i=1; i<=3; i++)
			V[i][ind] = -V[i][ind];

		for(i=1; i<=3; i++)
			for(j=1; j<=3; j++)
			{
				R[i-1][j-1] = 0.0f;
				for(k=1; k<=3; k++)
					R[i-1][j-1] += V[i][k]*U[j][k];
			}
	}

	for (i=0; i<4; i++) delete [] U[i];
	for (i=0; i<4; i++) delete [] V[i];
	delete [] U;
	delete [] V;
	delete [] W;
	return;
}

Quat ImGetLeastSquareQuaternionForPointSet(int num, Vector3f * source, Vector3f * target)
{
	Quat out;
	int i,j,k;

	// Finding least square fitting transformation to fit it: fitting templete to the given
	float ** K;				// 3 by 3 Covariant Matrix since index starts from 1;
	float ** Q;				// 4 by 4 Energe from quaternion index starts 0
	K = new float * [4];
	Q = new float * [4];
	for (i=0; i<4; i++) K[i] = new float[4];
	for (i=0; i<4; i++) Q[i] = new float[4];

	ImGetCovariantMatrixForTwoPointSet(num, source, target, K);

	Q[0][0] = K[1][1]+K[2][2]+K[3][3];	Q[0][1] = K[2][3]-K[3][2];			Q[0][2] = K[3][1]-K[1][3];			Q[0][3] = K[1][2]-K[2][1];	
	Q[1][0] = K[2][3]-K[3][2];			Q[1][1] = K[1][1]-K[2][2]-K[3][3];	Q[1][2] = K[1][2]+K[2][1];			Q[1][3] = K[3][1]+K[1][3];	
	Q[2][0] = K[3][1]-K[1][3];			Q[2][1] = K[1][2]+K[2][1];			Q[2][2] =-K[1][1]+K[2][2]-K[3][3];	Q[2][3] = K[2][3]+K[3][2];	
	Q[3][0] = K[1][2]-K[2][1];			Q[3][1] = K[3][1]+K[1][3];			Q[3][2] = K[2][3]+K[3][2];			Q[3][3] =-K[1][1]-K[2][2]+K[3][3];	

	float diag[4];
	float odiag[4];

	tred2(Q, 4, diag, odiag);
	tqli(diag, odiag, 4, Q);

	Quat q[4];
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			q[i][j] = Q[j][i];

	float length;
	for (i=0; i<4; i++)
	{
		length = q[i].length();
		q[i] = q[i]/length;
	}

			if(diag[0]>=diag[1] && diag[0]>=diag[2] && diag[0]>=diag[3]) out = q[0];
	else	if(diag[1]>=diag[0] && diag[1]>=diag[2] && diag[1]>=diag[3]) out = q[1];
	else	if(diag[2]>=diag[1] && diag[2]>=diag[0] && diag[2]>=diag[3]) out = q[2];
	else																 out = q[3];

	for (i=0; i<4; i++) delete [] K[i];
	for (i=0; i<4; i++) delete [] Q[i];
	delete [] K;
	delete [] Q;

	return out;
}

/*
void ImGetLeastSquareRotationForPointSet(int num, Vector3f* source, Vector3f* target, float** R)
{
	int i, j, k;

	// Finding least square fitting transformation to fit it: fitting templete to the given
	float** U;				// 4 by 4 Matrix since index starts from 1;
	float** V;
	float* W;
	U = new float* [4];
	V = new float* [4];
	W = new float[4];
	for (i = 0; i < 4; i++) U[i] = new float[4];
	for (i = 0; i < 4; i++) V[i] = new float[4];

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
		{
			U[i][j] = 0.0f;
			V[i][j] = 0.0f;
			W[i] = 0.0f;
		}

	ImGetCovariantMatrixForTwoPointSet(num, source, target, U);

	// void svdcmp(float** a, int m, int n, float* w, float** v);
	svdcmp(U, 3, 3, W, V);

	// Rotation matrix R = VUt
	for (i = 0; i < 3; i++) for (j = 0; j < 3; j++)
		for (i = 1; i <= 3; i++)
			for (j = 1; j <= 3; j++)
			{
				R[i - 1][j - 1] = 0.0f;
				for (k = 1; k <= 3; k++)
					R[i - 1][j - 1] += V[i][k] * U[j][k];
			}

	// Checking for reflection
	int determinant = ImGetDeterminant3by3(R);
	if (determinant < 0)
	{
		int ind = 1;
		float min = W[1];
		for (k = 1; k <= 3; k++)
			if (W[k] > min) { min = W[k]; ind = k; }

		for (i = 1; i <= 3; i++)
			V[i][ind] = -V[i][ind];

		for (i = 1; i <= 3; i++)
			for (j = 1; j <= 3; j++)
			{
				R[i - 1][j - 1] = 0.0f;
				for (k = 1; k <= 3; k++)
					R[i - 1][j - 1] += V[i][k] * U[j][k];
			}
	}

	for (i = 0; i < 4; i++) delete[] U[i];
	for (i = 0; i < 4; i++) delete[] V[i];
	delete[] U;
	delete[] V;
	delete[] W;
	return;
}

Quat ImGetLeastSquareQuaternionForPointSet(int num, Vector3f* source, Vector3f* target)
{
	Quat out;
	int i, j, k;

	// Finding least square fitting transformation to fit it: fitting templete to the given
	float** K;				// 3 by 3 Covariant Matrix since index starts from 1;
	float** Q;				// 4 by 4 Energe from quaternion index starts 0
	K = new float* [4];
	Q = new float* [4];
	for (i = 0; i < 4; i++) K[i] = new float[4];
	for (i = 0; i < 4; i++) Q[i] = new float[4];

	ImGetCovariantMatrixForTwoPointSet(num, source, target, K);

	Q[0][0] = K[1][1] + K[2][2] + K[3][3];	Q[0][1] = K[2][3] - K[3][2];			Q[0][2] = K[3][1] - K[1][3];			Q[0][3] = K[1][2] - K[2][1];
	Q[1][0] = K[2][3] - K[3][2];			Q[1][1] = K[1][1] - K[2][2] - K[3][3];	Q[1][2] = K[1][2] + K[2][1];			Q[1][3] = K[3][1] + K[1][3];
	Q[2][0] = K[3][1] - K[1][3];			Q[2][1] = K[1][2] + K[2][1];			Q[2][2] = -K[1][1] + K[2][2] - K[3][3];	Q[2][3] = K[2][3] + K[3][2];
	Q[3][0] = K[1][2] - K[2][1];			Q[3][1] = K[3][1] + K[1][3];			Q[3][2] = K[2][3] + K[3][2];			Q[3][3] = -K[1][1] - K[2][2] + K[3][3];

	float diag[4];
	float odiag[4];

	tred2(Q, 4, diag, odiag);
	tqli(diag, odiag, 4, Q);

	Quat q[4];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			q[i][j] = Q[j][i];

	float length;
	for (i = 0; i < 4; i++)
	{
		length = q[i].length();
		q[i] = q[i] / length;
	}

	if (diag[0] >= diag[1] && diag[0] >= diag[2] && diag[0] >= diag[3]) out = q[0];
	else	if (diag[1] >= diag[0] && diag[1] >= diag[2] && diag[1] >= diag[3]) out = q[1];
	else	if (diag[2] >= diag[1] && diag[2] >= diag[0] && diag[2] >= diag[3]) out = q[2];
	else																 out = q[3];

	for (i = 0; i < 4; i++) delete[] K[i];
	for (i = 0; i < 4; i++) delete[] Q[i];
	delete[] K;
	delete[] Q;

	return out;
}
*/

Quat ImGetLeastSquareQuaternionForSkewedPointSet(int num, Vector3f* source, Vector3f* target)
{
	Quat out;
	int i, j, k;

	// Finding least square fitting transformation to fit it: fitting templete to the given
	float** K;				// 3 by 3 Covariant Matrix since index starts from 1;
	float** Q;				// 4 by 4 Energe from quaternion index starts 0
	K = new float* [4];
	Q = new float* [4];
	for (i = 0; i < 4; i++) K[i] = new float[4];
	for (i = 0; i < 4; i++) Q[i] = new float[4];

	ImGetCovariantMatrixForTwoSkewedPointSet(num, source, target, K);

	Q[0][0] = K[1][1] + K[2][2] + K[3][3];	Q[0][1] = K[2][3] - K[3][2];			Q[0][2] = K[3][1] - K[1][3];			Q[0][3] = K[1][2] - K[2][1];
	Q[1][0] = K[2][3] - K[3][2];			Q[1][1] = K[1][1] - K[2][2] - K[3][3];	Q[1][2] = K[1][2] + K[2][1];			Q[1][3] = K[3][1] + K[1][3];
	Q[2][0] = K[3][1] - K[1][3];			Q[2][1] = K[1][2] + K[2][1];			Q[2][2] = -K[1][1] + K[2][2] - K[3][3];	Q[2][3] = K[2][3] + K[3][2];
	Q[3][0] = K[1][2] - K[2][1];			Q[3][1] = K[3][1] + K[1][3];			Q[3][2] = K[2][3] + K[3][2];			Q[3][3] = -K[1][1] - K[2][2] + K[3][3];

	float diag[4];
	float odiag[4];

	tred2(Q, 4, diag, odiag);
	tqli(diag, odiag, 4, Q);

	Quat q[4];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			q[i][j] = Q[j][i];

	float length;
	for (i = 0; i < 4; i++)
	{
		length = q[i].length();
		q[i] = q[i] / length;
	}

	if (diag[0] >= diag[1] && diag[0] >= diag[2] && diag[0] >= diag[3]) out = q[0];
	else	if (diag[1] >= diag[0] && diag[1] >= diag[2] && diag[1] >= diag[3]) out = q[1];
	else	if (diag[2] >= diag[1] && diag[2] >= diag[0] && diag[2] >= diag[3]) out = q[2];
	else																 out = q[3];

	for (i = 0; i < 4; i++) delete[] K[i];
	for (i = 0; i < 4; i++) delete[] Q[i];
	delete[] K;
	delete[] Q;

	return out;
}


void ImGetCovariantMatrixForTwoSkewedPointSet(int num, Vector3f* source, Vector3f* target, float** K)
{

	int i, j, k;

	// Reconstruct templete position from descrete coord;
	// and count current valid neighbor and get the position
	Vector3f center1 = Vector3f(0.0f, 0.0f, 0.0f), center2 = Vector3f(0.0f, 0.0f, 0.0f);
/*
	for (i = 0; i < num; i++)
	{
		center1 += source[i];
		center2 += target[i];
	}

	center1 /= (float)num;
	center2 /= (float)num;
*/
	// constructing K matrix // 4 by 4 Matrix since index starts from 1;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			K[i][j] = 0.0f;

	for (k = 0; k < num; k++)
		for (i = 1; i <= 3; i++)
			for (j = 1; j <= 3; j++)
				K[i][j] += (source[k][i - 1] - center1[i - 1]) * (target[k][j - 1] - center2[j - 1]);		// (modified)*(target)

	return;
}


void ImMatVecMult(int m, int n, float ** A, float * x, float * b)
{

	int i,j,k;

	for(i=0; i<m; i++) b[i] = 0.0f;

	for(i=0; i<m; i++)
	{
		b[i] = 0.0f;
		for(j=0; j<n;  j++)
			b[i] += A[i][j] * x[j];
	}
	return;
}



///////////////////////// Eigen Vector compute for a GENERAL MATRIX ///////////////////////



void
tred2( float **a, int n, float d[], float e[] )
{
	int l, k, j, i;
	float h, scale, f, g, hh;
	for( i=n-1; i>=1; i-- )
	{
		l=i-1;
		h=scale=0.0;
		if( l>0 )
		{
			for( k=0; k<=l; k++ )
				scale+= fabs( a[i][k] );
			if( scale==0.0f )
				e[i]=a[i][l];
			else
			{
				for( k=0; k<=l; k++ )
				{
					a[i][k]/=scale;
					h+=a[i][k]*a[i][k];
				}
				f=a[i][l];
				g=(f>0.0?-(float)sqrt(h):(float)sqrt(h));
				e[i]=scale*g;
				h-=f*g;
				a[i][l]=f-g;
				f=0.0f;
				for( j=0; j<=l; j++ )
				{
					a[j][i]=a[i][j]/h;
					g=0.0;
					for( k=0; k<=j; k++ )
						g+=a[j][k]*a[i][k];
					for( k=j+1;k<=l; k++ )
						g+=a[k][j]*a[i][k];
					e[j]=g/h;
					f+=e[j]*a[i][j];
				}
				hh=f/(h+h);
				for( j=0; j<=l; j++ )
				{
					f=a[i][j];
					e[j]=g=e[j]-hh*f;
					for(k=0; k<=j; k++ )
						a[j][k] -= (f*e[k]+g*a[i][k]);
				}
			}
		}else
			e[i]=a[i][l];
		d[i]=h;
	}
	d[0]=0.0f;
	e[0]=0.0f;
	for( i=0; i<n; i++ )
	{
		l=i-1;
		if( d[i] )
		{
			for( j=0; j<=l; j++ )
			{
				g=0.0f;
				for( k=0; k<=l; k++ )
					g+=a[i][k]*a[k][j];
				for( k=0; k<=l; k++ )
					a[k][j]-=g*a[k][i];
			}
		}
		d[i]=a[i][i];
		a[i][i]=1.0f;
		for(j=0; j<=l;j++) a[j][i]=a[i][j]=0.0f;
	}
}

void tqli( float d[], float e[], int n, float **z )
{
//	float pythag( float a, float b );
	int m, l, iter, i, k;
	float s, r, p, g, f, dd, c, b;
	for( i=1; i<n; i++ ) e[i-1]=e[i];
	e[n-1]=0.0f;

	for( l=0; l<n; l++ )
	{
		iter=0;
		do{
			for( m=l; m<n-1; m++ )
			{
				dd=fabs(d[m])+fabs(d[m+1]);
				if( (float)(fabs(e[m])+dd)==dd ) break;
			}
			if( m!=l )
			{
				if( iter++==30 ){ return;}
				g=(d[l+1]-d[l])/(2.0f*e[l]);
				r=PYTHAG(g,1.0f);
				g=d[m]-d[l]+e[l]/(g+(float)SIGN(r,g));
				s=c=1.0f;
				p=0.0f;
				for( i=m-1; i>=l; i-- )
				{
					f=s*e[i];
					b=c*e[i];
					e[i+1]=(r=PYTHAG(f,g));
					if( r==0.0 )
					{
						d[i+1]-=p;
						e[m]=0.0f;
						break;
					}
					s=f/r;
					c=g/r;
					g=d[i+1]-p;
					r=(d[i]-g)*s+2.0f*c*b;
					d[i+1]=g+(p=s*r);
					g=c*r-b;
					for( k=0; k<n; k++ )
					{
						f=z[k][i+1];
						z[k][i+1]=s*z[k][i]+c*f;
						z[k][i]=c*z[k][i]-s*f;
					}
				}
				if( r==0.0 && i ) continue;
				d[l]-=p;
				e[l]=g;
				e[m]=0.0;
			}
		}while(m!=l);
	}
}

