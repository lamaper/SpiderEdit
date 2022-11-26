#include "interactive.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QDateTime>
#include <QProcess>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SpiderEdit");
    this->setFixedSize(size());
    this->setWindowIcon(QIcon("://spider.ico"));
    // 创建 UI 的基本框架
    MainWindow::__init_UIStructureOfMainWindow();

    //===============================================================================================================================================
    // UI 发送信号

    // 登出
    connect(ui->LogOutBut , &QAction::triggered , this ,[=](){
        emit MainWindow::I_LogOut();
    });
    // 退出
    connect(ui->actionExit , &QAction::triggered , this ,[=](){
        qApp->exit(0);
    });
    // 保存 TODO
    connect(ui->SaveBut , &QAction::triggered , this ,[=](){
        emit MainWindow::I_Save(EDUclassName);
    });
    // 导出数据库
    connect(ui->ExpBut , &QAction::triggered , this ,[=](){
        emit MainWindow::I_Export(this->__dbposition);
    });

    // 添加考试
    connect(ui->ExamAddBut , &QAction::triggered , this , [=]()->void{
        label_begin:

        QLineEdit::EchoMode echoMode=QLineEdit::Normal;
        bool flag = false;
        QString text = QInputDialog::getText(this,"Add an exam","Input a name",echoMode,"",&flag);

        if( flag && !text.isEmpty() ) {

            if( this->_exaNum ) {
                QStringList exaname = MainWindow::analIdName( this->examIdName, "name" );

                if( exaname.contains(text) ) {
                    QMessageBox::warning( this, "Warning", "已存在考试。", QMessageBox::Ok, QMessageBox::Ok );
                    goto label_begin;
                }
            }


            int res = QMessageBox::warning( this, "Warning",
                                            "添加考试后不支持修改，请保证考试信息一次性输入正确。\n\n是否确认添加？",
                                            QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel );
            if( res == QMessageBox::Ok )
                emit I_AddExam( text, QDateTime::currentDateTime().toString("yyyy/MM/dd") );
        }
    } );

    //Submit page emit I_SubmitLinkToDB & I_SubmitImportFile
    connect(ui->S_SumitExamButton , &QPushButton::clicked , this , [=](){
        if( !ui->S_lineEdit4Imp->text().isEmpty() ) {
            emit MainWindow::I_SubmitImportFile( ui->S_lineEdit4Imp->text() );
            ui->S_lineEdit4Imp->clear();
            return;
        }

        if(ui->S_lineEdit4Exam->text().toUtf8() == "" ||
            ui->S_lineEdit4Student->text().toUtf8() == "" ||
            SubmitModel->rowCount() == 0){
            QMessageBox Dia(QMessageBox::Warning,"警告","您输入的信息不完全！");
            Dia.exec();

        } else {
            QString exa = ui->S_lineEdit4Exam->text();
            QString stu = ui->S_lineEdit4Student->text();

            ui->S_lineEdit4Wkns->clear();

            emit MainWindow::I_SubmitLinkToDB(exa,stu,SubmitModel);
        }
    } );

    // Quest page emit I_QuestLinkToDB
    connect(ui->Q_SearchButton , &QPushButton::clicked , this , [=](){
       if(this->StudentModel->columnCount() == 0 ||
           this->ExaModel->columnCount() == 0){
           QMessageBox Dia(QMessageBox::Warning,"警告","您输入的信息不完全！");
           Dia.exec();

       } else {
           ui->Q_lineEdit4Exa->clear();
           ui->Q_lineEdit4Student->clear();
           /*interface to db*/
           emit MainWindow::I_QuestLinkToDB(this->StudentModel,this->ExaModel);
           this->StudentModel->removeRows(0, this->StudentModel->rowCount());
           this->ExaModel->removeRows(0, this->ExaModel->rowCount());
           QMessageBox Dia(QMessageBox::Information,"通知","查询成功");
           Dia.exec();
       }
    });

    //显示查询结果
    connect(this , &MainWindow::I_Display , this , [=](QStringList student,QStringList exam,QStringList knoid,QStringList knoname,QStringList knocnt){
        TableDisplay *tab;
        tab = new TableDisplay();
        tab->shown(student,exam,knoid,knoname,knocnt);
    });
    //===============================================================================================================================================
    // 槽函数

    // 承接 导出数据库 信号 MainWindow::I_Export
    connect( this, &MainWindow::I_Export, this, [=](QString dbfile)->void{
        QString svnm = QFileDialog::getSaveFileName( this,
                                                    "Save dbfile",
                                                    "C:/"+this->EDUclassName+".db",
                                                    "Database(*.db)" );

        if( QFile::copy(dbfile,svnm) )
        {
            QMessageBox Dia(QMessageBox::Information,"通知","导出成功！");
            Dia.exec();
            return;
        } else {
            Interactive::msb("导出失败");
            return;
        }

    } );

    // 承接 Submit page 信号 MainWindow::I_SubmitImportFile
    connect( this, &MainWindow::I_SubmitImportFile, this, [=](QString excelfile)->void{
        QStringList student, exam, knowledge;

        if( Interactive::Que_xls( excelfile.toStdString(), "submit", "student", student ) <= 0 ||
            Interactive::Que_xls( excelfile.toStdString(), "submit", "exam", exam ) <= 0 ||
            Interactive::Que_xls( excelfile.toStdString(), "submit", "knowledge", knowledge ) <= 0
        ) {
            QMessageBox Dia(QMessageBox::Warning,"Warning","查询 EXCEL 失败，或者未能查到数据");
            Dia.exec();
            return;
        }

        qDebug() << student << exam << knowledge;

        if( student[0][0]=='*' || exam[0][0]=='*' || knowledge[0][0]=='*' )
        {
            QMessageBox Dia(QMessageBox::Warning,"Warning","查询 EXCEL 失败");
            Dia.exec();
            return;
        }
        else if( student.size() != exam.size() || student.size() != knowledge.size() )
        {
            QMessageBox Dia(QMessageBox::Warning,"Warning","数据数量不对应");
            Dia.exec();
            return;
        }

        bool flag = false; QStringList line; line << "*";
        bool err = false; QStringList errline;
        for( int i = 0; i < student.size(); ++i )
        {
            QString exanm = exam[i], stunm = student[i];

            // 检查 考试和学生
            QStringList examName = MainWindow::analIdName( this->examIdName, "name" ),
                        studentName = MainWindow::analIdName( this->studentIdName, "name" ),
                        knowledgeId = MainWindow::analIdName( this->knowledgeIdName, "id" );
            if( this->_knoNum==0 || this->_exaNum==0 || this->_stuNum==0 )
            {
                flag = true; break;
            }
            else if( !examName.contains(exanm) || !studentName.contains(stunm) )
            {
                flag = true; line << QString::number(i); continue;
            }

            // 处理知识点
            QStringList kno = knowledge[i].split('*',QString::SkipEmptyParts); // 分割

            if( kno.size() == 0 ) continue;

            kno = kno.toSet().toList(); // 去重

            // 该知识点编号是否存在
            for( int j = 0; j < kno.size(); ++j )
            {
                if( !knowledgeId.contains(kno[j]) )
                {
                    flag = true;
                    kno.removeAt(j);
                    if( !line.contains(QString::number(i)) )
                        line << QString::number(i);
                }
            }

            if( !kno.size() )
            {
                continue;
            }

            qSort( kno.begin(), kno.end(), [](const QString& s1,const QString& s2)->bool{
                return s1 < s2;
            } ); // 排序

            QString wksid = kno.join('*');

            qDebug() << exanm << stunm << wksid;

            QString res = MainWindow::submitInsert( exanm, stunm, wksid );

            if( res != "!" ) {
                err = true; errline << QString::number(i) + ": " + res;
            }
        }
        if( flag )
        {
            QString warning = "存在无法识别的知识点编号/学生/考试，已自动忽略\n" + line.join(", ");
            QMessageBox Dia(QMessageBox::Warning,"Warning", warning);
            Dia.exec();
        }
        if( err )
        {
            QString error = "存在插入错误，已自动忽略\n" + errline.join('\n');
            QMessageBox Dia(QMessageBox::Warning,"Warning", error);
            Dia.exec();
        }

        QMessageBox Dia(QMessageBox::Information,"Information", "提交完成，如果没有报错就是成功了");
        Dia.exec();
    } );

    // 承接 Submit page 信号 MainWindow::I_SubmitLinkToDB
    connect( this, &MainWindow::I_SubmitLinkToDB, this, [=](QString exa, QString stu, QStandardItemModel *sub)->void{
        // 检查 考试、学生 是否合法
        QStringList examName = MainWindow::analIdName( this->examIdName, "name" ),
                    studentName = MainWindow::analIdName( this->studentIdName, "name" ),
                    knowledgeName = MainWindow::analIdName( this->knowledgeIdName, "name" );
        if( this->_exaNum <= 0 || this->_stuNum <= 0 )
        {
            QMessageBox Dia(QMessageBox::Warning,"警告","未知的学生/考试");
            Dia.exec();
            return;
        }
        else if( !examName.contains(exa) || !studentName.contains(stu) )
        {
            QMessageBox Dia(QMessageBox::Warning,"警告","未知的学生/考试");
            Dia.exec();
            return;
        }

        // 获取并检查 知识点
        if( this->_knoNum <= 0 ) {
            QMessageBox Dia(QMessageBox::Warning,"Warning","未知的知识点");
            Dia.exec();
            return;
        }

        QStringList wksid_list;
        // 遍历表格
        bool flag = false; QStringList warning;
        int rows = sub->rowCount();
        for( int i = 0; i < rows; ++i )
        {
            QStandardItem* item = sub->item(i,0);

            if( item && !item->text().isEmpty() )
            {
                QString tmp = item->text();

                if( !knowledgeName.contains(tmp) ) {
//                    QMessageBox Dia(QMessageBox::Warning,"Warning","未知的知识点");
//                    Dia.exec();
//                    return;
                    flag = true; warning << tmp;
                    continue;
                }

//                where = "content=\'" + tmp + '\'';
//                wksid_list << Interactive::Select_col_where("KNOWLEDGE","id",where.toStdString());
                for( int j = 0; j < this->knowledgeIdName.size(); ++j )
                {
                    if( this->knowledgeIdName[j].name == tmp )
                    {
                        wksid_list << this->knowledgeIdName[j].id;
                        break;
                    }
                }
            }
        }
        // 排序去重后分割
        qSort( wksid_list.begin(), wksid_list.end(), [](const QString& s1,const QString& s2)->bool{
            return s1 < s2;
        } );
        QString wksid = wksid_list.toSet().toList().join('*');

        if( wksid.isEmpty() )
        {
            QMessageBox Dia(QMessageBox::Warning,"Warning","未识别知识点");
            Dia.exec();
            return;
        }

        if( flag ) {
            QMessageBox Dia(QMessageBox::Warning,"Warning","存在未知的知识点，已自动忽略\n"+warning.join('\n'));
            Dia.exec();
        }

        QString res = MainWindow::submitInsert( exa, stu, wksid );
        if( res == "!" )
        {
            QMessageBox Dia(QMessageBox::Information,"Information", "提交成功");
            Dia.exec();
        } else {
            Interactive::msb( res );
        }

    } );

    // 承接 BaseWork 信号 BaseWork::B_ReadExcel
    connect( this->knoWork , &BaseWork::B_ReadExcel, this, [=](QString file)->void{
        QStringList id, name;

        qDebug() << file;

        if( Interactive::Que_xls(file.toStdString(),"knowledge","id",id) <= 0 ||
            Interactive::Que_xls(file.toStdString(),"knowledge","content",name) <= 0
        ) {
            this->knoWork->showWarning("查询 EXCEL 失败，或未能查到数据");
            return;
        }

        if( !id.size() || !name.size() ) {
            this->knoWork->showWarning("未获取到信息");
            return;
        }
        else if( id[0][0] == '*' || name[0][0] == '*' ) {
            this->knoWork->showWarning("未获取到信息");
            return;
        }
        else if( id.size() != name.size() ) {
            this->knoWork->showWarning("数据数量不对应");
            return;
        }

        QList<__Id_Name> res;
        for( int i = 0; i < id.size(); ++i )
            res << (__Id_Name){ id[i], name[i] };

        this->knoWork->__initTheBaseWork( res, res.size() );
    } );
    connect( this->examWork , &BaseWork::B_ReadExcel, this, [=](QString file)->void{
        QStringList id, name;

        if( Interactive::Que_xls(file.toStdString(),"exam","id",id) <= 0 ||
            Interactive::Que_xls(file.toStdString(),"exam","content",name) <= 0
        ) {
            this->knoWork->showWarning("查询 EXCEL 失败，或未能查到数据");
            return;
        }

        if( !id.size() || !name.size() ) {
            this->examWork->showWarning("未获取到信息");
            return;
        }
        else if( id[0][0] == '*' || name[0][0] == '*' ) {
            this->examWork->showWarning("未获取到信息");
            return;
        }
        else if( id.size() != name.size() ) {
            this->examWork->showWarning("数据数量不对应");
            return;
        }

        QList<__Id_Name> res;
        for( int i = 0; i < id.size(); ++i )
            res << (__Id_Name){ id[i], name[i] };

        this->examWork->__initTheBaseWork( res, res.size() );
    } );
    connect( this->stuWork , &BaseWork::B_ReadExcel, this, [=](QString file)->void{
        QStringList id, name;

        if( Interactive::Que_xls(file.toStdString(),"class","id",id) <= 0 ||
            Interactive::Que_xls(file.toStdString(),"class","content",name) <= 0
        ) {
            this->knoWork->showWarning("查询 EXCEL 失败，或未能查到数据");
            return;
        }

        if( !id.size() || !name.size() ) {
            this->stuWork->showWarning("未获取到信息");
            return;
        }
        else if( id[0][0] == '*' || name[0][0] == '*' ) {
            this->stuWork->showWarning("未获取到信息");
            return;
        }
        else if( id.size() != name.size() ) {
            this->stuWork->showWarning("数据数量不对应");
            return;
        }

        QList<__Id_Name> res;
        for( int i = 0; i < id.size(); ++i )
            res << (__Id_Name){ id[i], name[i] };

        this->stuWork->__initTheBaseWork( res, res.size() );
    } );

    // 承接 BaseWork 信号 BaseWork::B_Change
    connect( this->knoWork, &BaseWork::B_Change, this, [=](QStringList id,QStringList name)->void{
        Interactive::Del_where( "KNOWLEDGE", "1=1" );
        this->knowledgeIdName.clear();

        QStringList data;
        QList<__Id_Name> IdName;
        for( int i = 0; i < id.size(); ++i )
        {
            data.clear();
            data << id[i] << name[i];
            Interactive::Insert_Init( "KNOWLEDGE", data );

            IdName << (__Id_Name){id[i],name[i]};
        }

        MainWindow::__init_knoNum( id.size() );
        MainWindow::__init_knon( IdName );

        qDebug() << this->knowledgeIdName.size();
    } );
    connect( this->stuWork, &BaseWork::B_Change, this, [=](QStringList id,QStringList name)->void{
        Interactive::Del_where( "STUDENT", "1=1" );

        QStringList data;
        QList<__Id_Name> IdName;
        for( int i = 0; i < id.size(); ++i )
        {
            data.clear();
            data << id[i] << name[i];
            Interactive::Insert_Init( "STUDENT", data );

            IdName << (__Id_Name){id[i],name[i]};
        }

        MainWindow::__init_stuNum( id.size() );
        MainWindow::__init_stun( IdName );
    } );
    // TODO
