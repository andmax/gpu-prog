#include "glapplication.h"
#include <QDebug>

GLApplication::GLApplication(int argc, char *argv[]): QApplication(argc, argv) {
    connect(this, SIGNAL(lastWindowClosed()), 
        this, SLOT(slotLastWindowClosed()));
}

GLApplication::~GLApplication() {
    ;
}

bool GLApplication::running(void) {
    return s_running;
}

void GLApplication::slotLastWindowClosed() {
    s_running = false;
    qDebug() << "Last window closed!";
}

bool GLApplication::s_running = true;
