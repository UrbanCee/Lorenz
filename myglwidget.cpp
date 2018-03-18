#include "myglwidget.h"
#include <QTimer>
#include <QMouseEvent>

MyGLWidget::MyGLWidget(QWidget *parent = 0)
    :QOpenGLWidget(parent),bRotate(false),oldMouseX(0),oldMouseY(0)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

}

MyGLWidget::~MyGLWidget()
{

}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    lorenzLine = new CLorezLine();
    lorenzLine->initialize(this);
}

void MyGLWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 1.0, zFar = 1000.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);

    //test

}

void MyGLWidget::paintGL()
{
    timer->start(10);
    glClear(GL_COLOR_BUFFER_BIT);
    QMatrix4x4 camera;
    camera.translate(0.0f,0.0f,-50.0f);
    QMatrix4x4 matrix;

    if (bRotate)
    {
        matrix=camera*currRot*transformation;
    }else{
        matrix=camera*transformation;
    }


    lorenzLine->paint(projection*matrix);
}

void MyGLWidget::mousePressEvent(QMouseEvent *e)
{
    oldMouseX=e->x();
    oldMouseY=e->y();
    if (e->button()==Qt::LeftButton)
    {
        bRotate=true;
        rotStartPos.setX(e->x());
        rotStartPos.setY(e->y());
        currRot.setToIdentity();
    }
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button()==Qt::LeftButton)
    {
        bRotate=false;
        transformation=currRot*transformation;
        currRot.setToIdentity();
    }
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    float dX=(e->x()-oldMouseX)/(float)height();
    float dY=(e->y()-oldMouseY)/(float)height();
    oldMouseX=e->x();
    oldMouseY=e->y();
    if (bRotate)
    {
        QVector3D currRotPos(e->x()-rotStartPos.x(),rotStartPos.y()-e->y(),0.0);
        currRotPos/=(height()/2.0f);
        if (currRotPos.lengthSquared()>=1.0)
            currRotPos.normalize();
        else
            currRotPos.setZ((float)-sqrt(1-currRotPos.x()*currRotPos.x()-currRotPos.y()*currRotPos.y()));
        currRot.setToIdentity();
        currRot.rotate(acos(QVector3D::dotProduct(currRotPos,QVector3D(0.0f,0.0f,-1.0f)))/M_PI*180.0f,QVector3D::crossProduct(currRotPos,QVector3D(0.0f,0.0f,-1.0f)));
    }else{
        if (e->buttons() & Qt::RightButton){
            QMatrix4x4 translation;
            translation.translate(dX*50.0,-dY*50.0);
            transformation=translation*transformation;
        }
    }

}
