#ifndef SAMOPLAYER_H
#define SAMOPLAYER_H
#include <QCoreApplication>
#include <QAudio>
#include <QMediaPlayer>
#include <QMainWindow>
#include "mainwindow.h"

class samoplayer:public QMediaPlayer
{
    QString name;
public:
    samoplayer();
    QString getCurrentTrack();
    void setCurrentTrack(QString name);
    void playMusic();
    void stopMusic();
    void pauseMusic();
    void toggleMute();
    void positionChanged(qint64 position);
};

#endif // SAMOPLAYER_H
