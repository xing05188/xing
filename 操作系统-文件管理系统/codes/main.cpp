#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("文件资源管理器-2351289周慧星");
    a.setApplicationVersion("1.0");
    a.setWindowIcon(QIcon("icons/app.png")); 
    MainWindow w;
    w.show();
    return a.exec();
}