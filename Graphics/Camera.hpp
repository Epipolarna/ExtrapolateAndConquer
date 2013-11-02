#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <QVector3D>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QElapsedTimer>

namespace graphics {

class Camera
{
public:
    Camera();

    QVector3D position, lookAtPoint, lookAtDirection, up;
    QMatrix4x4 vMatrix, rotationMatrix;
    float translationSpeed, pitchSpeed, rollSpeed, yawSpeed;

    void setPosition(QVector3D position);
    void setLookAt(QVector3D lookAt);
    void setUp(QVector3D up);

    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);
    void updatePosition();

    QMatrix4x4 skyboxMatrix();
private:
    std::map<int, bool> keyMap;

    QElapsedTimer timer;
    qint64 elapsedTime;
};

}

#endif // CAMERA_HPP
