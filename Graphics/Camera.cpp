#include "Camera.hpp"

namespace graphics {

Camera::Camera()
{
    position = QVector3D(100,10,101);
    lookAtPoint = QVector3D(100,10,100);
    lookAtDirection = QVector3D(0,0,-1);
    up = QVector3D(0,1,0);

    translationSpeed = 0.4;
    pitchSpeed = 2;
    rollSpeed = 1;
    yawSpeed = 3;
}

void Camera::setPosition(QVector3D _position)
{
    position = _position;
    lookAtDirection = lookAtPoint - position;
    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAtPoint, up);
}

void Camera::setLookAt(QVector3D _lookAt)
{
    lookAtPoint = _lookAt;
    lookAtDirection = lookAtPoint - position;
    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAtPoint, up);
}

void Camera::setUp(QVector3D _up)
{
    up = _up;
    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAtPoint, up);
}

void Camera::keyPressEvent(QKeyEvent *e)
{
    keyMap[e->key()] = true;
}

void Camera::keyReleaseEvent(QKeyEvent *e)
{
    keyMap[e->key()] = false;
}

void Camera::updatePosition()
{
    elapsedTime = timer.elapsed();
    timer.start();
    //qDebug() << elapsedTime;

    QVector3D rightVector = lookAtDirection.crossProduct(lookAtDirection, up);
    rightVector.normalize();

    QVector3D velocityVector(0,0,0);

    if(keyMap[Qt::Key_W]){
        velocityVector += lookAtDirection;
    }
    if(keyMap[Qt::Key_A]){
        velocityVector -= rightVector;
    }
    if(keyMap[Qt::Key_S]){
        velocityVector -= lookAtDirection;
    }
    if(keyMap[Qt::Key_D]){
        velocityVector += rightVector;
    }

    if(keyMap[Qt::Key_Up]){
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(pitchSpeed, rightVector);
        lookAtDirection = rotationMatrix*lookAtDirection;
        up = rotationMatrix*up;
        lookAtPoint = position + lookAtDirection;
    }
    if(keyMap[Qt::Key_Left]){
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(yawSpeed, up);
        lookAtDirection = rotationMatrix*lookAtDirection;
        lookAtPoint = position + lookAtDirection;
    }
    if(keyMap[Qt::Key_Down]){
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(-pitchSpeed, rightVector);
        lookAtDirection = rotationMatrix*lookAtDirection;
        up = rotationMatrix*up;
        lookAtPoint = position + lookAtDirection;
    }
    if(keyMap[Qt::Key_Right]){
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(-yawSpeed, up);
        lookAtDirection = rotationMatrix*lookAtDirection;
        lookAtPoint = position + lookAtDirection;
    }

    if(keyMap[Qt::Key_Q]){
        qDebug() << "rotating left";
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(-rollSpeed, lookAtDirection);
        up = rotationMatrix*up;
    }
    if(keyMap[Qt::Key_E]){
        qDebug() << "rotating right";
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(rollSpeed, lookAtDirection);
        up = rotationMatrix*up;
    }

    if(keyMap[Qt::Key_Space]){
        velocityVector += up;
    }
    if(keyMap[Qt::Key_Control]){
        velocityVector -= up;
    }

    velocityVector.normalize();
    // TODO: Multiply with velocity constant
    position += velocityVector*translationSpeed;
    lookAtPoint += velocityVector*translationSpeed;

    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAtPoint, up);
}

QMatrix4x4 Camera::skyboxMatrix()
{
    QMatrix4x4 skyMatrix = vMatrix;
    skyMatrix.setColumn(3, QVector4D(0,0,0,1));
    return skyMatrix;
}

}

