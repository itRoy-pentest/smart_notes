#include "notelistwidget.h"
#include "ui_notelistwidget.h"

NoteListWidget::NoteListWidget(QWidget *parent) : QWidget(parent), ui(new Ui::NoteListWidget)
{
    ui->setupUi(this);
}

NoteListWidget::~NoteListWidget()
{
    delete ui;
}

// Set title note
void NoteListWidget::setTextTitle(const QString &text)
{
    ui->nameNote->setText(text); // Устанавливаем заголовок
}

// Set date note
void NoteListWidget::setTextDate(const QString &text)
{
    ui->dateNote->setText(text); // Устанавливаем дату
}
