#include "interactive.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QLineEdit>
#include <QCompleter>

#include <iostream>
#include <fstream>

#include <QDebug>

QString dbpth = "../release/database"; // TODO
QStringList opt;
int sz;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    Interactive::py_init();
    Interactive::db_init(dbpth+"/index.db");

    if( Interactive::Is_connectable() != 1 )
    {
        Interactive::ext("无法链接到数据库");
    }

    // 以下是选择实现

    // 搜索补全
    sz = Interactive::TableRows( "DataBase" );
    if( sz < 0 )
        Interactive::ext("DataBase TableRows");

    for( int i = 1; i <= sz; ++i )
        opt << Interactive::Select_col_row( "DataBase", "clssnm", i );
    if( sz > 0 )
    {
        QCompleter *MyInfor = new QCompleter(opt);
        MyInfor->setCaseSensitivity(Qt::CaseInsensitive);
        ui->chsLine->setCompleter( MyInfor );

    }

    connect( ui->chsOK, &QPushButton::clicked, this, [&]()->void{
        if( !ui->chsLine->text().isEmpty() )
        {
            QString clssnm = ui->chsLine->text();

            if( sz <= 0 )
            {
                Interactive::msb("Unknowed class name");
                return;
            }
            else if ( !opt.contains(clssnm) )
            {
                Interactive::msb("Unknowed class name");
                return;
            }

            QString where = "clssnm=\'" + clssnm + '\'';
            QString dbpos = Interactive::Select_col_where( "DataBase", "dbnm", where.toStdString() );

            if( dbpos[0] == '*' )
            {
                Interactive::msb("Failed to select dbnm from DataBase where "+where);
                return;
            }

            StartMainWin( dbpos, clssnm );
        }
    } );


    // 以下是导入实现
    connect( ui->impBut, &QPushButton::clicked, this, [&]()->void{
        ui->impLine->setText( QFileDialog::getOpenFileName(
                                  this,
                                  "import a database",
                                  "C:/",
                                  "Database(*.db)" )
                              );
    } );

    connect( ui->impOK, &QPushButton::clicked, this, [&]()->void{
       if( !ui->impLine->text().isEmpty() && !ui->ClassName->text().isEmpty() )
       {
           QString file = ui->impLine->text();
           // db 检查
           int chk = Interactive::check_db(file.toStdString());
           if( chk == -33 )
           {
               int rpl = QMessageBox::warning( NULL,
                                               "Warning",
                                               "Failed to check the db file\nAre you sure it\'s OK?",
                                               QMessageBox::Yes,
                                               QMessageBox::No|QMessageBox::Default
                                              );

               if( rpl == QMessageBox::No ) return;
           }
           else if( chk != 1 )
           {
               Interactive::msb("Broken db file");
               return;
           }
           // 获取 id
           QString id;
           if( sz == 0 )
               id = "0";
           else {
               QString t = Interactive::Select_col_row( "DataBase", "id", sz );
               if( t[0] == '*' )
               {
                   Interactive::msb("Failed to select id from DataBase since "+t );
                   return;
               }
               else
                   id = QString::number(t.toInt()+1);
           }
           QString dbpos = dbpth+"/class/"+id+".db";
           // 班名查重
           QString clssnm = ui->ClassName->text();
           QString where = "clssnm=\'"+clssnm+'\'';
           QString t = Interactive::Select_col_where( "DataBase", "id", where.toStdString() );
           if( t == "*33" )
           {
               Interactive::msb("Failed to connect to index.db");
               return;
           }
           else if( t[0] != '*' )
           {
               Interactive::msb("Duplicate names");
               return;
           }

           QStringList data;
           data << dbpos << clssnm;
           Interactive::Insert_Init( "DataBase", data );

           if( QFile::copy( file, dbpos ) )
                StartMainWin( dbpos , clssnm );
           else
               Interactive::msb("Failed to import db file");
       }

    } );

    // 以下是创建实现
    connect( ui->crtOK, &QPushButton::clicked, this, [&]()->void{
        if( ui->crtLine->text().isEmpty() ) return;
        // 班名查重
        QString clssnm = ui->crtLine->text();
        QString where = "clssnm=\'"+clssnm+'\'';
        QString t = Interactive::Select_col_where( "DataBase", "id", where.toStdString() );
        if( t == "*33" )
        {
            Interactive::msb("Failed to connect to index.db");
            return;
        }
        else if( t[0] != '*' )
        {
            Interactive::msb("Duplicate names");
            return;
        }
        // 获取 id
        QString id;
        if( sz == 0 )
            id = "0";
        else {
            QString t = Interactive::Select_col_row( "DataBase", "id", sz );
            if( t[0] == '*' )
            {
                Interactive::msb("Failed to select id from DataBase since "+t );
                return;
            }
            else
                id = QString::number(t.toInt()+1);
        }
        QString dbpos = dbpth+"/class/"+id+".db";

        std::ofstream f;
        f.open( dbpos.toStdString() );
        if( !f.good() )
        {
            Interactive::msb("Fail to create db file");
            return;
        }
        f.close();

        QStringList data;
        data << dbpos << clssnm;
        Interactive::Insert_Init( "DataBase", data );

        if( Interactive::create_db(dbpos.toStdString()) == 1 )
             StartMainWin( dbpos , clssnm );
        else
            Interactive::msb("Failed to create db file");

    } );

}

MainWindow::~MainWindow()
{
    delete ui;
    Interactive::py_close();
    qDebug() << "exit normally";
}

void MainWindow::StartMainWin(QString dbpos, QString clssnm)
{
    qDebug() << dbpos << clssnm;
    qApp->exit(0);
}
