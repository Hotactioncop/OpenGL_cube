#include "widget.h"

#include <QMouseEvent>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent),m_texture(0),m_indexBuffer(QOpenGLBuffer::IndexBuffer),m_xRotate(0),m_yRotate(0)
{
    glEnable(GL_POLYGON_SMOOTH);
}

Widget::~Widget()
{

}


void Widget::initializeGL() //Вызывается 1 раз
{
    glClearColor(0.0f,0.0f,0.0f,1.0f);

    glEnable(GL_DEPTH_TEST);//Включаем буфер глубины
    glEnable(GL_CULL_FACE);//Включаем отсечение задних граней - чтобы не рисовались

    initShaders();
    initCube(1.0f);
}

void Widget::resizeGL(int w, int h) //Вызывается каждый раз при изменении размера виджета
{
    //Настраиваем матрицу проекции
    float aspect = w / (float)h; //Отношение ширины на высоту
    m_projectMatrix.setToIdentity();//Делаем матрицу единичной. У матрицы на главной диагонали стоит 1, остальные равны нулю
    m_projectMatrix.perspective(45,aspect,0.1f,10.0f);//Угол усеченного конуса камеры,..., передняя и дальние полскости отсечения
}

void Widget::paintGL()//Вызывается каждый раз при перерисовке содержимого окна
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Очищаем сцену
    //Загрузить матрицу проэкции в шейдер и модельно-видовую матрицу

    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.setToIdentity();
//    modelViewMatrix.rotate(m_rotation);
    modelViewMatrix.rotate(m_xRotate,1.0,0.0,0.0);
    modelViewMatrix.rotate(m_yRotate,0.0,1.0,0.0);
    modelViewMatrix.translate(0.0,0.0,-5.0);

    m_texture->bind(0);

    m_program.bind();//Доступ к программе
    m_program.setUniformValue("qt_ModelViewProjectionMatrix", m_projectMatrix*modelViewMatrix);
    m_program.setUniformValue("qt_Texture0", 0);//Номер текстуры которая будет отрисовываться

    int offset = 0;

    m_arrayBuffer.bind();

    int vertLoc = m_program.attributeLocation("qt_Vertex");
    m_program.enableAttributeArray(vertLoc);
    m_program.setAttributeBuffer(vertLoc,GL_FLOAT,offset,3,sizeof(VertexData));

    offset+= sizeof(QVector3D);

    int texLoc = m_program.attributeLocation("qt_MultiTexCoord0");
    m_program.enableAttributeArray(texLoc);
    m_program.setAttributeBuffer(texLoc,GL_FLOAT,offset,2, sizeof(VertexData));

    m_indexBuffer.bind();

    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(),GL_UNSIGNED_INT,0);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons()==Qt::LeftButton){
        m_mousePosition=QVector2D(event->localPos());//Указатель мыши согласно угла окна
    }
    event->accept();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()!=Qt::LeftButton)return;

    m_xRotate+=180*(GLfloat)(event->y()-m_mousePosition.y())/height();
    m_yRotate+=180*(GLfloat)(event->x()-m_mousePosition.x())/width();

    QVector2D diff=QVector2D(event->localPos())-m_mousePosition;//Сохраняем дельту между текущим положением и первичным
    m_mousePosition=QVector2D(event->localPos());

    float angle = diff.length()/2.0;//Угол поворота. Делим на 2 чтобы поворот не был слишком быстрым

    QVector3D axis = QVector3D(m_xRotate,m_yRotate,0.0);//Вектор вокруг которого осуществлять поворот

    m_rotation = QQuaternion::fromAxisAndAngle(axis,angle) * m_rotation; //Умножаем чтобы с каждым движением мыши поворот осуществлялся с нового положения а не с самого начала

    update();
}

void Widget::initShaders()
{
    if(!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,":/vshader.vsh"))
        close();
    if(!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment,":/fshader.fsh"))
        close();
    if(!m_program.link())//Линкуем все шейдеры в один
        close();

}

