#include "ExtrapolateAndConquer.hpp"

ExtrapolateAndConquer::ExtrapolateAndConquer(int argc, char *argv[])
{
    application = new QApplication(argc,argv);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loopBody()));
    timer->start(20);

}

int ExtrapolateAndConquer::run()
{
    w.show();
    int returnCode = application->exec();

    return returnCode;
}

void ExtrapolateAndConquer::loopBody()
{

}
