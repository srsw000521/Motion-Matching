#pragma once
#include <QMainWindow>
#include <QAction>
#include "MotionState.h"

class MotionGLWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void syncControlMenu();
    void syncViewMenu();

private:
    MotionState     m_state;
    MotionGLWidget* m_glWidget  = nullptr;

    // Actions whose check state must stay in sync with MotionGLWidget flags.
    QAction* m_actPlay             = nullptr;
    QAction* m_actShowSrc          = nullptr;
    QAction* m_actShowDst          = nullptr;
    QAction* m_actChangeTrajectory = nullptr;
    QAction* m_actReset            = nullptr;

    void createMenus();
};