/*    connect( this->examWork, &BaseWork::B_Change, this, [=](QStringList id,QStringList name)->void{
        Interactive::Del_where( "EXAM", "1=1" );

        QStringList data;
        QList<__Id_Name> IdName;
        for( int i = 0; i < id.size(); ++i )
        {
            data.clear();
            data << id[i] << name[i];
            Interactive::Insert_Init( "EXAM", data );

            IdName << (__Id_Name){id[i],name[i]};
        }

        MainWindow::__init_exaNum( id.size() );
        MainWindow::__init_exan( IdName );
    } ); */

    // 承接 I_AddExam
    connect( this, &MainWindow::I_AddExam, this, [=](QString examname,QString date)->void{
        qDebug() << examname;

        QStringList data;
        data << date << examname;
        int res = Interactive::Insert_Init( "EXAM", data );

        QString id = Interactive::Select_col_where( "EXAM", "id", "des=\'"+examname.toStdString()+'\'' );

        if( res <= 0 || id[0] == '*' ) {
            Interactive::msb("考试插入失败");
            return;
        }

        QList<__Id_Name> newexaIdName = this->examIdName;
        newexaIdName << (__Id_Name){ id, examname };

        MainWindow::__init_exaNum( this->_exaNum+1 );
        MainWindow::__init_exan( newexaIdName );

        qDebug() << MainWindow::analIdName(this->examIdName,"name");
        qDebug() << MainWindow::analIdName(this->examIdName,"id");
    } );

    // 承接 Quest page 信号 MainWindow::I_QuestLinkToDB
    connect( this, &MainWindow::I_QuestLinkToDB, this, [=](QStandardItemModel* student,QStandardItemModel* exam)->void{
        if( !this->_stuNum || !this->_exaNum || !this->_knoNum )
            return;

        QStringList studentL, examL;

        // 遍历表格
        QStringList stuName = MainWindow::analIdName( this->studentIdName, "name" );
        int rows = student->rowCount();
        for( int i = 0; i < rows; ++i )
        {
            QStandardItem* item = student->item(i,0);
            if( item && !item->text().isEmpty() )
            {
                QString tmp = item->text();

                if( tmp == "全班" ) {
                    studentL = stuName;
                    break;
                }

                if( !stuName.contains(tmp) ) {
                    continue;
                }
                studentL << tmp;
            }
        }

        QStringList exaName = MainWindow::analIdName( this->examIdName, "name" );
        rows = exam->rowCount();
        for( int i = 0; i < rows; ++i )
        {
            QStandardItem* item = exam->item(i,0);
            if( item && !item->text().isEmpty() )
            {
                QString tmp = item->text();

                if( tmp == "全部考试" ){
                    examL = exaName;
                    break;
                }

                if( !exaName.contains(tmp) ) {
                    continue;
                }
                examL << tmp;
            }
        }

        // name to id
        studentL = studentL.toSet().toList();
        examL = examL.toSet().toList();
        QList<__Id_Name> stuidname, exaidname;
        for( const QString &name : qAsConst(studentL) )
        {
            for( const __Id_Name &idname : qAsConst(this->studentIdName) )
                if( idname.name == name ) {
                    stuidname << idname;
                    break;
                }
        }
        for( const QString &name : qAsConst(examL) )
        {
            for( const __Id_Name &idname : qAsConst(this->examIdName) )
                if( idname.name == name ) {
                    exaidname << idname;
                    break;
                }
        }

        qSort( stuidname.begin(), stuidname.end(), [](const __Id_Name &a,const __Id_Name &b)->bool{
            return a.id.toInt() < b.id.toInt();
        } );
        qSort( exaidname.begin(), exaidname.end(), [](const __Id_Name &a,const __Id_Name &b)->bool{
            return a.id.toInt() < b.id.toInt();
        } );

        QStringList studentid, studentname, examid, examname;
        for( const __Id_Name &idname : qAsConst(stuidname) ) {
            studentid << idname.id;
            studentname << idname.name;
        }
        for( const __Id_Name &idname : qAsConst(exaidname) ) {
            examid << idname.id;
            examname << idname.name;
        }

        qDebug() << studentid << examid;

        // 查询知识点
        QStringList knowledgeid, knowledgename, knowledgecnt;

        QMap<QString,int> cnt;
        for( const QString &exaid : qAsConst(examid) )
            for( const QString &stuid : qAsConst(studentid) )
            {
                QString where = "stu_id=\'" + stuid + "\' AND exam_id=\'" + exaid +'\'';
                QString res = Interactive::Select_col_where( "WEAK", "knowledge_id", where.toStdString() );

                if( res[0] == '*' ) continue;

                const QStringList ress = res.split('*').toSet().toList();
                for( const QString &knoid : ress ) {
                    ++cnt[knoid];
                    if( !knowledgeid.contains(knoid) )
                            knowledgeid << knoid;
                }
            }

        knowledgeid = knowledgeid.toSet().toList();
        qSort( knowledgeid.begin(), knowledgeid.end(), [](const QString &a,const QString &b)->bool{
            return a.toInt() < b.toInt();
        } );

        for( const QString &knoid : qAsConst(knowledgeid) )
            for( const __Id_Name &idname : qAsConst(this->knowledgeIdName) )
                if( idname.id == knoid ) {
                    knowledgename << idname.name;
                    break;
                }

        for( const QString &knoid : qAsConst(knowledgeid) ) {
            knowledgecnt << QString::number(cnt[knoid]);
        }

        qDebug() << studentname << examname;
        qDebug() << knowledgeid << knowledgename << knowledgecnt;

        emit I_Display( studentname, examname, knowledgeid, knowledgename, knowledgecnt );
    } );
    //===============================================================================================================================================

    connect( this, &MainWindow::I_LogOut, this, []()->void{
        QProcess::startDetached("./SpiderEditWelcomepage.exe");
        qApp->exit(0);
    } );
}

