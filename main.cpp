#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = QApplication::arguments();

    QString filePath;
    if (args.size() > 1) {
        QFileInfo fileInfo(args[1]);
        if (fileInfo.exists() && fileInfo.isFile()) {
            filePath = fileInfo.absoluteFilePath();
        }
    }

    MainWindow w(nullptr, filePath);
    w.show();
    return a.exec();
}
