#include "Qt_HelloWorld.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qt_HelloWorld w;
    w.show();
    return a.exec();
}
