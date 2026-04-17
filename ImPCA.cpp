#include <cassert>
#include <cstdio>
#include "ImPCA.h"

#include "LinearAlgebra.h"



void ImPCA::Create()
{
	pMean = NULL;
	pEigenVector = NULL;
	pEigenValue = NULL;
	m_DimReduced = 0;
	m_DimOriginal = 0;
}

ImPCA::ImPCA()
{
	Create();
}

ImPCA::~ImPCA()
{
	MemoryRelease();
}

void ImPCA::MemoryAssign()
{
	int i;
	if (m_DimOriginal < 1) return;

	pMean = new float[m_DimOriginal];
	pEigenValue = new float[m_DimOriginal];
	pEigenVector = new float* [m_DimOriginal];
	for (i = 0; i < m_DimOriginal; i++) pEigenVector[i] = new float[m_DimOriginal];

	return;
}

void ImPCA::MemoryRelease()
{
	int i;

	if (pMean != NULL) delete[] pMean;
	if (pEigenVector != NULL)
	{
		for (i = 0; i < m_DimOriginal; i++)	delete[] pEigenVector[i];
		delete[] pEigenVector;
	}
	if (pEigenValue != NULL) delete[] pEigenValue;

	Create();

	return;
}



// Feature Vector: A set of Colume Vector 
void ImPCA::BuildPCA(int dim, int num_sample, float** featureVector, float accuracy)
{
	MemoryRelease();

	int i, j, k;

	m_Accuracy = accuracy;
	assert(m_Accuracy > 0.0f && m_Accuracy < 1.0f);
	m_DimOriginal = dim;

	MemoryAssign();


	///////////////////////////////////////////////////////////////////
	// Computing Mean Vector
	///////////////////////////////////////////////////////////////////
	for (i = 0; i < m_DimOriginal; i++)
		pMean[i] = 0.0f;

	for (i = 0; i < num_sample; i++)
		for (j = 0; j < m_DimOriginal; j++)
			pMean[j] += featureVector[j][i];

	for (i = 0; i < m_DimOriginal; i++)
		pMean[i] /= float(num_sample);

	///////////////////////////////////////////////////////////////////
	// Building Covariant Matrix
	///////////////////////////////////////////////////////////////////

	float* pFeature = new float[m_DimOriginal];
	float** pCov = pEigenVector;

	for (i = 0; i < m_DimOriginal; i++) for (j = 0; j < m_DimOriginal; j++) pCov[i][j] = 0.0f;

	for (i = 0; i < num_sample; i++)
	{
		for (j = 0; j < m_DimOriginal; j++)
			pFeature[j] = featureVector[j][i] - pMean[j];

		for (j = 0; j < m_DimOriginal; j++)
			for (k = 0; k < m_DimOriginal; k++)
				pCov[j][k] += (pFeature[j] * pFeature[k]);
	}

	for (i = 0; i < m_DimOriginal; i++)
		for (j = 0; j < m_DimOriginal; j++)
			pCov[i][j] /= float(num_sample);					// covariant matrix

	///////////////////////////////////////////////////////////////////
	// Computing Eigen vector and Eigen Value
	///////////////////////////////////////////////////////////////////

	float* diag = new float[m_DimOriginal];
	float* odiag = new float[m_DimOriginal];

	tred2(pCov, m_DimOriginal, diag, odiag);
	tqli(diag, odiag, m_DimOriginal, pCov);

	for (i = 0; i < m_DimOriginal; i++) pEigenValue[i] = diag[i];


	///////////////////////////////////////////////////////////////////
	// Sorting Eigenvalue in decreasing order
	///////////////////////////////////////////////////////////////////
	int max_ind;
	float maxVar;
	float temp;

	for (i = 0; i < m_DimOriginal - 1; i++)
	{
		maxVar = diag[i];
		max_ind = i;
		for (j = i + 1; j < m_DimOriginal; j++)
			if (diag[j] > maxVar)
			{
				max_ind = j;
				maxVar = diag[j];
			}
		if (max_ind != i)
		{
			// SWAP
			temp = diag[i];
			diag[i] = diag[max_ind];
			diag[max_ind] = temp;

			pEigenValue[i] = diag[i];
			pEigenValue[max_ind] = diag[max_ind];
			for (j = 0; j < m_DimOriginal; j++)
			{
				temp = pEigenVector[j][i];
				pEigenVector[j][i] = pEigenVector[j][max_ind];				// colume vector
				pEigenVector[j][max_ind] = temp;
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	// Finding dimensionality according to the accuracy
	///////////////////////////////////////////////////////////////////

	float sum = 0.0f;
	float subSum = 0.0f;
	for (i = 0; i < m_DimOriginal; i++)
		if (pEigenValue[i] > 0.0f) sum += pEigenValue[i];

	for (i = 0; i < m_DimOriginal; i++)
	{
		m_DimReduced = i + 1;
		if (pEigenValue[i] > 0.0f) subSum += pEigenValue[i];
		if (subSum / sum > m_Accuracy)
			break;
	}

	printf("EV: NumEFFFrame %d  NumDim %d: NumRed %d\n", num_sample, m_DimOriginal, m_DimReduced);

	for (i = 0; i < m_DimOriginal; i++)
		printf("%10.8f ", pEigenValue[i]);
	printf("\n");

	delete[] pFeature;
	delete[] diag;
	delete[] odiag;

}
void ImPCA::SavePCA(FILE* file)
{
	int i, j;
	fprintf(file, "%d %d %f\n", m_DimOriginal, m_DimReduced, m_Accuracy);

	fprintf(file, "\n");
	for (i = 0; i < m_DimOriginal; i++)
		fprintf(file, "%f ", pMean[i]);
	fprintf(file, "\n");
	for (i = 0; i < m_DimOriginal; i++)
	{
		fprintf(file, "%d ", i);
		fprintf(file, "%f ", pEigenValue[i]);
		for (j = 0; j < m_DimOriginal; j++)
		{
			fprintf(file, "%f ", pEigenVector[i][j]);
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n");
}

void ImPCA::LoadPCA(FILE* file)
{
	MemoryRelease();

	int i, j;
	fscanf_s(file, "%d %d %f", &m_DimOriginal, &m_DimReduced, &m_Accuracy);

	MemoryAssign();

	for (i = 0; i < m_DimOriginal; i++)
		fscanf_s(file, "%f ", &pMean[i]);
	for (i = 0; i < m_DimOriginal; i++)
	{
		int ind;
		fscanf_s(file, "%d ", &ind);
		assert(ind == i);
		fscanf_s(file, "%f ", &pEigenValue[i]);
		for (j = 0; j < m_DimOriginal; j++)
		{
			fscanf_s(file, "%f ", &pEigenVector[i][j]);
		}
	}




}

void ImPCA::GetReducedVector(float* input, float* output)
{
	int i, j;

	for (i = 0; i < m_DimReduced; i++)
	{
		output[i] = 0.0f;

		for (j = 0; j < m_DimOriginal; j++)
			output[i] += (input[j] - pMean[j]) * pEigenVector[j][i];		// because pEigenVector is a set of column vectors
	}

	return;
}

void ImPCA::GetReconstructedVector(float* input, float* output)
{
	int i, j;
	for (i = 0; i < m_DimOriginal; i++)
	{
		output[i] = pMean[i];
		for (j = 0; j < m_DimReduced; j++)
		{
			output[i] += input[j] * pEigenVector[i][j];
		}
	}

	return;

}
