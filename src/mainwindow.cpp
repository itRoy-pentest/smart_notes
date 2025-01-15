#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>
#include <QDir>
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), folder(), note()
{
    ui->setupUi(this);

    titleText = new QTextEdit(this);
    textMain = new QTextEdit(this);

    // Подключаем сигнал к слоту создания новой кнопки (createNewNoteButton)
    connect(ui->addNote, SIGNAL(clicked()), this, SLOT(createNewNote()));
    connect(ui->addFolder, SIGNAL(clicked()), this, SLOT(createNewDir()));

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
    currentNoteIndex = ui->tabWidget->addTab(noteWidget, "Untitled");

    ui->tabWidget->setCurrentIndex(currentNoteIndex);

    // Устанавливаем фокус на заголовок
    note.getTitleEdit()->setFocus();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);

    currentNoteIndex = index;
}


void MainWindow::createNewDir()
{
    QString dirName = QInputDialog::getText(this, "Create new folder", "Enter the name folder");

    QDir dir("autosave");  // Создаем объект для работы с папкой autosave
    if (dir.exists(dirName))
    {
        // Если папка с таким именем уже существует
        QMessageBox::warning(this, "Error", "A folder with that name already exists!");

        return;  // Завершаем выполнение метода
    }

    if (dir.mkpath(dirName))
    {
        // Если папка была успешно создана
        QMessageBox::information(this, "Success", "Folder successfully created!");
    }
    else
    {
        // Если возникла ошибка при создании
        QMessageBox::critical(this, "Error", "Cannot create folder");
    }

    ui->listWidget->addItem(dirName);  // Добавляем название папки в список
}

