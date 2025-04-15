#include "qt_elevator_os.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qt_elevator_os w;
    w.move(700, 500);
    w.show();
    return a.exec();
}
