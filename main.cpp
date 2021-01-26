#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
//    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication a(argc, argv);

    GLTest *t = new GLTest;
    t->start();
//    MainWindow w;
//    w.show();
    return a.exec();
}
