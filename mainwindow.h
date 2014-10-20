#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button_play_clicked();

    void on_button_stop_clicked();

    void on_button_pause_clicked();

    void on_action_200_triggered();

    void on_actionExit_triggered();

    void on_action_add_files_triggered();

    void on_volumeSlider_valueChanged(int value);

    void on_button_mute_toggled(bool checked);

    void on_button_play_prev_clicked();

    void on_button_play_next_clicked();

    void on_button_clearPlaylist_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_horizontalSlider_sliderMoved(int position);

    void on_deleteCurrentTrack_clicked();

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
