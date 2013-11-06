#include "ExtrapolateAndConquer.hpp"

ExtrapolateAndConquer::ExtrapolateAndConquer(int argc, char *argv[])
{
    application = new QApplication(argc,argv);
    graphicsWindow = new GraphicsWindow();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loopBody()));
    timer->start(20);

}

int ExtrapolateAndConquer::run()
{
    graphicsWindow->show();
    int returnCode = application->exec();

    return returnCode;
}

void ExtrapolateAndConquer::loopBody()
{

}
