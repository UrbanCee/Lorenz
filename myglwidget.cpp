#include "myglwidget.h"
#include <QTimer>
#include <QMouseEvent>

MyGLWidget::MyGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), QOpenGLFunctions_4_0_Core(), bRotate(false),zoomFactor(50.0f),oldMouseX(0),oldMouseY(0)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
}


MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::updateLorenzLine()
{
    makeCurrent();
     foreach(CLorenzLine *line, lorenzLines)
    {
        line->redoBuffers();
    }
     doneCurrent();
}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    QString versionString1(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
    emit(showStatusBarMessage(QString("OpenGL Version: ")+versionString1,10000));
    coordSys = new CCoordSys();
    lorenzLines.append(new CLorenzLine(CLorenzLine::startPosHSV,CLorenzLine::HSVType));
    lorenzLines.append(new CLorenzLine(CLorenzLine::startPosRed,CLorenzLine::RedType));
    lorenzLines.append(new CLorenzLine(CLorenzLine::startPosGreen,CLorenzLine::GreenType));
    lorenzLines.append(new CLorenzLine(CLorenzLine::startPosBlue,CLorenzLine::BlueType));
    coordSys->initialize(this);
    //toroid.initialize(this);
    transformation.translate(0.0f,0.0f,-20.0f);
    foreach (CLorenzLine *lorenzLine, lorenzLines) {
        lorenzLine->initialize(this);
    }
}

void MyGLWidget::resizeGL(int w, int h)
{
    updateProjectionMatrix(w,h);
}

void MyGLWidget::paintGL()
{
    timer->start(10);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);


    QMatrix4x4 camera;
    camera.translate(0.0f,0.0f,-zoomFactor);
    QMatrix4x4 matrix;
    QMatrix4x4 coordSysMatrix;
    QMatrix4x4 normalMatrix;
    if (bRotate)
    {
        camera=camera*currRot;
    }
    matrix=camera*transformation;
    coordSysMatrix=camera*transRotOnly;
    QMatrix4x4 mvp_Matrix=projection*matrix;

    normalMatrix=matrix.inverted().transposed();
    if (CLorenzLine::bShowHSVLine)
        lorenzLines[0]->paint(mvp_Matrix);
    if (CLorenzLine::bShowRGBLines)
    {
        for (int i=1;i<4;i++)
        {
            lorenzLines[i]->paint(mvp_Matrix);
        }
    }
    mvp_Matrix=projection*coordSysMatrix;
    coordSys->paint(mvp_Matrix);
}



void MyGLWidget::mousePressEvent(QMouseEvent *e)
{
    stopRotation();
    oldMouseX=e->x();
    oldMouseY=e->y();
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    stopRotation();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!bRotate)
        if ((e->buttons()==Qt::LeftButton) || (e->buttons()==(Qt::LeftButton | Qt::RightButton)))
            startRotation(e->x(),e->y());
    float sizeFactor =(float) height();
    float dX=(e->x()-oldMouseX)/sizeFactor;
    float dY=(e->y()-oldMouseY)/sizeFactor;
    oldMouseX=e->x();
    oldMouseY=e->y();
    if (e->buttons()==Qt::LeftButton)
    {
        QVector3D currRotPos(e->x()-rotStartPos.x(),rotStartPos.y()-e->y(),0.0);
        QVector3D axis(-currRotPos.y(),currRotPos.x(),0.0);
        double angle=currRotPos.length()/(sizeFactor)*180.0;
        currRot.setToIdentity();
        currRot.rotate(angle,axis);

    }else if (e->buttons() == Qt::RightButton){
        QMatrix4x4 translation;
        translation.translate(dX*zoomFactor,-dY*zoomFactor);
        transformation=translation*transformation;
    }
    else if (e->buttons() == Qt::MidButton){
        QMatrix4x4 translation;
        translation.translate(0.0f,0.0f,dY*zoomFactor);
        transformation=translation*transformation;
    }
    else if (e->buttons() == (Qt::LeftButton | Qt::RightButton))
    {
        QVector2D mid(width()/2,height()/2);
        QVector2D oldV=rotStartPos-mid;
        QVector2D newV(e->x()-mid.x(),e->y()-mid.y());
        float denom=oldV.length()*newV.length();
        float dAngle=0.0f;
        if (denom>0.0f)
            dAngle=acosf(QVector2D::dotProduct(oldV,newV)/denom);
        if (oldV.x()*newV.y()-oldV.y()*newV.x()>0)
            dAngle*=-1;
        dAngle*=180.0/M_PI;
        currRot.setToIdentity();
        QVector3D axis(0.0,0.0,1.0);
        currRot.rotate(dAngle,axis);
    }

}

void MyGLWidget::wheelEvent(QWheelEvent *e)
{
    if (e->delta()>0)
    {
        zoomFactor*=0.9;
    }else if (e->delta()<0)
    {
        zoomFactor/=0.9;
    }
    updateProjectionMatrix(width(),height());
    emit showStatusBarMessage(QString("Zoom: %1\% Dist: %2").arg(100.0f/(zoomFactor/5.0f)).arg(zoomFactor),1000);
}

void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Left)
    {

    }
}

void MyGLWidget::stopRotation()
{
    bRotate=false;
    transformation=currRot*transformation;
    transRotOnly=currRot*transRotOnly;
    currRot.setToIdentity();
}

void MyGLWidget::startRotation(int x, int y)
{
    bRotate=true;
    rotStartPos.setX(x);
    rotStartPos.setY(y);
    currRot.setToIdentity();
}



void MyGLWidget::updateProjectionMatrix(int w, int h)
{
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    const qreal zNear = zoomFactor/20.0f, zFar = zoomFactor*10.0f, fov = 60.0;
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);
}

