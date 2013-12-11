#include "Camera.hpp"

Camera::Camera()
{
    position = QVector3D(0,0,0);
    lookAtPoint = QVector3D(0,0,-1);
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
    updateLookAt();
}

void Camera::setLookAtDirection(QVector3D _lookAtDirection)
{
    lookAtDirection = _lookAtDirection;
    lookAtDirection.normalize();
    lookAtPoint = position + lookAtDirection;
    updateLookAt();
}

void Camera::setLookAtPoint(QVector3D _lookAtPoint)
{
    lookAtDirection = _lookAtPoint - position;
    lookAtDirection.normalize();
    lookAtPoint = position + lookAtDirection;
    updateLookAt();
}

void Camera::setUp(QVector3D _up)
{
    up = _up;
    up.normalize();
    updateLookAt();
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

    updateLookAt();
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
        rotationMatrix.setToIdentity();
        rotationMatrix.rotate(-rollSpeed*elapsedSeconds, lookAtDirection);
        up = rotationMatrix*up;
    }
    if(keyMap[Qt::Key_E]){
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
    position += velocityVector*translationSpeed*elapsedSeconds;
    lookAtPoint += velocityVector*translationSpeed*elapsedSeconds;

    updateLookAt();
}

void Camera::updateLookAt()
{
    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAtPoint, up);

    vMatrixInv = vMatrix;
    vMatrixInv.inverted();

    // Calculater bounding box of current frustum
    //qDebug() << "FRUSTUM CALC";
    //qDebug() << vMatrixInv*pMatrixInv*QVector4D(1,1,1,1);
    //qDebug() << vMatrixInv*pMatrixInv*QVector4D(1,1,0,1);

    for(int x = -1; x <= 1; x+=2){
        for(int y = -1; y <= 1; y+=2){
            for(int z = 0; z <= 1; z++){
                //qDebug() << QVector4D(x,y,z,1);
                //qDebug() << vMatrixInv*pMatrixInv*QVector4D(x,y,z,1);
            }
        }
    }

}

QMatrix4x4 Camera::skyboxMatrix()
{
    QMatrix4x4 skyMatrix = vMatrix;
    skyMatrix.setColumn(3, QVector4D(0,0,0,1));
    return skyMatrix;
}


