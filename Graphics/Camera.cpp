#include "Camera.hpp"

namespace graphics {

Camera::Camera()
{
    position = QVector3D(0,0,0);
    lookAt = QVector3D(0,0,0);
    up = QVector3D(0,0,0);
}

void Camera::setPosition(QVector3D _position)
{
    position = _position;
    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAt, up);
}

void Camera::setLookAt(QVector3D _lookAt)
{
    lookAt = _lookAt;
    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAt, up);
}

void Camera::setUp(QVector3D _up)
{
    up = _up;
    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAt, up);
}

}

