#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector3D>

class Material {
public:
    Material() {
        m_diffuseTexture = -1;
        m_normalmapTexture = -1;
        m_faceOffset = 0;
        m_faceCount = 0;
        m_diffuseRed = 255;
        m_diffuseGreen = 255;
        m_diffuseBlue = 255;
    }
    unsigned int faceOffset(void) const { return m_faceOffset; }
    void setFaceOffset(unsigned int faceOffset) { m_faceOffset = faceOffset; }
    unsigned int faceCount(void) const { return m_faceCount; }
    void setFaceCount(unsigned int faceCount) { m_faceCount = faceCount; }
    int diffuseTexture(void) const { return m_diffuseTexture; }
    int normalmapTexture(void) const { return m_normalmapTexture; }
    void setDiffuseTexture(int diffuseTexture) 
    { m_diffuseTexture = diffuseTexture; }
    void setNormalmapTexture(int normalmapTexture) 
    { m_normalmapTexture = normalmapTexture; }
    unsigned char diffuseRed(void) const { return m_diffuseRed; }
    unsigned char diffuseGreen(void) const { return m_diffuseGreen; }
    unsigned char diffuseBlue(void) const { return m_diffuseBlue; }
    void setDiffuseRed(unsigned char diffuseRed) { m_diffuseRed = diffuseRed; }
    void setDiffuseGreen(unsigned char diffuseGreen) 
    { m_diffuseGreen = diffuseGreen; }
    void setDiffuseBlue(unsigned char diffuseBlue) { m_diffuseBlue = diffuseBlue; }

private:
    int m_diffuseTexture, m_normalmapTexture; 
    unsigned int m_faceOffset;
    unsigned int m_faceCount;
    unsigned char m_diffuseRed, m_diffuseGreen, m_diffuseBlue;
};

#endif
