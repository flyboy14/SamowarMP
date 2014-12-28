#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
QDir configDir, plstDir;
#ifdef Q_OS_LINUX
bool os = true;
//configDir(QDir::homePath()+"/.config/samowar/conf"), plstDir(QDir::homePath()+"/.config/samowar/playlists");
#else
bool os = false;
//configDir = QApplication::applicationDirPath()+"/.config",
//plstDir = QApplication::applicationDirPath()+"/playlists";
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(os) {
        configDir = QDir(QDir::homePath()+"/.config/samowar/conf");
        plstDir = QDir(QDir::homePath()+"/.config/samowar/playlists");
    }
    else {
        configDir = QDir(a.applicationDirPath()+"/.config");
        plstDir = QDir(a.applicationDirPath()+"/playlists");
    }
    if(!configDir.exists())
    configDir.mkpath(configDir.path());
    if(!plstDir.exists())
    plstDir.mkpath(plstDir.path());
    MainWindow w;
    //QSharedMemory shared;
    //shared.setKey("129836-128366-99882");
    w.show();
    //return a.exec();
    int go = a.exec();
    if (go == 0) w.saveConfiguration();
    //else shared.~QSharedMemory();
}
