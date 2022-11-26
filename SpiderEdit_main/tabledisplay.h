#ifndef TABLEDISPLAY_H
#define TABLEDISPLAY_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDebug>
#include <QStringList>

namespace Ui {
class TableDisplay;
}

class TableDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit TableDisplay(QWidget *parent = nullptr);
    ~TableDisplay();
    void shown(QStringList stuName,QStringList examName,QStringList knoid,QStringList knoName,QStringList knoCnt);

private:
    Ui::TableDisplay *ui;
    QStandardItemModel *Dstudents,*Dexams,*Dwknss;

};

#endif // TABLEDISPLAY_H
