#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QLabel hello("Hello world!");

//    hello.show();
    w.show();
    return a.exec();
//    QApplication app(argc, argv);
//    QLabel hello("Hello world!");

//    hello.show();
//    return app.exec();
}
