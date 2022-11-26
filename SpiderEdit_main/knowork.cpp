#include "knowork.h"
#include "ui_knowork.h"
/*
@author: lamaper
@Date: 2022/10/2
@Version: 0.1.0 Alhpa
*/
KnoWork::KnoWork(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KnoWork)
{
    ui->setupUi(this);
    connect(ui->KCancleBut , &QPushButton::clicked , this ,[&](){
        this->close();
    });
}

KnoWork::~KnoWork()
{
    delete ui;
}
