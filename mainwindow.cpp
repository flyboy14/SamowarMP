#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QSharedMemory>
#include <fstream>
#include "samoplayer.h"
QString dir = "", language, versionRu;
samoplayer *plr= new samoplayer;
int nowSelected = 0, currentTab = 0, def_width, def_height;
bool repeat=false, randome=false, single=false, was_paused, playstate = false;
#ifdef Q_OS_LINUX
QString iconsDir = "/usr/share/samowar/icons", confDir = QDir::homePath()+"/.config/samowar/conf",
plsDir = QDir::homePath()+"/.config/samowar/playlists";
#else
QString iconsDir = QApplication::applicationDirPath()+"/.icons",
confDir = QApplication::applicationDirPath()+"/.config",
plsDir = QApplication::applicationDirPath()+"/playlists";
#endif
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    playlist = new QMediaPlaylist(plr);
    plr->setPlaylist(playlist);
        setVariables();
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),this, SLOT(mySliderValueChanged(int)));
    connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(watchPlaying()));
    connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(progress()));
    connect(plr,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(watchNextTrack()));
    connect(plr,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(watchStatus()));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(watchSelectedTrack()));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),ui->currentTrack_progressBar,SLOT(setValue(int)));
    connect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(atTrackEnd()));
    connect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(changeCurrentTab()));
    connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(atTrackEnd()));
    connect(playlist,SIGNAL(mediaRemoved(int,int)),this,SLOT(watchPlaylistChanges()));
    connect(playlist,SIGNAL(mediaInserted(int,int)),this,SLOT(watchPlaylistChanges()));
    //connect(ui->listWidget, SIGNAL(QEvent::ChildRemoved),this,SLOT(watchInternalDD()));
        loadConfiguration();
        add_files_from_behind();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_play_clicked()
{
    if(!playstate) {
        if(playlist->mediaCount() == 0) {
            on_action_add_files_triggered();
            ui->listWidget->setCurrentRow(0);
            ui->currentTrack_progressBar->setValue(1);
            plr->playMusic();
        }
                else {
                    if(plr->state() != 2) playlist->setCurrentIndex(nowSelected); //0 - stopped 1 - playing 2 - paused
                    else ui->listWidget->setCurrentRow(playlist->currentIndex());
                    plr->playMusic();
                }
    }
    else {
            ui->listWidget->setCurrentRow(playlist->currentIndex());
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
    if(language == "EN") msg.setText("This function is not supported yet");
    else msg.setText("К сожалению, эта функция еще не поддерживается");
    msg.setIcon(QMessageBox::Critical);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

void MainWindow::on_actionExit_triggered()
{
    QMessageBox msg;
    if(language == "EN") msg.setText("Are you 100% sure to exit Samowar session?");
    else msg.setText("Желаете покинуть САМОВАРЪ?");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    if(language == "EN") {
        msg.QMessageBox::setButtonText(QMessageBox::Ok, "Yes");
        msg.QMessageBox::setButtonText(QMessageBox::Cancel, "No");
    }
    else {
        msg.QMessageBox::setButtonText(QMessageBox::Ok, "Желаю");
        msg.QMessageBox::setButtonText(QMessageBox::Cancel, "Отнюдь");
    }
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
            default: break;
        }
}

void MainWindow::on_action_add_files_triggered()
{
    QStringList tmp;
    if(language == "EN") tmp.append(QFileDialog::getOpenFileNames(
                                          this, tr("Open music file(s)"), dir, tr("Music files (*.ogg *.mp3 *.3ga *.wav *.flac)")));
    else tmp.append(QFileDialog::getOpenFileNames(
                          this, tr("Открыть файл(ы) с музыкой"), dir, tr("Музыкальные файлы (*.ogg *.mp3 *.3ga *.wav *.flac)")));
    files.append(tmp);
    addToPlaylist(tmp);
}

void MainWindow::on_radio_mute_toggled(bool checked)
{
     if(plr->isMuted()) plr->setMuted(0);
     else plr->setMuted(1);
}

void MainWindow::on_button_play_prev_clicked()
{
    if(playlist->mediaCount()!=0) {
    if(playlist->playbackMode() == QMediaPlaylist::Sequential) {
            ui->currentTrack_progressBar->setValue(1);
            if(playlist->currentIndex() != 0) playlist->previous();
            ui->listWidget->setCurrentRow(playlist->currentIndex());
        }
    else {
        ui->currentTrack_progressBar->setValue(1);
        playlist->previous();
        ui->listWidget->setCurrentRow(playlist->currentIndex());
    }
    }
}

void MainWindow::on_button_play_next_clicked()
{
    if(playlist->mediaCount() != 0) {
        if(playlist->playbackMode() == QMediaPlaylist::Sequential ) {
            ui->currentTrack_progressBar->setValue(1);
            if(playlist->currentIndex() != playlist->mediaCount()-1) playlist->next();
            ui->listWidget->setCurrentRow(playlist->currentIndex());
        }
        else {
            ui->currentTrack_progressBar->setValue(1);
            playlist->next();
            ui->listWidget->setCurrentRow(playlist->currentIndex());
        }
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    plr->stopMusic();
    nowSelected = ui->listWidget->currentRow();
    playlist->setCurrentIndex(nowSelected);
    plr->playMusic();
}

void MainWindow::on_deleteCurrentTrack_clicked()
{
    if(playlist->mediaCount() == 1) {
        plr->stopMusic();
        on_actionClear_playlist_triggered();
    }
    if(playlist->mediaCount() != 0) {
        int tmp = playlist->currentIndex();
        if(nowSelected < tmp) {
            disconnect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(atTrackEnd()));
            disconnect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(atTrackEnd()));
            disconnect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(watchPlaying()));
            plr->stopMusic();
        }
        files.removeAt(nowSelected);
        playlist->removeMedia(nowSelected);
        int tmp_sel = nowSelected;
        if(tmp_sel == playlist->mediaCount()) tmp_sel--; //if last track is about to vanish, go select previous one
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
        nowSelected = 0;
    }
    connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(atTrackEnd()));
    connect(plr,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(atTrackEnd()));
    connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(watchPlaying()));

}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    plr->setPosition(plr->duration()/100*position);
}

