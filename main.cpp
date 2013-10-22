#include <QApplication>
#include "Graphics/GraphicsWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphicsWindow w;
    w.show();

    return a.exec();
}
