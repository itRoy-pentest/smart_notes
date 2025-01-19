#include "note.h"
#include "ui_note.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>


Note::Note(QWidget *parent) : QDialog(parent), ui(new Ui::Note)
{
    ui->setupUi(this);

    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(1000);  // Интервал в миллисекундах (1 ms)

    // Подключаем таймер к слоту для автоматического сохранения
    connect(autoSaveTimer, &QTimer::timeout, this, &Note::autoNoteSave);

    // Запускаем таймер
    autoSaveTimer->start();

    disconnect(autoSaveTimer, &QTimer::timeout, this, &Note::autoNoteSave);
    autoSaveTimer->stop();
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

    QString path = "autosave/";
    QString fileName = path + titleEdit + ".md";

    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << textEdit;
    }
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
