#include "mainwindow.h"
#include "stdlib.h"
//#include <sqlite3.h>
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QSharedMemory>
#include <iostream>
#include <fstream>
#include "samoplayer.h"
using namespace std;
QString dir = "";
samoplayer *plr= new samoplayer;
int currentVolume = 50;
int nextTrack = 0, nowSelected = 0, currentTab = 0;
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
    dir = "/home/master-p/Music";
    iconInfo = new QIcon(QApplication::applicationDirPath()+"/.icons/info.png");
    QApplication::setApplicationName("Samowar Music Player");
    QApplication::setApplicationVersion("2.2.13b");
    iconCredits = new QIcon(QApplication::applicationDirPath()+"/.icons/strange-creature.png");
    ui->actionCredits->setIcon(*iconCredits);
    iconSavePlaylist = new QIcon(QApplication::applicationDirPath()+"/.icons/submenu-save-playlist.png");
    ui->actionSave_playlist->setIcon(*iconSavePlaylist);
    iconOpenPlaylist = new QIcon(QApplication::applicationDirPath()+"/.icons/submenu-open-playlist.png");
    ui->actionOpen_playlist->setIcon(*iconOpenPlaylist);
    iconRemoveDuplicates = new QIcon(QApplication::applicationDirPath()+"/.icons/submenu-remove-duplicates.png");
    ui->actionRemove_duplicates->setIcon(*iconRemoveDuplicates);
    iconExit = new QIcon(QApplication::applicationDirPath()+"/.icons/submenu-exit.png");
    ui->actionExit->setIcon(*iconExit);
    iconDebugOutput = new QIcon(QApplication::applicationDirPath()+"/.icons/submenu-debug-output.png");
    ui->actionToggle_debug_output->setIcon(*iconDebugOutput);
    icon200 = new QIcon(QApplication::applicationDirPath()+"/.icons/submenu-200.png");
    ui->action_200->setIcon(*icon200);
    iconAddTrack = new QIcon(QApplication::applicationDirPath()+"/.icons/submenu-add-track.png");
    ui->action_add_files->setIcon(*iconAddTrack);
    iconAddFolder = new QIcon(QApplication::applicationDirPath()+"/.icons/submenu-add-folder.png");
    ui->actionAdd_directory_s->setIcon(*iconAddFolder);
