#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QQuaternion>
#include <GL/glu.h>

struct VertexData   //Структура содержит информацию о вершинах
{
    VertexData(){
    }
    VertexData(QVector3D p, QVector2D t, QVector3D n):
        position(p),texCoord(t),normal(n){
    }
    QVector3D position;
    QVector2D texCoord;
    QVector3D normal;
};

class Widget : public QOpenGLWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    // QOpenGLWidget interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void initShaders();
    void initCube(float);
private:
    QMatrix4x4 m_projectMatrix;
    QOpenGLShaderProgram m_program;
    QOpenGLTexture *m_texture;
    QOpenGLBuffer m_arrayBuffer;
    QOpenGLBuffer m_indexBuffer;
    QVector2D m_mousePosition;//Первичное положение мыши при клике
    QQuaternion m_rotation; //Содержит информацию о векторе вокруг которого осуществляется поворот и угле на который осуществляется
    GLfloat m_xRotate;
    GLfloat m_yRotate;

};

#endif // WIDGET_H
