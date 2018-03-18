#ifndef RENDEROBJECTS_H
#define RENDEROBJECTS_H

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include "matsnlights.h"

#include <GL/gl.h>
#include <QtCore>



class CBaseObjectFactory
{
public:
    CBaseObjectFactory(const QString &name, const QString &vert, const QString &frag);
    virtual ~CBaseObjectFactory(){}
    bool initialize(QObject *);
    bool paint(const QMatrix4x4 &modelViewProjection);
    bool paint(const QMatrix4x4 &modelViewProjection,const QMatrix4x4 &modelViewMatrix, const QMatrix4x4 &normalMatrix);
    bool paint(const QMatrix4x4 &modelViewProjection,const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &modelViewMatrix, const QMatrix4x4 &normalMatrix);
    bool createObject();
    void deleteObject();
protected:
    virtual bool createBuffers() = 0;
    virtual void uniformsAndDraw() = 0;
    virtual void deleteBuffers() = 0;
    bool bOk=true;
    QString qstrObjectName, qstrVertexFile, qstrFragmentFile;
    enum VAO_IDs { BaseObject, NumVAOs };
    GLuint VAOs[NumVAOs];
    QOpenGLShaderProgram *m_program;
    QOpenGLFunctions_4_0_Core* gl = 0;
private:
    CBaseObjectFactory(){}
};


class CCoordSys : public CBaseObjectFactory
{
public:
    CCoordSys();
    ~CCoordSys();
protected:
    virtual bool createBuffers();
    virtual void uniformsAndDraw();
    virtual void deleteBuffers();
    enum Buffer_IDs { CoordBuffer, ColorBuffer, NumBuffers };
    enum Attrib_IDs { vPosition = 0 , vColor = 1};
    GLuint Buffers[NumBuffers];
};


class CCuboid : public CBaseObjectFactory
{
public:
    CCuboid();
    ~CCuboid();
protected:
    virtual bool createBuffers();
    virtual void uniformsAndDraw();
    virtual void deleteBuffers();
    enum Buffer_IDs { CoordBuffer, NormalBuffer, IndexBuffer, NumBuffers };
    enum Attrib_IDs { vVertexPosition = 0 , vVertexNormal = 1 };
    //enum Texture_IDs { HSVtexture, NumTextures};


    GLuint Buffers[NumBuffers];
    //GLuint Textures[NumTextures];

    int iTriangleCount = 2;

    //QOpenGLTexture *textureTest;

};

class CToroid : public CBaseObjectFactory
{
public:
    CToroid();
    ~CToroid();
    void reshapeTorus(float outerRadius, float innerRadius, int rings, int segments);
    void reshapeTorus(int rings, int segments);
    void reshapeTorus(float outerRadius, float innerRadius);

protected:
    virtual bool createBuffers();
    virtual void uniformsAndDraw();
    virtual void deleteBuffers();
    enum Buffer_IDs { CoordBuffer, NormalBuffer, IndexBuffer, NumBuffers };
    enum Attrib_IDs { vVertexPosition = 0 , vVertexNormal = 1 };
    //enum Texture_IDs { HSVtexture, NumTextures};


    GLuint Buffers[NumBuffers];
    //GLuint Textures[NumTextures];

    int iTriangleCount = 2;
    int iRings=40;
    int iSegments=20;
    GLfloat fR1=1.0f;
    GLfloat fR2=0.4f;

    CMaterial mat;

    //QOpenGLTexture *textureTest;


};


class CPlane : public CBaseObjectFactory
{
public:
    CPlane();
    ~CPlane();

protected:
    virtual bool createBuffers();
    virtual void uniformsAndDraw();
    virtual void deleteBuffers();
    enum Buffer_IDs { CoordBuffer, NormalBuffer, IndexBuffer, NumBuffers };
    enum Attrib_IDs { vVertexPosition = 0 , vVertexNormal = 1 };


    GLuint Buffers[NumBuffers];

    int iTriangleCount = 2;
    CMaterial mat;


};


#endif // RENDEROBJECTS_H