void MainWindow::watchPlaying() {
    if(files.count() != 0 && playlist->currentIndex() < files.count() && playlist->currentIndex() != -1) {
        QFileInfo fi(files[playlist->currentIndex()]);
        int pos_secs = (plr->position()%60000)/1000;
        int dur_secs = (plr->duration()%60000)/1000;
        if(pos_secs < 10) ui -> labelDuration->setText(QString::number(plr->position()/60000) +":0"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":"+ QString::number(dur_secs));
        if(dur_secs < 10) ui -> labelDuration->setText(QString::number(plr->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":0"+ QString::number(dur_secs));
        if(pos_secs < 10 && dur_secs < 10) ui -> labelDuration->setText(QString::number(plr->position()/60000) +":0"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":0"+ QString::number(dur_secs));
        if(dur_secs > 10 && pos_secs > 10) ui -> labelDuration->setText(QString::number(plr->position()/60000) +":"+ QString::number(pos_secs) + " / " + QString::number(plr->duration()/60000) +":"+ QString::number(dur_secs));
        if(plr->isMuted() && language == "EN") QMainWindow::setWindowTitle("[muted] Samowar - Playing... " + fi.fileName());
        if(plr->isMuted() && language == "RU") QMainWindow::setWindowTitle("[без звука] САМОВАРЪ - Сейчас играет... " + fi.fileName());
        if(!plr->isMuted()) watchStatus();
    }
}

void MainWindow::watchInternalDD() {
}

void MainWindow::watchNextTrack() {
    for(int row=0; row != playlist->mediaCount(); row++)
        if(ui->listWidget->item(row)->isSelected()) ui->listWidget->item(row)->setSelected(false);
        if(nowSelected < playlist->currentIndex()) ui->listWidget->setCurrentRow(nowSelected);
        else ui->listWidget->setCurrentRow(playlist->currentIndex());
}

void MainWindow::watchSelectedTrack() {
    nowSelected = ui -> listWidget->currentRow();
}

void MainWindow::setSliderPosition(){
    ui -> horizontalSlider->setValue(plr->position()*100/plr->duration());
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    if(plr->state() == 2) was_paused = true;
    ui->horizontalSlider->setCursor(Qt::ClosedHandCursor);
    plr->pauseMusic();
    plr->disconnect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(progress()));
    plr->disconnect(ui->horizontalSlider, SIGNAL(valueChanged(int)),this, SLOT(mySliderValueChanged(int)));
}


