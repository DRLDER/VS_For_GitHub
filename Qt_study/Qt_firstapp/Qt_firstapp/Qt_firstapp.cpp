#include "Qt_firstapp.h"

int n = 0;
QString a;

Qt_firstapp::Qt_firstapp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void Qt_firstapp::on_ClickRetBtn_clicked() {
    n += 1;
    a = QString::number(n);
    ui.ReturnLine->setText("you click the btn in ["+a+"] times!");
}

