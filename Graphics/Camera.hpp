#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <QVector3D>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QElapsedTimer>

class Camera
{
public:
    Camera();

    QVector3D position, lookAtPoint, lookAtDirection, up;
    QMatrix4x4 vMatrix, rotationMatrix;
    float translationSpeed, pitchSpeed, rollSpeed, yawSpeed, mouseSensitivity;

    void setPosition(QVector3D position);
    void setLookAtDirection(QVector3D position);
    void setLookAtPoint(QVector3D _lookAtPoint);
    void setUp(QVector3D up);

    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void mouseMoveEvent(int dX, int dY);
    void updatePosition();

    QMatrix4x4 skyboxMatrix();

private:
    std::map<int, bool> keyMap;

    QElapsedTimer timer;
    qint64 elapsedTime;
    float elapsedSeconds;
};


#endif // CAMERA_HPP
