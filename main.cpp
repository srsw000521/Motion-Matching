#include <QApplication>
#include <QSurfaceFormat>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    // Configure the default OpenGL surface format before QApplication is
    // constructed. QOpenGLWidget inherits this format (Qt Commit 3 onwards).
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(16);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
