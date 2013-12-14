#ifndef AI_SYSTEM_HPP
#define AI_SYSTEM_HPP

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"
#include <opencv2/opencv.hpp>

class AISystem : public System<AI, Components>
{
public:
    void processStep(AI & ai) override {
        SpherePhysics & physics = getEntity(ai).get<SpherePhysics>();
        if(ai.hasLocationTarget) {
            // Calculate evasive movement direction considering hinderence from the influence map
            QVector2D movementDirection = ai.locationTarget - QVector2D(physics.position.x(), physics.position.z());
            QVector2D influenceVector = getLocalInfluenceDerivative(physics.position);
            movementDirection = (movementDirection.normalized()+2*influenceVector).normalized();

            // Move in movement direction
            physics.force += movementDirection*0.1;
            qDebug() << "movementDirection: " << movementDirection;
        }
    }
    void setInfluenceMap(cv::Mat & influenceMap) {
        this->influenceMap = influenceMap;
    }

private:
    cv::Mat influenceMap;
    QVector2D getLocalInfluenceDerivative(QVector3D & position) {
        QVector2D derivative = {0,0};
        cv::Point2i integerPosition(std::roundf(position.x()), std::roundf(position.z()));
        derivative -= QVector2D(0,1)      * influenceMap.at<uchar>(integerPosition + cv::Point2i(0,1));
        derivative -= QVector2D(0.5,0.5)  * influenceMap.at<uchar>(integerPosition + cv::Point2i(1,1));
        derivative -= QVector2D(1,0)      * influenceMap.at<uchar>(integerPosition + cv::Point2i(1,0));
        derivative -= QVector2D(0.5,-0.5) * influenceMap.at<uchar>(integerPosition + cv::Point2i(1,-1));
        derivative -= QVector2D(0,-1)     * influenceMap.at<uchar>(integerPosition + cv::Point2i(0,-1));
        derivative -= QVector2D(-0.5,-0.5)* influenceMap.at<uchar>(integerPosition + cv::Point2i(-1,-1));
        derivative -= QVector2D(-0.5,0.5) * influenceMap.at<uchar>(integerPosition + cv::Point2i(-1,1));
        return derivative;
    }
};

#endif