void MainWindow::on_horizontalSlider_sliderReleased()
{
    if(!was_paused) plr->playMusic();//0 - stopped 1 - playing 2 - paused
    ui->horizontalSlider->setCursor(Qt::ArrowCursor);
    plr->connect(plr,SIGNAL(positionChanged(qint64)),this,SLOT(progress()));
    plr->connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),this, SLOT(mySliderValueChanged(int)));
}

void MainWindow::atTrackEnd() {
    if(files.count() != 0) {
        ui->listWidget->setCurrentRow(playlist->currentIndex());      
    ui->currentTrack_progressBar->setValue(1);
    ui->label_5->setText(QString::number(playlist->currentIndex()+1));
    }
}

void MainWindow::watchStatus() {
    if(plr->state() == 0) {
        ui->button_play->setStyleSheet("QPushButton { border-image: url(/usr/share/samowar/icons/media-play.png);} QPushButton::hover { border-image: url(/usr/share/samowar/icons/media-play-hover.png); } QPushButton::pressed { border-image: url(/usr/share/samowar/icons/media-play.png);}");
        ui->currentTrack_progressBar->setValue(1);
        //ui->horizontalSlider->setValue(0);
        playstate = false;
        if(language == "EN") QMainWindow::setWindowTitle(QApplication::applicationName()+" "+QApplication::applicationVersion());
        else QMainWindow::setWindowTitle("МУЗЫКАЛЬНЫЙ ПРОИГРЫВАТЕЛЬ САМОВАРЪ "+ versionRu);
    }
    if(plr->state() == 2 && playlist->currentIndex() != -1) {
        QFileInfo fi(files[playlist->currentIndex()]);
        if (language == "EN") QMainWindow::setWindowTitle("[paused] Samowar - " + fi.fileName());
        else QMainWindow::setWindowTitle("[пауза] САМОВАРЪ - " + fi.fileName());
        if(playstate == true) {
            ui->button_play->setStyleSheet("QPushButton { border-image: url(/usr/share/samowar/icons/media-pause.png); } QPushButton::hover { border-image: url(/usr/share/samowar/icons/media-play-hover.png); } QPushButton::pressed { border-image: url(/usr/share/samowar/icons/media-pause.png); }");
            if (language == "EN") ui->button_play->setToolTip("Play music (F9)");
            else ui->button_play->setToolTip("Играть (F9)");
            playstate = false;
        } // to prevent memory leaks
    }
    if(plr->state() == 1 && playlist->currentIndex() != -1) {
        QFileInfo fi(files[playlist->currentIndex()]);
        if (language == "EN") QMainWindow::setWindowTitle("Samowar - Playing... " + fi.fileName() );
        else QMainWindow::setWindowTitle("САМОВАРЪ - Сейчас играет... " + fi.fileName() );
        if(playstate == false) {
            //ui->button_play->setIcon(*iconPause);
            ui->button_play->setStyleSheet("QPushButton { border-image: url(/usr/share/samowar/icons/media-play.png); } QPushButton::hover { border-image: url(/usr/share/samowar/icons/media-pause-hover.png); } QPushButton::pressed { border-image: url(/usr/share/samowar/icons/media-play.png); }");
            if(language == "EN") ui->button_play->setToolTip("Pause music (F9)");
            else ui->button_play->setToolTip("Пауза (F9)");
            playstate = true;
        } // to prevent memory leaks
    }
    if(playstate == false) {
        if (language == "EN") ui->button_play->setToolTip("Play music (F9)");
        else ui->button_play->setToolTip("Играть (F9)");
    }
    if(playlist->playbackMode() == QMediaPlaylist::Sequential && playlist->currentIndex() == playlist->mediaCount()-1) ui->button_play_next->setStyleSheet("QPushButton { border-image: url(/usr/share/samowar/icons/media-next.png); } QPushButton::hover { border-image:url(/usr/share/samowar/icons/media-next-inactive.png);} QPushButton::pressed { border-image: url(/usr/share/samowar/icons/media-next.png); }");
    else ui->button_play_next->setStyleSheet("QPushButton { border-image: url(/usr/share/samowar/icons/media-next.png); } QPushButton::hover { border-image:url(/usr/share/samowar/icons/media-next-hover.png);} QPushButton::pressed { border-image: url(/usr/share/samowar/icons/media-next.png); }");
    if(playlist->playbackMode() == QMediaPlaylist::Sequential && playlist->currentIndex() == 0) ui->button_play_prev->setStyleSheet("QPushButton { border-image: url(/usr/share/samowar/icons/media-previous.png); } QPushButton::hover { border-image:url(/usr/share/samowar/icons/media-previous-inactive.png);} QPushButton::pressed { border-image: url(/usr/share/samowar/icons/media-previous.png); }");
    else ui->button_play_prev->setStyleSheet("QPushButton { border-image: url(/usr/share/samowar/icons/media-previous.png); } QPushButton::hover { border-image:url(/usr/share/samowar/icons/media-previous-hover.png);} QPushButton::pressed { border-image: url(/usr/share/samowar/icons/media-previous.png); }");
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
//    QWidget *inst = new QWidget;
//    QGridLayout *gridL;
//    QListWidget *listw = new QListWidget;
//    gridL->addWidget(listw);
//    //gridL->addWidget(new QSlider);
//    //gridL->addWidget(new QLabel);
//    inst->setLayout(gridL);
//        ui->A->addTab(inst, "new");
}

