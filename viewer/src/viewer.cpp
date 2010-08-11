#include <QtGui>
#ifdef NEEDGLEE
#include "GLee.h"
#endif
#include <QDebug>
#include <QApplication>
#include <QMainWindow>
#include <QStatusBar>

#include "glwidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow win;
    GLWidget gl;
    win.setCentralWidget(&gl);
    gl.resize(gl.sizeHint());
    gl.setFocus();
    win.statusBar()->showMessage(QString("Ready"));
    QObject::connect(&gl, SIGNAL(fps(QString)), win.statusBar(), SLOT(showMessage(QString)));
    QObject::connect(&gl, SIGNAL(quit()), &app, SLOT(quit()));
    win.show();
    win.setWindowTitle("OpenGL Viewer");
    return app.exec();
}
