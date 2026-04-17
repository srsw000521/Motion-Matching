#include "MotionGLWidget.h"

MotionGLWidget::MotionGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

void MotionGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    // Same background as the MFC view: float t = 56 / 255.0f
    const float bg = 56.0f / 255.0f;
    glClearColor(bg, bg, bg, 1.0f);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // DrawScene() is added in Qt Commit 4.
}