void MainWindow::on_actionClear_playlist_triggered()
{
    plr->stopMusic();
    files.clear();
    ui->listWidget->clear();
    playlist->clear();
    nowSelected = 0;
}

void MainWindow::on_dialVolume_valueChanged(int value)
{
    plr -> setVolume(value);
    char *buffer = new char[3];
    sprintf(buffer,"%d",value);
    ui->labelVolumeDef -> setText(buffer);
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
    QString directory;
    if(language == "EN") directory = QFileDialog::getExistingDirectory(this,tr("Select dir to import files"));
    else directory = QFileDialog::getExistingDirectory(this,tr("Открыть каталог с музыкой"));
    dir = directory;
    if(directory.isEmpty())
        return;
    QStringList tmp_list;
    recursiveAddFolder(&tmp_list, directory);
    files.append(tmp_list);
    addToPlaylist(tmp_list);
    ui->listWidget->setCurrentRow(playlist->currentIndex());
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
    if(language=="EN") filename = QFileDialog::getSaveFileName(this, "Save playlisto", plsDir, tr("Samowar playlist files (*.smw)"));
    else filename = QFileDialog::getSaveFileName(this, "Сохранить список воспроизведения", plsDir, tr("Файлы списков воспроизведения (*.smw)"));
    if(!filename.contains(".smw")) filename+=".smw";
        saveToFile(files, filename);
    QFileInfo fi(filename);
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
    if (clickOnSlider) {
        // Attention! The following works only for Horizontal, Left-to-right sliders
        float posRatio = localMousePos.x() / (float )ui->horizontalSlider->size().width();
        int sliderRange = ui->horizontalSlider->maximum() - ui->horizontalSlider->minimum();
        int sliderPosUnderMouse = ui->horizontalSlider->minimum() + sliderRange * posRatio;
        if (sliderPosUnderMouse != newPos) {
            ui->horizontalSlider->setValue(sliderPosUnderMouse);
            if(files.count() != 0) plr->setPosition(plr->duration()/100*sliderPosUnderMouse);
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
                playlist->removeMedia(i);
            }
        }
    }
    ui->listWidget->clear();
    for(int j = 0; j < playlist->mediaCount(); j++) {
        QFileInfo fi(files[j]);
        ui->listWidget->addItem(fi.fileName());
    }
    ui->listWidget->item(playlist->currentIndex())->setSelected(true);
    }
}

