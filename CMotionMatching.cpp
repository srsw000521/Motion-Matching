#include <cstdio>
#include <cfloat>
#include <cmath>
#include <vector>
#include "CMotionMatching.h"

CMotionMatching::CMotionMatching()
{
	m_currentIndex = 100;
	m_currentTime = 0;
	m_lastTime = 0;

	srcMotion.initMotion();
	srcMotion.readMotionFile();
	srcMotion.m_pSkeleton->rootpos = srcMotion.rootposition[0];
	srcMotion.initFeature();

//	trajectory = new Trajectory();
	
	dstMotion.initMotion();

	positionErr = 0.0;
	poseErr = 0.0;
	err = 0.0;




}

CMotionMatching::~CMotionMatching()
{
}

int CMotionMatching::findBestMatchingIndex()
{
	double bestCost = DBL_MAX;
	int bestIndex = -1;
	for (int i = 0; i < srcMotion.numof_frame - 100; i++) {
		int candidateIndex = i;
		double thisCost = computeCost(candidateIndex);
		if (thisCost < bestCost) {
			bestCost = thisCost;
			bestIndex = candidateIndex;
		}
	}

	if (abs(m_currentIndex - bestIndex) > 20 || (srcMotion.numof_frame - m_currentIndex) < 100) {
		printf("best : %d ", bestIndex);
		computeCost(bestIndex, true);
//		m_currentIndex = bestIndex;
	}
	else bestIndex = m_currentIndex + 1;

	//return m_currentIndex;
	return bestIndex;
}


double CMotionMatching::computeCost(int candidateIndex, bool bPrint)
{
	double cost = 0.0;
	double w1 = 1.0;
	double w2 = 5000.0;
	double c1 = computeCurrentCost(candidateIndex);
	double c2 = computeFutureCost(candidateIndex);

	cost += w1 * c1;
	cost += w2 * c2;

	if (bPrint) {
		poseErr += c1;
		printf("c1 = %lf c2 = %lf Cost : %lf\n", w1 * c1, w2 * c2, cost);
	} 

	return cost;
}

double CMotionMatching::computeCurrentCost(int candidateIndex)
{
	double cost = 0.0;
	double w1 = 10.0;
	double w2 = 300.0;
	double w3 = 10.0;
	double w4 = 300.0;
	double w5 = 10.0;
	double w6 = 300.0;
	double w7 = 10.0;
	double w8 = 300.0;
	double w9 = 10.0;
	double w10 = 10.0;


	cost += w1 * (srcMotion.m_features[m_currentIndex].m_hipVel - srcMotion.m_features[candidateIndex].m_hipVel).Length2();
	cost += w2 * (srcMotion.m_features[m_currentIndex].m_LFootPos - srcMotion.m_features[candidateIndex].m_LFootPos).Length2();
	cost += w3 * (srcMotion.m_features[m_currentIndex].m_LFootVel - srcMotion.m_features[candidateIndex].m_LFootVel).Length2();
	cost += w4 * (srcMotion.m_features[m_currentIndex].m_RFootPos - srcMotion.m_features[candidateIndex].m_RFootPos).Length2();
	cost += w5 * (srcMotion.m_features[m_currentIndex].m_RFootVel - srcMotion.m_features[candidateIndex].m_RFootVel).Length2();
	cost += w6 * (srcMotion.m_features[m_currentIndex].m_LHandPos - srcMotion.m_features[candidateIndex].m_LHandPos).Length2();
	cost += w7 * (srcMotion.m_features[m_currentIndex].m_LHandVel - srcMotion.m_features[candidateIndex].m_LHandVel).Length2();
	cost += w8 * (srcMotion.m_features[m_currentIndex].m_RHandPos - srcMotion.m_features[candidateIndex].m_RHandPos).Length2();
	cost += w9 * (srcMotion.m_features[m_currentIndex].m_RHandVel - srcMotion.m_features[candidateIndex].m_RHandVel).Length2();
	
	if (dstMotion.postures.size() > 0)
	{
		MyPosture& p = dstMotion.postures.back();
		cost += w10 * (p.rootPosY - srcMotion.postures[candidateIndex].rootPosY).Length2();
	}

	return cost;
}




Vector3f Normalize(const Vector3f& v) {
	float length = v.Length();
	if (length == 0) {
		return Vector3f(0, 0, 0);
	}
	return Vector3f(v.x / length, v.y / length, v.z / length);
}

