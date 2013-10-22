#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QtGui>
#include "model.h"
#include "player.h"
#include "object.h"

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    QSize sizeHint() const;

    void loadModel();
    void loadTexture();
    Model *model, *squareModel;
    Object *object, *square;
    Player* player;

    GLuint FB, RB;
    GLuint tex;

    QGLFramebufferObject *FBO1, *FBO2, *FBO3;

    QMatrix4x4 pMatrix;
    QGLShaderProgram shaderProgram, program1, program2, program3;
    QGLShaderProgram *phongShader, *texShader, *threshShader;
    QVector<QVector3D> vertices;

    QImage im, imGL;

    void CHECK_FRAMEBUFFER_STATUS();
    void initFBO();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    QOpenGLContext* glContext;
    QGLFramebufferObjectFormat fboFormat;

    QElapsedTimer fpsMeter;
    qint64 nanoSex;
    double fps;

    void initShader(QGLShaderProgram* shader, QString vertexPath, QString fragmentPath);
    void useFBO(QGLFramebufferObject* FBO);
};

#endif // GLWIDGET_H
