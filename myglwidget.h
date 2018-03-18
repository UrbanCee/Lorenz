#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H
#include "renderobjects.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>
#include <QVector3D>


QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QMouseEvent)


class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_0_Core
{
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget *parent);
    ~MyGLWidget();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    CLorezLine *lorenzLine;
    QMatrix4x4 projection, transformation, currRot;
    QVector3D rotStartPos;
    bool bRotate;
    int oldMouseX,oldMouseY;


    QTimer *timer;

};


#endif // MYGLWIDGET_H
