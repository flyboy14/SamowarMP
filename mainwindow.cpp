#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QMediaPlaylist>
#include "samoplayer.h"
samoplayer *player= new samoplayer;
QString dir = "/home/master-p/Music";
QStringList pls;
int currentVolume = 50;
int nextTrack = 0;
QIcon *iconPlay, *iconPause, *iconStop, *iconPlayPrev, *iconPlayNext, *iconClearPls;
bool repeat, randome, single, tmp_pause, playstate = false;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    playlist = new QMediaPlaylist(player);
    player->setPlaylist(playlist);
    iconPlay = new QIcon("media-play.png");
    ui->button_play->setIcon(*iconPlay);
    iconPause = new QIcon("media-pause.png");
    //ui->button_pause->setIcon(*iconPause);
    iconStop = new QIcon("media-stop.png");
    ui->button_stop->setIcon(*iconStop);
    iconPlayPrev = new QIcon("media-previous.png");
    ui->button_play_prev->setIcon(*iconPlayPrev);
    iconPlayNext = new QIcon("media-next.png");
    ui->button_play_next->setIcon(*iconPlayNext);
    iconClearPls = new QIcon("desktop-brush-big.png");
    ui->button_clearPlaylist->setIcon(*iconClearPls);
        QObject::connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(watchPlaying()));
        QObject::connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(watchStatus()));
        QObject::connect(player,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(watchNextTrack()));
        QObject::connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(watchSelectedTrack()));
        QObject::connect(ui->currentTrack_progressBar,SIGNAL(valueChanged(int)),this,SLOT(setSliderPosition()));
        QObject::connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(atTrackEnd()));
                //connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(onPositionChanged(qint64)));
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_play_clicked()
{
    QList<QMediaContent> content;
    if(!playstate) {
   if(pls.count() == 0) {
        pls = QFileDialog::getOpenFileNames( this, tr("Open music file(s)"), dir, tr("Music files (*.ogg *.mp3 *.3ga *.wav *.flac)"));
        if(pls.count() != 0) {
            for(int i = 0; i < pls.count(); i++) {
            content.push_back(QUrl::fromLocalFile(pls[i]));
            QFileInfo fi(pls[i]);
            ui->listWidget->addItem(fi.fileName());
            }
            playlist->addMedia(content);
            ui->listWidget->setCurrentRow(playlist->currentIndex() != -1? playlist->currentIndex():0);
            player -> setCurrentTrack(pls.at(nextTrack));
            player -> playMusic();
        }
    }
            else {
                if(player->state() != 2)player -> setCurrentTrack(pls.at(nextTrack)); //0 - stopped 1 - playing 2 - paused
                player->playMusic();
            }
    }
    else {
        if(player -> getCurrentTrack() != "") {
            player->pauseMusic();
            }
    }
}

void MainWindow::on_button_stop_clicked()
{
    if(player -> getCurrentTrack() != "") {
    player->stopMusic();
    }
}

