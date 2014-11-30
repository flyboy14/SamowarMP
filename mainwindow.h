#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QMediaPlaylist>

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

    void atTrackEnd();

    void setSliderPosition();

    void watchSelectedTrack();

    void watchNextTrack();

    void watchStatus();

    void watchPlaying();

    void on_button_play_clicked();

    void on_button_stop_clicked();

    void on_action_200_triggered();

    void on_actionExit_triggered();

    void on_action_add_files_triggered();

    void on_volumeSlider_valueChanged(int value);

    void on_radio_mute_toggled(bool checked);

    void on_button_play_prev_clicked();

    void on_button_play_next_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_horizontalSlider_sliderMoved(int position);

    void on_deleteCurrentTrack_clicked();

    void on_horizontalSlider_sliderPressed();

    void on_horizontalSlider_sliderReleased();

    void on_checkBox_repeat_toggled(bool checked);

    void on_checkBox_random_toggled(bool checked);

    void on_checkBox_single_toggled(bool checked);

    void on_actionClear_playlist_triggered();

    void on_button_play_released();

    void on_volumeSlider_sliderPressed();

    void on_volumeSlider_sliderReleased();

    void on_actionToggle_debug_output_triggered();

    void on_buttonDebugClear_clicked();

protected:
    Ui::MainWindow *ui;
    QMediaPlaylist *playlist;
    QIcon *iconPlay, *iconPause, *iconStop, *iconPlayPrev, *iconPlayNext, *iconClearPls, *iconSamowar;
    //void dragEnterEvent(QDragEnterEvent *event);
    //void dropEvent(QDropEvent *event);

};

#endif // MAINWINDOW_H
