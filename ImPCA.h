#pragma once
class ImPCA
{
public:
	void Create();
	void MemoryRelease();
	void MemoryAssign();
	void BuildPCA(int dim, int num_sample, float** featureVector, float accuracy);
	ImPCA();
	virtual ~ImPCA();

	float* pMean;
	float** pEigenVector;							// A Set of Colume Vector
	float* pEigenValue;
	int m_DimReduced;
	int m_DimOriginal;

	float m_Accuracy;

	void SavePCA(FILE* file);
	void LoadPCA(FILE* file);

	void GetReducedVector(float* input, float* output);
	void GetReconstructedVector(float* input, float* output);
};