//    iconMenuPlaylist = new QIcon(QApplication::applicationDirPath()+"/.icons/menu-playlist.png");
//    ui->menuOptions->setIcon(*iconMenuPlaylist);
//    iconMenuOptions = new QIcon(QApplication::applicationDirPath()+"/.icons/menu-options.png");
//    ui->menuDonate->setIcon(*iconMenuOptions);
    iconPlay = new QIcon(QApplication::applicationDirPath()+"/.icons/media-play.png");
    ui->button_play->setIcon(*iconPlay);
    iconPause = new QIcon(QApplication::applicationDirPath()+"/.icons/media-pause.png");
    iconStop = new QIcon(QApplication::applicationDirPath()+"/.icons/media-stop.png");
    ui->button_stop->setIcon(*iconStop);
    iconPlayPrev = new QIcon(QApplication::applicationDirPath()+"/.icons/media-previous.png");
    ui->button_play_prev->setIcon(*iconPlayPrev);
    iconPlayNext = new QIcon(QApplication::applicationDirPath()+"/.icons/media-next.png");
    ui->button_play_next->setIcon(*iconPlayNext);
        iconClearPls = new QIcon(QApplication::applicationDirPath()+"/.icons/brush-big.png");
        ui->actionClear_playlist->setIcon(*iconClearPls);
    iconDeleteCurrent = new QIcon(QApplication::applicationDirPath()+"/.icons/brush-big.png");
    ui->deleteCurrentTrack->setIcon(*iconDeleteCurrent);
    ui->listDebug->setVisible(false);
    ui->buttonDebugClear->setVisible(false);
    window()->setWindowTitle(QApplication::applicationName()+" "+QApplication::applicationVersion());
    playlist = new QMediaPlaylist(plr);
    plr->setPlaylist(playlist);
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),this, SLOT(mySliderValueChanged(int)));
    connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(watchPlaying()));
    connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(progress()));
    connect(plr,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(watchNextTrack()));
    connect(plr,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(watchStatus()));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(watchSelectedTrack()));
    //connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(watchSelectedTrack()));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),ui->currentTrack_progressBar,SLOT(setValue(int)));
    connect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(atTrackEnd()));
    connect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(changeCurrentTab()));
    connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(atTrackEnd()));
    connect(playlist,SIGNAL(mediaRemoved(int,int)),this,SLOT(watchPlaylistChanges()));
    connect(playlist,SIGNAL(mediaInserted(int,int)),this,SLOT(watchPlaylistChanges()));
    connect(ui->A,SIGNAL(currentChanged(int)),this,SLOT(watchCurrentTab()));
    add_files_from_behind();
        loadConfiguration();
    //ui->A->currentWidget()->layout();
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
            ui->listWidget->setCurrentRow(nextTrack);
            //nextTrack = playlist->currentIndex();
            //ui->listWidget->setCurrentRow(playlist->currentIndex() != -1? playlist->currentIndex():0);
            ui->currentTrack_progressBar->setValue(1);
            plr->playMusic();
        }
                else {
                    if(plr->state() != 2)playlist->setCurrentIndex(nowSelected); //0 - stopped 1 - playing 2 - paused
                    //ui->listWidget->item(nextTrack)->setSelected(true);
                    //ui->currentTrack_progressBar->setValue(1);
                    plr->playMusic();
                }
    }
    else {
            ui->listWidget->item(nextTrack)->setSelected(true);
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
                    //saveConfiguration();
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
    //dir = QDir::;
    playlist->addMedia(new_content);
}

void MainWindow::on_radio_mute_toggled(bool checked)
{
     //plr -> toggleMute();
     if(checked) plr->setMuted(1);
     else plr->setMuted(0);
}

void MainWindow::on_button_play_prev_clicked()
{
    if(playlist->playbackMode() == QMediaPlaylist::Sequential)
    {
        if(nextTrack != 0) {
            ui->currentTrack_progressBar->setValue(1);
            playlist->previous();
            if(nextTrack == playlist->mediaCount()-1) nextTrack--;
            ui->listWidget->item(playlist->currentIndex())->setSelected(true);
            char *buffer = new char[3];
            char *buffer1 = new char[3];
            sprintf(buffer,"%d",nextTrack);
            sprintf(buffer1,"%d",playlist->currentIndex());
            const QString& str("play_prev_clicked. nextTrack is now ");
            const QString& str1(", curent index is now ");
            ui->listDebug->addItem(str+buffer+str1+buffer1);
        }
    }
    else if(playlist->mediaCount()!=0) {
        ui->currentTrack_progressBar->setValue(1);
        playlist->previous();
    }
}