double CMotionMatching::computeFutureCost(int candidateIndex)
{
	Vector3f curGoalPos = trajectory->getCurrentGoalPosition();
	double cost = 0.0f;
	MyPosture& p = dstMotion.postures.back();

	double w1 = 1.0;
	double w2 = 0.1;
	//double w2 = 0.0;

	double gamma = 0.7;

	double sum_gamma = 0.0;

	for (int i = 0; i < this->srcMotion.m_numStep; i++)
	{
		int future = (i + 1) * srcMotion.m_futureStep;
		Vector3f nexGoalPos = trajectory->getFuturePosition(future);
		Vector3f g = p.rootPosition + rotate(p.rootOriY, srcMotion.m_features[candidateIndex].m_posFuture[i]);
		g.y = 0.0f;

		int prevFuture = future - 1;
		int nexFuture = future + 1;
		Vector3f prevFutureGoalPos = trajectory->getFuturePosition(prevFuture);
		Vector3f nexFutureGoalPos = trajectory->getFuturePosition(nexFuture);

		Vector3f prevFuturePos = p.rootPosition + rotate(p.rootOriY, srcMotion.m_features[candidateIndex].m_velFuture[i].first);
		Vector3f nexFuturePos = p.rootPosition + rotate(p.rootOriY, srcMotion.m_features[candidateIndex].m_velFuture[i].second);
		prevFuturePos.y = 0;
		nexFuturePos.y = 0;

		double dt = srcMotion.frame_time;

		Vector3f nexGoalVel = (nexFutureGoalPos - prevFutureGoalPos) / (2*dt);
		Vector3f v = (nexFuturePos - prevFuturePos) / (2*dt);

		double gg = pow(gamma, i);
		
		cost += gg*w1*(nexGoalPos - g).Length2();
		cost += gg*w2*(nexGoalVel - v).Length2();
		//printf("        %lf\n", (nexGoalVel - v).Length2());

		sum_gamma += gg * w1;
	}
	
	if (srcMotion.m_numPastStep < 1) return cost / sum_gamma;

	for (int i = 0; i < this->srcMotion.m_numPastStep; i++)
	{
		int future = (i + 1) * srcMotion.m_pastStep;
		Vector3f nexGoalPos = trajectory->getFuturePosition(-future);
		Vector3f g = p.rootPosition + rotate(p.rootOriY, srcMotion.m_features[candidateIndex].m_posPast[i]);
		g.y = 0.0f;

		cost += w1 * (nexGoalPos - g).Length2();
		//printf("        %lf\n", (nexGoalVel - v).Length2());
	}
	
	return cost / (srcMotion.m_numStep + srcMotion.m_numPastStep);
}
double CMotionMatching::computeErr()
{
	int lastFrame = dstMotion.postures.size() - 1;

	dstMotion.m_pSkeleton->setPosture(dstMotion.postures.back());
	dstMotion.m_pSkeleton->root->setGlobalTransform();
	Vector3f currentPosition = Vector3f(dstMotion.m_pSkeleton->joints[0]->globalPos.x, 0, dstMotion.m_pSkeleton->joints[0]->globalPos.z);
	Vector3f currentGoal = trajectory->getCurrentGoalPosition();

	double curErr = (currentPosition - currentGoal).Length2();
	//printf("curErr : %lf  curretPos: %f %f goal Pos: %f %f \n", curErr, currentPosition.x, currentPosition.z, currentGoal.x, currentGoal.z);
//	printf("curErr : %lf\n", curErr);
	dstMotion.postures[lastFrame].m_distance = sqrt(curErr);
/*
	positionErr += curErr;
	if (trajectory->m_goalIndex == 0 ) {
		printf("positionErr : %lf  poseErr : %lf\n", positionErr,poseErr);
		positionErr = 0.0;
		poseErr = 0.0;
	}
*/
	return sqrt(curErr);
}
void CMotionMatching::testSetup()
{
	for (int i = 0; i < 200; i+=2)
		dstMotion.addDSTPosture(i, &srcMotion);
}


void CMotionMatching::setStartPosition()
{
	dstMotion.m_startPosition = trajectory->getPositionAt(0);
}


void CMotionMatching::applyMotionMatching()
{
	static int previousIndex = 0;
	static MyPosture dPos(dstMotion.m_pSkeleton->numJoints);

	if(m_currentTime == 0) setStartPosition();
	trajectory->setCurrentTime(m_currentTime);

	if (m_currentTime - m_lastTime > 20) {

		previousIndex = m_currentIndex;
		int bestMatchingIndex = findBestMatchingIndex();
		m_currentIndex = bestMatchingIndex;

		dstMotion.addDSTPosture(m_currentIndex, &(srcMotion));
		m_lastTime = m_currentTime;

		if (previousIndex != bestMatchingIndex)
			dPos.setDifference(srcMotion.getPosture(previousIndex), srcMotion.getPosture(bestMatchingIndex));
	}
	else
	{
		m_currentIndex++;
		dstMotion.addDSTPosture(m_currentIndex, &(srcMotion));
	}
	int passFrame = m_currentTime - m_lastTime;
	float t = passFrame / 20.0f;
	if (t > 1.0f) t = 1.0f;
	if (m_currentTime < 20) t = 1.0f;

	if (t < 1.0f) 	dstMotion.applyBlending(dPos, t);
	//printf("curFr = %d passFr = %d t = %f prev = %d curr = %d\n", m_currentTime-1, passFrame, t, previousIndex, MotionMatching->m_currentIndex);
	m_currentTime++;
}

int CMotionMatching::reset()
{
	m_currentIndex = 100;
	m_currentTime = 0;
	m_lastTime = 0;

	dstMotion.postures.clear();

	positionErr = 0.0;
	poseErr = 0.0;
	err = 0.0;
	return 0;
}


void CMotionMatching::getCurrentCostBound()
{
	long count = 0;
	double sum = 0;
	double max = 0;
	double min = FLT_MAX;
	double avg = 0;
	double std = 0;
	std::vector<double> errs;

	for (int i = 100; i < srcMotion.numof_frame-100; i++)
	{
		m_currentIndex = i;
		for (int j = 100; j < srcMotion.numof_frame - 100; j++)
		{
			if (i == j) continue;
			double err = computeCurrentCost(j);
			sum += err;
			if (err > max) max = err;
			if (err < min) min = err;
			errs.push_back(err);
		}
		printf("%d \n", i);
	}
	avg = sum / errs.size();
	for (double e : errs)
	{
		std += ((e - avg) * (e - avg));
	}
	std /= errs.size();
	std = sqrt(std);

	printf("avg = %lf \n", avg);
	printf("std = %lf \n", std);
	printf("min = %lf \n", min);
	printf("max = %lf \n", max);
}
