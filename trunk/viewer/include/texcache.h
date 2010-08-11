#ifndef TEXCACHE_H
#define TEXCACHE_H

#include <QMap>
#include <QGLWidget>

// Qt's bindTexture() return the same texture whenever it is
// called with the same pixmap. In a sense it performs
// implements a kind of texture cache. Unfortunately, it
// does not perform reference counting. So when you call
// deleteTexture(), it doesn't matter how many bindTexture()
// calls happened before. The texture is deleted
// imediatelly. That's why we need this class.
class TexCache {
public:
    TexCache(QGLWidget *widget): m_widget(widget) { 
        ; 
    }

    GLuint bindTexture(const QPixmap &pixmap) {
        GLuint tex = m_widget->bindTexture(pixmap, GL_TEXTURE_2D);
        m_cache[tex]++;
        return tex;
    }

    void deleteTexture(GLuint tex) {
        if (m_cache.contains(tex)) {
            m_cache[tex]--;
            if (m_cache[tex] == 0) {
                m_widget->deleteTexture(tex);
                m_cache.remove(tex);
            }
        }
    }

private:
    QMap<GLuint, int> m_cache;
    QGLWidget *m_widget;
};

#endif