void MainWindow::on_button_play_next_clicked()
{
    if(playlist->mediaCount() != 0) {
        if(playlist->playbackMode() == QMediaPlaylist::Sequential)
        {if(nextTrack != playlist->mediaCount()-1) {
                ui->currentTrack_progressBar->setValue(1);
                playlist->next();
                ui->listWidget->item(playlist->currentIndex())->setSelected(true);
                char *buffer = new char[100];
                char *buffer1 = new char[100];
                sprintf(buffer,"%d",nextTrack);
                sprintf(buffer1,"%d",playlist->currentIndex());
                const QString& str("play_next_clicked. nextTrack is now ");
                const QString& str1(", current index is now ");
             ui->listDebug->addItem(str+buffer+str1+buffer1);}
        }
        else {
            ui->currentTrack_progressBar->setValue(1);
            playlist->next();
        }
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    plr->stopMusic();
    nextTrack = ui->listWidget->currentRow();
    nowSelected = nextTrack;
    playlist->setCurrentIndex(nextTrack);
    plr->playMusic();
}

void MainWindow::on_deleteCurrentTrack_clicked()
{
    if(playlist->mediaCount() == 1) {
        plr->stopMusic();
        content.clear();
        playlist->clear();
        files.clear();
        ui->listWidget->clear();
        nowSelected = 0;
    }
    if(playlist->mediaCount() != 0) {
        int tmp = playlist->currentIndex();
        if(nowSelected < tmp) {disconnect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(atTrackEnd()));disconnect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(atTrackEnd()));plr->stopMusic();}
        char *buffer = new char[3];
        sprintf(buffer,"%d",playlist->currentIndex());
        const QString& str1("deleted track. current index is ");
        ui->listDebug->addItem(str1+buffer);
        content.removeAt(nowSelected);
        files.removeAt(nowSelected);
        playlist->removeMedia(nowSelected);
        int tmp_sel = nowSelected;
        if(tmp_sel == playlist->mediaCount()) tmp_sel--; //if last track is about to vanish, go select previous one
//                            //char *buffer = new char[3];
//                            sprintf(buffer,"%d",playlist->currentIndex());
//                            //const QString& str1("deleted track. current index is ");
//                            ui->listDebug->addItem(str1+buffer);
            ui->listWidget->clear();
            for(int i = 0; i < playlist->mediaCount(); i++) {
                QFileInfo fi(files[i]);
                ui->listWidget->addItem(fi.fileName());
            }
            nowSelected = tmp_sel;
            ui->listWidget->setCurrentRow(nowSelected);
    }
    else {
        plr->stopMusic();
        nextTrack = 0;
        nowSelected = 0;
    }
    connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(atTrackEnd()));
    connect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(atTrackEnd()));

}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    plr->setPosition(plr->duration()/100*position);
}

void MainWindow::watchPlaying() {
    QFileInfo fi(files[nextTrack]);
    int pos_secs = (plr->position()%60000)/1000;
    int dur_secs = (plr->duration()%60000)/1000;
    if(pos_secs < 10) ui -> label_test->setText(QString::number(plr->position()/60000) +":0"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":"+ QString::number(dur_secs));
    if(dur_secs < 10) ui -> label_test->setText(QString::number(plr->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":0"+ QString::number(dur_secs));
    if(pos_secs < 10 && dur_secs < 10) ui -> label_test->setText(QString::number(plr->position()/60000) +":0"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":0"+ QString::number(dur_secs));
    if(dur_secs > 10 && pos_secs > 10) ui -> label_test->setText(QString::number(plr->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":"+ QString::number(dur_secs));
    if(plr->isMuted())QMainWindow::setWindowTitle("[muted] Samowar - Playing... " + fi.fileName());
    else watchStatus();
}

void MainWindow::watchNextTrack() {
    if(debug) {
        char *buffer = new char[3];
        sprintf(buffer,"%d",nextTrack);
        const QString& str("watchNextTrack called. nextTrack was ");
        ui->listDebug->addItem(str+buffer);
    }
    for(int row=0; row != playlist->mediaCount(); row++)
        if(ui->listWidget->item(row)->isSelected()) ui->listWidget->item(row)->setSelected(false);
        ui->listWidget->item(nextTrack)->setSelected(true);
        if(debug) {
            char *buffer = new char[3];
            sprintf(buffer,"%d",nextTrack);
            const QString& str("And now nexttrack is ");
            ui->listDebug->addItem(str+buffer);
        }
}

void MainWindow::watchSelectedTrack() {
    //nextTrack = ui -> listWidget->currentRow();
    nowSelected = ui -> listWidget->currentRow();
    if(debug) {
        char *buffer = new char[3];
        sprintf(buffer,"%d",nowSelected);
        const QString& str1("watchSelectedTrack called. nowSelected is ");
        ui->listDebug->addItem(str1+buffer);
    }
}

void MainWindow::setSliderPosition(){
    ui -> horizontalSlider->setValue(plr->position()*100/plr->duration());
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    if(plr->state() == 2) tmp_pause = true;
    ui->horizontalSlider->setCursor(Qt::ClosedHandCursor);
    plr->pauseMusic();
    plr->disconnect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(progress()));
    plr->disconnect(ui->horizontalSlider, SIGNAL(valueChanged(int)),this, SLOT(mySliderValueChanged(int)));
}


void MainWindow::on_horizontalSlider_sliderReleased()
{
    if(!tmp_pause) plr->playMusic();//0 - stopped 1 - playing 2 - paused
    ui->horizontalSlider->setCursor(Qt::PointingHandCursor);
    plr->connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(progress()));
    plr->connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),this, SLOT(mySliderValueChanged(int)));
}