MainWindow::~MainWindow()
{
    MainWindow::__CLOSE();

    delete ui;

    delete this->examWork;
    delete this->knoWork;
    delete this->stuWork;

    qDebug() << "exit normally";
}

// 函数实现

QString MainWindow::submitInsert(QString exa, QString stu, QString wksid)
{
    QString exaid = "*", stuid = "*";
    for( int i = 0; i < this->examIdName.size(); ++i )
    {
        if( this->examIdName[i].name == exa )
        {
            exaid = this->examIdName[i].id;
            break;
        }
    }
    for( int i = 0; i < this->studentIdName.size(); ++i )
    {
        if( this->studentIdName[i].name == stu )
        {
            stuid = this->studentIdName[i].id;
            break;
        }
    }

    if( exaid[0] == '*' || stuid[0] == '*' )
    {
        return "查询考试/学生失败";
    }

    // 如果已存在记录直接修改
    QString where = "stu_id=\'" + stuid + "\' And exam_id=\'" + exaid + '\'';
    QString res = Interactive::Select_col_where("WEAK","knowledge_id",where.toStdString());
    if( res[0] != '*' )
    {
        if( Interactive::Modify_col_where("WEAK","knowledge_id",where.toStdString(),wksid.toStdString()) == 1 )
        {
            this->SubmitModel->removeRows(0,SubmitModel->rowCount());
            return "!";
        } else {
            return "Modify_col_where";
        }
    }

    // 插入数据
    QStringList data;
    data << stuid << exaid << wksid;

    qDebug() << stuid << exaid << wksid;

    if( Interactive::Insert_Init("WEAK",data) < 0 )
        return "Insert_Init";
    else {
        this->SubmitModel->removeRows(0,SubmitModel->rowCount());
        return "!";
    }
}

