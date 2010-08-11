#ifndef MODEL_H
#define MODEL_H

#include <QGLWidget>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include "bbox3d.h"
#include "material.h"
#include "texcache.h"
#include "rply.h"

class Model  {
public:
    Model(TexCache *texcache);
    Model(void);
    ~Model();
    int load(const char *filename);
    void unload(void);
    void draw() const;
    void drawBare(void) const;
    const QVector3D &size(void) const;
    const QVector3D &center(void) const;
    void computeNormals(void);
    void computeTangents(void);
    void computeBBox(void);
    void createBuffers(void);
    bool hasColors(void) const;
    bool hasTexCoords(void) const;
    bool hasTangents(void) const;
    bool hasBitangents(void) const;
    const BBox3D &bbox(void) const;

private:
    int loadMesh(p_ply ply, const char *filename);
    int loadTextures(p_ply ply, const char *filename);
    QVector<QVector3D> m_positions;
    QVector<QVector4D> m_tangents;
    QVector<QVector3D> m_bitangents;
    QVector<QVector3D> m_normals;
    QVector<QVector3D> m_colors;
    QVector<QVector2D> m_texcoords;
    QVector<GLuint> m_triangles;
    QVector<Material> m_materials;
    BBox3D m_bbox;
    TexCache *m_texcache;
    QVector<GLuint> m_textures;
    GLuint m_vertexbuffer;
    GLuint m_elementbuffer;
    QString m_name;
};

#endif
