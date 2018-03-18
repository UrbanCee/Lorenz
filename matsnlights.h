#ifndef MATSNLIGHTS_H
#define MATSNLIGHTS_H

#include <GL/gl.h>
#include <QtCore>
#include <QColor>
#include <QVector3D>

class QOpenGLShaderProgram;
class QOpenGLFunctions_4_0_Core;


class CMaterial
{
public:
    QColor emissive;
    QColor ambient;
    QColor diffuse;
    QColor specular;
    QVector3D vecValues[4];
    GLfloat shininess;
    CMaterial();
    CMaterial(QColor em, QColor am, QColor dif, QColor spec, GLfloat shininess);
    CMaterial(const CMaterial &mat);
    static CMaterial emerald,gold,ruby;
    void use(QOpenGLShaderProgram *);
};

/*class CLight
{
    QVector4D pos;
};*/




#endif // MATSNLIGHTS_H
