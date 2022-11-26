#include "tabledisplay.h"
#include "ui_tabledisplay.h"

TableDisplay::TableDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableDisplay)
{
    ui->setupUi(this);
    this->setFixedSize(size());
    this->setWindowIcon(QIcon("://spider.ico"));
    this->setWindowTitle("错题");
    Dexams = new QStandardItemModel(this);
    Dexams->setColumnCount(1);
    Dexams->setHeaderData(0,Qt::Horizontal, "考试");
    ui->ExamView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ExamView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ExamView->setModel(Dexams);

    Dstudents = new QStandardItemModel(this);
    Dstudents->setColumnCount(1);
    Dstudents->setHeaderData(0,Qt::Horizontal, "学生");
    ui->StudentView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->StudentView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->StudentView->setModel(Dstudents);

    Dwknss = new QStandardItemModel(this);
    Dwknss->setColumnCount(3);
    Dwknss->setHeaderData(0,Qt::Horizontal,"错题编号");
    Dwknss->setHeaderData(1,Qt::Horizontal,"错误频数");
    Dwknss->setHeaderData(2,Qt::Horizontal,"题型");
    ui->WknsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->WknsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->WknsView->setModel(Dwknss);

}


TableDisplay::~TableDisplay()
{
    delete ui;
}

void TableDisplay::shown(QStringList stuName,QStringList examName,QStringList knoid,QStringList knoName,QStringList knoCnt){

    for(int i = 0 ; i < stuName.length() ; ++i){
        QList<QStandardItem*> list1;
        list1 <<new QStandardItem(stuName[i]);
        Dstudents->appendRow(list1);
    }


    for(int i = 0 ; i < examName.length() ; ++i){
        QList<QStandardItem*> list2;
        list2 <<new QStandardItem(examName[i]);
        Dexams->appendRow(list2);
    }


    for(int i = 0 ; i < knoName.length() ; ++i){
        QList<QStandardItem*> list3;
        list3 <<new QStandardItem(knoid[i]) << new QStandardItem(knoCnt[i])<<new QStandardItem(knoName[i]);
        Dwknss->appendRow(list3);
    }

    this->show();
}

