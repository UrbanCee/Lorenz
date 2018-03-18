#ifndef RENDEROBJECTS_H
#define RENDEROBJECTS_H

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include <QOpenGLFunctions>
#include <QOpenGLTexture>


class QOpenGLShaderProgram;
class QOpenGLFunctions_4_0_Core;

class LorenzCalc
{
  public:
    LorenzCalc();
    LorenzCalc(double x,double y,double z);
    void nextStep(double dT);
    static double a;
    static double b;
    static double c;
    double x,y,z;
};


class CLorezLine
{
public:
    bool initialize(QObject *);
    bool createBuffers();
    bool paint(QMatrix4x4 mat);

private:
    enum VAO_IDs { BasicObject, NumVAOs };
    enum Buffer_IDs { VertexBuffer, ColorBuffer,TextCoordBuffer, NumBuffers };
    enum Attrib_IDs { vVertexPosition = 0 , vVertexColor = 1 , vVertexTexCoord = 2};
    enum Texture_IDs { HSVtexture, NumTextures};


    GLuint VAOs[NumVAOs];
    GLuint Buffers[NumBuffers];
    GLuint Textures[NumTextures];

    QOpenGLShaderProgram *m_program;
    QOpenGLFunctions_4_0_Core* gl = 0;

    QOpenGLTexture *textureTest;

    int iNumOfPoints = 4000;
    float texOffset=1.0f;


};


#endif // RENDEROBJECTS_H