void MainWindow::atTrackEnd() {
    if(playlist->playbackMode() == QMediaPlaylist::Sequential && nextTrack == playlist->mediaCount()-1) {}
    else
    nextTrack = playlist->currentIndex();
    ui->currentTrack_progressBar->setValue(1);
    if(debug) {
        char *buffer = new char[3];
        sprintf(buffer,"%d",nextTrack);
        const QString& str("atTrackEnd called. nexttrack is now ");
        ui->listDebug->addItem(str+buffer);
    }
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
            //ui->horizontalSlider->setValue(0);
        playstate = false;
        QMainWindow::setWindowTitle(QApplication::applicationName()+" "+QApplication::applicationVersion());
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
    content.clear();
    playlist->clear();
    files.clear();
    ui->listWidget->clear();
    nextTrack = 0;
    nowSelected = 0;
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
    QStringList tmp_list;
    QList<QMediaContent> new_content;
    int tmp = files.count();
    files.append(recursiveAddFolder(tmp_list, directory));
    for(int i = tmp; i < files.count(); i++) {
        content.push_back(QUrl::fromLocalFile(files[i]));
        new_content.push_back(QUrl::fromLocalFile(files[i]));
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

    QString filename;
    filename = QFileDialog::getSaveFileName(this, "Save playlisto", QApplication::applicationDirPath()+"/playlists", tr("Samowar playlist files (*.smw)"));
    if(!filename.contains(".smw"))filename+=".smw";
    QFile f( filename );
    QFileInfo fi(f);
    f.open( QIODevice::WriteOnly );
    QTextStream outstream(&f);
    for(int i = 0; i < files.count(); i++)
    outstream << files[i] << '\n';
    f.close();
    if(ui->A->tabText(currentTab).contains("*"))
        ui->A->setTabText(currentTab, fi.fileName());
}

void MainWindow::changeCurrentTab() {
    ui->A->setCurrentWidget(ui->firstTab);
}

void MainWindow::progress() {
    if(plr->duration() != 0) {
    ui -> currentTrack_progressBar->setValue(plr->position()*100/plr->duration());
    ui -> horizontalSlider->setValue(plr->position()*100/plr->duration());
    }
}

void MainWindow::mySliderValueChanged(int newPos)
{
    // Make slider to follow the mouse directly and not by pageStep steps
    Qt::MouseButtons btns = QApplication::mouseButtons();
    QPoint localMousePos = ui->horizontalSlider->mapFromGlobal(QCursor::pos());
    bool clickOnSlider = (btns & Qt::LeftButton) &&
                         (localMousePos.x() >= 0 && localMousePos.y() >= 0 &&
                          localMousePos.x() < ui->horizontalSlider->size().width() &&
                          localMousePos.y() < ui->horizontalSlider->size().height()+20);
    if (clickOnSlider)
    {
        // Attention! The following works only for Horizontal, Left-to-right sliders
        float posRatio = localMousePos.x() / (float )ui->horizontalSlider->size().width();
        int sliderRange = ui->horizontalSlider->maximum() - ui->horizontalSlider->minimum();
        int sliderPosUnderMouse = ui->horizontalSlider->minimum() + sliderRange * posRatio;
        if (sliderPosUnderMouse != newPos)
        {
            ui->horizontalSlider->setValue(sliderPosUnderMouse);
            plr->setPosition(plr->duration()/100*sliderPosUnderMouse);
            return;
        }
    }
    // ...
}

void MainWindow::on_actionRemove_duplicates_triggered()
{
    if(files.count() != 0) {
    for(int cur = 0;cur < files.count(); cur++) {
        for(int i = cur+1; i < files.count(); i++) {
            if(files[cur] == files[i]) {
                files.removeAt(i);
                content.removeAt(i);
                playlist->removeMedia(i);
            }
        }
    }
    ui->listWidget->clear();
    for(int j = 0; j < playlist->mediaCount(); j++) {
        QFileInfo fi(files[j]);
        ui->listWidget->addItem(fi.fileName());
    }
    ui->listWidget->item(nextTrack)->setSelected(true);
    }
}

void MainWindow::add_files_from_behind()
{
    QStringList cmdline_args = QApplication::arguments();
if(cmdline_args.count() > 0) {
    cmdline_args.removeAt(0);
    for(int i = 0; i < cmdline_args.count(); i++) {
        if(!cmdline_args[i].contains(QDir::currentPath())) {
            files.append(QDir::currentPath()+"/"+cmdline_args[i]);
            ui->listDebug->addItem(files[i]);
        }
        else files = cmdline_args;
    }
    for(int i = 0; i < files.count(); i++) {
        content.push_back(QUrl::fromLocalFile(files[i]));
        QFileInfo fi(files[i]);
        ui->listWidget->addItem(fi.fileName());
    }
    playlist->addMedia(content);
}
if(!playstate) plr->playMusic();
}

void MainWindow::on_actionOpen_playlist_triggered()
{
    QList<QMediaContent> new_content;
    QFile f(QFileDialog::getOpenFileName(this, tr("Open samowar playlist"), QApplication::applicationDirPath()+"/playlists", tr("Samowar playlist files (*.smw)")));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    on_actionClear_playlist_triggered();
    QFileInfo fi(f);
    QTextStream in(&f);
    QString line, tmp;
//if(ui->A->tabText(0) == "♫") {
    while (!in.atEnd()) {
        line = in.readAll();
        for(int i = 0; i < line.count();i++) {
            if(line.at(i) == '\n') {
                files.append(tmp);
                new_content.push_back(QUrl::fromLocalFile(files.last()));
                QFileInfo fi(files.last());
                ui->listWidget->addItem(fi.fileName());
                ui->listDebug->addItem(tmp);
                tmp = "";
            }
            else tmp.append(line.at(i));
        }
    }
    f.close();
playlist->addMedia(new_content);
ui->A->setTabText(currentTab, fi.fileName());
ui->A->setCurrentIndex(currentTab);
    }
//    }
//    else {
//    currentTab++;
//        ui->A->tabBar()->addTab(fi.fileName());
//        ui->A->setCurrentIndex(currentTab);

        //ui->A->currentWidget()-> need to add different listwidget(s) & progress bar(s) to any tab
//    }
}

