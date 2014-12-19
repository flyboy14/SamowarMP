#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
#ifdef Q_OS_LINUX
QDir configDir(QDir::homePath()+"/.config/samowar/conf"), plstDir(QDir::homePath()+"/.config/samowar/playlists");
#else
QDir configDir = QApplication::applicationDirPath()+"/.config",
plstDir = QApplication::applicationDirPath()+"/playlists";
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //QSharedMemory shared;
    //shared.setKey("129836-128366-99882");

    if(!configDir.exists())
    configDir.mkpath(configDir.path());
    if(!plstDir.exists())
    plstDir.mkpath(plstDir.path());
    w.show();
    //return a.exec();
    if (a.exec() == 0) w.saveConfiguration();
    //else shared.~QSharedMemory();
}
