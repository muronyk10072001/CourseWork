#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QVector>
#include "data_structure.cpp"

class GLWidget : public QGLWidget
{
public:
    explicit GLWidget(QWidget *parent = 0);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    ////////////
    void selectObject();
    QVector<figure> figures;
    int selectedFigure;
    int type_of_new_figure;
    ////////////
    QVector<figure> *GetData();
private:
    //////////////// for textures
    QImage img;
    GLuint *texture;
    QImage GL_formatted_image;
    ////////////
    int mouseDeltaX;
    int mouseDeltaY;

    int mousePosX;
    int mousePosY;
    ////////////
    float SizeSquare;
    int SizeChessboard;
    QTimer mpTimer;

    int myCeil(float);
};

#endif // GLWIDGET_H
