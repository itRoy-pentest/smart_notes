#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_NewNote_clicked()
{
    hide();
    note.setMinimumSize(1920, 1080);  // Устанавливаем минимальный размер
    note.setStyleSheet("background-color:gray;");
    note.setAutoFillBackground( true );

    note.setModal(true);
    note.show();
}

