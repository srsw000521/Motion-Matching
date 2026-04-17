#include <QMenuBar>
#include "MotionGLWidget.h"
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Load BVH data and wire CMotionMatching + Trajectory.
    // This mirrors CMotionMatchingDoc::OnNewDocument().
    m_state.init();

    m_glWidget = new MotionGLWidget(&m_state, this);
    setCentralWidget(m_glWidget);

    setWindowTitle("Motion Matching");
    resize(1024, 768);

    createMenus();
}

void MainWindow::createMenus()
{
    menuBar()->addMenu(tr("&File"));

    // ------------------------------------------------------------------
    // Control menu
    // ------------------------------------------------------------------
    QMenu* ctrlMenu = menuBar()->addMenu(tr("&Control"));

    m_actPlay = ctrlMenu->addAction(tr("Play"));
    m_actPlay->setCheckable(true);
    // triggered() fires after Qt has already toggled the action's own check
    // state; we delegate the flag toggle to the widget to keep it authoritative.
    connect(m_actPlay, &QAction::triggered, m_glWidget, &MotionGLWidget::togglePlay);
    // aboutToShow syncs the check mark to the widget's live state before display,
    // mirroring the MFC ON_UPDATE_COMMAND_UI pattern.
    connect(ctrlMenu, &QMenu::aboutToShow, this, &MainWindow::syncControlMenu);

    // ------------------------------------------------------------------
    // View menu
    // ------------------------------------------------------------------
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));

    m_actShowSrc = viewMenu->addAction(tr("Show Src Motion"));
    m_actShowSrc->setCheckable(true);
    connect(m_actShowSrc, &QAction::triggered, m_glWidget, &MotionGLWidget::toggleShowSrcMotion);

    m_actShowDst = viewMenu->addAction(tr("Show Dst Motion"));
    m_actShowDst->setCheckable(true);
    connect(m_actShowDst, &QAction::triggered, m_glWidget, &MotionGLWidget::toggleShowDstMotion);

    viewMenu->addSeparator();

    m_actPerspective = viewMenu->addAction(tr("Perspective (Experimental)"));
    m_actPerspective->setCheckable(true);
    connect(m_actPerspective, &QAction::triggered, m_glWidget, &MotionGLWidget::toggleProjection);

    connect(viewMenu, &QMenu::aboutToShow, this, &MainWindow::syncViewMenu);

    // ------------------------------------------------------------------
    // MotionMatching menu
    // ------------------------------------------------------------------
    QMenu* mmMenu = menuBar()->addMenu(tr("&MotionMatching"));

    m_actChangeTrajectory = mmMenu->addAction(tr("Change Trajectory"));
    connect(m_actChangeTrajectory, &QAction::triggered,
            m_glWidget, &MotionGLWidget::changeTrajectory);

    m_actReset = mmMenu->addAction(tr("Reset"));
    connect(m_actReset, &QAction::triggered,
            m_glWidget, &MotionGLWidget::resetPlayback);
}

void MainWindow::syncControlMenu()
{
    m_actPlay->setChecked(m_glWidget->isPlaying());
}

void MainWindow::syncViewMenu()
{
    m_actShowSrc->setChecked(m_glWidget->isShowingSrcMotion());
    m_actShowDst->setChecked(m_glWidget->isShowingDstMotion());
    m_actPerspective->setChecked(m_glWidget->isPerspective());
}
