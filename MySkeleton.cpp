#include "pch.h"
#include "MySkeleton.h"
MySkeleton::MySkeleton(void)
{
	root = nullptr;
	joints = nullptr;
	rootpos = Vector3f(0, 0, 0);
	numJoints = 38;            //없애야 함
	m_numofActivatedJoint = 0;
	filename = "LocomotionFlat01_000.bvh";
	m_height = 0.0f; 

}
MySkeleton::~MySkeleton(void)
{
}
void MySkeleton::init() {
	readHierachyFile();
}
void MySkeleton::readHierachyFile() {
	char line[1000];
	int cnt = 0;
	bool flag = 0;
	float off_a, off_b, off_c;
	FILE* file = fopen(filename.c_str(), "r");
	int joint_current = 1;
	int num = 0;
	bool name = 0;

	joints = new MyJoint * [numJoints];
	for (int i = 0; i < numJoints; i++)
	{
		joints[i] = new MyJoint();
	}
	root = joints[0];

	MyJoint* p_current = joints[0];
	MyJoint* p_next = joints[1];

	while (!feof(file)) {

		fgets(line, sizeof(line), file);

		if (strcmp(line, "MOTION\n") == 0)  break;

		char* token = strtok(line, "\t");
		while (token != NULL) {
			char* token2 = token;
			token2 = strtok(token2, " ");
			if (name == 1) {
				p_current->name = strtok(token2, "\n");
				name = 0;
			}
			if (strcmp(token2, "OFFSET") == 0) {
				flag = 1;
			}
			if (flag == 1 && cnt < 4) {
				if (cnt == 1) {
					off_a = atof(token2);
				}
				else if (cnt == 2) {
					off_b = atof(token2);
				}
				else if (cnt == 3) {
					off_c = atof(token2);
				}
				cnt++;
				if (cnt == 4) {
					flag = 0;
					cnt = 0;
					joints[num]->init(num, Vector3f(off_a, off_b, off_c));
					num++;
				}
			}

			if (strcmp(token2, "JOINT") == 0 ) {
				p_current->addChild(p_next);
				p_current->activate = 1;
				p_next->parent = p_current;
				p_current = p_next;
				p_next = joints[++joint_current];
				name = 1;
				m_numofActivatedJoint++;
			}
			else if (strcmp(token2, "End") == 0) {
				p_current->addChild(p_next);
				p_current->activate = 1;
				p_next->parent = p_current;
				p_current = p_next;
				p_next = joints[++joint_current];
				name = 1;
			}
			else if (strcmp(token2, "ROOT") == 0) {
				name = 1;
				m_numofActivatedJoint++;
			}
			else if (strcmp(token2, "}\n") == 0) {
				if (p_current->parent != NULL) {
					p_current = p_current->parent;
				}
			}
			token = strtok(NULL, "\t");
		}
	}
	numJoints = num;
	fclose(file);
	//0.0 red 0.3 green 0.6 blue
	for (int i = 0; i < numJoints; i++) {
		joints[i]->color = HSV2RGB(Vector3f(i / ((float)numJoints - 1), 1, 1));
	}
	
}
void MySkeleton::draw(Vector3f c, int lineWidth, bool bSphere, bool m_bShowSrcMotionRootOnly, bool m_bShowDstMotionRootOnly,float distance)
{
	if (bSphere == false)
		glDisable(GL_LIGHTING);

	if (m_bShowDstMotionRootOnly == true) {
		root->setGlobalTransform();
		Vector3f p = root->globalPos;
		
		float thresh = .5f;
		float height = getHeight();

		float err = distance;
		err /= height;
		err *= 1.8;

		float h = (0.6 - err / thresh * 0.6);
		h = (h > 0) ? h : 0;
/*
		if (distance > 7) {
			rootcolor = HSV2RGB(Vector3f(0.0, 1, 1)); // 빨간색
		}
		else if (distance > 3) {
			rootcolor = HSV2RGB(Vector3f(0.3, 1, 1)); // 초록색
		}
		else {
			rootcolor = HSV2RGB(Vector3f(0.6, 1, 1)); // 파란색
		}
*/
		rootcolor = HSV2RGB(Vector3f(h, 1, 1));

		glColor3f(rootcolor.x, rootcolor.y, rootcolor.z);
		glPushMatrix();
		glTranslatef(p.x, 0, p.z);
		GLUquadricObj* obj;
		obj = gluNewQuadric();
		gluQuadricDrawStyle(obj, GLU_FILL);

		gluSphere(obj, 0.3f, 20, 20);
		gluDeleteQuadric(obj);
		glPopMatrix();

		return; 
	}
	if (m_bShowSrcMotionRootOnly == true)
	{
		root->setGlobalTransform();
		Vector3f p = root->globalPos;
		glColor3f(c.x, c.y, c.z);
		glPushMatrix();
		glTranslatef(p.x, 0, p.z);
		GLUquadricObj* obj;
		obj = gluNewQuadric();
		gluQuadricDrawStyle(obj, GLU_FILL);

		gluSphere(obj, 0.3f, 20, 20);
		gluDeleteQuadric(obj);
		glPopMatrix();     

		return;
	}

	root->draw(c, lineWidth);
	root->setGlobalTransform();
	root->drawShadow(Vector3f(0.3, 0.3, 0.3), lineWidth);

	if (bSphere)
	{
		for (int i = 0; i < numJoints; i++)
		{
			Vector3f p = joints[i]->globalPos;
			glPushMatrix();
			glTranslatef(p.x, p.y, p.z);
			GLUquadricObj* obj;
			obj = gluNewQuadric();
			gluQuadricDrawStyle(obj, GLU_FILL);

			Vector3f c = joints[i]->color;
			glColor3f(c.x, c.y, c.z);
			gluSphere(obj, 0.3f, 20, 20);
			gluDeleteQuadric(obj);

			glPopMatrix();
		}
	}
}

#include "MyPosture.h"

void MySkeleton::setPosture(const MyPosture& p)
{
	int joint_num = 0;
	for (int i = 0; i < numJoints; i++)
	{
		if (joints[i]->activate == 1) {
			joints[i]->localRot = p.Ori[joint_num];
			joint_num++;
		}
	}
	root->localPos = p.rootPosition;
}

float MySkeleton::getHeight()
{
	m_height = 0.0f;
	
	root->setGlobalTransform();

	float maxy;
	float miny;

	maxy=root->globalPos.y;
	miny= root->globalPos.y;
	for (int i = 0; i < numJoints; i++)
	{
		if (joints[i]->globalPos.y > maxy)
			maxy = joints[i]->globalPos.y;
		if (joints[i]->globalPos.y < miny)
			miny = joints[i]->globalPos.y;
	}

	m_height = maxy - miny;
	return m_height;
}