void Widget::initCube(float width)
{
    float width_div_2 = width/2.0f;
    QVector<VertexData> vertexes;
    vertexes.append(VertexData(QVector3D(-width_div_2,width_div_2,width_div_2),
                               QVector2D(0.0,1.0),QVector3D(0.0,0.0,1.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,-width_div_2,width_div_2),
                               QVector2D(0.0,0.0),QVector3D(0.0,0.0,1.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,width_div_2,width_div_2),
                               QVector2D(1.0,1.0),QVector3D(0.0,0.0,1.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,-width_div_2,width_div_2),
                               QVector2D(1.0,0.0),QVector3D(0.0,0.0,1.0)));

    vertexes.append(VertexData(QVector3D(width_div_2,width_div_2,width_div_2),
                               QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,-width_div_2,width_div_2),
                               QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,width_div_2,-width_div_2),
                               QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,-width_div_2,-width_div_2),
                               QVector2D(1.0,0.0),QVector3D(1.0,0.0,0.0)));

    vertexes.append(VertexData(QVector3D(width_div_2,width_div_2,width_div_2),
                               QVector2D(0.0,1.0),QVector3D(0.0,1.0,0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,width_div_2,-width_div_2),
                               QVector2D(0.0,0.0),QVector3D(0.0,1.0,0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,width_div_2,width_div_2),
                               QVector2D(1.0,1.0),QVector3D(0.0,1.0,0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,width_div_2,-width_div_2),
                               QVector2D(1.0,0.0),QVector3D(0.0,1.0,0.0)));

    vertexes.append(VertexData(QVector3D(width_div_2,width_div_2,-width_div_2),
                               QVector2D(0.0,1.0),QVector3D(0.0,0.0,-1.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,-width_div_2,-width_div_2),
                               QVector2D(0.0,0.0),QVector3D(0.0,0.0,-1.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,width_div_2,-width_div_2),
                               QVector2D(1.0,1.0),QVector3D(0.0,0.0,-1.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,-width_div_2,-width_div_2),
                               QVector2D(1.0,0.0),QVector3D(0.0,0.0,-1.0)));

    vertexes.append(VertexData(QVector3D(-width_div_2,width_div_2,width_div_2),
                               QVector2D(0.0,1.0),QVector3D(-1.0,0.0,0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,width_div_2,-width_div_2),
                               QVector2D(0.0,0.0),QVector3D(-1.0,0.0,0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,-width_div_2,width_div_2),
                               QVector2D(1.0,1.0),QVector3D(-1.0,0.0,0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,-width_div_2,-width_div_2),
                               QVector2D(1.0,0.0),QVector3D(-1.0,0.0,0.0)));

    vertexes.append(VertexData(QVector3D(-width_div_2,-width_div_2,width_div_2),
                               QVector2D(0.0,1.0),QVector3D(0.0,-1.0,0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2,-width_div_2,-width_div_2),
                               QVector2D(0.0,0.0),QVector3D(0.0,-1.0,0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,-width_div_2,width_div_2),
                               QVector2D(1.0,1.0),QVector3D(0.0,-1.0,0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2,-width_div_2,-width_div_2),
                               QVector2D(1.0,0.0),QVector3D(0.0,-1.0,0.0)));

    QVector<GLuint> indexes;
    for(int i=0; i<24; i+=4){
        indexes.append(i+0);
        indexes.append(i+1);
        indexes.append(i+2);
        indexes.append(i+2);
        indexes.append(i+1);
        indexes.append(i+3);
    }

    //Вершинный буфер
    m_arrayBuffer.create();//Создаем
    m_arrayBuffer.bind();//Размещаем
    m_arrayBuffer.allocate(vertexes.constData(),vertexes.size()*sizeof (VertexData));//Выделяем память и загружает данные
    m_arrayBuffer.release();//Освобождаем пока это не нужно

    m_indexBuffer.create();
    m_indexBuffer.bind();
    m_indexBuffer.allocate(indexes.constData(),indexes.size()*sizeof(GLuint));
    m_indexBuffer.release();

    m_texture = new QOpenGLTexture(QImage(":/Images/box.png").mirrored());//Отражение по вертикали mirrored

    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

    m_texture->setWrapMode(QOpenGLTexture::Repeat);
}
