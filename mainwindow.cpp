#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const QString& openfileName)
    : QMainWindow(parent)
    , fileName(openfileName)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("icons\\icon_play.ico"));
    ui->centralwidget->setStyle(QStyleFactory::create("Fusion"));
    pause = false;
    repeat = false;

    ui->lbl_sp2->setText("");
    ui->lbl_end->setText("");
    ui->pb_pause->setText("");
    ui->pb_back->setText("");
    ui->pb_forward->setText("");
    ui->pb_mute->setText("");
    ui->pb_frameBack->setText("");
    ui->pb_frameForward->setText("");
    ui->pb_file->setText("");
    ui->pb_repeat->setText("");
    ui->pb_fullscreen->setText("");

    ui->pb_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pb_back->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pb_forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pb_mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->pb_frameBack->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->pb_frameForward->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->pb_file->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->pb_repeat->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pb_fullscreen->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));

    ui->pb_pause->setToolTip("Play / Pause");
    ui->pb_back->setToolTip("5 sec backward");
    ui->pb_forward->setToolTip("5 sec forward");
    ui->pb_mute->setToolTip("Mute");
    ui->pb_frameBack->setToolTip("1 frame backward");
    ui->pb_frameForward->setToolTip("1 frame forward");
    ui->pb_file->setToolTip("Open file");
    ui->pb_repeat->setToolTip("Repeat");
    ui->pb_fullscreen->setToolTip("Fullscreen");

    ui->pb_pause->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");
    ui->pb_back->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");
    ui->pb_forward->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");
    ui->pb_mute->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");
    ui->pb_frameBack->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");
    ui->pb_frameForward->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");
    ui->pb_file->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");
    ui->pb_repeat->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");
    ui->pb_fullscreen->setStyleSheet("QToolTip { color: black; background-color: #ffffdc; }");

    player = new QMediaPlayer();
    videoOutput = new QVideoWidget();
    videoOutput->installEventFilter(this);
    QShortcut *fullscreenShortcut_F11 = new QShortcut(Qt::Key_F11, this);
    QShortcut *fullscreenShortcut_Left = new QShortcut(Qt::Key_Left, this);
    QShortcut *fullscreenShortcut_Right = new QShortcut(Qt::Key_Right, this);
    QShortcut *fullscreenShortcut_Return = new QShortcut(Qt::Key_Return, this);
    QShortcut *fullscreenShortcut_Up = new QShortcut(Qt::Key_Up, this);
    QShortcut *fullscreenShortcut_Down = new QShortcut(Qt::Key_Down, this);
    connect(fullscreenShortcut_F11, &QShortcut::activated, this, &MainWindow::on_pb_fullscreen_clicked);
    connect(fullscreenShortcut_Left, &QShortcut::activated, this, &MainWindow::on_pb_back_clicked);
    connect(fullscreenShortcut_Right, &QShortcut::activated, this, &MainWindow::on_pb_forward_clicked);
    connect(fullscreenShortcut_Return, &QShortcut::activated, this, &MainWindow::on_pb_pause_clicked);
    connect(fullscreenShortcut_Up, &QShortcut::activated, this, &MainWindow::up_clicked);
    connect(fullscreenShortcut_Down, &QShortcut::activated, this, &MainWindow::down_clicked);

    audioOutput = new QAudioOutput();
    player->setVideoOutput(videoOutput);
    player->setAudioOutput(audioOutput);
    ui->horizontalLayout->addWidget(videoOutput);
    videoOutput->show();

    if (!fileName.isEmpty()) {
        player->setSource(QUrl::fromLocalFile(fileName));
        player->play();
    } else {
        fileName = QFileDialog::getOpenFileName(this, "Выберите Файл", "", "Видео файлы "
                                                                           "(*.mp4 *.avi *.mov *.mkv *.flv *.wmv *.webm *.mpeg *.mpg);;"
                                                                           "Все файлы (*)");
        if (!fileName.isEmpty()) {
            player->setSource(QUrl::fromLocalFile(fileName));
            player->play();
        }
    }

    player->setSource(QUrl(fileName));
    QStringList name = fileName.split("/");
    this->setWindowTitle("Smpl Play — " + name.last());
    player->play();

    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 duration_ms) {
        ui->time_slider->setMaximum(duration_ms / 10);
    });

    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 pos_ms) {
        if (!ui->time_slider->isSliderDown()) {
            ui->time_slider->setValue(pos_ms / 10);
        }
    });

    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 pos_ms) {
        if (!ui->time_slider->isSliderDown()) {
            ui->time_slider->setValue(pos_ms / 10);
        }

        qint64 duration_ms = player->duration();

        auto formatTime = [](qint64 ms) -> QString {
            qint64 seconds = (ms / 1000) % 60;
            qint64 minutes = (ms / 60000) % 60;
            qint64 milliseconds = ms % 1000;
            return QString("%1:%2.%3")
                .arg(minutes, 2, 10, QLatin1Char('0'))
                .arg(seconds, 2, 10, QLatin1Char('0'))
                .arg(milliseconds, 3, 10, QLatin1Char('0'));
        };

        ui->lbl_end->setText(
            QString("%1 | %2")
                .arg(formatTime(pos_ms))
                .arg(formatTime(duration_ms))
            );
    });

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && repeat) {
            player->setPosition(0);
            player->play();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_volume_slider_valueChanged(int value)
{
    audioOutput->setVolume(static_cast<float>(value) / 100.0);
}


void MainWindow::on_time_slider_sliderMoved(int position)
{
    player->setPosition(position * 10);
}


void MainWindow::on_pb_pause_clicked()
{
    if (player->isPlaying()){
        pause = true;
        player->pause();
        ui->pb_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    } else {
        pause = false;
        player->play();
        ui->pb_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
}


void MainWindow::on_pb_frameBack_clicked()
{
    qint64 currentPos = player->position();
    QMediaMetaData metaData = player->metaData();
    qreal fps = metaData.value(QMediaMetaData::VideoFrameRate).toReal();
    if (fps <= 0.0) {
        fps = 30.0;
    }
    qint64 frameDurationMs = qRound(1000.0 / fps);
    qint64 newPos = qMax<qint64>(0, currentPos - frameDurationMs);
    player->setPosition(newPos);
}


void MainWindow::on_pb_frameForward_clicked()
{
    qint64 currentPos = player->position();
    QMediaMetaData metaData = player->metaData();
    qreal fps = metaData.value(QMediaMetaData::VideoFrameRate).toReal();
    if (fps <= 0.0) {
        fps = 30.0;
    }
    qint64 frameDurationMs = qRound(1000.0 / fps);
    qint64 newPos = currentPos + frameDurationMs;
    player->setPosition(newPos);
}


void MainWindow::on_pb_mute_clicked()
{
    if (audioOutput->isMuted()){
        audioOutput->setMuted(false);
        ui->pb_mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    } else {
        audioOutput->setMuted(true);
        ui->pb_mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
}


void MainWindow::on_time_slider_sliderPressed()
{
    if (pause == false){
        player->pause();
    }
}


void MainWindow::on_time_slider_sliderReleased()
{
    if (pause == false){
        player->play();
    }
}

void MainWindow::on_pb_file_clicked()
{
    player->pause();
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите Файл", "", "Видео файлы "
                                                                               "(*.mp4 *.avi *.mov *.mkv *.flv *.wmv *.webm *.mpeg *.mpg);;"
                                                                               "Все файлы (*)");
    if (fileName == ""){
        player->play();
    } else {
        player->stop();
        player->setSource(QUrl(fileName));
        QStringList name = fileName.split("/");
        this->setWindowTitle("Smpl Play — " + name.last());
        player->play();
        ui->pb_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
}


void MainWindow::on_pb_back_clicked()
{
    player->setPosition(qMax(player->position() - 5000, 0));
}


void MainWindow::on_pb_forward_clicked()
{
    if (!player->isAvailable()) return;
    player->setPosition(qMin(player->position() + 5000, player->duration()));
}

void MainWindow::up_clicked(){
    if ((ui->volume_slider->value() + 10) > 100){
        ui->volume_slider->setValue(100);
    } else {
        ui->volume_slider->setValue(ui->volume_slider->value() + 10);
    }
    audioOutput->setVolume(static_cast<float>(ui->volume_slider->value()) / 100);
}

void MainWindow::down_clicked(){
    if ((ui->volume_slider->value() - 10) < 0){
        ui->volume_slider->setValue(0);
    } else {
        ui->volume_slider->setValue(ui->volume_slider->value() - 10);
    }
    audioOutput->setVolume(static_cast<float>(ui->volume_slider->value()) / 100);
}

void MainWindow::on_pb_repeat_clicked()
{
    repeat = ui->pb_repeat->isChecked();
}


void MainWindow::on_speed_slider_sliderMoved(int position)
{
    qreal speed = static_cast<qreal>(position) / 100;
    player->setPlaybackRate(speed);
    ui->lbl_speed->setText(QString("×%1").arg(speed, 0, 'f', 2));
}


void MainWindow::on_pb_fullscreen_clicked()
{
    if (videoOutput->isFullScreen()) {
        videoOutput->setFullScreen(false);
        this->showNormal();
    } else {
        videoOutput->setFullScreen(true);
    }
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == videoOutput && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_F11) {
            on_pb_fullscreen_clicked();
            return true;
        } else if (keyEvent->key() == Qt::Key_Left) {
            on_pb_back_clicked();
            return true;
        } else if (keyEvent->key() == Qt::Key_Right) {
            on_pb_forward_clicked();
            return true;
        } else if (keyEvent->key() == Qt::Key_Return) {
            on_pb_pause_clicked();
            return true;
        } else if (keyEvent->key() == Qt::Key_Up) {
            up_clicked();
            return true;
        } else if (keyEvent->key() == Qt::Key_Down) {
            down_clicked();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
