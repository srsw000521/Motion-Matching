#pragma once
#include "Motion.h"

class FeatureVector
{
public:
	Vector3f m_LFootPos;
	Vector3f m_LFootVel;
	Vector3f m_RFootPos;
	Vector3f m_RFootVel;
	Vector3f m_LHandPos;
	Vector3f m_LHandVel;
	Vector3f m_RHandPos;
	Vector3f m_RHandVel;
	Vector3f m_hipVel;
	Vector3f m_posFuture[10];
	std::pair<Vector3f, Vector3f> m_velFuture[10];
	Vector3f m_posPast[10];
};


class MotionFeatured : public Motion
{
public:

	MotionFeatured();
	~MotionFeatured();
	 
	int m_futureStep;
	int m_numStep;

	int m_pastStep;
	int m_numPastStep;

	FeatureVector* m_features;

	void initFeature();
	void allocMemory();
	void releaseMemory();
};

