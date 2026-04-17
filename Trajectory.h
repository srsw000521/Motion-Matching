#pragma once
#include "Vector3d.h"
#include "quat.h"

#include <GL/freeglut.h>
class Trajectory
{

public:
	Vector3f* m_trajectory;
	int m_goalIndex;
	int m_numSample;
	bool oneLap;
	double m_radius;

	Trajectory();
	~Trajectory();

	void initTrajectory(int mode = 0);
	void setCircleTrajectory();
	void setSquareTrajectory();
	void setTriTrajectory();
	void setStarTrajectory();

	void setCurrentTime(int currentFrame = 0);
	Vector3f getPositionAt(int frame);
	Vector3f getFuturePosition(int df);
	Vector3f getCurrentGoalPosition();
};

