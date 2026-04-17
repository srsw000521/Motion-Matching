// freeglut.h must precede Qt GL headers to avoid Windows GL type redefinitions.
// It provides legacy immediate-mode GL (glBegin/glEnd/glVertex/...) and GLU
// (gluNewQuadric/gluSphere/gluCylinder/...) which are not part of QOpenGLFunctions.
#include <GL/freeglut.h>
#include <cmath>
#include "MotionGLWidget.h"
#include "MotionState.h"    // transitively: CMotionMatching, Motion, MySkeleton, Trajectory
#include "MyJoint.h"
#include "MyPosture.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

MotionGLWidget::MotionGLWidget(MotionState* state, QWidget* parent)
    : QOpenGLWidget(parent), m_state(state)
{
    // Match CMotionMatchingView::OnInitialUpdate()
    m_Rotate = Quat(5.0f * 3.14f / 180.0f, -5.0f * 3.14f / 180.0f, 0.0f, 1.0f);

    connect(&m_timer, &QTimer::timeout, this, &MotionGLWidget::onTick);
    m_timer.start(30);

    setFocusPolicy(Qt::StrongFocus);
}

// ---------------------------------------------------------------------------
// QOpenGLWidget overrides
// ---------------------------------------------------------------------------

void MotionGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
}

void MotionGLWidget::resizeGL(int w, int h)
{
    m_CX = w;
    m_CY = h;
    m_Aspect = (h > 0) ? float(w) / float(h) : 1.0f;
    glViewport(0, 0, w, h);
}

void MotionGLWidget::paintGL()
{
    DrawScene();
}

void MotionGLWidget::onTick()
{
    if (m_bPlay && m_state)
    {
        m_state->forwardFrame();
        m_state->applyMotionMatching();
        update();
    }
}

void MotionGLWidget::togglePlay()          { m_bPlay          = !m_bPlay;          update(); }
void MotionGLWidget::toggleShowSrcMotion() { m_bShowSrcMotion = !m_bShowSrcMotion; update(); }
void MotionGLWidget::toggleShowDstMotion() { m_bShowDstMotion = !m_bShowDstMotion; update(); }
void MotionGLWidget::toggleProjection()    { m_bPerspective   = !m_bPerspective;   update(); }

void MotionGLWidget::changeTrajectory()
{
    // Mirrors CMotionMatchingView::OnMotionmatchingChangetrajectory() exactly.
    m_trajectoryMode = (m_trajectoryMode + 1) % 3;
    if (m_state && m_state->trajectory)
        m_state->trajectory->initTrajectory(m_trajectoryMode);
    update();
}

void MotionGLWidget::resetPlayback()
{
    if (m_state)
        m_state->reset();
    m_dFrame = 0;
    update();
}

// ---------------------------------------------------------------------------
// Camera input — Qt mouse / wheel handlers
// ---------------------------------------------------------------------------

void MotionGLWidget::mousePressEvent(QMouseEvent* e)
{
    m_lastMousePos = e->pos();
}

void MotionGLWidget::mouseReleaseEvent(QMouseEvent* /*e*/)
{
    // no state to clear — button tracking delegated to e->buttons() in move
}

void MotionGLWidget::mouseMoveEvent(QMouseEvent* e)
{
    const int dx =  e->x() - m_lastMousePos.x();
    const int dy =  e->y() - m_lastMousePos.y();
    m_lastMousePos = e->pos();

    if (e->buttons() & Qt::LeftButton)
    {
        // Orbit: horizontal drag → world-Y rotation, vertical drag → world-X rotation.
        // 0.5 deg/pixel matches the MFC sensitivity.
        const float scale = 3.141592f / 360.0f;

        Quat pitchDelta, yawDelta;
        pitchDelta.AngleAxis(dy * scale, Vector3f(1.0f, 0.0f, 0.0f));
        yawDelta.AngleAxis(dx * scale, Vector3f(0.0f, 1.0f, 0.0f));

        // Left-multiply: apply new rotation on top of the existing accumulated rotation.
        m_Rotate = yawDelta * pitchDelta * m_Rotate;
        update();
    }
    else if (e->buttons() & Qt::MiddleButton)
    {
        // Pan: map pixel delta to world units using the ortho scale.
        // worldPerPixel = 2*m_Size / viewport_width; same in both axes (square pixels).
        if (m_CX > 0)
        {
            const float s = 2.0f * m_Size / float(m_CX);
            m_Trans[0] += dx * s;
            m_Trans[1] -= dy * s;   // screen Y is down; world Y is up
        }
        update();
    }
}

