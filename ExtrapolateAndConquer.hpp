#ifndef EXTRAPOLATEANDCONQUER_HPP
#define EXTRAPOLATEANDCONQUER_HPP

#include <QApplication>
#include <QTimer>
#include <vector>

#include "Graphics/Object.hpp"
#include "Graphics/GraphicsWindow.hpp"
#include "Graphics/ModelLoader.hpp"

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"
#include "Systems/Systems.hpp"

#include "Graphics/Renderer.hpp"

#include "../ResourceManager.hpp"

class ExtrapolateAndConquer : QObject
{
    Q_OBJECT

public:
    ExtrapolateAndConquer(int argc, char *argv[]);
    ~ExtrapolateAndConquer();

    int run();

public slots:
    void loopBody();

public:

    ResourceManager rm;
    graphics::Camera cam;
    
    //EntityManager<Components> entityManager;

    std::vector<graphics::Object> *objectList;

    QApplication* application;
    GraphicsWindow* graphicsWindow;

    QTimer* timer;
};

#endif // EXTRAPOLATEANDCONQUER_HPP
