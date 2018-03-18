#ifndef LORENZOBJECTS_H
#define LORENZOBJECTS_H

#include "renderobjects.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

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


class CLorezLine : public CBaseObjectFactory
{
public:
    CLorezLine();
    ~CLorezLine();
protected:
    virtual bool createBuffers();
    virtual void uniformsAndDraw();
    virtual void deleteBuffers();

private:
    enum VAO_IDs { BasicObject, NumVAOs };
    enum Buffer_IDs { VertexBuffer, ColorBuffer,TextCoordBuffer, NumBuffers };
    enum Attrib_IDs { vVertexPosition = 0 , vVertexColor = 1 , vVertexTexCoord = 2};
    enum Texture_IDs { HSVtexture, NumTextures};


    GLuint Buffers[NumBuffers];
    GLuint Textures[NumTextures];

    QOpenGLTexture *textureTest;

    int iNumOfPoints = 4000;
    float texOffset=1.0f;


};



#endif // LORENZOBJECTS_H
