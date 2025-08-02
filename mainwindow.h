#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QFileDialog>
#include <QGraphicsItem>
#include <windows.h>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, const QString& openfileName = QString());
    ~MainWindow();

    QString fileName;
    bool pause;
    bool repeat;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_volume_slider_valueChanged(int value);
    void on_time_slider_sliderMoved(int position);
    void on_pb_pause_clicked();
    void on_pb_frameBack_clicked();
    void on_pb_frameForward_clicked();
    void on_pb_mute_clicked();
    void on_time_slider_sliderPressed();
    void on_time_slider_sliderReleased();
    void on_pb_file_clicked();
    void on_pb_back_clicked();
    void on_pb_forward_clicked();
    void up_clicked();
    void down_clicked();
    void on_pb_repeat_clicked();
    void on_speed_slider_sliderMoved(int position);
    void on_pb_fullscreen_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QVideoWidget *videoOutput;
    QAudioOutput *audioOutput;
};

#endif // MAINWINDOW_H
