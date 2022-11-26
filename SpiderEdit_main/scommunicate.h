#ifndef SCOMMUNICATE_H
#define SCOMMUNICATE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QStandardItemModel>
/*
This is for communicate with db
*/
class SCommunicate : public QObject
{
    Q_OBJECT
private:
    QStringList stringlist;
    QStandardItemModel *model;

public:
    explicit SCommunicate(QObject *parent = nullptr);

    const QStringList &getStringlist() const;
    void setStringlist(const QStringList &newStringlist);

    QStandardItemModel *getModel() const;
    void setModel(QStandardItemModel *newModel);

    void S_ToDisplayTheSearch();//提前传入好数据，db用此方法来告诉程序内容
signals:
    void ForSearch_S(QStandardItemModel *Model);
};

#endif // SCOMMUNICATE_H
