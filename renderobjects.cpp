#include "renderobjects.h"


#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLContext>
#include <QtMath>



CBaseObjectFactory::CBaseObjectFactory(const QString &name, const QString &vert, const QString &frag)
    :qstrObjectName(name),qstrVertexFile(vert),qstrFragmentFile(frag)
{VAOs[BaseObject]=0;}
bool CBaseObjectFactory::initialize(QObject *parent)
{
    gl = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_0_Core>();

    if (!gl)
    {
        qDebug() << "OpenGLFunctions not initialized or not supported";
        bOk=false;
        return bOk;
    }
    m_program = new QOpenGLShaderProgram(parent);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, qstrVertexFile);
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, qstrFragmentFile);
    bOk=bOk && m_program->link();
    qDebug() << QString("Shader log of %1:").arg(qstrObjectName) << m_program->log();
    bOk = bOk && createObject();
    return bOk;
}
bool CBaseObjectFactory::createObject()
{
    if (!bOk)
        return bOk;
    gl->glGenVertexArrays(NumVAOs, VAOs);
    if (VAOs[BaseObject]==0)
    {
        qDebug() << "Error creating Vertex Array Object in " << qstrObjectName;
        bOk=false;
        return bOk;
    }
    gl->glBindVertexArray(VAOs[BaseObject]);
    qDebug() << "Create Vertex Array Object " << qstrObjectName << ": " << VAOs[BaseObject] ;
    createBuffers();
    gl->glBindVertexArray(0);
    return bOk;
}
void CBaseObjectFactory::deleteObject()
{
    if (VAOs[BaseObject]==0)
        return;
    gl->glDeleteVertexArrays(NumVAOs,VAOs);
    gl->glBindVertexArray(0);
    deleteBuffers();
    VAOs[BaseObject]=0;
}
bool CBaseObjectFactory::paint(const QMatrix4x4 &modelViewProjectionMatrix)
{
    bOk=bOk&&m_program->bind();
    if (!bOk || VAOs[BaseObject]==0)
        return bOk;
    m_program->setUniformValue("mvp_matrix",modelViewProjectionMatrix);
    gl->glBindVertexArray(VAOs[BaseObject]);
    uniformsAndDraw();
    gl->glBindVertexArray(0);
    m_program->release();
    return bOk;

}
bool CBaseObjectFactory::paint(const QMatrix4x4 &modelViewProjectionMatrix,const QMatrix4x4 &modelViewMatrix, const QMatrix4x4 &normalMatrix)
{
    bOk=bOk&&m_program->bind();
    if (!bOk || VAOs[BaseObject]==0)
        return bOk;
    m_program->setUniformValue("normal_matrix",normalMatrix);
    m_program->setUniformValue("modelview_matrix",modelViewMatrix);
    return paint(modelViewProjectionMatrix);
}
bool CBaseObjectFactory::paint(const QMatrix4x4 &modelViewProjectionMatrix, const QMatrix4x4 &projectionMatrix, const QMatrix4x4 &modelViewMatrix, const QMatrix4x4 &normalMatrix)
{
    Q_UNUSED(projectionMatrix);
    bOk=bOk&&m_program->bind();
    if (!bOk || VAOs[BaseObject]==0)
        return bOk;
    return paint(modelViewProjectionMatrix,modelViewMatrix,normalMatrix);
}





CCoordSys::CCoordSys()
    :CBaseObjectFactory("Coordinate System",":/Shader/CoordSys.vert",":/Shader/CoordSys.frag")
{}
CCoordSys::~CCoordSys()
{
    deleteObject();
}
bool CCoordSys::createBuffers()
{
    GLfloat vertices[6][3] = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 0.0f, 0.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 0.0f},
        { 0.0f, 0.0f, 1.0f},
    };
    GLfloat colors[6][3] = {
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 1.0f},
        { 0.0f, 0.0f, 1.0f},
    };
    gl->glGenBuffers(NumBuffers, Buffers);
    gl->glBindBuffer(GL_ARRAY_BUFFER, Buffers[CoordBuffer]);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);
    gl->glVertexAttribPointer(vPosition, 3, GL_FLOAT,GL_FALSE, 0, BUFFER_OFFSET(0));
    gl->glEnableVertexAttribArray(vPosition);
    gl->glBindBuffer(GL_ARRAY_BUFFER, Buffers[ColorBuffer]);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(colors),colors, GL_STATIC_DRAW);
    gl->glVertexAttribPointer(vColor, 3, GL_FLOAT,GL_FALSE, 0, BUFFER_OFFSET(0));
    gl->glEnableVertexAttribArray(vColor);
    return true;
}
void CCoordSys::uniformsAndDraw()
{
    gl->glDrawArrays(GL_LINES, 0, 6);
}
void CCoordSys::deleteBuffers()
{
    gl->glDeleteBuffers(NumBuffers,Buffers);
}



