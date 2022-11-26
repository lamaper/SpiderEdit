#include "mainwindow.h"
#include "scommunicate.h"

#include <QApplication>
#include <QStringList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    // TODO
    w.__INIT("../release/database/class/0.db","L2");

    w.show();

    return a.exec();
}