void MainWindow::watchPlaylistChanges() {
    if(!ui->A->tabText(currentTab).contains("*"))
        ui->A->setTabText(currentTab, ui->A->tabText(currentTab)+'*');
}

void MainWindow::listWidgetMenu(QPoint point) {
   //context menu delete
}

void MainWindow::watchCurrentTab() {
    currentTab = ui->A->currentIndex();
}

void MainWindow::saveConfiguration() {
    QString filename;
    //-- volume
    filename = QApplication::applicationDirPath()+"/.config/volume.conf";
    QFile fVol( filename );
    fVol.open( QIODevice::WriteOnly );
    QTextStream outstream1(&fVol);
    outstream1 << plr->volume();
    fVol.close();
    //--
    //-- playlist
    filename = QApplication::applicationDirPath()+"/.config/playlist.conf";
    QFile fPls( filename );
    fPls.open( QIODevice::WriteOnly );
        QTextStream outstream2(&fPls);
    for(int i = 0; i < files.count(); i++)
    outstream2 << files[i] << '\n';
    fPls.close();
    //--
    //-- nextTrack
    filename = QApplication::applicationDirPath()+"/.config/nexttrack.conf";
    QFile fNext( filename );
    fNext.open( QIODevice::WriteOnly );
        QTextStream outstream3(&fNext);
    outstream3 << nextTrack;
    fNext.close();
    //--
    //-- currentTabText
    filename = QApplication::applicationDirPath()+"/.config/currenttabtext.conf";
    QFile fTabText( filename );
    fTabText.open( QIODevice::WriteOnly );
        QTextStream outstream4(&fTabText);
    outstream4 << ui->A->tabText(currentTab);
    fTabText.close();
    //--
    //-- plr->position()
    filename = QApplication::applicationDirPath()+"/.config/position.conf";
    QFile fPos( filename );
    fPos.open( QIODevice::WriteOnly );
        QTextStream outstream5(&fPos);
    outstream5 << plr->position();
    fPos.close();
    //--
    //-- plr->state()
    filename = QApplication::applicationDirPath()+"/.config/state.conf";
    QFile fState( filename );
    fState.open( QIODevice::WriteOnly );
        QTextStream outstream6(&fState);
    outstream6 << plr->state();
    fState.close();
    //--
    //-- flags()
    filename = QApplication::applicationDirPath()+"/.config/flags.conf";
    QFile fFlags( filename );
    fFlags.open( QIODevice::WriteOnly );
        QTextStream outstream7(&fFlags);
    outstream7 << repeat;
    outstream7 << single;
    outstream7 << randome;
    fFlags.close();
    //--
    //-- mute
    filename = QApplication::applicationDirPath()+"/.config/mute.conf";
    QFile fMute( filename );
    fMute.open( QIODevice::WriteOnly );
        QTextStream outstream8(&fMute);
    outstream8 << plr->isMuted();
    fMute.close();
    //--
    //-- continue
    filename = QApplication::applicationDirPath()+"/.config/continue_playing.conf";
    QFile fCont( filename );
    fCont.open( QIODevice::WriteOnly );
        QTextStream outstream9(&fCont);
    outstream9 << ui->actionAuto_pause_when_closed->isChecked();
    fCont.close();
    //--
}

