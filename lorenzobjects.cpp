#include "lorenzobjects.h"


#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLContext>
#include <QFileInfo>
#include <QColor>
#include <QtMath>
#include <QImage>
#include <QOpenGLTexture>


double LorenzCalc::a=10;
double LorenzCalc::b=28;
double LorenzCalc::c=8/3;




LorenzCalc::LorenzCalc():
    x(0.0),y(0.0),z(0.0)
{}

LorenzCalc::LorenzCalc(double x, double y, double z):
    x(x),y(y),z(z)
{}

void LorenzCalc::nextStep(double dT)
{
    double newX=x+(a*(y-x))*dT;
    double newY=y+(x*(b-z)-y)*dT;
    double newZ=z+(x*y-c*z)*dT;
    x=newX;
    y=newY;
    z=newZ;
}



CLorezLine::CLorezLine()
    : CBaseObjectFactory("Lorenz Line",":/Shader/texturedLine.vert",":/Shader/texturedLine.frag")
{}

CLorezLine::~CLorezLine()
{
    deleteObject();
}

bool CLorezLine::createBuffers()
{
    QImage img3(1024,1,QImage::Format_RGB32);
    img3.fill(Qt::black);
    for (int i=0;i<64;i++)
    {
        QColor col(255-i*4,255-i*4,255-i*4,255);
        img3.setPixelColor(1023-i,0,col);
    }

    textureTest = new QOpenGLTexture(img3);
    textureTest->setMagnificationFilter(QOpenGLTexture::Nearest);
    textureTest->setMinificationFilter(QOpenGLTexture::Nearest);
    textureTest->setWrapMode(QOpenGLTexture::Repeat);

    gl->glGenVertexArrays(NumVAOs,VAOs);
    gl->glBindVertexArray(VAOs[BasicObject]);



     GLfloat *vertexCoords = new GLfloat[iNumOfPoints*3];
     GLfloat *vertexColors = new GLfloat[iNumOfPoints*3];
     GLfloat *vertexTexCoords = new GLfloat[iNumOfPoints];
     GLfloat *hsvColors = new GLfloat[16*3];

     LorenzCalc lor(0.0,10.0,0.0);



     for (int i=0;i<iNumOfPoints;i++)
     {
         //double dRelative = (double) i / (double) iNumOfPoints;
         QColor col;
         col.setHsv((9*i)%360,255,255);
         lor.nextStep(0.01);
         vertexCoords[i*3]=(GLfloat) lor.x;
         vertexCoords[i*3+1]=(GLfloat) lor.y;
         vertexCoords[i*3+2]=(GLfloat) lor.z;
         vertexColors[i*3]=col.redF();
         vertexColors[i*3+1]=col.greenF();
         vertexColors[i*3+2]=col.blueF();
         vertexTexCoords[i]=i/500.0f;
     }

     for (int i=0;i<16;i++)
     {
         QColor col;
         col.setHsv((i*360)/16,255,255);
         hsvColors[i*3]=col.redF();
         hsvColors[i*3+1]=col.greenF();
         hsvColors[i*3+2]=col.blueF();
     }


    gl->glGenBuffers(NumBuffers,Buffers);

    gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[VertexBuffer]);
    gl->glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*iNumOfPoints*3,vertexCoords, GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(vVertexPosition);
    gl->glVertexAttribPointer(vVertexPosition,3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

    gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[ColorBuffer]);
    gl->glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*iNumOfPoints*3,vertexColors, GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(vVertexColor);
    gl->glVertexAttribPointer(vVertexColor,3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));


    gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[TextCoordBuffer]);
    gl->glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*iNumOfPoints,vertexTexCoords, GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(vVertexTexCoord);
    gl->glVertexAttribPointer(vVertexTexCoord,1,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));


     gl->glGenTextures(NumTextures,Textures );
     gl->glBindTexture(GL_TEXTURE_1D, Textures[HSVtexture]);
     gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     gl->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 16, 0, GL_RGB, GL_FLOAT, hsvColors);
     gl->glBindTexture(GL_TEXTURE_1D,0);

    gl->glBindVertexArray(0);

    delete[] vertexCoords;
    delete[] vertexColors;
    delete[] hsvColors;
    return true;
}

void CLorezLine::deleteBuffers()
{
    gl->glDeleteBuffers(NumBuffers,Buffers);
    gl->glDeleteBuffers(NumTextures,Textures);
}

void CLorezLine::uniformsAndDraw()
{
    gl->glDisable(GL_CULL_FACE);
    texOffset-=0.001f;
    m_program->setUniformValue("texOffset",texOffset);
    m_program->setUniformValue("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0
    m_program->setUniformValue("highlight", 1); //set to 1 because the texture is bound to GL_TEXTURE0
    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_1D, Textures[HSVtexture]);
    gl->glActiveTexture(GL_TEXTURE1);
    textureTest->bind();
    gl->glDrawArrays(GL_LINE_STRIP,0,iNumOfPoints);
    gl->glActiveTexture(GL_TEXTURE1);
    textureTest->release();
    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_1D,0);
}

