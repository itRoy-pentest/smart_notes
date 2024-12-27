#pragma once

#include <QDialog>
#include <QDateTime>
#include <QTextDocument>
#include <QList>  // Для хранения списка заметок
#include <QString>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include "models/modelsNote.h" // Если у вас есть модель заметок
#include "notelistwidget.h"

namespace Ui
{
    class Note;
}

class Note : public QDialog
{
    Q_OBJECT

public:
    explicit Note(QWidget *parent = nullptr);
    ~Note();

private slots:
    void addNote();
    void removeNote();

private:
    Ui::Note *ui;
    QList<QString> notes;  // Список для хранения текста заметок
    NoteListWidget noteListWidget;
    ModelNote modelNote;
    QList<ModelNote> listModelNote;
};
