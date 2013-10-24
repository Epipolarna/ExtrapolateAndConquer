#ifndef GLWIDGET_HPP
#define GLWIDGET_HPP

#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>

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

    QGLBuffer vertexBuffer;

    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer VBO;

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
    void keyReleaseEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);

public slots:
    void timerUpdate();



protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    QTimer* timer;
    QMatrix4x4 pMatrix;

    /*
    QOpenGLContext* glContext;
    QGLFramebufferObjectFormat fboFormat;
*/


    QElapsedTimer fpsMeter;
    qint64 nanoSex;
    double fps;

    QGLShaderProgram* initShader(QString vertexPath, QString fragmentPath);
    void useFBO(QGLFramebufferObject* FBO);
};

#endif // GLWIDGET_HPP
