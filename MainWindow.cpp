#include <QMenuBar>
#include "MotionGLWidget.h"
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Load BVH data and wire CMotionMatching + Trajectory.
    // This mirrors CMotionMatchingDoc::OnNewDocument().
    m_state.init();

    setCentralWidget(new MotionGLWidget(this));

    setWindowTitle("Motion Matching");
    resize(1024, 768);

    createMenus();
}

void MainWindow::createMenus()
{
    // Empty menus — actions and slots are added in Qt Commit 9.
    menuBar()->addMenu(tr("&File"));
    menuBar()->addMenu(tr("&Control"));
    menuBar()->addMenu(tr("&View"));
    menuBar()->addMenu(tr("&MotionMatching"));
}
