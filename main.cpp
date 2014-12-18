#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QSharedMemory shared;
    shared.setKey("129836-128366-99882");
    //if(!shared.create(512,QSharedMemory::ReadWrite) == true) exit(0);
    w.show();
    //return a.exec();
    if (a.exec() == 0) w.saveConfiguration();
    //else shared.~QSharedMemory();
}
