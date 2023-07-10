#include "pch.h"
#include "MotionFeatured.h"

MotionFeatured::MotionFeatured()
{
	m_features = nullptr;
	m_futureStep = 20;
	m_numStep = 3;
	m_pastStep = m_futureStep;
	m_numPastStep = 1;
}

MotionFeatured::~MotionFeatured()
{
	releaseMemory();
}
void MotionFeatured::allocMemory()
{
	Motion::allocMemory();
	m_features = new FeatureVector[numof_frame];
}
void MotionFeatured::releaseMemory()
{
	Motion::releaseMemory();
	delete[] m_features;
	m_features = nullptr;
}


void MotionFeatured::initFeature()
{
	assert (m_features != nullptr);

	Vector3f* hipPos = new Vector3f[numof_frame];
	Vector3f prev_lgFootPos;
	Vector3f prev_rgFootPos;
	Vector3f prev_rgHandPos;
	Vector3f prev_lgHandPos;

	for (int i = 0; i < numof_frame; i++)
	{
		MyPosture &p = postures[i];
		m_pSkeleton->setPosture(p);
		Quat inverseY = p.rootOriY;
		Inverse(inverseY);

		MyJoint* root = m_pSkeleton->root;
		root->setGlobalTransform();

		MyJoint* j = root->getByName("RightHand");
		Vector3f rh = j->globalPos - rootposition[i];
		m_features[i].m_RHandPos = rotate(inverseY, rh);

		Vector3f rgHandPos = j->globalPos;
		if (i == 0) prev_rgHandPos = rgHandPos;

		j = root->getByName("LeftHand");
		Vector3f lh = j->globalPos - rootposition[i];
		m_features[i].m_LHandPos = rotate(inverseY, lh);
		Vector3f lgHandPos = j->globalPos;
		if (i == 0) prev_lgHandPos = lgHandPos;

		j = root->getByName("RightFoot");
		Vector3f rf = j->globalPos - rootposition[i];
		m_features[i].m_RFootPos = rotate(inverseY, rf);
		Vector3f rgFootPos = j->globalPos;
		if(i==0) prev_rgFootPos = rgFootPos; 

		j = root->getByName("LeftFoot");
		Vector3f lf = j->globalPos - rootposition[i];
		m_features[i].m_LFootPos = rotate(inverseY, lf);
		Vector3f lgFootPos = j->globalPos;
		if(i==0) prev_lgFootPos = lgFootPos;

		hipPos [i] = root->globalPos;

		float dt = frame_time;
		int prev_i = (i>0) ? i - 1 : i;
		m_features[i].m_hipVel = rotate(inverseY, (hipPos[i] - hipPos[prev_i]) / dt);
		m_features[i].m_RHandPos = rotate(inverseY, (rgHandPos - prev_rgHandPos) / dt);
		m_features[i].m_LHandPos = rotate(inverseY, (lgHandPos - prev_lgHandPos) / dt);
		m_features[i].m_RFootVel = rotate(inverseY, (rgFootPos - prev_rgFootPos) / dt);
		m_features[i].m_LFootVel = rotate(inverseY, (lgFootPos - prev_lgFootPos) / dt);

		prev_lgHandPos = lgHandPos;
		prev_rgHandPos = rgHandPos;
		prev_lgFootPos = lgFootPos;
		prev_rgFootPos = rgFootPos;

	}
	Vector3f* HV = new Vector3f[numof_frame];
	Vector3f* RFV = new Vector3f[numof_frame];
	Vector3f* LFV = new Vector3f[numof_frame];
	Vector3f* RHV = new Vector3f[numof_frame];
	Vector3f* LHV = new Vector3f[numof_frame];

	for (int i = 3; i < numof_frame-3; i++) {
		HV[i] = Vector3f(0, 0, 0);
		RFV[i] = Vector3f(0, 0, 0);
		LFV[i] = Vector3f(0, 0, 0);
		RHV[i] = Vector3f(0, 0, 0);
		LHV[i] = Vector3f(0, 0, 0);

		float w[] = { 1,2,3,5,3,2,1 };
		float w_sum = 0.0f;
		for (int u = -3; u <= 3; u++)
		{
			HV[i] += w[u+3] * m_features[i + u].m_hipVel;
			RFV[i] += w[u + 3] * m_features[i + u].m_RFootVel;
			LFV[i] += w[u + 3] * m_features[i + u].m_LFootVel;
			RHV[i] += w[u + 3] * m_features[i + u].m_RHandPos;
			LHV[i] += w[u + 3] * m_features[i + u].m_LHandPos;
			w_sum += w[u + 3];
		}
		HV[i] /= w_sum;
		RFV[i] /= w_sum;
		LFV[i] /= w_sum;
		RHV[i] /= w_sum;
		LHV[i] /= w_sum;
	}
	for (int i = 3; i < numof_frame - 3; i++) {
		m_features[i].m_hipVel = HV[i];
		m_features[i].m_RFootVel = RFV[i];
		m_features[i].m_LFootVel = LFV[i];
		m_features[i].m_RHandPos = RHV[i];
		m_features[i].m_LHandPos = LHV[i];
	}
	delete[] HV;
	delete[] RFV;
	delete[] LFV;
	delete[] RHV;
	delete[] LHV;

	for (int i = 0; i < numof_frame; i++) {
		MyPosture& p = postures[i];
		Quat inverseY = p.rootOriY;
		Inverse(inverseY);

		Vector3f PlanarHipPos = Vector3f(hipPos[i].x, 0, hipPos[i].z);

		for (int j = 0; j < m_numStep; j++)
		{
			m_features[i].m_posFuture[j] = Vector3f(0, 0, 0);
			m_features[i].m_velFuture[j].first = Vector3f(0, 0, 0);
			m_features[i].m_velFuture[j].second = Vector3f(0, 0, 0);

			int frame = i + m_futureStep *(j+1);
			int prevFuture = frame - 1;
			int nexFuture = frame + 1;
			if (frame > numof_frame - 1)  frame = numof_frame - 1; 
			if (nexFuture > numof_frame - 1) nexFuture = numof_frame - 1;
			if (prevFuture < 0) prevFuture = 0;
			
			m_features[i].m_posFuture[j] = Vector3f(hipPos[frame].x, 0, hipPos[frame].z) - PlanarHipPos;
			m_features[i].m_posFuture[j] = rotate(inverseY, m_features[i].m_posFuture[j]);

			m_features[i].m_velFuture[j].first = Vector3f(hipPos[prevFuture].x,0, hipPos[prevFuture].z) - PlanarHipPos;
			m_features[i].m_velFuture[j].first = rotate(inverseY, m_features[i].m_velFuture[j].first);
			m_features[i].m_velFuture[j].second = Vector3f(hipPos[nexFuture].x, 0, hipPos[nexFuture].z) - PlanarHipPos;
			m_features[i].m_velFuture[j].second = rotate(inverseY, m_features[i].m_velFuture[j].second);
		}
	}

	for (int i = 0; i < numof_frame; i++) {
		MyPosture& p = postures[i];
		Quat inverseY = p.rootOriY;
		Inverse(inverseY);

		Vector3f PlanarHipPos = Vector3f(hipPos[i].x, 0, hipPos[i].z);

		for (int j = 0; j < m_numStep; j++)
		{
			m_features[i].m_posPast[j] = Vector3f(0, 0, 0);

			int frame = i - m_pastStep * (j + 1);
			if (frame < 0)  frame = 0;

			m_features[i].m_posPast[j] = Vector3f(hipPos[frame].x, 0, hipPos[frame].z) - PlanarHipPos;
			m_features[i].m_posPast[j] = rotate(inverseY, m_features[i].m_posFuture[j]);

		}
	}

	delete[] hipPos;
}