void MotionGLWidget::wheelEvent(QWheelEvent* e)
{
    // angleDelta positive = scroll toward user = zoom in (shrink m_Size).
    const float factor = (e->angleDelta().y() > 0) ? (1.0f / 1.1f) : 1.1f;
    m_Size = qMax(0.1f, m_Size * factor);
    update();
}

void MotionGLWidget::keyPressEvent(QKeyEvent* e)
{
    if (!m_state) { QOpenGLWidget::keyPressEvent(e); return; }

    switch (e->key())
    {
    case Qt::Key_Space:
        m_bPlay = !m_bPlay;
        if (m_bPlay) break;          // just resumed — timer handles the rest

        // Just paused: reset offset, advance state one step, redraw.
        // Mirrors MFC OnKeyDown VK_SPACE pause branch exactly.
        m_dFrame = 0;
        m_state->forwardFrame();
        m_state->applyMotionMatching();
        update();
        break;

    case Qt::Key_Left:
        // Step one frame back into history; no lower-bound guard (matches MFC).
        m_dFrame--;
        update();
        break;

    case Qt::Key_Right:
        // Can only step forward if we are behind the current state position.
        // Matches MFC guard: if (m_dFrame >= 0) break;
        if (m_dFrame >= 0) break;
        m_dFrame++;
        update();
        break;

    default:
        QOpenGLWidget::keyPressEvent(e);
        break;
    }
}

// ---------------------------------------------------------------------------
// Scene drawing — ported from CMotionMatchingView
// ---------------------------------------------------------------------------

