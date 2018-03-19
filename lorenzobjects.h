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


class CLorenzLine : public CBaseObjectFactory
{
public:
    CLorenzLine();
    ~CLorenzLine();
protected:
    virtual bool createBuffers();
    virtual void uniformsAndDraw();
    virtual void deleteBuffers();

private:
    static void createTextures(QOpenGLFunctions_4_0_Core *);
    enum VAO_IDs { BasicObject, NumVAOs };
    enum Buffer_IDs { VertexBuffer, TextCoordBuffer, NumBuffers };
    enum Attrib_IDs { vVertexPosition = 0 , vVertexTexCoord = 1};
    enum Texture_IDs { HSVtexture, HighlightTexture, RedTexture, GreenTexture, BlueTexture , NumTextures};


    GLuint Buffers[NumBuffers];
    static GLuint Textures[NumTextures];


    int iNumOfPoints = 100000;
    float texOffset=0.0f;

    static bool texturesCreated;


};



#endif // LORENZOBJECTS_H
