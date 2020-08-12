#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Qt_HelloWorld.h"

class Qt_HelloWorld : public QMainWindow
{
    Q_OBJECT

public:
   
    Qt_HelloWorld(QWidget *parent = Q_NULLPTR);
    

private:
    Ui::Qt_HelloWorldClass ui;
};



