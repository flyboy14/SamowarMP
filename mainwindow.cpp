#include "mainwindow.h"
#include "stdlib.h"
//#include <sqlite3.h>
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include "samoplayer.h"
samoplayer *plr= new samoplayer;
QString dir = "/home/master-p/Music";
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
    QCoreApplication::setApplicationName("Samowar Music Player");
    QCoreApplication::setApplicationVersion("1.8.71a");
    iconPlay = new QIcon(QApplication::applicationDirPath()+"/media-play.png");
    ui->button_play->setIcon(*iconPlay);
    iconPause = new QIcon(QApplication::applicationDirPath()+"/media-pause.png");
    iconStop = new QIcon(QApplication::applicationDirPath()+"/media-stop.png");
    ui->button_stop->setIcon(*iconStop);
    iconPlayPrev = new QIcon(QApplication::applicationDirPath()+"/media-previous.png");
    ui->button_play_prev->setIcon(*iconPlayPrev);
    iconPlayNext = new QIcon(QApplication::applicationDirPath()+"/media-next.png");
    ui->button_play_next->setIcon(*iconPlayNext);
    iconClearPls = new QIcon(QApplication::applicationDirPath()+"/brush-big.png");
    ui->deleteCurrentTrack->setIcon(*iconClearPls);
    ui->listDebug->setVisible(false);
    ui->buttonDebugClear->setVisible(false);
    playlist = new QMediaPlaylist(plr);
    plr->setPlaylist(playlist);
    QObject::connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(watchPlaying()));
    QObject::connect(plr,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(watchNextTrack()));
    QObject::connect(plr,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(watchStatus()));
    QObject::connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(watchSelectedTrack()));
    QObject::connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),ui->currentTrack_progressBar,SLOT(setValue(int)));
    QObject::connect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(atTrackEnd()));
    QObject::connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(atTrackEnd()));
    QStringList cmdline_args = QApplication::arguments();
    if(cmdline_args.count() > 1) {
        cmdline_args.removeAt(0);
        files = cmdline_args;
        for(int i = 0; i < files.count(); i++) {
            content.push_back(QUrl::fromLocalFile(files[i]));
            QFileInfo fi(files[i]);
            ui->listWidget->addItem(fi.fileName());
        }
        playlist->addMedia(content);
        ui->listWidget->setCurrentRow(playlist->currentIndex() != -1? playlist->currentIndex():0);
        plr->playMusic();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_play_clicked()
{
    if(!playstate) {
        if(playlist->mediaCount() == 0) {
            files = QFileDialog::getOpenFileNames( this, tr("Open music file(s)"), dir, tr("Music files (*.ogg *.mp3 *.3ga *.wav *.flac)"));
            for(int i = 0; i < files.count(); i++) {
                content.push_back(QUrl::fromLocalFile(files[i]));
                QFileInfo fi(files[i]);
                ui->listWidget->addItem(fi.fileName());
            }
            playlist->addMedia(content);
            ui->listWidget->setCurrentRow(playlist->currentIndex() != -1? playlist->currentIndex():0);
            plr->playMusic();
        }
                else {
                    if(plr->state() != 2)playlist->setCurrentIndex(nextTrack); //0 - stopped 1 - playing 2 - paused
                    plr->playMusic();
                }
    }
    else {
            plr->pauseMusic();
    }
}

void MainWindow::on_button_stop_clicked()
{
        plr->stopMusic();
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

void MainWindow::on_action_add_files_triggered()
{
    QList<QMediaContent> new_content;
    int tmp = files.count();
    files.append(QFileDialog::getOpenFileNames( this, tr("Open music file(s)"), dir, tr("Music files (*.ogg *.mp3 *.3ga *.wav *.flac)")));
    for(int i = tmp; i < files.count(); i++) {
        content.push_back(QUrl::fromLocalFile(files[i]));
        new_content.push_back(QUrl::fromLocalFile(files[i]));
        QFileInfo fi(files[i]);
        ui->listWidget->addItem(fi.fileName());
    }
    playlist->addMedia(new_content);
}

void MainWindow::on_radio_mute_toggled(bool checked)
{
     plr -> toggleMute();
}

void MainWindow::on_button_play_prev_clicked()
{
    if(playlist->playbackMode() == QMediaPlaylist::Sequential)
    {if(nextTrack != 0) playlist->previous();}
    else playlist->previous();
}

void MainWindow::on_button_play_next_clicked()
{
    if(debug) {
        char *buffer = new char[100];
        char *buffer1 = new char[100];
        sprintf(buffer,"%d",nextTrack);
        sprintf(buffer1,"%d",playlist->mediaCount());
        const QString& str("play_next_clicked. nextTrack is now ");
        const QString& str1(", pls.count() is now ");
        ui->listDebug->addItem(str+buffer+str1+buffer1);
    }
    if(playlist->mediaCount() != 0) {
        if(playlist->playbackMode() == QMediaPlaylist::Sequential)
        {if(nextTrack != playlist->mediaCount()-1) playlist->next();}
        else playlist->next();
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    plr->stopMusic();
    nextTrack = ui->listWidget->currentRow();
    playlist->setCurrentIndex(nextTrack);
    plr->playMusic();
}

void MainWindow::on_deleteCurrentTrack_clicked()
{
    if(playlist->mediaCount() != 0) {
        content.removeAt(nextTrack);
        playlist->removeMedia(nextTrack);
        files.removeAt(nextTrack);
        ui->listWidget->clear();
        for(int i = 0; i < playlist->mediaCount(); i++) {
            QFileInfo fi(files[i]);
            ui->listWidget->addItem(fi.fileName());
        }
        if(nextTrack != playlist->mediaCount()) ui->listWidget->setCurrentRow(nextTrack);
        else {
            nextTrack--;
            ui->listWidget->setCurrentRow(nextTrack);
        }
    }
    else {
        plr->stopMusic();
        nextTrack = 0;
    }
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    plr->setPosition(plr->duration()/100*position);
}

void MainWindow::watchPlaying() {
    QFileInfo fi(files[nextTrack]);
    if(plr->isMuted())QMainWindow::setWindowTitle("[muted] Samowar - Playing... " + fi.fileName());
    else watchStatus();
    int pos_secs = (plr->position()-plr->position()/60000*60000)/1000;
    int dur_secs = (plr->duration()-plr->duration()/60000*60000)/1000;
    if(pos_secs < 10) ui -> label_test->setText(QString::number(plr->position()/60000) +":0"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":"+ QString::number(dur_secs));
    if(dur_secs < 10) ui -> label_test->setText(QString::number(plr->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":0"+ QString::number(dur_secs));
    if(pos_secs < 10 && dur_secs < 10) ui -> label_test->setText(QString::number(plr->position()/60000) +":0"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":0"+ QString::number(dur_secs));
    if(dur_secs > 10 && pos_secs > 10) ui -> label_test->setText(QString::number(plr->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":"+ QString::number(dur_secs));
    ui -> currentTrack_progressBar->setValue(plr->position()*100/plr->duration());
    ui -> horizontalSlider->setValue(plr->position()*100/plr->duration());
}

void MainWindow::watchNextTrack() {
//    if(debug) {
//        char *buffer = new char[100];
//        sprintf(buffer,"%d",nextTrack);
//        const QString& str("watchNextTrack called. nextTrack was ");
//        ui->listDebug->addItem(str+buffer);
//    }
    for(int row=0; row != playlist->mediaCount(); row++)
        if(ui->listWidget->item(row)->isSelected()) ui->listWidget->item(row)->setSelected(false);
        ui->listWidget->item(nextTrack)->setSelected(true);
//        if(debug) {
//            char *buffer = new char[100];
//            sprintf(buffer,"%d",nextTrack);
//            const QString& str("And now nexttrack is ");
//            ui->listDebug->addItem(str+buffer);
//        }
}

void MainWindow::watchSelectedTrack() {
//    if(debug) {
//        char *buffer = new char[100];
//        sprintf(buffer,"%d",nextTrack);
//        const QString& str("watchSelectedTrack called. nextTrack was ");
//        ui->listDebug->addItem(str+buffer);
//    }
    nextTrack = ui -> listWidget->currentRow();
//    if(debug) {
//        char *buffer = new char[100];
//        sprintf(buffer,"%d",nextTrack);
//        const QString& str1("and now nextTrack is ");
//        ui->listDebug->addItem(str1+buffer);
//    }
}

void MainWindow::setSliderPosition(){
    ui -> horizontalSlider->setValue(plr->position()*100/plr->duration());
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    if(plr->state() == 2) tmp_pause = true;
    ui->horizontalSlider->setCursor(Qt::ClosedHandCursor);
    plr->pauseMusic();
    plr->disconnect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(watchPlaying()));
}


void MainWindow::on_horizontalSlider_sliderReleased()
{
    if(!tmp_pause) plr->playMusic();//0 - stopped 1 - playing 2 - paused
    ui->horizontalSlider->setCursor(Qt::OpenHandCursor);
        plr->connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(watchPlaying()));
}

void MainWindow::atTrackEnd() {
    //ui->currentTrack_progressBar->setValue(0);
    if(playlist->playbackMode() == QMediaPlaylist::Sequential && nextTrack == playlist->mediaCount()-1){}
    else nextTrack = playlist->currentIndex();
//    if(debug) {
//        char *buffer = new char[100];
//        sprintf(buffer,"%d",nextTrack);
//        const QString& str("atTrackEnd called. nexttrack is now ");
//        ui->listDebug->addItem(str+buffer);
//    }
}

void MainWindow::watchStatus() {
//    if(debug) {
//        char *buffer = new char[100];
//        sprintf(buffer,"%d",nextTrack);
//        const QString& str("watchStatus called. nextTrack is now ");
//        ui->listDebug->addItem(str+buffer);
//    }
    if(plr->state() == 0) {
            ui->button_play->setIcon(*iconPlay);
            ui->currentTrack_progressBar->setValue(1);
            ui->horizontalSlider->setValue(0);
        playstate = false;
        QMainWindow::setWindowTitle("Samowar Music Player v.1.8.71a" );
    }
    if(plr->state() == 2) {
        QFileInfo fi(files[nextTrack]);
        QMainWindow::setWindowTitle("[paused] Samowar - " + fi.fileName());
        if(playstate == true) {
            ui->button_play->setIcon(*iconPlay);
            ui->button_play->setToolTip("Play music (F9)");
        } // to prevent memory leaks
        playstate = false;
    }
    if(plr->state() == 1) {
        QFileInfo fi(files[nextTrack]);
        QMainWindow::setWindowTitle("Samowar - Playing... " + fi.fileName() );
        if(playstate == false) {
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
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        ui->checkBox_single->setChecked(0);
        ui->checkBox_random->setChecked(0);
    }
    else repeat = false;
    if(!repeat && !randome && !single) playlist->setPlaybackMode(QMediaPlaylist::Sequential);
}

void MainWindow::on_checkBox_random_toggled(bool checked)
{
    if(checked) {
        randome = true;
        playlist->setPlaybackMode(QMediaPlaylist::Random);
        ui->checkBox_repeat->setChecked(0);
        ui->checkBox_single->setChecked(0);
    }
    else {
        randome = false;
        if(!repeat && !randome && !single) playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    }
}


void MainWindow::on_checkBox_single_toggled(bool checked)
{
    if(checked) {
        single = true;
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->checkBox_repeat->setChecked(0);
        ui->checkBox_random->setChecked(0);
    }
    else {
        single = false;
        if(!repeat && !randome && !single) playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    }
}

void MainWindow::on_actionClear_playlist_triggered()
{
    plr->stopMusic();
    nextTrack = 0;
    content.clear();
    playlist->clear();
    files.clear();
    ui->listWidget->clear();
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
    plr -> setVolume(value);
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
    QList<QMediaContent> new_content;
    int tmp = files.count();
    files.append(dirs.entryList(QStringList() << "*.mp3" << "*.flac" << "*.wav" << "*.ogg" << "*.3ga"));
    for(int i = tmp; i < files.count(); i++) {
        //files.append(files[i]);
        content.push_back(QUrl::fromLocalFile(dirs.path()+"/"+files[i]));
        new_content.push_back(QUrl::fromLocalFile(dirs.path()+"/"+files[i]));
        QFileInfo fi(files[i]);
        ui->listWidget->addItem(fi.fileName());
    }
    playlist->addMedia(new_content);
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

