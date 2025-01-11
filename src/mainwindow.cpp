#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), folder(), note()
{
    ui->setupUi(this);

    titleEdit = new QTextEdit(this);
    textEdit = new QTextEdit(this);

    // Подключаем сигнал к слоту создания новой кнопки (createNewNoteButton)
    connect(ui->addNote, SIGNAL(clicked()), this, SLOT(createNewNote()));
    connect(titleEdit, &QTextEdit::textChanged, this, &MainWindow::autoNoteSave);
    connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::autoNoteSave);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createNewNote()
{
    QListWidgetItem *item = new QListWidgetItem("Untitled");

    // Добавляем её в список заметок
    ui->listWidget->addItem(item);

    QWidget* noteWidget = note.getNoteWidget();

    // Добавляем новую вкладку
    noteIndex = ui->tabWidget->addTab(noteWidget, "Untitled");

    ui->tabWidget->setCurrentIndex(noteIndex);

    // Устанавливаем фокус на заголовок
    note.getTitleEdit()->setFocus();
}

void MainWindow::autoNoteSave()
{
    titleEdit->toPlainText();
    textEdit->toPlainText();

    ui->tabWidget->widget(noteIndex);
}

void MainWindow::tabWidgetClosed()
{
    ui->tabWidget->removeTab(noteIndex);
}





