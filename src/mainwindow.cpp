#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), folder(), note()
{
    ui->setupUi(this);

    // Подключаем сигнал к слоту создания новой кнопки (createNewNoteButton)
    connect(ui->addNote, SIGNAL(clicked()), this, SLOT(createNewNote()));

    // QIcon noteIcon(":/icon/addNote.png");
    // ui->addNote->setIcon(noteIcon);
    // ui->addNote->setIconSize(QSize(32, 32));

    // QIcon folderIcon(":/icon/addFolder.png");
    // ui->addFolder->setIcon(folderIcon);
    // ui->addFolder->setIconSize(QSize(32, 32));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createNewNote()
{
    QListWidgetItem *item = new QListWidgetItem("Untitled");
    ui->listWidget->addItem(item);
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
}