void MainWindow::add_files_from_behind()
{
    QString wrongfile;
    QStringList cmdline_args = QApplication::arguments();
    if(cmdline_args.count() > 1) {
        cmdline_args.removeAt(0);
        if(cmdline_args.contains("--help") || cmdline_args.contains("-h")) {
            if(language == "EN") cout << "Usage: samowar [options] [file(s)]\nOptions:\n--help or -h     Show help and exit\n--language or -l    set language(values ru, en)" << endl;
            else cout << "Использование: samowar [options] [file(s)]\nОпции:\n--help или -h     вывести на экран справку и выйти\n--language или -l    выставить язык(значения ru, en)" << endl;
            exit(0);
        }
       // if(cmdline_args.contains("-l") || cmdline_args.contains("--language")) {
            for(int l,v,t = 0; l < cmdline_args.count(); l++, v++, t++) {
                if(cmdline_args.at(l) == "-l" || cmdline_args.at(l) == "--language") {
                    if(cmdline_args.at(l+1) == "en") {
                        on_actionEnglish_triggered();
                        cmdline_args.removeAt(0);
                        cmdline_args.removeAt(0);
                        if(cmdline_args.count() == 0) return;
                    }
                    if(cmdline_args.at(l+1) == "ru") {
                        on_action_triggered();
                        cmdline_args.removeAt(0);
                        cmdline_args.removeAt(0);
                        if(cmdline_args.count() == 0) return;
                    }
                }
                if(cmdline_args.at(v) == "-v" || cmdline_args.at(v) == "--volume") {
                    plr->setVolume(cmdline_args.at(v+1).toInt());
                }
                //if(cmdline_args(t) == "-t" || cmdline_args(t) == "--timer") {
                //    plr->setVolume(cmdline_args.at(t+1).toInt());
                //}
            }
        //}
        for(int i = 1;i < cmdline_args.count()+1;i++) {
            i--;
            QFileInfo fi(cmdline_args[i]);
            QString suf = fi.suffix();
            if (suf != "mp3" && suf != "flac" && suf != "wav" && suf != "ogg"  && suf != "3ga") {
                if(language == "EN") {
                    wrongfile = "file "+fi.fileName()+" is not a music file!";
                QMessageBox::critical(this, tr("Wrong filetype"), wrongfile, QMessageBox::Ok, QMessageBox::Ok);
                }
                else {
                    wrongfile = "файл "+fi.fileName()+" не похож на музыкальный!";
                QMessageBox::critical(this, tr("Неверный тип файла"), wrongfile, QMessageBox::Ok, QMessageBox::Ok);
                }
                cmdline_args.removeAt(i);
            }
            else i++;
        }
        if(cmdline_args.count() == 0) return;
        if(ui->listWidget->count() != 0) {
            on_actionClear_playlist_triggered();
            ui->A->tabBar()->setTabText(currentTab, "♫*");
        }
        QDir dirs(QDir::currentPath());
        QStringList entrys = dirs.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
        for(int i = 0; i < cmdline_args.count(); i++)
            for(int j = 0; j < entrys.count();j++) {
            QFileInfo fi(entrys[j]);
            if(fi.fileName() == cmdline_args[i])
                files.append(QDir::currentPath()+"/"+cmdline_args[i]);
            }
        if(files.count() == 0) files = cmdline_args;
        addToPlaylist(files);
        if(!playstate) plr->playMusic();
    }
}

void MainWindow::on_actionOpen_playlist_triggered()
{
    QList<QMediaContent> new_content;
    QString file;
    if(language == "EN") file = QFileDialog::getOpenFileName(
                                     this, tr("Open samowar playlist"), plsDir, tr("Samowar playlist files (*.smw)"));
    else file = QFileDialog::getOpenFileName(
                     this, tr("Открыть список воспроизведения"), plsDir, tr("Списки файлов (*.smw)"));
        QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        cout << "fail";
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
   watchStatusBar();
}

void MainWindow::watchStatusBar() {
    if(files.count() != 0) {
        ui->label_3->setText(QString::number(playlist->mediaCount()));
        ui->label_7->setText(QString::number(playlist->mediaCount()));
        if(language == "EN") {
            if(playlist->mediaCount() == 1) ui->label_2->setText("file");
            else ui->label_2->setText("files");
        }
        if(language == "RU") {
            if(playlist->mediaCount()%100 == 11 || playlist->mediaCount()%100 == 12 || playlist->mediaCount()%100 == 13 ||
                    playlist->mediaCount()%100 == 14) {
                ui->label_2->setText("файлов");
                ui->label->setText("Загружено");
            }
            else if(playlist->mediaCount()%10 == 1) {
                ui->label_2->setText("файл");
                ui->label->setText("Загружен");
            }
            else if(playlist->mediaCount()%10 == 2 || playlist->mediaCount()%10 == 3 ||
                    playlist->mediaCount()%10 == 4) {
                ui->label_2->setText("файла");
                ui->label->setText("Загружено");
            }
            else {
                ui->label_2->setText("файлов");
                ui->label->setText("Загружено");
            }
        }
    }
    else {
        ui->label_3->setText(QString::number(playlist->mediaCount()));
        ui->label_7->setText(QString::number(playlist->mediaCount()));
            if(language == "RU") {
                ui->label_5->setText(ui->label_7->text());
                ui->label_2->setText("");
                ui->label_3->setText("пуст");
                ui->label->setText("Список");
            }
            if(language == "EN") {
                ui->label_5->setText(ui->label_7->text());
                ui->label_2->setText("Playlist");
                ui->label_3->setText("is");
                ui->label->setText("empty");
            }
    }
}

