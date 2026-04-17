#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class MotionGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit MotionGLWidget(QWidget* parent = nullptr);
    ~MotionGLWidget() override = default;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    int   m_CX     = 0;
    int   m_CY     = 0;
    float m_Aspect = 1.0f;
};
