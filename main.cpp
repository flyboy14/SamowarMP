#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //QSharedMemory shared;
    //shared.setKey("129836-128366-99882");
    QDir confDir(QDir::homePath()+"/.config/samowar/conf"), plsDir(QDir::homePath()+"/.config/samowar/playlists");
    if(!confDir.exists())
    confDir.mkpath(confDir.path());
    if(!confDir.exists())
    plsDir.mkpath(plsDir.path());
    w.show();
    //return a.exec();
    if (a.exec() == 0) w.saveConfiguration();
    //else shared.~QSharedMemory();
}
