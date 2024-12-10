#include "notelistwidget.h"
#include "ui_notelistwidget.h"

NoteListWidget::NoteListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NoteListWidget)
{
    ui->setupUi(this);
}

NoteListWidget::~NoteListWidget()
{
    delete ui;
}
