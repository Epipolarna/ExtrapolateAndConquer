#include <QApplication>
#include "ExtrapolateAndConquer.hpp"
#include "Graphics/OpenGLWindow.hpp"

int main(int argc, char *argv[])
{
    ExtrapolateAndConquer ExNCo(argc,argv);

    return ExNCo.run();
}
