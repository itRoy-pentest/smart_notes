#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow MainWindow;

    MainWindow.setMinimumSize(1920, 1080);  // Устанавливаем минимальный размер

    MainWindow.setStyleSheet("background-color:gray;");
    MainWindow.setAutoFillBackground( true );

    MainWindow.show();

    return a.exec();
}
