#pragma once

#include "MyJoint.h"
#include "Vector3d.h"
#include "quat.h"
#include <vector>
#include <string>

#include <GL/freeglut.h>

class MyPosture;

class MySkeleton
{
public:
	MySkeleton();
	~MySkeleton();

	MyJoint** joints = nullptr;
	MyJoint* root;
	
	int numJoints;
	int m_numofActivatedJoint;

	float m_height;

	std::string filename;
	Vector3f rootpos;
	Vector3f rootcolor;

	void init();
	void draw(Vector3f c = Vector3f(-1,-1,-1), int lineWidth = 0, bool bSphere = true, bool m_bShowSrcMotionRootOnly = false, bool m_bShowDstMotionRootOnly = false,float distance =0.0f);
	void readHierachyFile();
	void setPosture(const MyPosture& p);

	float getHeight();


	
	
};

