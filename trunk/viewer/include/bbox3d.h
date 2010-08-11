#ifndef BBOX_H
#define BBOX_H

#include <QVector3D>
#include <cfloat>

class BBox3D {
public:
    BBox3D(void): m_min(FLT_MAX, FLT_MAX, FLT_MAX), 
                m_max(-FLT_MAX, -FLT_MAX, -FLT_MAX)
    { ; }

    BBox3D(const QVector3D &min, const QVector3D max): m_min(min), m_max(max) 
    { ; } 

    void merge(const QVector3D &p) {
        if (p.x() < m_min.x()) m_min.setX(p.x());
        if (p.y() < m_min.y()) m_min.setY(p.y());
        if (p.z() < m_min.z()) m_min.setZ(p.z());
        if (p.x() > m_max.x()) m_max.setX(p.x());
        if (p.y() > m_max.y()) m_max.setY(p.y());
        if (p.z() > m_max.z()) m_max.setZ(p.z());
    }

    void merge(const BBox3D &box) {
        merge(box.min());
        merge(box.max());
    }

    void reset(void) {
        m_min = QVector3D(FLT_MAX, FLT_MAX, FLT_MAX);
        m_max = QVector3D(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    const QVector3D &min() const  { return m_min; }
    void setMin(const QVector3D &min) {
        m_min = min;
    }

    const QVector3D &max() const  { return m_max; }
    void setMax(const QVector3D &max) {
        m_max = max;
    }

    QVector3D center() const { return (m_max + m_min)*0.5; }
    QVector3D size() const { return m_max - m_min; }
private:
    QVector3D m_min, m_max;
};

#endif // BBOX_H
