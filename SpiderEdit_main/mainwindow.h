#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QStringList>
#include <QCompleter>

#include "basework.h"
#include "scommunicate.h"
#include "tabledisplay.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    // 指向编辑考试和知识点两个子窗口的指针
    BaseWork *examWork = new BaseWork();
    BaseWork *knoWork = new BaseWork();
    BaseWork *stuWork = new BaseWork();

    QStandardItemModel* SubmitModel;
    QStandardItemModel* StudentModel;
    QStandardItemModel* ExaModel;
    // 学生、知识点、考试的数目及其具体内容
    int _stuNum, _knoNum, _exaNum;
    QList<__Id_Name> studentIdName,knowledgeIdName,examIdName;
//    QStringList studentId, knowledgeId, examId;
//    QStringList studentName, knowledgeName, examName;
    // 班名、数据库位置
    QString EDUclassName, __dbposition;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // 总初始化
    void __INIT( QString dbpos, QString clssnm );

    // 初始化 学生、知识点、考试、班级
    void __init_stun(QList<__Id_Name> Student);
    void __init_knon(QList<__Id_Name> Knoledge);
    void __init_exan(QList<__Id_Name> Exam);
    void __init_data(QList<__Id_Name> Student,QList<__Id_Name> Knoledge,QList<__Id_Name> Exam);

    // 初始化 学生、知识点、考试 个数
    void __init_stuNum( int stuNum ) {
        _stuNum = stuNum;
    }
    void __init_knoNum( int knoNum ) {
        _knoNum = knoNum;
    }
    void __init_exaNum( int exaNum ) {
        _exaNum = exaNum;
    }

    // 初始化班名、数据库位置
    void __init_clssn(QString ClassName) {
        EDUclassName = ClassName;
    }
    void __init_dbpos(QString dbpos) {
        __dbposition = dbpos;
    }

    // 退出前操作
    void __CLOSE();

    // 初始化 UI 框架
    void __init_UIStructureOfMainWindow();

public:
    QStringList analIdName( QList<__Id_Name> idnm, QString opt ) { // 便于将 __Id_Name 拆开处理
        QStringList res;
        for( int i = 0; i < idnm.size(); ++i ) {
            if( opt == "id" )
                res << idnm[i].id;
            else res << idnm[i].name;
        }
        return res;
    }

public:
    // 实现插入
    QString submitInsert(QString exa, QString stu, QString wksid);
    // 显示查询结果
    void showInfoSolt(QStandardItemModel *model);

signals:
//    void I_MainWindowLogOut();//User log out
//    void I_MainWindowExportInfo(QString DBname);//User wants to export data from database
    void I_SubmitLinkToDB(QString exam, QString student, QStandardItemModel* submit);
    void I_QuestLinkToDB(QStandardItemModel *name,QStandardItemModel* kno);
    void I_LogOut();
    void I_Export(QString dbfile);
    void I_Save(QString EDUClassName);

    void I_AddExam(QString examname, QString date);

    void I_SubmitImportFile(QString excelfile);
    void I_Display(QStringList student,QStringList exam,QStringList knoid,QStringList knoname,QStringList knocnt);
};

#endif // MAINWINDOW_H
