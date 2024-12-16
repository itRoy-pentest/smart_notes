#pragma once

#include <QDialog>
#include <QDateTime>
#include <QTextDocument> // Для работы с QTextDocument


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

    int id;                    // Идентификатор заметки
    QString title;             // Заголовок заметки
    QDateTime lastModified;    // Время последнего изменения
    QString content;           // Содержимое заметки

private slots:
    // // Обработчики сигналов от UI
    // void onAddNoteclicked();
    // void onRemoveNoteclicked();

    // // Обработчики сигналов от NoteManager
    // void onNewNoteCreated(int id);
    // void onNoteContentChanged(int id);

    // // Обработчики сигналов от NoteListWidget
    // void onSelectedNoteChanged(int id);
    // void onRemoveNote(int id);

    // // Для реализации главной функции
    // void mainNote(int argc, char *argv[]);

private:
    // void addNoteToList(const Note& note);
    // void removeNote(int id);

    // void init();
    // void makeConnection();

private:
    Ui::Note *ui;
};
