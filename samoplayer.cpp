#include "samoplayer.h"

samoplayer::samoplayer() {
    setCurrentTrack("");
}
    void samoplayer::setCurrentTrack(QString name)
    {
        this -> name = name;
        setMedia(QUrl::fromLocalFile(name));
    }

    void samoplayer::setNextTrack(QString name)
    {
        this -> name = name;
    }

    QString samoplayer::getCurrentTrack()
    {
        return name;
    }

    QString samoplayer::getNextTrack()
    {
        return next_name;
    }

    void samoplayer::toggleMute()
    {
        if(this -> isMuted() == 1) this -> setMuted(0);
        else this -> setMuted(1);
    }

    void samoplayer::playMusic()
    {
        this -> play();
    }

    void samoplayer::stopMusic()
    {
        this -> stop();
    }

    void samoplayer::pauseMusic()
    {
        this -> pause();
    }



