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

void Camera::controlEvent(QKeyEvent *e)
{
    qDebug() << "Camera controllEvent: " << e->text();

    QVector3D forwardVector = position - lookAt;
    forwardVector.setY(0);
    forwardVector.normalize();

    QVector3D leftVector = up.crossProduct(up, forwardVector);
    leftVector.normalize();

    float rotationAngle = 10;

    switch(e->key()){
    case Qt::Key_W:
        position = position + forwardVector;
        lookAt = lookAt + forwardVector;
        break;
    case Qt::Key_A:
        position = position + leftVector;
        lookAt = lookAt + leftVector;
        break;
    case Qt::Key_S:
        position = position - forwardVector;
        lookAt = lookAt - forwardVector;
        break;
    case Qt::Key_D:
        position = position - leftVector;
        lookAt = lookAt - leftVector;
        break;

    case Qt::Key_Up:
        vMatrix.translate(-position);
        vMatrix.rotate(rotationAngle, -leftVector);
        vMatrix.translate(position);
        break;
    case Qt::Key_Left:
        vMatrix.translate(-position);
        vMatrix.rotate(rotationAngle, up);
        vMatrix.translate(position);
        break;
    case Qt::Key_Down:
        vMatrix.translate(-position);
        vMatrix.rotate(rotationAngle, leftVector);
        vMatrix.translate(position);
        break;
    case Qt::Key_Right:
        vMatrix.translate(-position);
        vMatrix.rotate(rotationAngle, -up);
        vMatrix.translate(position);
        break;
    }

    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAt, up);
    qDebug() << position;
    qDebug() << lookAt;
    qDebug() << up;
}


}

