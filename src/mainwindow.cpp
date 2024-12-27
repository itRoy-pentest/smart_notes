#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), folder(), note()
{
    ui->setupUi(this);

    // Подключаем сигнал к слоту создания новой кнопки (createNewNoteButton)
    connect(ui->newNote, SIGNAL(clicked()), this, SLOT(createNewNoteButton()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createNewNoteButton()
{
    hide();  // Скрыть главное окно
    note.setMinimumSize(1920, 1080);  // Устанавливаем минимальный размер
    note.setStyleSheet("background-color:gray;");
    note.setAutoFillBackground(true);

    note.setModal(true);  // Сделать окно модальным
    note.show();  // Показать окно заметки
}
