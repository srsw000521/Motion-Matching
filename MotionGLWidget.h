#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include "quat.h"       // Quat, ln(); transitively provides Vector3f via vector3d.h

class MotionState;
class MyJoint;
class MySkeleton;
class MyPosture;
class Motion;
class Trajectory;
class CMotionMatching;

class MotionGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit MotionGLWidget(MotionState* state, QWidget* parent = nullptr);
    ~MotionGLWidget() override = default;

private slots:
    void onTick();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    MotionState* m_state = nullptr;
    QTimer       m_timer;
    bool         m_bPlay = true;
    int          m_dFrame = 0;    // frame offset for pause-step; mirrors MFC m_dFrame
    QPoint       m_lastMousePos;

    // -----------------------------------------------------------------------
    // Camera state — values match CMotionMatchingView::OnInitialUpdate()
    // -----------------------------------------------------------------------
    Quat     m_Rotate;           // set in constructor
    Vector3f m_Trans   { 0.0f, 0.0f, 0.0f };
    float    m_Size    = 100.0f;
    float    m_Near    = -10001.0f;
    float    m_Far     =  10000.0f;

    // Render flags
    bool m_bShowAxis             = true;    // forced on for visual verification
    bool m_bShowLightSource      = true;
    bool m_bShowDstMotion        = false;
    bool m_bShowSrcMotion        = false;
    bool m_bShowSrcMotionRootOnly = false;
    bool m_bShowDstMotionRootOnly = false;
    int  m_Show1000Frame         = 0;

    // Viewport dimensions — kept in sync by resizeGL()
    int   m_CX     = 0;
    int   m_CY     = 0;
    float m_Aspect = 1.0f;

    // -----------------------------------------------------------------------
    // Scene drawing — mirrors CMotionMatchingView methods
    // -----------------------------------------------------------------------
    void DrawScene();
    void DrawGrid();     // defined but not called from DrawScene in Qt Commit 5+
    void DrawBoard();
    void DrawAxis(int mode, float scale);
    void DrawSettingLight();
    void DrawObjects();

    // -----------------------------------------------------------------------
    // Render helpers — ported from CMotionMatchingView
    // -----------------------------------------------------------------------
    void RenderJoint(MyJoint* joint, Vector3f color, int lineWidth);
    void RenderJointShadow(MyJoint* joint, Vector3f color, int lineWidth);
    void RenderSkeleton(MySkeleton* skel, Vector3f color, int lineWidth,
                        bool bSphere, bool srcOnly, bool dstOnly, float distance);
    void RenderPosture(MyPosture* posture, MySkeleton* skel, Vector3f color,
                       bool bSphere, bool srcOnly, bool dstOnly);
    void RenderMotion(Motion* motion, int numFrames, int stFrame, int edFrame,
                      bool srcOnly, bool dstOnly);
    void RenderTrajectoryPath(Trajectory* traj);
    void RenderTrajectoryCurrentGoal(Trajectory* traj);
    void RenderTrajectoryFutureGoal(Trajectory* traj, int dframe);
    void RenderFuturePositions(CMotionMatching* mm);
};
