#include "note.h"
#include "ui_note.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>

Note::Note(QWidget *parent) : QDialog(parent), ui(new Ui::Note)
{
    ui->setupUi(this);
}

Note::~Note()
{
    delete ui;
}

QTextEdit *Note::getTitleEdit()
{
    return ui->titleText;
}

QTextEdit *Note::getTextEdit()
{
    return ui->textEdit;
}

QWidget *Note::getNoteWidget()
{
    return ui->noteWidget;
}

QString Note::setText(const QString &title)
{

}

QString Note::setTitle(const QString &text)
{

}