void MainWindow::__INIT( QString dbpos, QString clssnm )
{
    // 初始化 编辑考试 和 编辑知识点 两个子窗口
    this->examWork->BaseWorkNameInit("exam");
    this->knoWork->BaseWorkNameInit("knowledge");
    this->stuWork->BaseWorkNameInit("class");

    // 与数据库交互
    MainWindow::__init_dbpos( dbpos );
    MainWindow::__init_clssn( clssnm );

    Interactive::db_init(__dbposition);
    Interactive::py_init();

    if( Interactive::Is_connectable() != 1 )
        Interactive::ext("无法连接到数据库 "+__dbposition);

    // 查询所有的 学生、知识点、考试 及其对应编号
    QList<__Id_Name> stu, kno, exa;

    int stur = Interactive::TableRows("STUDENT");
    for( int i = 1; i <= stur; ++i )
        stu << (__Id_Name){Interactive::Select_col_row("STUDENT","id",i),Interactive::Select_col_row("STUDENT","name",i)};

    int knor = Interactive::TableRows("KNOWLEDGE");
    for( int i = 1; i <= knor; ++i )
        kno << (__Id_Name){Interactive::Select_col_row("KNOWLEDGE","id",i),Interactive::Select_col_row("KNOWLEDGE","content",i)};

    int exar = Interactive::TableRows("EXAM");
    for( int i = 1; i <= exar; ++i )
        exa << (__Id_Name){Interactive::Select_col_row("EXAM","id",i),Interactive::Select_col_row("EXAM","des",i)};

    if( stur < 0 || knor < 0 || exar < 0 )
        Interactive::ext("TableRows");

    // 初始化 学生、考试、知识点 的个数
    MainWindow::__init_stuNum(stur);
    MainWindow::__init_knoNum(knor);
    MainWindow::__init_exaNum(exar);
    // 初始化 学生、考试、知识点 的具体内容
    MainWindow::__init_data(stu,kno,exa);
}

