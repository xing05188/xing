#include "simulator.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    PageReplacementSimulator w;
    w.show();
    return a.exec();
}
