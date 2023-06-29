#include "pch.h"
#include "Motion.h"
Motion::Motion()
{
	filename = "";
	numof_frame = 0;
	numof_frame_persecond = 0;
	frame_time = 0;
	activated_joint = 0;
	m_pSkeleton = nullptr;
	rootposition = nullptr;

	m_startPosition = Vector3f(0, 0, 0);
}

void Motion::initMotion()
{
	if (isReady() == true) releaseMemory();

	numof_frame = 0;
	numof_frame_persecond = 0;
	filename = "LocomotionFlat01_000.bvh";
	if (m_pSkeleton == nullptr) {
		m_pSkeleton = new MySkeleton();
		m_pSkeleton->init();
	}
	activated_joint = m_pSkeleton->m_numofActivatedJoint;
}

void Motion::SeparateYRot(Quat &Ori, Quat &OriY, Quat& OriXZ) {
	
	Ori.Normalize();
	Vector3f oz(0, 0, 1);
	Vector3f nz = rotate(Ori, oz);

	nz.y = 0.0f;

	float rad = atan2(nz.x, nz.z);
	OriY.AngleAxis(rad, Vector3f(0, 1, 0));
	if (Ori % OriY < 0) OriY = -1.0f * OriY;

	Quat inverseY = OriY;
	Inverse(inverseY);
	OriXZ = inverseY * Ori;

}

void Motion::applyBlending(Motion* srcMotion, int previousIndex, float t)
{
	MyPosture& current = postures.back();
	MyPosture& src = srcMotion->getPosture(previousIndex);

	int joint_num = 0;
	for (int i = 1; i < current.m_numJoint; i++) {
		//printf("%f %f %f %f %f    ->    ", current.Ori[i].X(), current.Ori[i].Y(), current.Ori[i].Z(), current.Ori[i].W(), current.Ori[i].length());
		Quat q;
		q.Slerp(src.Ori[i], current.Ori[i], t);
		current.Ori[i] = q;
		//printf("%f %f %f %f %f\n", current.Ori[i].X(), current.Ori[i].Y(), current.Ori[i].Z(), current.Ori[i].W(), current.Ori[i].length());
	}
}

void Motion::applyBlending(MyPosture & dPos, float t)
{
	MyPosture& current = postures.back();

	int joint_num = 0;
	for (int i = 1; i < current.m_numJoint; i++) {

		Quat q;
		q.Slerp(dPos.Ori[i], Quat(0,0,0,1), t);
		Quat q_to = current.Ori[i];
		current.Ori[i] = q * q_to;
		//printf("%f %f %f %f %f\n", current.Ori[i].X(), current.Ori[i].Y(), current.Ori[i].Z(), current.Ori[i].W(), current.Ori[i].length());
	}
	/*printf("pre :    %f %f %f %f", current.rootOriXZ.X(), current.rootOriXZ.Y(), current.rootOriXZ.Z(), current.rootOriXZ.W(), current.rootOriXZ.length());
*/	
	Quat q;
	q.Slerp(dPos.rootOriXZ, Quat(0, 0, 0, 1), t);
	Quat q_to = current.rootOriXZ;
	current.rootOriXZ = q * q_to;

	current.rootPosition.y += (1-t) *(dPos.rootPosition.y);

}

void Motion::addPostureTest(Motion* srcMotion)
{




}

Quat FromTwoVectors(Vector3f a, Vector3f b) {

	a.Normalize();
	b.Normalize();

	float dot = a.Dot(b);
	Vector3f axis = a.Cross(b);
	float cross = axis.Normalize();
	//float theta = acos(dot);
	float theta = atan2(cross, dot);
	Quat quat;
	quat.AngleAxis(theta, axis);

	return quat;
}
void Motion::setDPosture(int frame)
{
	if (frame < 0 || frame > numof_frame - 1) return;

	int current = frame;
	int prev = frame -1;
	if (prev < 0) prev = 0;
	
	MyPosture& in_current = getPosture( current );
	MyPosture& in_prev = getPosture( prev );

	
	SeparateYRot(in_current.Ori[0], in_current.rootOriY, in_current.rootOriXZ);

	Quat invPreY = in_prev.rootOriY;
	Inverse(invPreY);

	Quat invCurY = in_current.rootOriY;
	Inverse(invCurY);

	in_current.rootOriDy = in_current.rootOriY * invPreY;

	Vector3f oriDxz = in_current.rootPosition - in_prev.rootPosition;
	oriDxz.y = 0.0f;

	in_current.rootPosDxz = rotate(invCurY, oriDxz);
	in_current.rootPosY = Vector3f(0, in_current.rootPosition.y, 0);

	return;

}

