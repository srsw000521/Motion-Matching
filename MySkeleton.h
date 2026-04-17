#pragma once

#include "MyJoint.h"
#include "Vector3d.h"
#include "quat.h"
#include <vector>
#include <string>

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
	void readHierachyFile();
	void setPosture(const MyPosture& p);

	float getHeight();


	
	
};

