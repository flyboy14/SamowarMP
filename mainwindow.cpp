#include "mainwindow.h"
#include "stdlib.h"
//#include <sqlite3.h>
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
int def_width, def_height;
bool debug=false, repeat=false, randome=false, single=false, tmp_pause, playstate = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->size().width(), this->size().height());
    this->setGeometry(230,150,0,0);
    def_width = this->size().width();
    def_height = this->size().height();
    //iconSamowar = new QIcon("media-information.png");
    playlist = new QMediaPlaylist(player);
    player->setPlaylist(playlist);
    iconPlay = new QIcon("media-play.png");
    //ui->button_play->setIcon(*iconPlay);
    iconPause = new QIcon("media-pause.png");
    //ui->button_pause->setIcon(*iconPause);
    //iconStop = new QIcon("media-stop.png");
    //ui->button_stop->setIcon(*iconStop);
    //iconPlayPrev = new QIcon("media-previous.png");
    //ui->button_play_prev->setIcon(*iconPlayPrev);
    //iconPlayNext = new QIcon("media-next.png");
    //ui->button_play_next->setIcon(*iconPlayNext);
    //iconClearPls = new QIcon("desktop-brush-big.png");
    ui->listDebug->setVisible(false);
    ui->buttonDebugClear->setVisible(false);
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
    if(!playstate) {
        if(pls.count() == 0) {
            pls = QFileDialog::getOpenFileNames( this, tr("Open music file(s)"), dir, tr("Music files (*.ogg *.mp3 *.3ga *.wav *.flac)"));
            if(pls.count() != 0) {
                for(int i = 0; i < pls.count(); i++) {
                QFileInfo fi(pls[i]);
                if(debug) {
                    char *buffer = new char[100];
                    sprintf(buffer,"%d",i+1);
                    const QString& str("pls.count() is now ");
                    ui->listDebug->addItem(str+buffer);
                }
                ui->listWidget->addItem(fi.fileName());
                dir = fi.filePath();
                }
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
    msg.setText("100% sure to exit Samowar session?");
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

#include "mainwindow.h"
void MainWindow::on_action_add_files_triggered()
{
    pls.append(QFileDialog::getOpenFileNames(this, tr("Open music file(s)"), dir, tr("Music files (*.ogg *.mp3 *.3ga *.wav *.flac)")));
    ui->listWidget->clear();
    for(int i = 0; i < pls.count(); i++) {
        QFileInfo fi(pls[i]);
        ui->listWidget->addItem(fi.fileName());
        dir = fi.filePath();
        }
    ui->listWidget->setCurrentRow(nextTrack);

    if(pls.count() != 0) {
        if(player->getCurrentTrack() == "")
            player->setCurrentTrack(pls.first());
    }
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
    if(debug) {
        char *buffer = new char[100];
        char *buffer1 = new char[100];
        sprintf(buffer,"%d",nextTrack);
        sprintf(buffer1,"%d",pls.count());
        const QString& str("play_next_clicked. nextTrack is now ");
        const QString& str1(", pls.count() is now ");
        ui->listDebug->addItem(str+buffer+str1+buffer1);
    }
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
        if (nextTrack != pls.count()-1 && !single) {
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

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    player->stopMusic();
    nextTrack = ui->listWidget->currentRow();
    player->setCurrentTrack(pls.at(nextTrack));
    player->playMusic();
}

void MainWindow::on_deleteCurrentTrack_clicked()
{
    ui->button_play->setChecked(true);
    if(pls.count() != 0) {
        pls.removeAt(nextTrack);
        if(debug) {
            char *buffer = new char[100];
            sprintf(buffer,"%d",pls.count());
            const QString& str("delete_current_track_clicked. pls.count() is now ");
            ui->listDebug->addItem(str+buffer);
        }
        ui->listWidget->clear();
        for(int i = 0; i < pls.count(); i++) {
            QFileInfo fi(pls[i]);
            ui->listWidget->addItem(fi.fileName());
        }
        if(nextTrack != pls.count()) ui->listWidget->setCurrentRow(nextTrack);
        else ui->listWidget->setCurrentRow(0);
    }
    if(pls.count() == 0) {
        player->stopMusic();
        nextTrack = 0;
    }
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
    if(debug) {
        char *buffer = new char[100];
        sprintf(buffer,"%d",nextTrack);
        const QString& str("watchNextTrack called. nextTrack was ");
        ui->listDebug->addItem(str+buffer);
    }
    for(int row=0; row != pls.count();row++)
        if(ui->listWidget->item(row)->isSelected()) ui->listWidget->item(row)->setSelected(false);
        ui->listWidget->item(nextTrack)->setSelected(true);
}

void MainWindow::watchSelectedTrack() {
    if(debug) {
        char *buffer = new char[100];
        sprintf(buffer,"%d",nextTrack);
        const QString& str("watchSelectedTrack called. nextTrack was ");
        ui->listDebug->addItem(str+buffer);
    }
    nextTrack = ui -> listWidget->currentRow();
    if(debug) {
        char *buffer = new char[100];
        sprintf(buffer,"%d",nextTrack);
        const QString& str1("and now nextTrack is ");
        ui->listDebug->addItem(str1+buffer);
    }
}

void MainWindow::setSliderPosition(){
    ui -> horizontalSlider->setValue(player->position()*100/player->duration());
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    if(player->state() == 2) tmp_pause = true;
    ui->horizontalSlider->setCursor(Qt::ClosedHandCursor);
    player->pauseMusic();
    player->disconnect(ui->currentTrack_progressBar,SIGNAL(valueChanged(int)),this,SLOT(setSliderPosition()));
}


void MainWindow::on_horizontalSlider_sliderReleased()
{
    player->connect(ui->currentTrack_progressBar,SIGNAL(valueChanged(int)),this,SLOT(setSliderPosition()));
    if(!tmp_pause) player->playMusic();//0 - stopped 1 - playing 2 - paused
    ui->horizontalSlider->setCursor(Qt::OpenHandCursor);
}

void MainWindow::atTrackEnd() {
    if(debug) {
        char *buffer = new char[100];
        sprintf(buffer,"%d",pls.count());
        const QString& str("atTrackEnd called. pls.count() is now ");
        ui->listDebug->addItem(str+buffer);
    }
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
    if(debug) {
        char *buffer = new char[100];
        sprintf(buffer,"%d",nextTrack);
        const QString& str("watchStatus called. nextTrack is now ");
        ui->listDebug->addItem(str+buffer);
    }
    if(player->state() == 0) {
            ui->button_play->setIcon(*iconPlay);
            ui->currentTrack_progressBar->setValue(1);
        playstate = false;
        QMainWindow::setWindowTitle("Samowar Music Player v.1.6.19a" );
    }
    if(player->state() == 2) {
        QFileInfo fi(player->getCurrentTrack());
        QMainWindow::setWindowTitle("[paused] Samowar - " + fi.fileName());
        if(playstate == true) {
            //ui->button_play->setFlat(0);
            //ui->button_stop->setFlat(1);
            ui->button_play->setIcon(*iconPlay);
            ui->button_play->setToolTip("Play music (F9)");
        } // to prevent memory leaks
        playstate = false;
    }
    if(player->state() == 1) {
        QFileInfo fi(player->getCurrentTrack());
        QMainWindow::setWindowTitle("Samowar - Playing... " + fi.fileName() );
        if(playstate == false) {
            //ui->button_play->setFlat(0);
            //ui->button_stop->setFlat(1);
            ui->button_play->setIcon(*iconPause);
            ui->button_play->setToolTip("Pause music (F9)");
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

void MainWindow::on_actionClear_playlist_triggered()
{
    player->stopMusic();
    pls.clear();
    ui->listWidget->clear();
    nextTrack = 0;
}

void MainWindow::on_button_play_released()
{
    ui->button_play->setStyleSheet("QPushButton::hover { border-image:url(:/media-information.png);}");
}

void MainWindow::on_actionToggle_debug_output_triggered()
{
    if(debug) {
        ui->listDebug->setVisible(false);
        ui->buttonDebugClear->setVisible(false);
        debug = false;
        window()->setFixedSize(def_width,def_height);
    }
    else {
        ui->listDebug->setVisible(true);
        ui->buttonDebugClear->setVisible(true);
        debug = true;
        window()->setFixedSize(this->size().width(), this->size().height()+190);
    }
}

void MainWindow::on_buttonDebugClear_clicked()
{
    ui->listDebug->clear();
}

void MainWindow::on_dialVolume_valueChanged(int value)
{
    player -> setVolume(value);
    char *buffer = new char[3];
    sprintf(buffer,"%d",value);
    ui->labelVolume -> setText(buffer);
}

void MainWindow::on_dialVolume_sliderPressed()
{
    ui->dialVolume->setCursor(Qt::ClosedHandCursor);
}

void MainWindow::on_dialVolume_sliderReleased()
{
    ui->dialVolume->setCursor(Qt::OpenHandCursor);
}

void MainWindow::on_actionAdd_directory_s_triggered()
{
    QString directory = QFileDialog::getExistingDirectory(this,tr("Select dir to import files"));
    dir = directory;
    if(directory.isEmpty())
        return;
    QDir dirs(directory);
    QStringList files = (dirs.entryList(QStringList() << "*.mp3" << "*.flac" << "*.wav" << "*.ogg" << "*.3ga",QDir::Files));
    for(int i = 0; i < files.count(); i++)
        pls.append(directory+"/"+files[i]);
    ui->listWidget->clear();
    for(int i = 0; i < pls.count(); i++)
    {
        QFileInfo fi(pls[i]);
        ui->listWidget->addItem(fi.fileName());
    }
    ui->listWidget->setCurrentRow(nextTrack);
}

void MainWindow::on_actionSave_playlist_triggered()
{
//    sqlite3 *currentPlaylist;
//    sqlite3_stmt *ppStmt;
//    const char* tail;
//    int rc=sqlite3_open("/home/master-p/test.db", &currentPlaylist);
//    sqlite3_prepare(currentPlaylist,"aaaarrrrrrrrrrrrrrrrrgggggggggggggh",10,&ppStmt, &tail);
//    sqlite3_column_table_name()
}
