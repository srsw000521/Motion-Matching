#include "pch.h"
#include "MyPosture.h"

MyPosture::MyPosture(int numjoint, Vector3f _rootPos, Vector3f* _jointArray)
{
	m_numJoint = numjoint;
	jointAngles = new Vector3f[m_numJoint];
	Ori = new Quat[m_numJoint];
	rootPosition = _rootPos;

	rootOriDy = Quat(0, 0, 0, 1);
	rootOriXZ = Quat(0, 0, 0, 1);
	rootOriY = Quat(0, 0, 0, 1);

	rootPosDxz = Vector3f(0, 0, 0);
	rootPosXZ = Vector3f(0, 0, 0);
	rootPosY = Vector3f(0, 0, 0);

	if (_jointArray != nullptr) {
		for (int i = 0; i < m_numJoint; i++) {
			jointAngles[i] = _jointArray[i];
			Quat q;
			Quat r_z; r_z.AngleAxis(jointAngles[i].z / 180.0f * 3.141592f, Vector3f(0, 0, 1));
			Quat r_y; r_y.AngleAxis(jointAngles[i].y / 180.0f * 3.141592f, Vector3f(0, 1, 0));
			Quat r_x; r_x.AngleAxis(jointAngles[i].x / 180.0f * 3.141592f, Vector3f(1, 0, 0));
			q = r_z * r_y * r_x;
			Ori[i] = q;
		}
	}
	else {
		for (int i = 0; i < m_numJoint; i++) {
			jointAngles[i] = Vector3f(0, 0, 0);
			Ori[i].Identity();
		}
	}

}
MyPosture::~MyPosture()
{
	delete[] jointAngles;
	delete[] Ori;
}

void MyPosture::clone(const MyPosture& in)
{
	ASSERT(m_numJoint == in.m_numJoint);
	ASSERT(Ori != nullptr);
	ASSERT(jointAngles != nullptr);

	rootPosition = in.rootPosition;
	rootOriDy = in.rootOriDy;
	rootOriXZ = in.rootOriXZ;
	rootOriY = in.rootOriY;

	rootPosDxz = in.rootPosDxz;
	rootPosXZ = in.rootPosXZ;
	rootPosY = in.rootPosY;

	for (int i = 0; i < m_numJoint; i++) {
		jointAngles[i] = in.jointAngles[i];
		Ori[i] = in.Ori[i];
	}
}

MyPosture::MyPosture(const MyPosture& in)
{
	m_numJoint = in.m_numJoint;
	jointAngles = new Vector3f[m_numJoint];
	Ori = new Quat[m_numJoint];

	clone(in);
}
const MyPosture& MyPosture::operator= (const MyPosture& in)
{
	if (this == &in) return *this;

	if (jointAngles != nullptr) delete[] jointAngles;
	if (Ori != nullptr) delete[] Ori;
	
	m_numJoint = in.m_numJoint;
	jointAngles = new Vector3f[m_numJoint];
	Ori = new Quat[m_numJoint];

	clone(in);

	return *this;
}

#include <GL/freeglut.h>
#include "MySkeleton.h"

void MyPosture::setDifference(MyPosture& from, MyPosture& to)
{
//	MyPosture& from = MotionMatching->srcMotion.getPosture(previousIndex);
//	MyPosture& to = MotionMatching->srcMotion.getPosture(bestMatchingIndex);

	for (int i = 0; i < m_numJoint; i++)
	{
		Quat q_from = from.Ori[i];
		Quat q_to = to.Ori[i];
		if ((q_to % q_from) < 0) q_from = -1.0f * q_from;

		Quat q_to_inv = q_to;
		q_to_inv.Reciprocal();

		Ori[i] = q_from * q_to_inv;
		if (Ori[i] % Quat(0, 0, 0, 1) < 0) Ori[i] = -1.0f * Ori[i];
	}
	Quat OriXZ_to_inv = to.rootOriXZ;
	if (from.rootOriXZ % to.rootOriXZ < 0) OriXZ_to_inv = -1.0f * OriXZ_to_inv;

	OriXZ_to_inv.Reciprocal();
	rootOriXZ = from.rootOriXZ * OriXZ_to_inv;
	if (rootOriXZ % Quat(0, 0, 0, 1) < 0) rootOriXZ = -1.0f * rootOriXZ;

//	printf("%f %f %f", from.rootOriXZ.X(), from.rootOriXZ.Y(), from.rootOriXZ.Z());

	rootPosition = from.rootPosition - to.rootPosition;

}
