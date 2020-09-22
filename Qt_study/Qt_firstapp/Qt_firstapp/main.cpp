#include "Qt_firstapp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qt_firstapp w;
    w.show();
    return a.exec();
}
