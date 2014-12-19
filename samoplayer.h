#ifndef SAMOPLAYER_H
#define SAMOPLAYER_H
#include <QCoreApplication>
#include <QAudio>
#include <QMediaPlayer>
#include <QMainWindow>
#include <QFileInfo>
#include "mainwindow.h"

class samoplayer:public QMediaPlayer
{
    QString name, next_name;
public:
    samoplayer();
    QString getCurrentTrack(QStringList filez, QMediaPlaylist *pls);
    QString getNextTrack();
    void setCurrentTrack(QString name);
    void setNextTrack(QString next_name);
    void playMusic();
    void stopMusic();
    void pauseMusic();
    void toggleMute();
};

#endif // SAMOPLAYER_H
