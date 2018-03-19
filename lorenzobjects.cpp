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

bool CLorenzLine::texturesCreated = false;
GLuint CLorenzLine::Textures[CLorenzLine::NumTextures];
int CLorenzLine::iIterationSteps = 10000;
double CLorenzLine::dIterationDT = 0.01;

bool CLorenzLine::bHSVLorenzLine=true;
bool CLorenzLine::bColoredLorenzLines=false;
QVector3D CLorenzLine::startPosRed(0.0f,10.1f,0.0f);
QVector3D CLorenzLine::startPosBlue(0.0f,10.0f,0.1f);
QVector3D CLorenzLine::startPosGreen(0.1f,10.0f,0.0f);
QVector3D CLorenzLine::startPosHSV(0.0f,10.0f,0.0f);
bool CLorenzLine::bShowRGBLines=false;
bool CLorenzLine::bShowHSVLine=true;



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



CLorenzLine::CLorenzLine(const QVector3D &startPoint=CLorenzLine::startPosHSV,LorenzLineTypes type=HSVType)
    : CBaseObjectFactory("Lorenz Line",":/Shader/texturedLine.vert",":/Shader/texturedLine.frag"),type(type),vStartPos(startPoint)
{}

CLorenzLine::~CLorenzLine()
{
    deleteObject();
}

bool CLorenzLine::createBuffers()
{
     iNumOfPoints=CLorenzLine::iIterationSteps;


     GLfloat *vertexCoords = new GLfloat[iNumOfPoints*3];
     GLfloat *vertexColors = new GLfloat[iNumOfPoints*3];
     GLfloat *vertexTexCoords = new GLfloat[iNumOfPoints];

     LorenzCalc lor(vStartPos.x(),vStartPos.y(),vStartPos.z());



     for (int i=0;i<iNumOfPoints;i++)
     {
         lor.nextStep(CLorenzLine::dIterationDT);
         vertexCoords[i*3]=(GLfloat) lor.x;
         vertexCoords[i*3+1]=(GLfloat) lor.y;
         vertexCoords[i*3+2]=(GLfloat) lor.z;
         vertexTexCoords[i]=i/(50000*CLorenzLine::dIterationDT);
     }


    gl->glBindVertexArray(VAOs[BasicObject]);

    gl->glGenBuffers(NumBuffers,Buffers);

    gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[VertexBuffer]);
    gl->glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*iNumOfPoints*3,vertexCoords, GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(vVertexPosition);
    gl->glVertexAttribPointer(vVertexPosition,3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

    gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[TextCoordBuffer]);
    gl->glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*iNumOfPoints,vertexTexCoords, GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(vVertexTexCoord);
    gl->glVertexAttribPointer(vVertexTexCoord,1,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

    if (!texturesCreated)
        createTextures(gl);

    gl->glBindVertexArray(0);

    delete[] vertexCoords;
    delete[] vertexColors;
    return true;
}

void CLorenzLine::deleteBuffers()
{
    gl->glDeleteBuffers(NumBuffers,Buffers);
}

void CLorenzLine::createTextures(QOpenGLFunctions_4_0_Core *gl)
{
    GLfloat *hsvColors = new GLfloat[16*3];
    GLfloat *highlight = new GLfloat[1024*3];
    GLfloat *red = new GLfloat[2*3];
    GLfloat *green = new GLfloat[2*3];
    GLfloat *blue = new GLfloat[2*3];

    for (int i=0;i<16;i++)
    {
        QColor col;
        col.setHsv((i*360)/16,255,255);
        hsvColors[i*3]=col.redF();
        hsvColors[i*3+1]=col.greenF();
        hsvColors[i*3+2]=col.blueF();
    }
    for (int i=0;i<6;i++)
    {
        red[i]=0;
        green[i]=0;
        blue[i]=0;
    }
    for (int i=0;i<2;i++)
    {
        red[3*i]=1.0f;
        green[3*i+1]=1.0f;
        blue[3*i+2]=1.0f;
    }
    for (int i=0;i<1024;i++)
    {
        GLfloat val=(i<992?0.0f:(i-991.0f)/32.0f);
        for (int j=0;j<3;j++)
            highlight[i*3+j]=val;
    }



    gl->glGenTextures(NumTextures,Textures );

    gl->glBindTexture(GL_TEXTURE_1D, Textures[HSVtexture]);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 16, 0, GL_RGB, GL_FLOAT, hsvColors);

    gl->glBindTexture(GL_TEXTURE_1D, Textures[HighlightTexture]);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 1024, 0, GL_RGB, GL_FLOAT, highlight);

    gl->glBindTexture(GL_TEXTURE_1D, Textures[RedTexture]);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 2, 0, GL_RGB, GL_FLOAT, red);

    gl->glBindTexture(GL_TEXTURE_1D, Textures[GreenTexture]);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 2, 0, GL_RGB, GL_FLOAT, green);

    gl->glBindTexture(GL_TEXTURE_1D, Textures[BlueTexture]);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 2, 0, GL_RGB, GL_FLOAT, blue);

    gl->glBindTexture(GL_TEXTURE_1D,0);

    delete[] hsvColors;
    delete[] highlight;
    delete[] red;
    delete[] green;
    delete[] blue;
    texturesCreated=true;
}

void CLorenzLine::uniformsAndDraw()
{
    gl->glDisable(GL_CULL_FACE);
    texOffset-=0.001f;
    m_program->setUniformValue("texOffset",texOffset);
    m_program->setUniformValue("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0
    m_program->setUniformValue("highlight", 1); //set to 1 because the texture is bound to GL_TEXTURE0
    gl->glActiveTexture(GL_TEXTURE0);
    if (type==HSVType)
        gl->glBindTexture(GL_TEXTURE_1D, Textures[HSVtexture]);
    else if (type==RedType)
        gl->glBindTexture(GL_TEXTURE_1D, Textures[RedTexture]);
    else if (type==GreenType)
        gl->glBindTexture(GL_TEXTURE_1D, Textures[GreenTexture]);
    else if (type==BlueType)
        gl->glBindTexture(GL_TEXTURE_1D, Textures[BlueTexture]);
    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_1D, Textures[HighlightTexture]);
    gl->glDrawArrays(GL_LINE_STRIP,0,iNumOfPoints);
    gl->glActiveTexture(GL_TEXTURE1);
    gl->glBindTexture(GL_TEXTURE_1D,0);
    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_1D,0);
}

