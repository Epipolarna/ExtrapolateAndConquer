#ifndef EXTRAPOLATEANDCONQUER_HPP
#define EXTRAPOLATEANDCONQUER_HPP

#include <QApplication>
#include "Graphics/GraphicsWindow.hpp"
#include "Graphics/ModelLoader.hpp"

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"
#include "Systems/Systems.hpp"

#include <QTimer>

class ExtrapolateAndConquer : QObject
{
    Q_OBJECT

public:
    ExtrapolateAndConquer(int argc, char *argv[]);

    void initialize() {}
    void update() {}
    void render() {}

    int run();

public slots:
    void loopBody();

public:
    EntityManager<Components> entityManager;

    QApplication* application;
    GraphicsWindow* graphicsWindow;

    QTimer* timer;
};

#endif // EXTRAPOLATEANDCONQUER_HPP
