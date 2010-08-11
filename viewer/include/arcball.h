#ifndef ARCBALL_H
#define ARCBALL_H

#include <QMouseEvent>
#include <QVector2D>
#include <QVector3D>
#include <QQuaternion>
#include <qmath.h>

class Arcball {
public:
    Arcball(const QVector2D &center = QVector2D(), qreal radius = 1.0, 
            const QQuaternion &rotation = QQuaternion()): 
        m_center(center), m_radius(radius), m_rotation(rotation) 
    { ; }

    void resize(QVector2D center, float radius) {
        m_center = center;
        m_radius = radius;
    }

    void setRotation(const QQuaternion &rotation = QQuaternion()) {
        m_rotation = rotation;
    }

    void mousePressEvent(QMouseEvent *event) {
        m_down = getContactPoint(QVector2D(event->posF()));
        m_dragging = QQuaternion();
    }

    void mouseMoveEvent(QMouseEvent *event) {
        QVector3D current = getContactPoint(QVector2D(event->posF()));
        QVector3D cross = QVector3D::crossProduct(m_down, current);
        qreal dot = QVector3D::dotProduct(m_down, current);
         m_dragging = QQuaternion(dot, cross);
    }

    void mouseReleaseEvent(QMouseEvent *event) {
        (void) event;
        m_rotation = m_dragging * m_rotation;
        m_dragging = QQuaternion();
    }

    QQuaternion rotation(void) const {
        return m_dragging * m_rotation;
    }

private:
    // ball center and radius
    QVector2D m_center; 
    qreal m_radius;
    // mouse down position
    QVector3D m_down;
    // cumulative rotation
    QQuaternion m_rotation, m_dragging;

    // find contact point of mouse and ball
    QVector3D getContactPoint(QVector2D p) {
        QVector2D q = (p-m_center) / m_radius;
        // flip y because of screen coordinates
        q.setY(-q.y());
        qreal len = QVector2D::dotProduct(q, q);
        if (len >= 1.0) return QVector3D(q).normalized();
        else return QVector3D(q, qSqrt(1.0-len));
    }
};

#endif // ARCBALL_H
