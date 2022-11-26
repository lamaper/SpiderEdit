#ifndef BASEWORK_H
#define BASEWORK_H

#include <QWidget>
#include <QStandardItemModel>
#include "scommunicate.h"

struct __Id_Name {
    QString id, name;
};

struct __Action {
    QString action, id, name;
};

namespace Ui {
class BaseWork;
}

class BaseWork : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWork(QWidget *parent = nullptr);
    ~BaseWork();

    void __initTheBaseWork(QList<__Id_Name> NameIdList, int NameNumber);
    void BaseWorkNameInit(QString name);

    QStringList QSIMtoStringList(QStandardItemModel *model, int colmn);

    void showWarning(QString warning);
    void clear();

public:
    bool ischange = false;

private:
    Ui::BaseWork *ui;
    QStringList nameList, idList;
//    QList<__Id_Name> nameidList;
    QStandardItemModel *nameModel;
    QString baseworkName;
    int nameNumber;

//    QList<__Action> oprtAction; // 记录操作


signals:
//    void B_AddNew(QString name);
//    void B_Remove(QString name);
//    void B_ChangeName(int id,QString newName);
//    void B_SearchFor(QString name);

    void B_ReadExcel(QString file);
    void B_Change(QStringList id, QStringList name);
};

#endif // BASEWORK_H