void MainWindow::saveConfiguration() {
    saveToFile(window()->geometry(), confDir+"/geometry.conf");
    saveToFile(language, confDir+"/lang.conf");
    saveToFile(plr->volume(), confDir+"/volume.conf");
    saveToFile(files, confDir+"/playlist.conf");
    saveToFile(playlist->currentIndex(), confDir+"/nexttrack.conf");
    saveToFile(ui->A->tabText(currentTab), confDir+"/currenttabtext.conf");
    saveToFile(plr->position(), confDir+"/position.conf");
    saveToFile(plr->state(), confDir+"/state.conf");
    saveToFile(plr->isMuted(), confDir+"/mute.conf");
    saveToFile(ui->actionAuto_pause_when_closed->isChecked(), confDir+"/continue_playing.conf");
    QString filename = confDir+"/flags.conf";
    QFile f(filename);
    f.open( QIODevice::WriteOnly );
    QTextStream outstream(&f);
    outstream << repeat;
    outstream << single;
    outstream << randome;
    f.close();
}

void MainWindow::loadConfiguration() {
    QString line;
    QFile fGeo(confDir+"/geometry.conf");
    if (!fGeo.open(QIODevice::ReadOnly | QIODevice::Text))
        cout << "fail";
    else {
        QTextStream in2(&fGeo);
        QString line, tmp;
        QList<int> li;
        while (!in2.atEnd()) {
            line = in2.readAll();
            for(int i = 0;i < line.count();i++) {
                if(line.at(i) == '\n') {
                    li.append(tmp.toInt());
                    tmp = "";
                }
                else tmp.append(line.at(i));
            }
    }
        window()->setGeometry(li[2],li[3],li[1],li[0]);
    }
    QFile fPls(confDir+"/playlist.conf");
    if (!fPls.open(QIODevice::ReadOnly | QIODevice::Text))
        cout << "fail";
    else {
        QTextStream in1(&fPls);
        QString line, tmp;
        while (!in1.atEnd()) {
            line = in1.readAll();
            for(int i = 0; i < line.count();i++) {
                if(line.at(i) == '\n') {
                    files.append(tmp);
                    tmp = "";
                }
                else tmp.append(line.at(i));
            }
        }
        addToPlaylist(files);
        fPls.close();
    }
    if(readFromFile(confDir+"/lang.conf") != "err")
        language = readFromFile(confDir+"/lang.conf");
    if(readFromFile(confDir+"/volume.conf") != "err") {
        line = readFromFile(confDir+"/volume.conf");
        ui->dialVolume->setValue(line.toInt());
    }
    if(readFromFile(confDir+"/nexttrack.conf") != "err") {
        line = readFromFile(confDir+"/nexttrack.conf");
        nowSelected = line.toInt();
        if(files.count() != 0) playlist->setCurrentIndex(line.toInt());
    }
    if(readFromFile(confDir+"/continue_playing.conf") != "err") {
        line = readFromFile(confDir+"/continue_playing.conf");
        ui->actionAuto_pause_when_closed->setChecked(line.toInt());
    }
    if(readFromFile(confDir+"/state.conf") != "err") {
        line = readFromFile(confDir+"/state.conf");
        if(line.toInt() == 1) {
            if(ui->actionAuto_pause_when_closed->isChecked()) plr->pauseMusic();
            else plr->playMusic();
        }
        if(line.toInt() == 2) plr->pauseMusic();
    }
    if(readFromFile(confDir+"/currenttabtext.conf") != "err") {
        ui->A->setTabText(currentTab, readFromFile(confDir+"/currenttabtext.conf"));
    }
    if(readFromFile(confDir+"/flags.conf") != "err") {
        line = readFromFile(confDir+"/flags.conf");
        if(line == "100") ui->checkBox_repeat->setChecked(true);
        if(line == "010") ui->checkBox_single->setChecked(true);
        if(line == "001") ui->checkBox_random->setChecked(true);
    }
    if(readFromFile(confDir+"/mute.conf") != "err") {
        line = readFromFile(confDir+"/mute.conf");
        if(line.toInt() > 0) ui->radio_mute->setChecked(true);
        }
    if(readFromFile(confDir+"/position.conf") != "err") {
        line = readFromFile(confDir+"/position.conf");
        plr->setPosition(line.toInt());
    }
    if(language == "RU") on_action_triggered();
    else on_actionEnglish_triggered();
    watchStatusBar();
        //ui->label_3->setText(QString::number(files.count()));
}

