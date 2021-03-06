#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QDir>
#include <iostream>
using namespace std;

namespace Ui {
    class MainWindow;
    class samoplayer;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void fill_listwidget_from_playlist();

    void fill_listwidget_from_stringlist(QStringList l);

    QStringList playlist_to_qstringlist(QMediaPlaylist *pls);

    void watchInternalDD();

    void watchStatusBar();

    void recursiveAddFolder(QStringList *out, QString path);

    void add_files_from_behind();

    void watchPlaylistChanges();

    void mySliderValueChanged(int newPos);

    void progress();

    void changeCurrentTab();

    void atTrackEnd();

    void setSliderPosition();

    void watchNextTrack();

    void watchStatus();

    void watchPlaying();

    void on_button_play_clicked();

    void on_button_stop_clicked();

    void on_action_200_triggered();

    void on_actionExit_triggered();

    void on_action_add_files_triggered();

    void on_radio_mute_toggled();

    void on_button_play_prev_clicked();

    void on_button_play_next_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_horizontalSlider_sliderMoved(int position);

    void on_deleteCurrentTrack_clicked();

    void on_horizontalSlider_sliderPressed();

    void on_horizontalSlider_sliderReleased();

    void on_checkBox_repeat_clicked();

    void on_checkBox_random_clicked();

    void on_checkBox_single_clicked();

    void on_actionClear_playlist_triggered();

    void on_dialVolume_valueChanged(int value);

    void on_dialVolume_sliderPressed();

    void on_dialVolume_sliderReleased();

    void on_actionAdd_directory_s_triggered();

    void on_actionSave_playlist_triggered();

    void on_actionRemove_duplicates_triggered();

    void on_actionOpen_playlist_triggered();

    void on_actionShuffle_entire_playlist_triggered();

    void on_actionJump_to_random_song_triggered();

    void saveToFile(int var, QString name);

    void saveToFile(QStringList var, QString name);

    void saveToFile(QString var, QString filename);

    void saveToFile(QRect var, QString filename);

    void addToPlaylist(QStringList files);

    QString readFromFile(QString filename);

    void on_action_triggered();

    void on_actionEnglish_triggered();

    void on_shuffleButton_clicked();

    void on_pushButton_clicked();

public slots:
    void saveConfiguration();
    void setVariables();
    void loadConfiguration();

protected:
    Ui::MainWindow *ui;
    QMediaPlaylist *playlist, *tmp_playlist;
    QIcon *iconEn, *iconRu, *iconLanguage, *iconInfo,*iconClearPls, *iconDeleteCurrent, *iconSamowar, *iconSavePlaylist, *iconOpenPlaylist, *iconRemoveDuplicates, *iconExit, *iconDebugOutput, *icon200, *iconAddTrack, *iconAddFolder, *iconMenuPlaylist, *iconMenuOptions;
};

#endif // MAINWINDOW_H
