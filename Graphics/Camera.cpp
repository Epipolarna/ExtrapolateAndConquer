#include "Camera.hpp"

Camera::Camera()
{
    position = QVector3D(0,10,1);
    lookAtPoint = QVector3D(0,10,0);
    lookAtDirection = QVector3D(0,0,-1);
    up = QVector3D(0,1,0);

    translationSpeed = 20;  // m/s
    pitchSpeed = 120;       // degrees/s
    rollSpeed = 90;         // degrees/s
    yawSpeed = 160;         // degrees/s

    mouseSensitivity = (float) 10 /10000;
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

void Camera::mouseMoveEvent(int dX, int dY)
{
    QVector3D rightVector = lookAtDirection.crossProduct(lookAtDirection, up);
    rightVector.normalize();

    rotationMatrix.setToIdentity();
    rotationMatrix.rotate(pitchSpeed*dY*mouseSensitivity, rightVector);
    rotationMatrix.rotate(yawSpeed*dX*mouseSensitivity, up);
    lookAtDirection = rotationMatrix*lookAtDirection;
    up = rotationMatrix*up;
    lookAtPoint = position + lookAtDirection;

    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAtPoint, up);
}

void Camera::updatePosition()
{
    elapsedTime = timer.elapsed();
    timer.restart();
    elapsedSeconds = (float)elapsedTime/1000;

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
        rotationMatrix.rotate(pitchSpeed*elapsedSeconds, rightVector);
        lookAtDirection = rotationMatrix*lookAtDirection;
        up = rotationMatrix*up;
        lookAtPoint = position + lookAtDirection;
    }
    if(keyMap[Qt::Key_Left]){
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(yawSpeed*elapsedSeconds, up);
        lookAtDirection = rotationMatrix*lookAtDirection;
        lookAtPoint = position + lookAtDirection;
    }
    if(keyMap[Qt::Key_Down]){
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(-pitchSpeed*elapsedSeconds, rightVector);
        lookAtDirection = rotationMatrix*lookAtDirection;
        up = rotationMatrix*up;
        lookAtPoint = position + lookAtDirection;
    }
    if(keyMap[Qt::Key_Right]){
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(-yawSpeed*elapsedSeconds, up);
        lookAtDirection = rotationMatrix*lookAtDirection;
        lookAtPoint = position + lookAtDirection;
    }

    if(keyMap[Qt::Key_Q]){
        qDebug() << "rotating left";
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(-rollSpeed*elapsedSeconds, lookAtDirection);
        up = rotationMatrix*up;
    }
    if(keyMap[Qt::Key_E]){
        qDebug() << "rotating right";
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(rollSpeed*elapsedSeconds, lookAtDirection);
        up = rotationMatrix*up;
    }

    if(keyMap[Qt::Key_Space]){
        velocityVector += up;
    }
    if(keyMap[Qt::Key_Control]){
        velocityVector -= up;
    }

    velocityVector.normalize();
    qDebug() << velocityVector*translationSpeed*elapsedSeconds;
    position += velocityVector*translationSpeed*elapsedSeconds;
    lookAtPoint += velocityVector*translationSpeed*elapsedSeconds;

    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAtPoint, up);
}

QMatrix4x4 Camera::skyboxMatrix()
{
    QMatrix4x4 skyMatrix = vMatrix;
    skyMatrix.setColumn(3, QVector4D(0,0,0,1));
    return skyMatrix;
}