CCuboid::CCuboid()
    :CBaseObjectFactory("Cuboid",":/Shader/Cuboid.vert",":/Shader/Cuboid.frag")
{}
CCuboid::~CCuboid()
{
    deleteObject();
}
bool CCuboid::createBuffers()
{
     GLfloat vertices[8][3] = {
         { 0.0f, 0.0f, 0.0f},
         { 1.0f, 0.0f, 0.0f},
         { 1.0f, 1.0f, 0.0f},
         { 0.0f, 1.0f, 0.0f},
         { 0.0f, 0.0f, 2.0f},
         { 1.0f, 0.0f, 2.0f},
         { 1.0f, 1.0f, 2.0f},
         { 0.0f, 1.0f, 2.0f}
     };

     iTriangleCount=12;
     GLuint triangles[iTriangleCount][3] = {
         {0,2,1}, //bottom
         {0,3,2},
         {4,5,6}, //top
         {4,6,7},
         {4,0,1}, //side 1
         {4,1,5},
         {6,2,3}, //side 2
         {6,3,7},
         {0,7,3}, //side 3
         {0,4,7},
         {1,2,6}, //side 4
         {1,6,5}
     };


 /*    for (int i=0;i<8;i++)
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
     }*/



    gl->glGenBuffers(NumBuffers,Buffers);

    gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[CoordBuffer]);
    gl->glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices, GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(vVertexPosition);
    gl->glVertexAttribPointer(vVertexPosition,3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Buffers[IndexBuffer]);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(triangles),triangles,GL_STATIC_DRAW);


/*     gl->glGenTextures(NumTextures,Textures );
     gl->glBindTexture(GL_TEXTURE_1D, Textures[HSVtexture]);
     gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     gl->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 16, 0, GL_RGB, GL_FLOAT, hsvColors);
     gl->glBindTexture(GL_TEXTURE_1D,0);*/

    return true;
}
void CCuboid::uniformsAndDraw()
{
    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_BACK);
/*    m_program->setUniformValue("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0
    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_1D, Textures[HSVtexture]);*/
    gl->glDrawElements(GL_TRIANGLES,iTriangleCount*3,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
//    gl->glDrawArrays(GL_LINE_STRIP,0,iNumOfPoints);
//    gl->glBindTexture(GL_TEXTURE_1D,0);
}
void CCuboid::deleteBuffers()
{
    gl->glDeleteBuffers(NumBuffers,Buffers);
}




CToroid::CToroid()
    :CBaseObjectFactory("Torus",":/Shader/Fragment_Phong.vert",":/Shader/Fragment_Phong.frag"),mat(CMaterial::ruby)
{}
CToroid::~CToroid()
{
    deleteObject();
}

void CToroid::reshapeTorus(float outerRadius, float innerRadius, int rings, int segments)
{
    fR1=outerRadius;fR2=innerRadius;iRings=rings;iSegments=segments;
    deleteObject();
    createObject();
}
void CToroid::reshapeTorus(int rings, int segments)
{reshapeTorus(fR1,fR2,rings,segments);}
void CToroid::reshapeTorus(float outerRadius, float innerRadius)
{reshapeTorus(outerRadius,innerRadius,iRings,iSegments);}
bool CToroid::createBuffers()
    {
        iTriangleCount=2*iSegments*iRings;
         GLfloat *vertices = new GLfloat[iSegments*iRings*3];
         GLfloat *normals = new GLfloat[iSegments*iRings*3];
         GLuint *triangles= new GLuint[iTriangleCount*3];
         for (int i=0;i<iTriangleCount*3;i++)
             triangles[i]=0;
         GLfloat fAngleInc1=2.0*M_PI/(GLfloat)iRings;
         GLfloat fAngleInc2=2.0*M_PI/(GLfloat)iSegments;
         GLfloat fAngle1=0.0f;
         int iVertIndex=0;
         int iFaceIndex=0;
         for (int i=0;i<iRings;i++){
             GLfloat fAngle2=0.0f;
             for(int j=0;j<iSegments;j++)
             {
                 vertices[iVertIndex]=(fR1+fR2*cos(fAngle2))*cos(fAngle1);
                 normals[iVertIndex++]=fR2*cos(fAngle2)*cos(fAngle1);
                 vertices[iVertIndex]=(fR1+fR2*cos(fAngle2))*sin(fAngle1);
                 normals[iVertIndex++]=fR2*cos(fAngle2)*sin(fAngle1);
                 vertices[iVertIndex]=fR2*sin(fAngle2);
                 normals[iVertIndex++]=fR2*sin(fAngle2);
                 fAngle2+=fAngleInc2;
                 triangles[iFaceIndex++]=i*iSegments+j;
                 triangles[iFaceIndex++]=((i+1)%iRings)*iSegments+j;
                 triangles[iFaceIndex++]=((i+1)%iRings)*iSegments+(j+1)%iSegments;
                 triangles[iFaceIndex++]=i*iSegments+j;
                 triangles[iFaceIndex++]=((i+1)%iRings)*iSegments+(j+1)%iSegments;
                 triangles[iFaceIndex++]=i*iSegments+(j+1)%iSegments;
             }
             fAngle1+=fAngleInc1;
         }



        gl->glGenBuffers(NumBuffers,Buffers);


        gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[CoordBuffer]);
        gl->glBufferData(GL_ARRAY_BUFFER,sizeof(*vertices)*iSegments*iRings*3,vertices, GL_STATIC_DRAW);
        gl->glEnableVertexAttribArray(vVertexPosition);
        gl->glVertexAttribPointer(vVertexPosition,3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

        gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[NormalBuffer]);
        gl->glBufferData(GL_ARRAY_BUFFER,sizeof(*normals)*iSegments*iRings*3,normals, GL_STATIC_DRAW);
        gl->glEnableVertexAttribArray(vVertexNormal);
        gl->glVertexAttribPointer(vVertexNormal,3,GL_FLOAT,GL_TRUE,0,BUFFER_OFFSET(0));


        gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Buffers[IndexBuffer]);
        gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(*triangles)*iTriangleCount*3,triangles,GL_STATIC_DRAW);

        qDebug() << qstrObjectName << "Coord Buffer: " << Buffers[CoordBuffer] << "Normal Buffer: " << Buffers[NormalBuffer] << "Index Buffer: " << Buffers[IndexBuffer];

        delete[]vertices;
        delete[]normals;
        delete[]triangles;
        return true;
}

