#include "renderobjects.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLContext>
#include <QFileInfo>
#include <QColor>
#include <QtMath>



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




bool CLorezLine::initialize(QObject *parent)
{

    gl = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_0_Core>();

    if (!gl)
    {
        qDebug() << "OpenGLFunctions not initialized or not supported";
        return false;
    }
    bool bOk=true;
    m_program = new QOpenGLShaderProgram(parent);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shader/texturedLine.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shader/texturedLine.frag");
    bOk=bOk && m_program->link();
    qDebug() << "Lorentz Line Shader Log: " << m_program->log();
    createBuffers();
    return bOk;
}

bool CLorezLine::createBuffers()
{
    QImage img2(16,1,QImage::Format_RGB32);
    for (int i=0;i<16;i++)
    {
        QColor col;
        col.setHsv((i*360)/16,255,255);
        img2.setPixelColor(i,0,col);
    }

    textureTest = new QOpenGLTexture(img2);

    gl->glGenVertexArrays(NumVAOs,VAOs);
    gl->glBindVertexArray(VAOs[BasicObject]);



     GLfloat *vertexCoords = new GLfloat[iNumOfPoints*3];
     GLfloat *vertexColors = new GLfloat[iNumOfPoints*3];
     GLfloat *vertexTexCoords = new GLfloat[iNumOfPoints];
     GLfloat *hsvColors = new GLfloat[16*3];

     LorenzCalc lor(0.0,10.0,0.0);



     for (int i=0;i<iNumOfPoints;i++)
     {
         double dRelative = (double) i / (double) iNumOfPoints;
         QColor col;
         col.setHsv((9*i)%360,255,255);
         lor.nextStep(0.01);
         vertexCoords[i*3]=(GLfloat) lor.x;
         vertexCoords[i*3+1]=(GLfloat) lor.y;
         vertexCoords[i*3+2]=(GLfloat) lor.z;
         vertexColors[i*3]=col.redF();
         vertexColors[i*3+1]=col.greenF();
         vertexColors[i*3+2]=col.blueF();
         vertexTexCoords[i]=i/100.0f;
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

bool CLorezLine::paint(QMatrix4x4 mat)
{
    m_program->bind();
    gl->glBindVertexArray(VAOs[BasicObject]);


    gl->glDisable(GL_CULL_FACE);
    m_program->setUniformValue("mvp_matrix",mat);
    texOffset-=0.02f;
    m_program->setUniformValue("texOffset",texOffset);
    m_program->setUniformValue("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0
    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_1D, Textures[HSVtexture]);
    gl->glDrawArrays(GL_LINE_STRIP,0,iNumOfPoints);

    gl->glBindTexture(GL_TEXTURE_1D,0);
    gl->glBindVertexArray(0);


    return true;

}