void MotionGLWidget::DrawScene()
{
    const float bg = 56.0f / 255.0f;
    glEnable(GL_DEPTH_TEST);
    glClearColor(bg, bg, bg, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // DrawGrid() removed from render path in Qt Commit 5 — method still exists.

    glPolygonMode(GL_FRONT, GL_FILL);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (m_Size < 0.0f) m_Size = 0.0f;
    if (m_bPerspective)
    {
        // EXPERIMENTAL: gluPerspective with camera pulled back by m_Size*6 so
        // wheel-zoom still narrows/widens the view in a useful way.
        // near = m_Size*0.1 keeps the near plane scaled with zoom level.
        gluPerspective(45.0, m_Aspect, m_Size * 0.1f, m_Size * 200.0f);
    }
    else
    {
        glOrtho(-m_Size, m_Size, -m_Size / m_Aspect, m_Size / m_Aspect, m_Near, m_Far);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // In perspective mode the camera must be physically behind the scene;
    // in ortho mode -20 is sufficient (z has no foreshortening effect).
    glTranslatef(0.0f, 0.0f, m_bPerspective ? -(m_Size * 6.0f) : -20.0f);

    DrawSettingLight();

    Vector3f rot = ln(m_Rotate);
    rot = rot / 3.141592f * 180.0f * 2.0f;
    float leng = sqrt(rot % rot);
    if (leng > 0.0001f)
    {
        rot = rot / leng;
        glRotated(leng, rot.x, rot.y, rot.z);
    }
    glTranslatef(m_Trans[0], m_Trans[1], m_Trans[2]);

    if (m_bShowAxis) DrawAxis(1, m_Size / 5.0f);

    DrawBoard();

    DrawObjects();
}

void MotionGLWidget::DrawGrid()
{
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (m_Size < 0.0f) m_Size = 0.0f;
    glOrtho(-m_Size, m_Size, -m_Size / m_Aspect, m_Size / m_Aspect, m_Near, m_Far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(75.0f / 255.0f, 75.0f / 255.0f, 75.0f / 255.0f);

    float tt = 1.0f;
    while (true)
    {
        if (tt > m_Size) break;
        tt *= 10.0f;
    }
    float step = tt / 10.0f;
    for (float x = -m_Size; x < m_Size; x += step)
    {
        glVertex2f(x, -m_Size / m_Aspect);
        glVertex2f(x,  m_Size / m_Aspect);
    }
    for (float y = -m_Size / m_Aspect; y < m_Size / m_Aspect; y += step)
    {
        glVertex2f(-m_Size, y);
        glVertex2f( m_Size, y);
    }
    glEnd();

    glEnable(GL_DEPTH_TEST);
}

void MotionGLWidget::DrawBoard()
{
    GLint colorCode = 1;

    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(-300, 0, -300);
    glRotatef(90, 1, 0, 0);
    for (GLint y = 25; y <= 525; y += 25)
    {
        for (GLint x = 25; x <= 525; x += 25)
        {
            if (colorCode == 1) {
                glColor3f(0.75f, 0.75f, 0.8f);
                colorCode = 0;
            } else {
                glColor3f(0.9f,  0.9f,  1.0f);
                colorCode = 1;
            }
            glNormal3f(0, 0, 1);
            glBegin(GL_QUADS);
            glVertex2i(x,      y);
            glVertex2i(x,      y + 25);
            glVertex2i(x + 25, y + 25);
            glVertex2i(x + 25, y);
            glEnd();
        }
    }
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void MotionGLWidget::DrawAxis(int mode, float scale)
{
    GLUquadricObj* q = gluNewQuadric();
    gluQuadricDrawStyle(q, GLU_FILL);
    gluQuadricNormals(q, GLU_SMOOTH);

    glPushMatrix();

    if (mode == 0)
    {
        glTranslatef(-(10.0f - 1.0f), -(10.0f) / m_Aspect + 1.0f, 0.0f);
        Vector3f rot = ln(m_Rotate);
        rot = rot / 3.141592f * 180.0f * 2.0f;
        float leng = sqrt(rot % rot);
        if (leng > 0.0001f)
        {
            rot = rot / leng;
            glRotated(leng, rot.x, rot.y, rot.z);
        }
    }

    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f * scale, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f * scale, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f * scale);
    glEnd();
    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.0f * scale);
    gluCylinder(q, 0.1 * scale, 0.0, 0.2 * scale, 8, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0f * scale, 0.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(q, 0.1 * scale, 0.0, 0.2 * scale, 8, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 1.0f * scale, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(q, 0.1 * scale, 0.0, 0.2 * scale, 8, 1);
    glPopMatrix();

    glPopMatrix();

    gluDeleteQuadric(q);
}

void MotionGLWidget::DrawSettingLight()
{
    GLfloat global_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glShadeModel(GL_SMOOTH);

    GLfloat ambientLight0[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat diffuseLight0[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specularLight0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat position0[]      = { 300.0f, 300.0f, 300.0f, 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientLight0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseLight0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
    glLightfv(GL_LIGHT0, GL_POSITION, position0);

    GLfloat ambientLight1[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat diffuseLight1[]  = { 0.3f, 0.3f, 0.4f, 1.0f };
    GLfloat specularLight1[] = { 0.3f, 0.3f, 0.4f, 1.0f };
    GLfloat position1[]      = { -300.0f, -300.0f, 0.0f, 0.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT,  ambientLight1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  diffuseLight1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
    glLightfv(GL_LIGHT1, GL_POSITION, position1);

    glEnable(GL_FLAT);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat mspecular[]  = { 0.2f, 0.2f, 0.3f, 1.0f };
    GLfloat memission[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mspecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, memission);
    glMateriali(GL_FRONT, GL_SHININESS, 50);

    if (m_bShowLightSource)
    {
        GLUquadricObj* sphere = gluNewQuadric();
        glDisable(GL_LIGHTING);

        glPushMatrix();
        glTranslatef(position0[0], position0[1], position0[2]);
        glColor4fv(diffuseLight0);
        gluSphere(sphere, 2.00f, 16, 16);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(position1[0], position1[1], position1[2]);
        glColor4fv(diffuseLight1);
        gluSphere(sphere, 2.00f, 16, 16);
        glPopMatrix();

        gluDeleteQuadric(sphere);
        glEnable(GL_LIGHTING);
    }
}

// ---------------------------------------------------------------------------
// DrawObjects — mirrors CMotionMatchingView::DrawObjects(), MFC refs replaced
// ---------------------------------------------------------------------------

void MotionGLWidget::DrawObjects()
{
    if (!m_state || !m_state->mm) return;

    CMotionMatching* mm = m_state->mm;

    if (!mm->dstMotion.postures.empty())
    {
        int framenum = m_state->frameNum;
        if (framenum > (int)mm->dstMotion.postures.size() - 1)
            framenum = (int)mm->dstMotion.postures.size() - 1;
        mm->dstMotion.m_pSkeleton->setPosture(mm->dstMotion.postures[framenum]);
        RenderSkeleton(mm->dstMotion.m_pSkeleton, Vector3f(-1, -1, -1), 0, true, false, false, 0.0f);
        mm->computeErr();
        RenderTrajectoryCurrentGoal(m_state->trajectory);
        for (int i = 0; i < mm->srcMotion.m_numStep; i++)
            RenderTrajectoryFutureGoal(m_state->trajectory, mm->srcMotion.m_futureStep * (i + 1));
        RenderFuturePositions(mm);
    }

    RenderTrajectoryPath(m_state->trajectory);

    if (!mm->dstMotion.postures.empty())
    {
        int framenum = m_state->currentTime + m_dFrame;
        if (framenum < 0) framenum = 0;
        if (framenum > (int)mm->dstMotion.postures.size() - 1)
            framenum = (int)mm->dstMotion.postures.size() - 1;
        mm->dstMotion.m_pSkeleton->setPosture(mm->dstMotion.postures[framenum]);
        RenderSkeleton(mm->dstMotion.m_pSkeleton, Vector3f(-1, -1, -1), 0, true, false, false, 0.0f);
        mm->computeErr();
        RenderTrajectoryCurrentGoal(m_state->trajectory);
        for (int i = 0; i < mm->srcMotion.m_numStep; i++)
            RenderTrajectoryFutureGoal(m_state->trajectory, mm->srcMotion.m_futureStep * (i + 1));
        RenderFuturePositions(mm);
    }

    if (m_bShowDstMotion)
        RenderMotion(&mm->dstMotion, -1, -1, -1, m_bShowSrcMotionRootOnly, m_bShowDstMotionRootOnly);

    if (m_bShowSrcMotion)
    {
        if (m_Show1000Frame > mm->srcMotion.numof_frame / 1000)
            m_Show1000Frame = 0;
        if (m_Show1000Frame == 0)
            RenderMotion(&mm->srcMotion, -1, -1, -1, m_bShowSrcMotionRootOnly, false);
        else
            RenderMotion(&mm->srcMotion, 500, (m_Show1000Frame - 1) * 1000, m_Show1000Frame * 1000, false, false);
    }
}

// ---------------------------------------------------------------------------
// Render helpers — ported from CMotionMatchingView, MFC refs removed
// ---------------------------------------------------------------------------

void MotionGLWidget::RenderJoint(MyJoint* joint, Vector3f color, int lineWidth)
{
    glPushMatrix();

    if (lineWidth < 1)
        glLineWidth(5.0f);
    else
        glLineWidth(lineWidth);

    glBegin(GL_LINES);

    Vector3f c = joint->color;
    if (color.x >= 0.0f)
        c = color;

    if (joint->parent != nullptr) {
        glColor3f(c.x, c.y, c.z);
        glVertex3f(0, 0, 0);
        glVertex3f(joint->localPos.x, joint->localPos.y, joint->localPos.z);
    }

    glEnd();

    glTranslatef(joint->localPos.x, joint->localPos.y, joint->localPos.z);

    Vector3f rot = ln(joint->localRot);
    rot = rot / 3.141592f * 180.0f * 2.0f;
    float leng = sqrt(rot % rot);
    if (leng > 0.0001f)
    {
        rot = rot / leng;
        glRotatef(leng, rot.x, rot.y, rot.z);
    }

    glPushMatrix();
    glPopMatrix();

    if (joint->child   != nullptr) RenderJoint(joint->child,   color, lineWidth);
    glPopMatrix();
    if (joint->sibling != nullptr) RenderJoint(joint->sibling, color, lineWidth);
}

void MotionGLWidget::RenderJointShadow(MyJoint* joint, Vector3f color, int lineWidth)
{
    glPushMatrix();

    if (lineWidth < 1)
        glLineWidth(5.0f);
    else
        glLineWidth(lineWidth);

    if (joint->parent != nullptr)
    {
        glBegin(GL_LINES);
        glColor3f(color.x, color.y, color.z);
        glVertex3f(joint->shadowPos.x,          joint->shadowPos.y,          joint->shadowPos.z);
        glVertex3f(joint->parent->shadowPos.x,  joint->parent->shadowPos.y,  joint->parent->shadowPos.z);
        glEnd();
    }

    glPopMatrix();

    if (joint->child   != nullptr) RenderJointShadow(joint->child,   color, lineWidth);
    if (joint->sibling != nullptr) RenderJointShadow(joint->sibling, color, lineWidth);
}

void MotionGLWidget::RenderSkeleton(MySkeleton* skel, Vector3f color, int lineWidth,
                                    bool bSphere, bool srcOnly, bool dstOnly, float distance)
{
    if (!bSphere)
        glDisable(GL_LIGHTING);

    if (dstOnly)
    {
        skel->root->setGlobalTransform();
        Vector3f p = skel->root->globalPos;

        float height = skel->getHeight();
        float err = (height > 0.0f) ? (distance / height * 1.8f) : 0.0f;
        float h = 0.6f - err / 0.5f * 0.6f;
        if (h < 0.0f) h = 0.0f;

        skel->rootcolor = HSV2RGB(Vector3f(h, 1, 1));

        glColor3f(skel->rootcolor.x, skel->rootcolor.y, skel->rootcolor.z);
        glPushMatrix();
        glTranslatef(p.x, 0, p.z);
        GLUquadricObj* obj = gluNewQuadric();
        gluQuadricDrawStyle(obj, GLU_FILL);
        gluSphere(obj, 0.3f, 20, 20);
        gluDeleteQuadric(obj);
        glPopMatrix();
        return;
    }

    if (srcOnly)
    {
        skel->root->setGlobalTransform();
        Vector3f p = skel->root->globalPos;
        glColor3f(color.x, color.y, color.z);
        glPushMatrix();
        glTranslatef(p.x, 0, p.z);
        GLUquadricObj* obj = gluNewQuadric();
        gluQuadricDrawStyle(obj, GLU_FILL);
        gluSphere(obj, 0.3f, 20, 20);
        gluDeleteQuadric(obj);
        glPopMatrix();
        return;
    }

    RenderJoint(skel->root, color, lineWidth);
    skel->root->setGlobalTransform();
    RenderJointShadow(skel->root, Vector3f(0.3f, 0.3f, 0.3f), lineWidth);

    if (bSphere)
    {
        for (int i = 0; i < skel->numJoints; i++)
        {
            Vector3f p = skel->joints[i]->globalPos;
            glPushMatrix();
            glTranslatef(p.x, p.y, p.z);
            GLUquadricObj* obj = gluNewQuadric();
            gluQuadricDrawStyle(obj, GLU_FILL);
            glColor3f(skel->joints[i]->color.x, skel->joints[i]->color.y, skel->joints[i]->color.z);
            gluSphere(obj, 0.3f, 20, 20);
            gluDeleteQuadric(obj);
            glPopMatrix();
        }
    }
}

void MotionGLWidget::RenderPosture(MyPosture* posture, MySkeleton* skel, Vector3f color,
                                   bool bSphere, bool srcOnly, bool dstOnly)
{
    skel->setPosture(*posture);
    RenderSkeleton(skel, color, 1, bSphere, srcOnly, dstOnly, posture->m_distance);
}

void MotionGLWidget::RenderMotion(Motion* motion, int numFrames, int stFrame, int edFrame,
                                  bool srcOnly, bool dstOnly)
{
    if (motion->m_pSkeleton == nullptr) return;
    if (motion->postures.empty()) return;

    if (stFrame < 1) stFrame = 0;
    if (edFrame < 1) edFrame = (int)motion->postures.size();
    if (stFrame > edFrame) stFrame = edFrame;

    if (numFrames < 1) numFrames = edFrame;
    int step = (edFrame - stFrame) / numFrames;
    if (step < 1) step = 1;

    for (int i = stFrame; i < edFrame; i += step)
    {
        MyPosture& p = motion->postures[i];
        Vector3f c = Vector3f(0, float(i) / motion->postures.size(),
                                 1 - float(i) / motion->postures.size());
        RenderPosture(&p, motion->m_pSkeleton, c, false, srcOnly, dstOnly);
    }
}

void MotionGLWidget::RenderTrajectoryPath(Trajectory* traj)
{
    glLineWidth(10.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < traj->m_numSample; i++)
        glVertex3f(traj->m_trajectory[i].x, traj->m_trajectory[i].y, traj->m_trajectory[i].z);
    glEnd();
}

void MotionGLWidget::RenderTrajectoryCurrentGoal(Trajectory* traj)
{
    Vector3f p = traj->getPositionAt(traj->m_goalIndex);
    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);
    GLUquadricObj* obj = gluNewQuadric();
    gluQuadricDrawStyle(obj, GLU_FILL);
    glColor3f(1, 0, 0);
    gluSphere(obj, 0.7f, 20, 20);
    gluDeleteQuadric(obj);
    glPopMatrix();
}

void MotionGLWidget::RenderTrajectoryFutureGoal(Trajectory* traj, int dframe)
{
    Vector3f p = traj->getFuturePosition(dframe);
    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);
    GLUquadricObj* obj = gluNewQuadric();
    gluQuadricDrawStyle(obj, GLU_FILL);
    glColor3f(1, 0, 0);
    gluSphere(obj, 0.5f, 20, 20);
    gluDeleteQuadric(obj);
    glPopMatrix();
}

void MotionGLWidget::RenderFuturePositions(CMotionMatching* mm)
{
    if (mm->dstMotion.postures.empty()) return;

    GLUquadricObj* obj = gluNewQuadric();
    gluQuadricDrawStyle(obj, GLU_FILL);

    MyPosture& p = mm->dstMotion.postures.back();
    Quat q = p.rootOriY;

    for (int i = 0; i < mm->srcMotion.m_numStep; i++)
    {
        Vector3f pos = p.rootPosition + rotate(q, mm->srcMotion.m_features[mm->m_currentIndex].m_posFuture[i]);
        glPushMatrix();
        glTranslatef(pos.x, 0, pos.z);
        glColor3f(0, 1, 0);
        gluSphere(obj, 0.5f, 20, 20);
        glPopMatrix();
    }

    gluDeleteQuadric(obj);
}
