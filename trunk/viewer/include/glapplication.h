#ifndef GLAPPLICATION_H
#define GLAPPLICATION_H

#include <QApplication>

class GLApplication : public QApplication {
    Q_OBJECT
public:
    GLApplication(int argc, char *argv[]);
    static bool running(void);
    ~GLApplication();
public slots: 
    void slotLastWindowClosed();
private:
    static bool s_running;
};

#endif