void MainWindow::__init_data(QList<__Id_Name> Student, QList<__Id_Name> Knoledge, QList<__Id_Name> Exam){
    // 初始化 学生、考试、知识点 的具体内容
    __init_stun(Student);
    __init_knon(Knoledge);
    __init_exan(Exam);

    // 输出调试
//    for( int i = 0; i < this->studentIdName.size(); ++i )
//        qDebug() << this->studentIdName[i].id << this->studentIdName[i].name;
//    qDebug() << this->studentId;
//    qDebug() << this->studentName;

//    for( int i = 0; i < this->knowledgeIdName.size(); ++i )
//        qDebug() << knowledgeIdName[i].id << this->knowledgeIdName[i].name;
//    qDebug() << this->knowledgeId;
//    qDebug() << this->knowledgeName;

//    for( int i = 0; i < this->examIdName.size(); ++i )
//        qDebug() << this->examIdName[i].id << this->examIdName[i].name;
//    qDebug() << this->examId;
//    qDebug() << this->examName;

}

void MainWindow::__init_stun(QList<__Id_Name> Student)
{
    if( this->_stuNum == 0 ) return;

    // 更新类中相关变量
    this->studentIdName = Student;

    this->stuWork->__initTheBaseWork( this->studentIdName, this->_stuNum );


    // 设置 UI
    QStringList StudentName;
    for( int i = 0; i < this->studentIdName.size(); ++i )
        StudentName << this->studentIdName[i].name;

    QStringList StudentId;
    for( int i = 0; i < this->studentIdName.size(); ++i )
        StudentId << this->studentIdName[i].id;

    ui->S_combo4Student->clear();
    ui->S_combo4Student->addItems(StudentName);

    QCompleter *comS = new QCompleter(StudentName, this);
    comS->setCaseSensitivity(Qt::CaseInsensitive);
    ui->S_lineEdit4Student->setCompleter(comS);

    StudentName << "全班";
    QCompleter *comS1 = new QCompleter(StudentName, this);
    comS->setCaseSensitivity(Qt::CaseInsensitive);
    ui->Q_lineEdit4Student->setCompleter(comS1);
}

