#pragma once
#include <QMainWindow>
#include "MotionState.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    MotionState m_state;

    void createMenus();
};
