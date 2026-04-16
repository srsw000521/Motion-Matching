
// MotionMatchingView.h: CMotionMatchingView 클래스의 인터페이스
//

#pragma once

#include "Vector3d.h"
#include "quat.h"

#include <GL/freeglut.h>

class MyJoint;
class MySkeleton;
class MyPosture;
class Motion;
class Trajectory;
class CMotionMatching;

class CMotionMatchingView : public CView
{
protected: // serialization에서만 만들어집니다.
	CMotionMatchingView() noexcept;
	DECLARE_DYNCREATE(CMotionMatchingView)

// 특성입니다.
public:
	CMotionMatchingDoc* GetDocument() const;

	HGLRC m_hRC;
	CDC* m_pDC;
	virtual BOOL SetupPixelFormat(PIXELFORMATDESCRIPTOR* pPFD = 0);

	void bitmapInfo(int wi, int he, BITMAPINFO& DIBInfo, int& dx, int& dy);

	CPoint MouseDownPoint;
	CPoint MouseMovePoint;

	double X_Angle;
	double Y_Angle;
	Quat m_Rotate;
	Vector3f m_Trans;

	int m_CX;
	int m_CY;

	float m_Aspect;
	float m_Size;
	float m_Far;
	float m_Near;


	bool m_bRBut;
	bool m_bMBut;
	bool m_bLBut;
	bool m_bShowAxis;


	bool m_bPlay;
	bool m_bShowSrcMotion;
	bool m_bShowSrcMotionRootOnly;
	bool m_bShowDstMotionRootOnly;
	int m_Show1000Frame;

	bool m_bShowDstMotion;
	int m_trajectoryMode;


	int m_dFrame;
// 작업입니다.
public:

	void DrawAxis(int mode = 0, float scale = 1.0f);
	void DrawScene();
	void DrawGrid();
	void DrawObjects(void);
	void DrawBoard();

	void DrawSettingLight(void);

	bool m_bShowLightSource;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();

protected:

// 구현입니다.
public:
	virtual ~CMotionMatchingView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

private:
	void RenderJoint(MyJoint* joint, Vector3f color, int lineWidth);
	void RenderJointShadow(MyJoint* joint, Vector3f color, int lineWidth);
	void RenderSkeleton(MySkeleton* skel, Vector3f color, int lineWidth, bool bSphere,
	                    bool srcOnly, bool dstOnly, float distance);
	void RenderPosture(MyPosture* posture, MySkeleton* skel, Vector3f color,
	                   bool bSphere, bool srcOnly, bool dstOnly);
	void RenderMotion(Motion* motion, int numFrames, int stFrame, int edFrame,
	                  bool srcOnly, bool dstOnly);
	void RenderTrajectoryPath(Trajectory* traj);
	void RenderTrajectoryCurrentGoal(Trajectory* traj);
	void RenderTrajectoryFutureGoal(Trajectory* traj, int dframe);
	void RenderFuturePositions(CMotionMatching* mm);

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnControlPlay();
	afx_msg void OnShowSrcMotion();
	afx_msg void OnUpdateShowSrcMotion(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnViewShowSrc1000frame();
	afx_msg void OnShowSrcMotionRootOnly();
	afx_msg void OnMotionmatchingChangetrajectory();
	afx_msg void OnViewShowDstMotion();
	afx_msg void OnShowDstMotionRootOnly();
};

#ifndef _DEBUG  // MotionMatchingView.cpp의 디버그 버전
inline CMotionMatchingDoc* CMotionMatchingView::GetDocument() const
   { return reinterpret_cast<CMotionMatchingDoc*>(m_pDocument); }
#endif