void MainWindow::__init_knon(QList<__Id_Name> Knowledge)
{
    if( this->_knoNum == 0 ) return;

    this->knowledgeIdName = Knowledge;

    QStringList KnoName;
    for( int i = 0 ;i < Knowledge.size(); ++i )
        KnoName << Knowledge[i].name;

    QStringList KnoId;
    for( int i = 0 ;i < Knowledge.size(); ++i )
        KnoId << Knowledge[i].id;


    this->knoWork->__initTheBaseWork(this->knowledgeIdName,this->_knoNum);

    QCompleter *comK = new QCompleter(KnoName, this);
    comK->setCaseSensitivity(Qt::CaseInsensitive);
    ui->S_lineEdit4Wkns->setCompleter(comK);
}

void MainWindow::__init_exan(QList<__Id_Name> Exam)
{
    if( this->_exaNum == 0 ) return;

    this->examIdName = Exam;

    QStringList ExamName;
    for( int i = 0; i < Exam.size(); ++i )
        ExamName << Exam[i].name;

    QStringList ExamId;
    for( int i = 0; i < Exam.size(); ++i )
        ExamId << Exam[i].id;


    ui->S_combo4Exam->clear();
    ui->S_combo4Exam->addItems(ExamName);

    this->examWork->__initTheBaseWork(this->examIdName,this->_exaNum);

    QCompleter *comE = new QCompleter(ExamName, this);
    comE->setCaseSensitivity(Qt::CaseInsensitive);
    ui->S_lineEdit4Exam->setCompleter(comE);

    ExamName << "全部考试";
    QCompleter *comE1 = new QCompleter(ExamName, this);
    comE->setCaseSensitivity(Qt::CaseInsensitive);
    ui->Q_lineEdit4Exa->setCompleter(comE1);
}

