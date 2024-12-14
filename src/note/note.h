#pragma once

#include <QDialog>
#include <QDateTime>


class NoteManager;

namespace Ui
{
    class Note;
}

class Note : public QDialog
{
    Q_OBJECT

public:
    explicit Note(NoteManager& manager, QWidget *parent = nullptr);
    Note(QWidget *parent = nullptr);
    ~Note();

    int id;                    // Идентификатор заметки
    QString title;             // Заголовок заметки
    QDateTime lastModified;    // Время последнего изменения
    QString content;           // Содержимое заметки

private slots:
    //handle signal from ui
    void onAddNoteclicked();
    void onRemoveNoteclicked();

    //handle NoteManager signal
    void onNewNoteCreated(int id);
    void onNoteContentChanged(int id);

    //handle NoteListWidget signal
    void onSelectedNoteChanged(int id);
    void onRemoveNote(int id);

    // Для реализации всего написаного
    void mainNote(int argc, char *argv[]);

private:
    void addNoteToList(const Note& note);
    void removeNote(int id);

    void init();
    void makeConnection();

    NoteManager* noteManager;


private:
    Ui::Note *ui;


};