void MainWindow::loadConfiguration() {
    QString line;
    //-- playlist
    QFile fPls(QApplication::applicationDirPath()+"/.config/playlist.conf");
    if (!fPls.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    QTextStream in1(&fPls);
    QString line, tmp;
    ui->listDebug->addItem("go");
    while (!in1.atEnd()) {
        line = in1.readAll();
        for(int i = 0; i < line.count();i++) {
            if(line.at(i) == '\n') {
                files.append(tmp);
                content.push_back(QUrl::fromLocalFile(files.last()));
                QFileInfo fi(files.last());
                ui->listWidget->addItem(fi.fileName());
                ui->listDebug->addItem(tmp);
                tmp = "";
            }
            else tmp.append(line.at(i));
        }
    }
    ui->listDebug->addItem("end");
    playlist->addMedia(content);
    fPls.close();
    }
    //--
    //-- volume
    QFile fVol(QApplication::applicationDirPath()+"/.config/volume.conf");
    if (!fVol.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    QTextStream in2(&fVol);
    line = in2.readAll();
    ui->dialVolume->setValue(line.toInt());
    fVol.close();
    }
    //--
    //-- nextTrack
    QFile fNext(QApplication::applicationDirPath()+"/.config/nexttrack.conf");
    if (!fNext.open(QIODevice::ReadOnly | QIODevice::Text))
    ui->listDebug->addItem("fail");
    else {
    QTextStream in3(&fNext);
    line = in3.readAll();
    nextTrack = line.toInt();
    nowSelected = nextTrack;
    //ui->listWidget->setCurrentRow(nowSelected);
    if(playlist->mediaCount() != 0) playlist->setCurrentIndex(nextTrack);
    fVol.close();
    }
    //--
    //-- plr->position()
    QFile fPos(QApplication::applicationDirPath()+"/.config/position.conf");
    if (!fPos.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    QTextStream in4(&fPos);
    line = in4.readAll();
    if(line.toInt()!=0) {
        plr->setPosition(line.toInt());
    }
    fPos.close();
    }
    //--
    //-- continue
    QFile fCont(QApplication::applicationDirPath()+"/.config/continue_playing.conf");
    if (!fCont.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    QTextStream in9(&fCont);
    line = in9.readAll();
    ui->actionAuto_pause_when_closed->setChecked(line.toInt());
    fCont.close();
    }
    //--
    //-- plr->state()
    QFile fState(QApplication::applicationDirPath()+"/.config/state.conf");
    if (!fState.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    QTextStream in5(&fState);
    line = in5.readAll();
    if(line.toInt() == 1) {
        if(ui->actionAuto_pause_when_closed->isChecked()) {
            plr->pauseMusic();
        }
    else plr->playMusic();
    }
    if(line.toInt() == 2) plr->pauseMusic();
    fState.close();
    }
    //--
    //-- Tab text
    QFile fTabText(QApplication::applicationDirPath()+"/.config/currenttabtext.conf");
    if (!fTabText.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    QTextStream in6(&fTabText);
    line = in6.readAll();
    ui->A->setTabText(currentTab, line);
    fTabText.close();
    }
    //--
    //-- Flags
    QFile fFlags(QApplication::applicationDirPath()+"/.config/flags.conf");
    if (!fFlags.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    QTextStream in7(&fFlags);
    line = in7.readAll();
    if(line == "100") ui->checkBox_repeat->setChecked(true);
    if(line == "010") ui->checkBox_single->setChecked(true);
    if(line == "001") ui->checkBox_random->setChecked(true);
    fFlags.close();
    }
    //--
    //-- mute
    QFile fMute(QApplication::applicationDirPath()+"/.config/mute.conf");
    if (!fMute.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->listDebug->addItem("fail");
    else {
    QTextStream in8(&fMute);
    line = in8.readAll();
    if(line.toInt() > 0) ui->radio_mute->setChecked(true);
    fMute.close();
    }
    //--
}

QStringList MainWindow::recursiveAddFolder(QStringList out, QString path) {
//                file = QDir::toNativeSeparators(file);
//                QStringList zapishem; //store string with file names
//                static QStringList sohranim;
//                QFileInfo filnfo(file);
//                if (filnfo.isDir()) {
//            QDir dirs(file);
//            zapishem = dirs.entryList(QStringList() << "*.mp3" << "*.flac" << "*.wav" << "*.ogg" << "*.3ga");
//            zapishem.append(dirs.entryList(QDir::Dirs |QDir::NoDotAndDotDot | QDir::Hidden));
//            while(i != zapishem.count()-1) {
//                    i++;
//                    recursiveAddFolder(i, tmp_list, file+"/"+zapishem.at(i));
//                    QString that = QDir::toNativeSeparators(zapishem.at(i));
//                    QFileInfo finfo(that);
//                    if (!finfo.isDir())sohranim.append(zapishem.at(i));
//            }
//    } else {
//    }
    QFileInfo finfo(path);
    static QStringList sohranim;
    if (finfo.isDir()) {
            QDir dirs(path);
            QList<QString> entrys = dirs.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
            while(!entrys.isEmpty()) {
                recursiveAddFolder(out, path+"/"+entrys.takeFirst());
            }
    }
    else {
            QString suf = finfo.suffix();
            if (suf == "mp3" || suf == "flac" || suf == "wav" || suf == "ogg"  || suf == "3ga") {
                sohranim.append(path);
            }
    }
    return sohranim;
}
