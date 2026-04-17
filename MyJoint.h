#pragma once
#include "Vector3d.h"
#include "quat.h"

#include <string>
class MyJoint
{
public:
	MyJoint();
	~MyJoint();

	MyJoint* parent;
	MyJoint* sibling;
	MyJoint* child;

	Vector3f localPos;
	Vector3f globalPos;

	Quat localRot;
	Quat globalRot;

	Vector3f shadowPos;

	Vector3f color;
	std::string name;
	int number;
	bool activate;

	void init(int number, Vector3f offset);
	void addChild(MyJoint* c);
	MyJoint* getLastChild();
	MyJoint* getByName(std::string in);

	void setGlobalTransform(Vector3f parentoffset = Vector3f(0, 0, 0), Quat parentRot = Quat(0, 0, 0, 1));
	//void drawFeature();
	
	void printGlobalPos();
};