void Motion::addPosture(Vector3f rootPos, Vector3f* jointArray)
{
	MyPosture posture(m_pSkeleton->m_numofActivatedJoint, rootPos, jointArray);
	postures.push_back(posture);
}
void Motion::addDSTPosture(int framenum, Motion *srcMotion)
{		
	MyPosture np = srcMotion->getPosture(framenum);

	if (postures.size() > 0)
	{
		//printf("%d pos : %f %f %f", framenum, np.rootPosition.x, np.rootPosition.y, np.rootPosition.z);
		//printf("%d ori : %f %f %f %f %f", framenum, np.rootOriY.X(), np.rootOriY.Y(), np.rootOriY.Z(), np.rootOriY.W(),np.rootOriY.length());
		
		MyPosture prev = postures.back();
		
		np.rootOriY = np.rootOriDy * prev.rootOriY;
		np.Ori[0] = np.rootOriY * np.rootOriXZ;
		np.Ori[0] = np.rootOriY;
		
		np.rootPosition = prev.rootPosition + rotate(np.rootOriY, np.rootPosDxz);
		np.rootPosition.y = np.rootPosY.y;


		//printf(" -> %f %f %f \n", np.rootPosition.x, np.rootPosition.y, np.rootPosition.z);
		//printf("   ->    % f %f %f %f %f\n", np.Ori[0].X(), np.Ori[0].Y(), np.Ori[0].Z(), np.Ori[0].W(), np.Ori[0].length());
		postures.push_back(np);
	}
	else
	{
		np.rootPosition.x = m_startPosition.x;
		np.rootPosition.z = m_startPosition.z;

		/*Quat rotY;
		rotY.AngleAxis(-3.141592 / 2, Vector3f(0, 1, 0));

		np.Ori[0] = rotY * np.Ori[0];
		np.rootOriY = rotY * np.rootOriY;
		np.Ori[0].Normalize();*/

		postures.push_back(np);
	}
	
}

void Motion::allocMemory()
{
	rootposition = new Vector3f[numof_frame];
}
void Motion::releaseMemory()
{
	delete[] rootposition;
	rootposition = nullptr;

	filename = "";
	numof_frame = 0;
	numof_frame_persecond = 0;
	frame_time = 0;
	activated_joint = 0;
}

Motion::~Motion()
{
	releaseMemory();
}

void Motion::readMotionFile() {
	FILE* file = fopen(filename.c_str(), "r");
	char line[1000];

	bool flag_motion = 0;
	int cnt = 0;
	float ang_a, ang_b, ang_c;
	bool flag_root = 0;

	int frame_cnt = -2;
	int joint_cnt = 0;
	int num;
	char* token = nullptr;

	Vector3f *jointAngleArray = new Vector3f[activated_joint];
	Vector3f rootPos;
	Vector3f rootPosDT;

	if (!file) {
		printf("none");
	}
	while (!feof(file)) {
		fgets(line, sizeof(line), file);
		if (strcmp(line, "MOTION\n") == 0) {
			flag_motion = 1;
			continue;
		}
		if (flag_motion == 1) {
			token = strtok(line, " ");

			while (token != NULL) {
				if (strcmp(token, "Frames:") == 0) {
					token = strtok(NULL, " ");
					numof_frame = atof(token);
					allocMemory();
					break;
				}
				if (strcmp(token, "Frame") == 0) {
					token = strtok(NULL, " ");
					token = strtok(NULL, " ");
					frame_time = atof(token);
					numof_frame_persecond = (int)(1 / atof(token));
					break;
				}
				if (cnt % 3 == 0) {
					ang_a = atof(token);
				}
				else if (cnt % 3 == 1) {
					ang_b = atof(token);
				}
				else if (cnt % 3 == 2) {
					ang_c = atof(token);
					if (flag_root == 0) {
						rootposition[frame_cnt] = Vector3f(ang_a, ang_b, ang_c);    //
						rootPos = Vector3f(ang_a, ang_b, ang_c);
						rootPosDT = Vector3f(0, 0, 0);
						flag_root = 1;
					}
					else {
						jointAngleArray[joint_cnt] = Vector3f(ang_c, ang_b, ang_a);
						joint_cnt++;
					}
				}
				cnt++;
				token = strtok(NULL, " ");
			}
			addPosture(rootPos, jointAngleArray);
			frame_cnt++;
			flag_root = 0;
			joint_cnt = 0;
			cnt = 0;
		}
	}
	fclose(file);

	postures.pop_front(); 
	postures.pop_front();

	for (int i = 1; i < postures.size(); i++) {
		MyPosture& prev = postures[i - 1];
		MyPosture& cur = postures[i];

		for (int j = 0; j < cur.m_numJoint; j++)
		{
			if (prev.Ori[j] % cur.Ori[j] < 0)
				cur.Ori[j] = -1.0f * cur.Ori[j];
		}
	}
	for (int i = 0; i < postures.size(); i++) {
		setDPosture(i);
	}

}

void Motion::drawMotionGL(int numDrawingFrame, int stFrame, int edFrame, bool m_bShowSrcMotionRootOnly, bool m_bShowDstMotionRootOnly, Vector3f goalPos)
{
	if (m_pSkeleton == nullptr) return;
	if (postures.size() == 0) return;
	if (stFrame < 1) stFrame = 0;
	if (edFrame < 1) edFrame = postures.size();
	if (stFrame > edFrame) stFrame = edFrame;
	
	if (numDrawingFrame < 1) numDrawingFrame = edFrame;
	int step = (edFrame-stFrame) / numDrawingFrame;
	if (step < 1) step = 1;

	for (int i = stFrame; i < edFrame; i+=step)
	{
		MyPosture& p = postures[i];
		Vector3f c = Vector3f(0,float(i) / postures.size(), 1 - float(i) / postures.size());
		p.drawGL(m_pSkeleton, c, false, m_bShowSrcMotionRootOnly, m_bShowDstMotionRootOnly, goalPos);
	}
}