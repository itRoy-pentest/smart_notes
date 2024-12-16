#pragma once

#include <QWidget>
#include <QListWidget>
#include "note.h"

namespace Ui
{
    class NoteListWidget;
}

class NoteListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteListWidget(QWidget *parent = nullptr);
    ~NoteListWidget();

//     void addNote(const Note& note);
//     void removeChangedNote();
//     void updateCurrentNote(const Note& note);

//     int currentNoteId();

// signals:
//     void selectNoteChanged(int id);

// private:
//     void onItemSelectionChanged();
//     void moveCurrentItemToTop(const Note& note);
//     void setupNoteItem(const Note& note, QListWidgetItem* item);

private:
    Ui::NoteListWidget *ui;
};