void MainWindow::recursiveAddFolder(QStringList *out, QString path) {
    QFileInfo finfo(path);
    if (finfo.isDir()) {
        QDir dirs(path);
        QList<QString> entrys = dirs.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
        while(!entrys.isEmpty()) {
            recursiveAddFolder(out, path+"/"+entrys.takeFirst());
        }
    }
    else {
            QString suf = finfo.suffix();
            if (suf == "mp3" || suf == "flac" || suf == "wav" || suf == "ogg"  || suf == "3ga")
                out->append(path);
    }
}

void MainWindow::saveToFile(int var, QString filename) {
    QFile f( filename );
    f.open( QIODevice::WriteOnly );
    QTextStream outstream(&f);
    outstream << var;
    f.close();
}

void MainWindow::saveToFile(QStringList var, QString filename) {
    QFile f( filename );
    f.open( QIODevice::WriteOnly );
    QTextStream outstream(&f);
    for(int i = 0; i < var.count(); i++)
    outstream << var[i] << '\n';
    f.close();
}

void MainWindow::saveToFile(QString var, QString filename) {
    QFile f( filename );
    f.open( QIODevice::WriteOnly );
    QTextStream outstream(&f);
    outstream << var;
    f.close();
}

void MainWindow::saveToFile(QRect var, QString filename) {
    QFile f( filename );
    f.open( QIODevice::WriteOnly );
    QTextStream outstream(&f);
    outstream << var.height() << '\n' << var.width() << '\n' << var.x() << '\n' << var.y() << '\n';
    f.close();
}

QString MainWindow::readFromFile(QString filename) {
    QFile f(filename);
    QString line;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return "err";
    else {
        QTextStream in(&f);
        line = in.readAll();
        f.close();
    }
    return line;
}

void MainWindow::addToPlaylist(QStringList files) {
    QList<QMediaContent> new_content;
    for(int i = 0; i < files.count(); i++) {
        new_content.push_back(QUrl::fromLocalFile(files[i]));
        QFileInfo fi(files[i]);
        ui->listWidget->addItem(fi.fileName());
        dir = fi.path();
    }
    playlist->addMedia(new_content);
}

void MainWindow::on_action_triggered()
{
    language = "RU";
    //ui->labelDuration->setText("");
    ui->dialVolume->setToolTip("Вращать барабан");
    ui->labelGreeting->setText("ДОБРО ПОЖАЛОВАТЬ ВЪ САМОВАР, ТОВАРИЩ");
    ui->labelGreeting->setToolTip("Слушайте музыку с наслаждением");
    ui->labelVolume->setText("Громкость");
    ui->radio_mute->setText("Без звука");
    ui->checkBox_random->setText("Случайная");
    ui->checkBox_random->setToolTip("Случайный выбор песни");
    ui->checkBox_repeat->setText("Повтор");
    ui->checkBox_repeat->setToolTip("Повтор всего списка");
    ui->checkBox_single->setText("Одиночная");
    ui->checkBox_single->setToolTip("Повтор одной песни");
    ui->deleteCurrentTrack->setText(" Удалить песню");
    ui->deleteCurrentTrack->setToolTip("Убрать выбранную дорожку из списка");
    ui->button_stop->setToolTip("Остановить (F10)");
    ui->button_play_prev->setToolTip("Предыдущая песня (F11)");
    ui->button_play_next->setToolTip("Следующая песня (F12)");
    ui->menuDonate->setTitle("Настройки");
    ui->menuOptions->setTitle("Список");
    ui->groupBox->setTitle("Управление");
    ui->actionAdd_directory_s->setText("Добавить папку");
    ui->action_add_files->setText("Добавить файл(ы)");
    ui->actionSave_playlist->setText("Сохранить список");
    ui->actionOpen_playlist->setText("Открыть список");
    ui->actionClear_playlist->setText("Очистить список");
    ui->actionRemove_duplicates->setText("Удалить повторы");
    ui->actionExit->setText("Выйти");
    ui->actionAdd_directory_s->setText("Добавить папку");
    ui->action_200->setText("Помочь материально");
    ui->menuLanguage->setTitle("Язык");
    ui->actionAuto_pause_when_closed->setText("Остановить при выходе");
    ui->label->setText("Загружено");
    ui->label_4->setText("Текущая позиция");
    ui->label_6->setText("из");
    watchStatusBar();
    watchStatus();
    window()->setLocale(QLocale::Russian);
}

