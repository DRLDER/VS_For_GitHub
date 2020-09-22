#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QMainWindow>
#include "ui_Qt_firstapp.h"

class Qt_firstapp : public QMainWindow
{
    Q_OBJECT

public:
    Qt_firstapp(QWidget *parent = Q_NULLPTR);

private slots:
    void on_ClickRetBtn_clicked();


private:
    Ui::Qt_firstappClass ui;
};
