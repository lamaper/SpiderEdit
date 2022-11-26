#include "basework.h"
#include "ui_basework.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
/*
@author: lamaper
@Date: 2022/10/3
@Version: 0.1.0 Alhpa
*/
BaseWork::BaseWork(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BaseWork)
{
    ui->setupUi(this);
    ui->QuestView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setWindowIcon(QIcon("://spider.ico"));
    this->setFixedSize(size());
    connect(ui->CancleBut , &QPushButton::clicked , this ,[&](){
        BaseWork::clear();
        this->close();
    });

    connect( ui->OKBut, &QPushButton::clicked, this, [this]()->void{
        BaseWork::clear();
        if( !this->ischange ) {
            this->close();
            return;
        }

        int res = QMessageBox::warning( this, "WARNING",
                                        "您刚刚点击了“确定”按钮，这意味着所有修改将会写入数据库且所有改动无法撤回。\n\n"
                                        "您可以点击“确定”使得修改生效，或者点击\"取消\"关闭此对话框。如果您要确认您的修改，请确保仔细阅读以下内容。\n"
                                        "如果您点击“确定”，那么数据库中的先前数据将会覆盖，取而代之的是刚才窗口中的表格所展示的数据，这是一次无法撤回的修改。\n"
                                        "您必须知晓，在数据库的内部实现中，一律采用 ID 作为唯一识别依据，这也就意味这即便您不更改内容，仅仅是重新编号就会使得之前的数据无效。\n"
                                        "另外，即使您的操作正确，我们无法保证在大批量修改过程中出现某种未料及的情况造成数据的丢失。毕竟，定期备份数据总是一个好习惯。\n\n"
                                        "请问您是否确认将修改写入数据库？",
                                        QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel );

        if( res == QMessageBox::Ok ) {
                emit BaseWork::B_Change( this->idList, this->nameList );
                this->close();
        }
    } );

    // Add UI
    connect(ui->pushButton4Add , &QPushButton::clicked , this , [&](){
        if(ui->lineEdit4Add->text().toUtf8()!="" && !ui->lineEdit4AddID->text().isEmpty() ) {
            QString name = ui->lineEdit4Add->text().toUtf8(),
                    id = ui->lineEdit4AddID->text().toUtf8();

            if( this->nameNumber>0 && this->nameList.contains(name) )
            {
                BaseWork::showWarning("已存在该项目");
                return;
            }
            if( this-> nameNumber>0 && this->idList.contains(id) )
            {
                BaseWork::showWarning("已存在该项目");
                return;
            }

            this->ischange = true;

            this->nameList << name;
            this->idList << id;
            ++this->nameNumber;
//            this->nameidList << (__Id_Name){ id, name };

            QList<QStandardItem*> list;
            list << new QStandardItem(id) << new QStandardItem(name);
            this->nameModel->appendRow( list );

//            this->oprtAction << (__Action){ "add", id, name };

            ui->lineEdit4Add->clear();
        }
    } );

    // Remove UI
    connect(ui->pushButton4Remove, &QPushButton::clicked , this , [&](){
        // 根据 name 删
        if(ui->lineEdit4Remove->text().toUtf8() != "") {
            QString curtext = ui->lineEdit4Remove->text().toUtf8();

            for( int i = 0; i < this->nameNumber; ++i )
            {
                if( this->nameList[i] == curtext )
                {
                    this->ischange = true;

                    this->nameList.removeAt(i);
                    this->idList.removeAt(i);
                    --this->nameNumber;
                }
            }

            nameModel->clear();
            this->nameModel->setColumnCount(2);
            this->nameModel->setHeaderData(0,Qt::Horizontal, "ID");
            this->nameModel->setHeaderData(1,Qt::Horizontal, "CONTENT");
            for( int i = 0; i < this->nameNumber; ++i )
            {
                 QList<QStandardItem*> list;
                 list << new QStandardItem(this->idList[i]) << new QStandardItem(this->nameList[i]);
                 nameModel->appendRow(list);
            }

        }
        // 根据 id 删
        if( !ui->lineEdit4RemoveID->text().isEmpty() ) {
            QString curid = ui->lineEdit4RemoveID->text().toUtf8();

            for( int i = 0; i < this->nameNumber; ++i )
            {
                if( this->idList[i] == curid )
                {
                    this->ischange = true;

                    this->nameList.removeAt(i);
                    this->idList.removeAt(i);
                    --this->nameNumber;
                }
            }

            nameModel->clear();
            this->nameModel->setColumnCount(2);
            this->nameModel->setHeaderData(0,Qt::Horizontal, "ID");
            this->nameModel->setHeaderData(1,Qt::Horizontal, "CONTENT");
            for( int i = 0; i < this->nameNumber; ++i )
            {
                 QList<QStandardItem*> list;
                 list << new QStandardItem(this->idList[i]) << new QStandardItem(this->nameList[i]);
                 nameModel->appendRow(list);
            }
        }
    } );

    // Import UI
    connect(ui->pushButton4Imp, &QPushButton::clicked , this , [&]()->void{
        QString file = QFileDialog::getOpenFileName(  this,
                                                      "choose an Excel",
                                                      "C:/",
                                                      "Excel(*.xls *.xlsx)" );
        emit BaseWork::B_ReadExcel(file);

        this->ischange = true;
    } );
}

BaseWork::~BaseWork()
{
    delete ui;
}

void BaseWork::showWarning(QString warning)
{
    QMessageBox Dia(QMessageBox::Warning,"Warning",warning);
    Dia.exec();
}

void BaseWork::BaseWorkNameInit(QString name){
    this->setWindowTitle(name);
    QString labelText = "Edit your "+name;
    ui->label->setText(labelText);
    this->baseworkName = name;

    this->nameModel = new QStandardItemModel(this);
}

void BaseWork::__initTheBaseWork(QList<__Id_Name> NameIdList, int NameNumber){
    qSort( NameIdList.begin(), NameIdList.end(), [](const __Id_Name& a,const __Id_Name& b)->bool{
        return a.id.toInt() < b.id.toInt();
    } );

    this->nameNumber = NameNumber;
//    this->nameidList = NameIdList;

    this->nameList.clear();
    this->idList.clear();
    for( int i = 0; i < this->nameNumber; ++i )
    {
        this->idList << NameIdList[i].id;
        this->nameList << NameIdList[i].name;
    }

    this->nameModel->clear();
    this->nameModel->setColumnCount(2);
    this->nameModel->setHeaderData(0,Qt::Horizontal, "ID");
    this->nameModel->setHeaderData(1,Qt::Horizontal, "CONTENT");


    for( int i = 0; i < this->nameList.size(); ++i )
    {
        QList<QStandardItem*> list;
        list << new QStandardItem(this->idList[i]) << new QStandardItem(this->nameList[i]);
        this->nameModel->appendRow( list );
    }

    qDebug() << BaseWork::QSIMtoStringList( nameModel, 0 );
    qDebug() << BaseWork::QSIMtoStringList( nameModel, 1 );

    ui->QuestView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->QuestView->setModel(nameModel);
}

QStringList BaseWork::QSIMtoStringList(QStandardItemModel *model, int colmn)
{ // QSIM -> QStringList
    QStringList res;

    int rows = model->rowCount();
    for( int i = 0; i < rows ; ++i )
    {
        QStandardItem* item = model->item(i,colmn);

        if( item && !item->text().isEmpty() )
            res << item->text();
    }
    return res;
}

void BaseWork::clear() {
    ui->lineEdit4Add->clear();
    ui->lineEdit4AddID->clear();
    ui->lineEdit4Remove->clear();
    ui->lineEdit4RemoveID->clear();
}
