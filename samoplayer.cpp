#include "samoplayer.h"


samoplayer::samoplayer(Ui::MainWindow *ui) {
    setVolume(50);
    setCurrentTrack("");
}
    void samoplayer::setCurrentTrack(QString name)
    {
        this -> name = name;
        setMedia(QUrl::fromLocalFile(name));
    }

    QString samoplayer::getCurrentTrack()
    {
    return name;
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

     void samoplayer::positionChanged(qint64 position) {
        //int pos_secs = (this->position()-this->position()/1000)/1000-60*(this->position()/60000);
        //int dur_secs = (this->duration()-this->duration()/1000)/1000-60*(this->duration()/60000);
        //ui -> label_test->setText(QString::number(this->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(this->duration()/60000) +":"+ QString::number(dur_secs));
        //ui -> currentTrack_progressBar->setValue(this->position()*100/this->duration());
         //ui->currentTrack_progressBar->setValue(position*100);
    }