void MainWindow::on_actionEnglish_triggered()
{
    language = "EN";
    //ui->labelDuration->setText("");
    ui->dialVolume->setToolTip("Keep spinning the wheel");
    ui->labelGreeting->setText("Welcome to Samowar Music Player(beta)");
    ui->labelGreeting->setToolTip("Listen to music with flavour");
    ui->labelVolume->setText("Volume");
    ui->radio_mute->setText("Mute");
    ui->checkBox_random->setText("Random");
    ui->checkBox_random->setToolTip("Toggle random playback");
    ui->checkBox_repeat->setText("Repeat");
    ui->checkBox_repeat->setToolTip("Toggle playlist repeat");
    ui->checkBox_single->setText("Single");
    ui->checkBox_single->setToolTip("Toggle single song repeat");
    ui->deleteCurrentTrack->setText(" Delete current");
    ui->deleteCurrentTrack->setToolTip("Remove selected track from playlist");
    ui->button_stop->setToolTip("Stop playing (F10)");
    ui->button_play_prev->setToolTip("Previous track (F11)");
    ui->button_play_next->setToolTip("Next track (F12)");
    ui->menuDonate->setTitle("Options");
    ui->menuOptions->setTitle("Playlist");
    ui->groupBox->setTitle("Controls");
    ui->actionAdd_directory_s->setText("Add folder");
    ui->action_add_files->setText("Add file(s)");
    ui->actionSave_playlist->setText("Save playlist");
    ui->actionOpen_playlist->setText("Open playlist");
    ui->actionClear_playlist->setText("Clear playlist");
    ui->actionRemove_duplicates->setText("Remove duplicates");
    ui->actionExit->setText("Quit");
    ui->action_200->setText("Contribute");
    ui->menuLanguage->setTitle("Language");
    ui->actionAuto_pause_when_closed->setText("Auto pause when close");
    ui->label->setText("Loaded");
    ui->label_4->setText("Track position is");
    ui->label_6->setText("out of");
    watchStatusBar();
    watchStatus();
    window()->setLocale(QLocale::English);
}

void MainWindow::setVariables() {
    dir = QDir::homePath();
    QApplication::setApplicationName("Samowar Music Player");
    QApplication::setApplicationVersion("v2.7.61b");
    versionRu = "2.7.61б";
    iconRu = new QIcon(iconsDir+"/ru.png");
    ui->action->setIcon(*iconRu);
    iconEn = new QIcon(iconsDir+"/en.png");
    ui->actionEnglish->setIcon(*iconEn);
    iconLanguage = new QIcon(iconsDir+"/flags.png");
    ui->menuLanguage->setIcon(*iconLanguage);
    iconSavePlaylist = new QIcon(iconsDir+"/submenu-save-playlist.png");
    ui->actionSave_playlist->setIcon(*iconSavePlaylist);
    iconOpenPlaylist = new QIcon(iconsDir+"/submenu-open-playlist.png");
    ui->actionOpen_playlist->setIcon(*iconOpenPlaylist);
    iconRemoveDuplicates = new QIcon(iconsDir+"/submenu-remove-duplicates.png");
    ui->actionRemove_duplicates->setIcon(*iconRemoveDuplicates);
    iconExit = new QIcon(iconsDir+"/submenu-exit.png");
    ui->actionExit->setIcon(*iconExit);
    icon200 = new QIcon(iconsDir+"/submenu-200.png");
    ui->action_200->setIcon(*icon200);
    iconAddTrack = new QIcon(iconsDir+"/submenu-add-track.png");
    ui->action_add_files->setIcon(*iconAddTrack);
    iconAddFolder = new QIcon(iconsDir+"/submenu-add-folder.png");
    ui->actionAdd_directory_s->setIcon(*iconAddFolder);
    iconClearPls = new QIcon(iconsDir+"/brush-big.png");
    ui->actionClear_playlist->setIcon(*iconClearPls);
    iconDeleteCurrent = iconClearPls;
    ui->deleteCurrentTrack->setIcon(*iconDeleteCurrent);
    window()->setWindowTitle(QApplication::applicationName()+" "+QApplication::applicationVersion());
    //ui->button_play->setStyleSheet("QPushButton { border-image: url(/usr/share/samowar/icons/media-play.png); } QPushButton::hover { border-image: url(/usr/share/samowar/icons/media-play-hover.png); } QPushButton::pressed { border-image: url(/usr/share/samowar/icons/media-play.png); }");
    ui->currentTrack_progressBar->setVisible(0);
}


