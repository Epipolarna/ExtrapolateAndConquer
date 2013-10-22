#include "player.h"

Player::Player()
{
    //position = QVector3D(100, 50, 100);   // Bilen
    position = QVector3D(20, 20, 20);          // Monkey
    lookAt = QVector3D(0, 0, 0);
    up = QVector3D(0, 1, 0);

    vMatrix.lookAt(position, lookAt, up);
}

void Player::setPosition(double x, double y, double z){
    position = QVector3D(x,y,z);
    vMatrix.setToIdentity();
    vMatrix.lookAt(position, lookAt, up);
}

void Player::setLookAt(QVector3D _lookAt){
    lookAt = _lookAt;
    vMatrix.lookAt(position, lookAt, up);
}

void Player::setUp(QVector3D _up){
    up = _up;
    vMatrix.lookAt(position, lookAt, up);
}