void MainWindow::on_action_200_triggered()
{
    QMessageBox msg;
    msg.setText("This function is not supported yet");
    msg.setIcon(QMessageBox::Critical);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

void MainWindow::on_actionExit_triggered()
{
    QMessageBox msg;
    msg.setText("100% sure to exit Samowar?");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.QMessageBox::setButtonText(QMessageBox::Ok, "Yes");
    msg.QMessageBox::setButtonText(QMessageBox::Cancel, "Nope");
    msg.setDefaultButton(QMessageBox::Ok);
    int ret = msg.exec();
        switch (ret) {
               case QMessageBox::Cancel: {
                    break;
                }
               case QMessageBox::Ok: {
MainWindow::QMainWindow::close();
                   break;
               }
               default:
                   // Сюда пишем обработку события по умолчанию
                   break;
             }
}


void MainWindow::on_action_add_files_triggered()
{
    pls.append(QFileDialog::getOpenFileNames(this, tr("Open music file(s)"), dir, tr("Music files (*.ogg *.mp3 *.3ga *.wav *.flac)")));
    QList<QMediaContent> content;
    //for(const QString& f:files)
        ui->listWidget->clear();
        for(int i = 0; i < pls.count(); i++) {
        content.push_front(QUrl::fromLocalFile(pls[i]));
        QFileInfo fi(pls[i]);
        ui->listWidget->addItem(fi.fileName());
        }
        playlist->addMedia(content);
        ui->listWidget->setCurrentRow(playlist->currentIndex() != -1? playlist->currentIndex():nextTrack);

    if(pls.count() != 0) {
    if(player->getCurrentTrack() == "")
        player->setCurrentTrack(pls.first());
    }
}


void MainWindow::on_volumeSlider_valueChanged(int value)
{
    player -> setVolume(value);
}

void MainWindow::on_radio_mute_toggled(bool checked)
{
     player -> toggleMute();
}

void MainWindow::on_button_play_prev_clicked()
{
    if(pls.count() != 0) {
    if(single) {
        player -> setCurrentTrack(pls.at(nextTrack));
        player->playMusic();
    }
    if(randome) {
        nextTrack = rand();
        nextTrack %= pls.count();
        player -> setCurrentTrack(pls.at(nextTrack));
        player->playMusic();
    }
    if (nextTrack != 0 && !single) {
        nextTrack--;
        player -> setCurrentTrack(pls.at(nextTrack));
        player->playMusic();
    }
    else {
        if(repeat) {
            nextTrack = pls.count()-1;
            player -> setCurrentTrack(pls.at(nextTrack));
            player->playMusic();
        }
    }
    }
}

void MainWindow::on_button_play_next_clicked()
{
    if(pls.count() != 0) {
        if(single) {
            player -> setCurrentTrack(pls.at(nextTrack));
            player->playMusic();
        }
        if(randome) {
            nextTrack = rand();
            nextTrack %= pls.count();
            player -> setCurrentTrack(pls.at(nextTrack));
            player->playMusic();
        }
        if (pls.at(nextTrack) != pls.last() && !single) {
            nextTrack++;
            player -> setCurrentTrack(pls.at(nextTrack));
            player->playMusic();
        }
        else {
            if(repeat) {
                nextTrack = 0;
                player->setCurrentTrack(pls.at(nextTrack));
                player->playMusic();
            }
        }
    }
}

void MainWindow::on_button_clearPlaylist_clicked()
{
    pls.clear();
    ui->listWidget->clear();
    nextTrack = 0;
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    player->stopMusic();
    nextTrack = ui->listWidget->currentRow();
    player->setCurrentTrack(pls.at(nextTrack));
    player->playMusic();
}

void MainWindow::on_deleteCurrentTrack_clicked()
{
    QList<QMediaContent> content;
    pls.removeAt(ui->listWidget->currentRow());
    ui->listWidget->clear();
    for(int i = 0; i < pls.count(); i++) {
    content.push_front(QUrl::fromLocalFile(pls[i]));
    QFileInfo fi(pls[i]);
    ui->listWidget->addItem(fi.fileName());
    }
    ui->listWidget->setCurrentRow(nextTrack);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    player->setPosition(player->duration()/100*position);
}

void MainWindow::watchPlaying() {
    if(player->isMuted())QMainWindow::setWindowTitle("[muted] Samowar - Playing... " + player->getCurrentTrack());
    else watchStatus();
        int pos_secs = (player->position()-player->position()/60000*60000)/1000;
        int dur_secs = (player->duration()-player->duration()/60000*60000)/1000;
        if(pos_secs < 10) ui -> label_test->setText(QString::number(player->position()/60000) +":0"+ QString::number(pos_secs) + " / " + QString::number(player->duration()/60000) +":"+ QString::number(dur_secs));
        if(dur_secs < 10) ui -> label_test->setText(QString::number(player->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(player->duration()/60000) +":0"+ QString::number(dur_secs));
        if(pos_secs < 10 && dur_secs < 10) ui -> label_test->setText(QString::number(player->position()/60000) +":0"+ QString::number(pos_secs) + " / " + QString::number(player->duration()/60000) +":0"+ QString::number(dur_secs));
        if(dur_secs > 10 && pos_secs > 10) ui -> label_test->setText(QString::number(player->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(player->duration()/60000) +":"+ QString::number(dur_secs));
        ui -> currentTrack_progressBar->setValue(player->position()*100/player->duration());
}

void MainWindow::watchNextTrack() {
    for(int row=0; row != pls.count();row++)
        if(ui->listWidget->item(row)->isSelected()) ui->listWidget->item(row)->setSelected(false);
    ui->listWidget->item(nextTrack)->setSelected(true);
}

void MainWindow::watchSelectedTrack() {
    nextTrack = ui -> listWidget->currentRow();
}

void MainWindow::setSliderPosition(){
    ui -> horizontalSlider->setValue(player->position()*100/player->duration());
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    if(player->state() == 2) tmp_pause = true;
    player->pauseMusic();
    player->disconnect(ui->currentTrack_progressBar,SIGNAL(valueChanged(int)),this,SLOT(setSliderPosition()));
}


void MainWindow::on_horizontalSlider_sliderReleased()
{
    player->connect(ui->currentTrack_progressBar,SIGNAL(valueChanged(int)),this,SLOT(setSliderPosition()));
    if(!tmp_pause)player->playMusic();//0 - stopped 1 - playing 2 - paused
}

void MainWindow::atTrackEnd(){
    if(player->mediaStatus() == 7) { //last song ended
        if(pls.count() != 0) {
        if(single) {
            player->setCurrentTrack(pls.at(nextTrack));
            player->playMusic();
        }
        else {
            if(randome) {
                nextTrack = rand();
                nextTrack %= pls.count();
                player -> setCurrentTrack(pls.at(nextTrack));
                player->playMusic();
            }
            if(pls.at(nextTrack) != pls.last()) {
                nextTrack++;
                player->setCurrentTrack(pls.at(nextTrack));
                player->playMusic();
                }
            else {
                if(repeat) {
                    nextTrack = 0;
                    player->setCurrentTrack(pls.at(nextTrack));
                    player->playMusic();
                }
            }
        }
        }
    }
}

void MainWindow::watchStatus() {
    if(player->state() == 0) {
        if(ui->button_stop->isFlat()) {
            ui->button_play->setFlat(1);
            ui->button_stop->setFlat(0);
            ui->button_play->setIcon(*iconPlay);
            ui->currentTrack_progressBar->setValue(1);
        }
        playstate = false;
        QMainWindow::setWindowTitle("Samowar Music Player v.1.3.19a" );
    }
    if(player->state() == 2) {
        QMainWindow::setWindowTitle("[paused] Samowar - " + player->getCurrentTrack());
        //ui->button_pause->setFlat(0);
        if(playstate == true) {
            ui->button_play->setFlat(0);
            ui->button_stop->setFlat(1);
            ui->button_play->setIcon(*iconPlay);
        } // to prevent memory leaks
        playstate = false;
    }
    if(player->state() == 1) {
        QMainWindow::setWindowTitle("Samowar - Playing... " + player->getCurrentTrack());
        if(playstate == false) {
            ui->button_play->setFlat(0);
            ui->button_stop->setFlat(1);
            ui->button_play->setIcon(*iconPause);
        } // to prevent memory leaks
        playstate = true;
    }
}

void MainWindow::on_checkBox_repeat_toggled(bool checked)
{
    if(checked) {
        repeat = true;
        ui->checkBox_single->setChecked(0);
    }
    else repeat = false;
}

void MainWindow::on_checkBox_random_toggled(bool checked)
{
    if(checked) {
        randome = true;
        ui->checkBox_repeat->setChecked(1);
        ui->checkBox_single->setChecked(0);
    }
    else {
        randome = false;
        ui->checkBox_repeat->setChecked(0);
    }
}


void MainWindow::on_checkBox_single_toggled(bool checked)
{
    if(checked) {
        single = true;
        ui->checkBox_repeat->setChecked(0);
        ui->checkBox_random->setChecked(0);
    }
    else {
        single = false;
    }
}
