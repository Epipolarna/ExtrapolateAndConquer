#ifndef OBJECT_H
#define OBJECT_H

#include "model.h"
#include "player.h"

#include <QGLShaderProgram>

class Object : protected QOpenGLFunctions
{
public:
    Object(Model* model, QGLShaderProgram* program);

    void draw(Player* player, QMatrix4x4 &pMatrix, GLuint tex0 = 0);

    Model* model;
    GLuint texture;
    QGLShaderProgram* program;
    QVector3D position;



    /*
    Object(	Model* _model, GLuint _program, cv::Vec4f shaderParametes, GLuint _texture0, GLuint _texture1, GLuint _specularityMap, GLuint _normalMap);
    ~Object();

    Object* clone();

    void init(	Model* model, GLuint _program,
                cv::Vec4f shaderParameters = cv::Vec4f(0.3f, 0.4f, 0.3f, 10),
                GLuint _texture0 = 0, GLuint _texture1 = 0,
                GLuint _specularityMap = 0, GLuint _normalMap = 0);

    void draw(Player* player, float dt, GLint numberOfPlanets = 0, GLfloat* positions = 0, GLfloat* radius = 0);
    void update(cv::Vec3f position,
                cv::Vec3f scale,
                cv::Vec3f rotAngles);
    void update(cv::Vec3f dl, float dt = 0);
    void satMapUpdate(std::list<Object*>& allPlanets, cv::Vec3f accMovement, float dt);

    void set(cv::Vec3f position,
                cv::Vec3f scale,
                cv::Vec3f rotAngles,
                cv::Vec3f velocity,
                float density);
    void setPosition(cv::Vec3f _position);

    void addSatellite(Object * object, float distance);
    void getSatellites(std::list<Object*>* objectList);

    void setOrbit(Object* orbits, float distance);

    cv::Vec3f position;
    cv::Vec3f scale;
    cv::Vec3f rotAngles;
    cv::Vec3f velocity;
    cv::Vec4f color;
    Object* orbits;
    Model* model;
    float distance;
    float mass;
    std::map<float, Object*> satelliteMap;

    GLuint program, texture0, texture1, normalMap, specularityMap;
    GLchar *vertexAttributeName, *normalAttributeName, *texCoordAttributeName, *colourAttributeName;
    GLfloat ambientCoeff, diffuseCoeff, specularCoeff;
    GLuint  specularExponent;
private:
    void updateMatrices();
    GLfloat transX, transY, transZ,
            scaleX, scaleY, scaleZ,
            angX, angY, angZ,
            speedX, speedY, speedZ;

    //cv::Mat scale;

    cv::Mat scaleTrans, rotX, rotY, rotZ;
    */
};

#endif // OBJECT_H
