#ifndef PLAYER_H
#define PLAYER_H

#include <QtOpenGL>

class Player
{
public:
    Player();

    QVector3D position, lookAt, up;
    QMatrix4x4 vMatrix;

    void setPosition(double x, double y, double z);
    void setLookAt(QVector3D);
    void setUp(QVector3D);

    /*
    void init();
    void lookAtUpdate(float dt);
    //void lookAtUpload(GLuint program);

    //void move(cv::Vec3f newPos);
    void moveForward(float dt);
    void moveBackward(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    void moveUp(float dt);
    void moveDown(float dt);
    void lookUp(float dt);
    void lookDown(float dt);
    void lookLeft(float dt);
    void lookRight(float dt);

    void xLook(float dt, int dx);
    void yLook(float dt, int dy);

    //cv::Vec3f getLookAtDirection() {return normalize(lookAtVector - position);};

    //cv::Vec3f position, lookAtVector, upVector;
    //cv::Mat lookAtMatrix;
    //GLfloat sensitivity, arrowSensitivity, movementSpeed;
    */
};

#endif // PLAYER_H
