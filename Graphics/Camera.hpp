#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <QVector3D>
#include <QMatrix4x4>
#include <QKeyEvent>

namespace graphics {

class Camera
{
public:
    Camera();

    QVector3D position, lookAt, up;
    QMatrix4x4 vMatrix;

    void setPosition(QVector3D position);
    void setLookAt(QVector3D lookAt);
    void setUp(QVector3D up);

    void controlEvent(QKeyEvent*);
private:

};

}

#endif // CAMERA_HPP