void CToroid::uniformsAndDraw()
{
    mat.use(m_program);
    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_BACK);
    gl->glPolygonMode(GL_FRONT,GL_LINE);
    gl->glDrawElements(GL_TRIANGLES,iTriangleCount*3,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
}

void CToroid::deleteBuffers()
{
    gl->glDeleteBuffers(NumBuffers,Buffers);
}








CPlane::CPlane()
    :CBaseObjectFactory("Plane",":/Shader/Fragment_Phong.vert",":/Shader/Fragment_Phong.frag"),mat(CMaterial::gold)
{}
CPlane::~CPlane()
{
    deleteObject();
}
bool CPlane::createBuffers()
    {
        iTriangleCount=32;
        int iVerticies=25;
         GLfloat *vertices = new GLfloat[iVerticies*3];
         GLfloat *normals = new GLfloat[iVerticies*3];
         GLuint *triangles= new GLuint[iTriangleCount*3];
         int iVertIndex=0;
         int iFaceIndex=0;
         for (int i=0;i<iTriangleCount*3;i++)
             triangles[i]=0;
         for (int i=-2;i<3;i++){
             for(int j=-2;j<3;j++)
             {
                 vertices[iVertIndex]=(GLfloat) j;
                 normals[iVertIndex++]=0.0f;
                 vertices[iVertIndex]=(GLfloat) i;
                 normals[iVertIndex++]=0.0f;
                 vertices[iVertIndex]=0.0f;
                 normals[iVertIndex++]=1.0f;
                 if (i==2 || j==2)
                     continue;
                 triangles[iFaceIndex++]=j+2+5*(i+2);
                 triangles[iFaceIndex++]=j+2+5*(i+3);
                 triangles[iFaceIndex++]=j+3+5*(i+3);
                 triangles[iFaceIndex++]=j+2+5*(i+2);
                 triangles[iFaceIndex++]=j+3+5*(i+3);
                 triangles[iFaceIndex++]=j+3+5*(i+2);
             }
         }

        gl->glGenBuffers(NumBuffers,Buffers);

        gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[CoordBuffer]);
        gl->glBufferData(GL_ARRAY_BUFFER,sizeof(*vertices)*iVerticies*3,vertices, GL_STATIC_DRAW);
        gl->glEnableVertexAttribArray(vVertexPosition);
        gl->glVertexAttribPointer(vVertexPosition,3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

        gl->glBindBuffer(GL_ARRAY_BUFFER,Buffers[NormalBuffer]);
        gl->glBufferData(GL_ARRAY_BUFFER,sizeof(*normals)*iVerticies*3,normals, GL_STATIC_DRAW);
        gl->glEnableVertexAttribArray(vVertexNormal);
        gl->glVertexAttribPointer(vVertexNormal,3,GL_FLOAT,GL_TRUE,0,BUFFER_OFFSET(0));


        gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Buffers[IndexBuffer]);
        gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(*triangles)*iTriangleCount*3,triangles,GL_STATIC_DRAW);

        qDebug() << qstrObjectName << "Coord Buffer: " << Buffers[CoordBuffer] << "Normal Buffer: " << Buffers[NormalBuffer] << "Index Buffer: " << Buffers[IndexBuffer];

        delete[]vertices;
        delete[]normals;
        delete[]triangles;
        return true;
}

void CPlane::uniformsAndDraw()
{
    mat.use(m_program);
    gl->glDisable(GL_CULL_FACE);
//    gl->glCullFace(GL_BACK);
    gl->glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    gl->glDrawElements(GL_TRIANGLES,iTriangleCount*3,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
}

void CPlane::deleteBuffers()
{
    gl->glDeleteBuffers(NumBuffers,Buffers);
}