void MainWindow::__init_UIStructureOfMainWindow()
{
    //------------------------------------------------------------------------------
    // 打开子窗口
//    connect(ui->ExamEditBut , &QAction::triggered , this , [=](){
//        examWork->show();
//    });
    connect(ui->KnoEditBut , &QAction::triggered , this , [=](){
        knoWork->show();
    });
    connect(ui->ClsEditBut , &QAction::triggered , this , [=](){
        stuWork->show();
    });

    //******************************************************************************

    //-------------------------------------------------------------------------------
    //init SubmitView
    SubmitModel = new QStandardItemModel(this);
    SubmitModel->setColumnCount(1);
    SubmitModel->setHeaderData(0,Qt::Horizontal, "知识点");
    ui->S_WknsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->S_WknsView->setModel(SubmitModel);
    //init StudentView
    StudentModel = new QStandardItemModel(this);
    StudentModel->setColumnCount(1);
    StudentModel->setHeaderData(0,Qt::Horizontal, "学生姓名");
    ui->Q_tableView4Student->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->Q_tableView4Student->setModel(StudentModel);
    //init KnowledegeView
    ExaModel = new QStandardItemModel(this);
    ExaModel->setColumnCount(1);
    ExaModel->setHeaderData(0,Qt::Horizontal, "考试");
    ui->Q_tableView4Kno->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->Q_tableView4Kno->setModel(ExaModel);
    //init combo
    QString initText = "null";
    ui->S_combo4Exam->addItem(initText);
    ui->S_combo4Student->addItem(initText);
    //***********************************************************************************

    //------------------------------------------------------------------------------------
    // Submit page UI 实现
    connect(ui->S_pushButton4Wkns , &QPushButton::clicked , this , [=](){
        if(ui->S_lineEdit4Wkns->text().toUtf8() != ""){
            QList<QStandardItem*> list;
            QString input = ui->S_lineEdit4Wkns->text().toUtf8();
            list <<new QStandardItem(input);
            SubmitModel->insertRow(0,list);
            ui->S_lineEdit4Wkns->clear();
        }
    });
    connect(ui->S_pushButton4clr , &QPushButton::clicked , this , [=](){
            SubmitModel->clear();
            SubmitModel->setColumnCount(1);
            SubmitModel->setHeaderData(0,Qt::Horizontal, "知识点");
            ui->S_WknsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    });
    //**************************************************************************************

    //-----------------------------------------------------------------------------------
    //Quest page UI 实现
    connect(ui->Q_pushButton4Student , &QPushButton::clicked , this , [=](){
        if(ui->Q_lineEdit4Student->text().toUtf8() != ""){
            QList<QStandardItem*> list;
            QString input = ui->Q_lineEdit4Student->text().toUtf8();
            list << new QStandardItem(input);
            StudentModel->insertRow(0,list);
            ui->Q_lineEdit4Student->clear();
        }
    });

    connect(ui->Q_pushButton4Kno , &QPushButton::clicked , this , [=](){
        if(ui->Q_lineEdit4Exa->text().toUtf8() != ""){
            QList<QStandardItem*> list;
            QString input = ui->Q_lineEdit4Exa->text().toUtf8();
            list <<new QStandardItem(input);
            ExaModel->insertRow(0,list);
            ui->Q_lineEdit4Exa->clear();
        }
    });

    connect( ui->Q_ClearButton, &QPushButton::clicked , this , [=]()->void{
        this->StudentModel->removeRows(0, this->StudentModel->rowCount());
        this->ExaModel->removeRows(0, this->ExaModel->rowCount());
    } );
    //************************************************************************************

    //-----------------------------------------------------------------------------------
    //Submit page UI 实现
    connect(ui->S_combo4Exam , static_cast<void (QComboBox::*)
            (int)>(&QComboBox::currentIndexChanged) , this ,[=](){
        ui->S_lineEdit4Exam->setText(ui->S_combo4Exam->itemText(ui->S_combo4Exam->currentIndex()));
    });
    connect(ui->S_combo4Student , static_cast<void (QComboBox::*)
            (int)>(&QComboBox::currentIndexChanged) , this ,[=](){
        ui->S_lineEdit4Student->setText(ui->S_combo4Student->
                                        itemText(ui->S_combo4Student->currentIndex()));
    });
    //************************************************************************************

    // 导入文件
    connect( ui->S_pushButton4Imp, &QPushButton::clicked , this , [=]()->void{
        ui->S_lineEdit4Imp->setText( QFileDialog::getOpenFileName(  this,
                                                                    "choose an Excel",
                                                                    "C:/",
                                                                    "Excel(*.xls *.xlsx)" ) );
    } );
}

void MainWindow::__CLOSE()
{
    Interactive::py_close();
}
