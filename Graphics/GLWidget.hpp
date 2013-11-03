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

    graphics::Model *skyboxModel, *monkeyModel, *oceanModel;
    GLuint skyboxTex, oceanTex, tex0, tex1, tex2;
    graphics::Object *skybox, *monkey, *ocean;
    graphics::Camera *currentCamera, *player;
    QGLShaderProgram *skyboxShader, *phongShader, *phongTexShader;

    QGLBuffer vertexBuffer;

    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer VBO;

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
    QMatrix4x4 pMatrix, skyboxvMatrix;

    QElapsedTimer fpsMeter;
    qint64 nanoSex;
    double fps;

    QGLShaderProgram* initShader(QString vertexPath, QString fragmentPath);
    void useFBO(QGLFramebufferObject* FBO);
    GLuint uploadTexture(QString imagePath, bool mipmap = false);
};

#endif // GLWIDGET_HPP
