#include <QApplication>
#include "Graphics/GraphicsWindow.hpp"
#include "Graphics/ModelLoader.hpp"

#include "ExtrapolateAndConquer.hpp"

int main(int argc, char *argv[])
{
    ExtrapolateAndConquer ExNCo(argc,argv);

    return ExNCo.run();
}
