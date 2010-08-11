#ifndef ZOOM_H
#define ZOOM_H

#include <QWheelEvent>
#include <QDebug>
#include <qmath.h>

class Zoom {
public:
    Zoom(void): m_scale(1.0)
    { ; }

    void setScale(qreal scale = 1.0) {
        m_scale = scale;
    }

    void wheelEvent(QWheelEvent *event) {
        qreal degrees = 0.0f;
        if (event->orientation() == Qt::Vertical)
            degrees = event->delta() / 8.0;
        m_scale *= qPow(2.0, degrees/90.0);
        event->accept();
    }

    qreal scale(void) const {
        return m_scale;
    }

private:
    qreal m_scale;
};

#endif // ZOOM_H
