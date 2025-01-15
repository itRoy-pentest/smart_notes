#include "note.h"
#include "ui_note.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

Note::Note(QWidget *parent) : QDialog(parent), ui(new Ui::Note)
{
    ui->setupUi(this);
}

Note::~Note()
{
    delete ui;
}

void Note::autoNoteSave()
{


    QString titleEdit = titleText->toPlainText();
    QString textEdit = textMain->toPlainText();

    setTitle(titleEdit);
    setText(textEdit);


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

void Note::setText(const QString &title)
{
    setTextMain = title;
}

void Note::setTitle(const QString &text)
{
    setTitleText = text;
}

void Note::saveToFile()
{

}
