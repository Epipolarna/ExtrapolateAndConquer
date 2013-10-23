#ifndef GLWIDGET_HPP
#define GLWIDGET_HPP

#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>

#include "Model.hpp"
#include "Object.hpp"
#include "Camera.hpp"

namespace Ui {
class GLWidget;
}

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(QGLFormat format, QWidget *parent = 0);
    ~GLWidget();

    graphics::Model *monkeyModel;
    graphics::Object *monkey;
    graphics::Camera *currentCamera, *player;
    QGLShaderProgram *phongShader;

    /*
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
    */

    void keyPressEvent(QKeyEvent *);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    QMatrix4x4 pMatrix;

    /*
    QOpenGLContext* glContext;
    QGLFramebufferObjectFormat fboFormat;
*/
    QElapsedTimer fpsMeter;
    qint64 nanoSex;
    double fps;

    void initShader(QGLShaderProgram* shader, QString vertexPath, QString fragmentPath);
    void useFBO(QGLFramebufferObject* FBO);
};

#endif // GLWIDGET_HPP
