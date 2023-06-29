#pragma once
#include "Vector3d.h"
#include "quat.h"
#include "Motion.h"
#include "MotionFeatured.h"
#include "MySkeleton.h"
#include "Trajectory.h"
#include <GL/freeglut.h>
#include <vector>

class MyPosture;

class CMotionMatching
{
public:
	CMotionMatching();
	~CMotionMatching();

	int m_currentIndex;
	int m_currentTime;
	int m_lastTime;


	MotionFeatured srcMotion;
	Motion dstMotion;

	MySkeleton *skeleton;
	Trajectory *trajectory;

	int findBestMatchingIndex();

	double computeCost(int candidateIndex, bool bPrint = false);
	double computeCurrentCost(int candidateIndex);
	double computeFutureCost(int candidateIndex);
	double computeErr();

	double positionErr;
	double poseErr;
	double err;

	void drawFuturePos();
	void draw(int framenum, bool bPlay = true);
	void testSetup();
	void setStartPosition();

	void applyMotionMatching();


	int reset();
	void getCurrentCostBound();
};

