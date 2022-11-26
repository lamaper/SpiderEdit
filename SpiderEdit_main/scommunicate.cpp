#include "scommunicate.h"

const QStringList &SCommunicate::getStringlist() const
{
    return stringlist;
}

void SCommunicate::setStringlist(const QStringList &newStringlist)
{
    stringlist = newStringlist;
}

QStandardItemModel *SCommunicate::getModel() const
{
    return model;
}

void SCommunicate::setModel(QStandardItemModel *newModel)
{
    model = newModel;
}

SCommunicate::SCommunicate(QObject *parent) : QObject(parent)
{

}

void SCommunicate::S_ToDisplayTheSearch(){
    emit ForSearch_S(model);
}
