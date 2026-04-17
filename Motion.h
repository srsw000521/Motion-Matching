#pragma once
#include "Vector3d.h"
#include "quat.h"
#include "MySkeleton.h"
#include "MyPosture.h"
#include <GL/freeglut.h>
#include <utility>
#include <string>
#include <deque>

class Motion
{
public:
	Motion();
	virtual ~Motion();

	void initMotion();

	MySkeleton* m_pSkeleton;

	Vector3f m_startPosition;

	std::string filename;
	int numof_frame;
	int numof_frame_persecond;
	float frame_time;
	int activated_joint;

	std::deque<MyPosture> postures;

	void addPostureTest(Motion* srcMotion);
	void setDPosture(int frame);
	void addPosture(Vector3f rootPos, Vector3f * jointArray);
	void addDSTPosture(int framenum,Motion *srcMotion);
	void SeparateYRot(Quat &Ori, Quat &OriY, Quat& OriXZ);
	void applyBlending(Motion* srcMotion, int previousIndex, float t);
	void applyBlending(MyPosture & dPos, float t);

	MyPosture& getPosture(int frame)
	{
		if (frame < 0) frame = 0;
		if (frame > postures.size() - 1) frame = postures.size() - 1;
		return postures[frame];
	};

	Vector3f* rootposition;

	virtual void readMotionFile();
	virtual void allocMemory();
	virtual void releaseMemory();

	bool isReady() { if (numof_frame < 1) return false; else return true; }

};


