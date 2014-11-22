#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include "samoplayer.h"

samoplayer *player= new samoplayer;
QString dir = "/home/master-p/Music";
QStringList pls;
int currentVolume = 50;
int nextTrack = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QIcon *iconPlay = new QIcon("media-play.png");
    ui->button_play->setIcon(*iconPlay);
    QIcon *iconPause = new QIcon("media-pause.png");
    ui->button_pause->setIcon(*iconPause);
    QIcon *iconStop = new QIcon("media-stop.png");
    ui->button_stop->setIcon(*iconStop);
    QIcon *iconPlayPrev = new QIcon("media-previous.png");
    ui->button_play_prev->setIcon(*iconPlayPrev);
    QIcon *iconPlayNext = new QIcon("media-next.png");
    ui->button_play_next->setIcon(*iconPlayNext);
    QIcon *iconClearPls = new QIcon("desktop-brush-big.png");
    ui->button_clearPlaylist->setIcon(*iconClearPls);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_play_clicked() //костыли будут тут
{
    QTimer *timer = new QTimer(this);
         connect(timer, SIGNAL(timeout()), this, SLOT(updateCaption()));
         timer->start(1000);

   if(pls.count() == 0) {
        pls = QFileDialog::getOpenFileNames( this, tr("Open music file(s)"), dir, tr("Music files (*.ogg *.mp3 *.3ga *.wav *.flac)"));
        if(pls.count() != 0) {
        ui->listWidget->addItems(pls);
            player -> setCurrentTrack(pls.first());
            player -> playMusic();
            ui->listWidget->item(0)->setSelected(true);
        }
    }
            else {
                player->playMusic();
                while(timer->isActive()) { // new
                ui->listWidget->item(nextTrack)->setSelected(true);
                int pos_secs = (player->position()-player->position()/1000)/1000-60*(player->position()/60000);
                int dur_secs = (player->duration()-player->duration()/1000)/1000-60*(player->duration()/60000);
                QMainWindow::setWindowTitle("Samowar - Playing... " + player->getCurrentTrack());
                ui -> label_test->setText(QString::number(player->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(player->duration()/60000) +":"+ QString::number(dur_secs));
                ui -> currentTrack_progressBar->setValue(player->position()*100/player->duration());
               // ui -> horizontalSlider->setValue(player->position()*100/player->duration());//}
                }
            }
}

void MainWindow::on_button_stop_clicked()
{
    if(player -> getCurrentTrack() != "") {
    player->stopMusic();
    QMainWindow::setWindowTitle("Samowar Music player v.0.7.50");
    }
}

void MainWindow::on_button_pause_clicked()
{
    if(player -> getCurrentTrack() != "") {
    player->pauseMusic();
    QMainWindow::setWindowTitle("Samowar - " + player->getCurrentTrack() + " [paused]");
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
    ui->listWidget->clear();
    ui->listWidget->addItems(pls);
    if(pls.count() != 0) {
    //int i = 0;
    //ui->listWidget->clear();
   // do {ui->listWidget->addItem(pls.at(i)); i++;}
    //while(i != pls.count());
    if(player->getCurrentTrack() == "")
        player->setCurrentTrack(pls.first());
        //ui->listWidget->item(0)->setSelected(true);
    }
}


void MainWindow::on_volumeSlider_valueChanged(int value)
{
    player -> setVolume(value);
}

void MainWindow::on_button_mute_toggled(bool checked)
{
            player -> toggleMute();
}

void MainWindow::on_button_play_prev_clicked()
{
    if (nextTrack != 0) {
        nextTrack--;
        player -> setCurrentTrack(pls.at(nextTrack));
        QMainWindow::setWindowTitle("Samowar - Playing... " + player->getCurrentTrack());
        if(player->isAudioAvailable() == 1) {
            player->playMusic();
            ui->listWidget->item(nextTrack)->setSelected(true);
        }
    }
}

void MainWindow::on_button_play_next_clicked()
{
    if(pls.count() != 0) {
   // if(player->mediaStatus() != 'NoMedia') { //no Media
    if (pls.at(nextTrack) != pls.last()) {
        nextTrack++;
        player -> setCurrentTrack(pls.at(nextTrack));
        QMainWindow::setWindowTitle("Samowar - Playing... " + player->getCurrentTrack());
        if(player->isAudioAvailable() == 1) {
            player->playMusic();
            ui->listWidget->item(nextTrack)->setSelected(true);
        }
    }
    }
}

void MainWindow::on_button_clearPlaylist_clicked()
{
    pls.clear();
    ui->listWidget->clear();
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    player->stopMusic();
    player->setCurrentTrack(item->text());
    nextTrack = ui->listWidget->currentRow();
    player->playMusic();
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    player->setPosition(player->duration()/100*position);
}


void MainWindow::on_deleteCurrentTrack_clicked()
{
    pls.removeAt(ui->listWidget->currentRow());
    ui->listWidget->clear();
    ui->listWidget->addItems(pls);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    player->setPosition(player->duration()/100*value);
}

