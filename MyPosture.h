#pragma once
#include "vector3d.h"
#include "quat.h"

class MySkeleton;

class MyPosture
{
public:
	MyPosture(int numjoint, Vector3f _rootPos = Vector3f(0, 0, 0), Vector3f* _jointArray = nullptr);
	~MyPosture();
	MyPosture(const MyPosture& in);

	const MyPosture& operator= (const MyPosture& in);
	void clone(const MyPosture& in);



	int m_numJoint;

	Vector3f * jointAngles;
	Quat* Ori;

	Quat rootOriY;
	Quat rootOriXZ;
	Quat rootOriDy;

	Vector3f rootPosition;

	Vector3f rootPosY;
	Vector3f rootPosXZ;
	Vector3f rootPosDxz;

	float m_distance;

	void drawGL(MySkeleton* pSkeleton, Vector3f color = Vector3f(1,1,1), bool bSphere = true, bool m_bShowSrcMotionRootOnly = false, bool m_bShowDstMotionRootOnly = false, Vector3f goalPos = Vector3f(-1,-1,-1));

	void setDifference(MyPosture& from, MyPosture& to);
};

