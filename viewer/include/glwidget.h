#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QFont>
#include <QTimer>

#include "arcball.h"
#include "chronos.h"
#include "zoom.h"
#include "model.h"
#include "bbox3d.h"
#include "texcache.h"

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void slotTimer();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void quit(void);
    void fps(QString newFPS);

private:
    Arcball m_modelarcball;
    Arcball m_lightarcball;
    Zoom m_zoom;
    QVector<Model *> m_models;
    BBox3D m_bbox;
    TexCache m_texcache;
    Chronos m_time;
    QFont m_font;
    QTimer m_timer;
    int m_frames;
    double m_start;
    double m_fps;
    bool m_modeldrag, m_lightdrag, m_wireframe;
    GLuint m_vertshader, m_fragshader, m_program;
};

#endif
