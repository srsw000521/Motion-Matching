#include <cstdio>
#include <cmath>
#include "Trajectory.h"

Trajectory::Trajectory()
{
	m_goalIndex = 0;
	m_numSample = 3000;
	oneLap = false;
	m_trajectory = nullptr;

	m_radius = 150.0f;
	initTrajectory();
}

Trajectory::~Trajectory()
{
}

void Trajectory::initTrajectory(int mode)
{
	if (m_trajectory != nullptr)
		delete [] m_trajectory;
	m_trajectory = new Vector3f[m_numSample];

	if (mode < 0) mode = 0;
	if (mode > 2) mode = 2;

	printf("trajectory mode = %d \n", mode);
	switch (mode)
	{
	case 0:	setCircleTrajectory();	break;
	case 1: setSquareTrajectory();  break;
	case 2: setStarTrajectory();	break;
	default:						break;
	}
}
void Trajectory::setCircleTrajectory()
{
	for (int i = 0; i < m_numSample; i++)
	{
		float angle = i * 2 * 3.141592 / m_numSample;
		float x = m_radius * cos(angle);
		float z = m_radius * sin(angle);
		m_trajectory[i] = Vector3f(x, 0, z);
	}
}

void Trajectory::setStarTrajectory()
{
	float radius = m_radius/1.3f;

	Vector3f ct(0, 0, 0);

	float dtheta = 2 * PI / 5.0;
	float r1 = radius;
	float r2 = radius*(1-0.588);
	
	Vector3f st(0, 0, 0), md(0, 0, 0), ed(0, 0, 0);

	int ds = m_numSample / 10;

	int ind = 0;
	for (int i = 0; i < 5; i++)
	{
		float theta1 = dtheta * i;
		float theta2 = dtheta * (i+0.5);
		float theta3 = dtheta * (i + 1);

		st.x = ct.x + r1 * cos(theta1);
		st.z = ct.z + r1 * sin(theta1);
		md.x = ct.x + r2 * cos(theta2);
		md.z = ct.z + r2 * sin(theta2);
		ed.x = ct.x + r1 * cos(theta3);
		ed.z = ct.z + r1 * sin(theta3);
		
		for (int j = 0; j < ds; j++)
		{
			float t = float(j) / ds;
			Vector3f p = (1-t)*st + t*md;
			m_trajectory[ind] = p;
			ind++;
		}
		for (int j = 0; j < ds; j++)
		{
			float t = float(j) / ds;
			Vector3f p = (1 - t) * md + t * ed;
			m_trajectory[ind] = p;
			ind++;
		}
				
	}

}
void Trajectory::setSquareTrajectory()
{
	float length = m_radius *3.141592 / 8.0f *2;
	float d = length * 8 / m_numSample;

	float x = length;
	float z = 0.0f;
	for (int i = 0; i < m_numSample; i++) {
		m_trajectory[i] = Vector3f(x, 0, z);
		if ( x >= length) {
			x = length;
			z += d;
		}
		if (z >= length) {
			x -= d;
			z = length;
		}
		if (x <= -length) {
			x = -length;
			z -= d;
		}
		if (z <= -length) {
			x += d;
			z = -length;
		}

	}

}

void Trajectory::setTriTrajectory()
{
	float length = 150.0f;
	float d = length * 3 / m_numSample;

	float x = 150.0f;
	float z = 0.0f;

	for (int i = 0; i < m_numSample; i++) {
		m_trajectory[i] = Vector3f(x, 0, z);
		if (x >= length) {
			x -= d;
			z = -x + length;
		}
		/*if (x <= 0) {
			x += d;
			z = x + length;
		}
		if (x <= -length) {
			x += d;
			z = 0;
		}*/


	}
}




void Trajectory::setCurrentTime(int currentFrame)
{
	m_goalIndex  = currentFrame % m_numSample;
}

Vector3f Trajectory::getPositionAt(int frame)
{
	int index = frame % m_numSample;
	return m_trajectory[index];
}
Vector3f Trajectory::getFuturePosition(int df)
{
	int index = m_goalIndex + df;

	while(index < 0)
		index += m_numSample;

	return getPositionAt(index);

	
}

Vector3f Trajectory::getCurrentGoalPosition()
{
	return getPositionAt(m_goalIndex);
}
