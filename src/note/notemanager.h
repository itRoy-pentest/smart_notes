#pragma once

#include <QObject>
#include <QList>
#include <QTextDocument>
#include <memory>
#include <vector>
#include <cstddef> // Для size_t
#include "note.h"

class NoteManager : public QObject
{
    Q_OBJECT

public:
    explicit NoteManager(QObject *parent = nullptr);
    ~NoteManager();

    void createNewNote();
    void removeNote(int id);
    void renameNote(int id, const QString &newTitle);
    const Note& note(int id) const;
    size_t count() const;

    QTextDocument* noteDocument(int id) const;
    std::vector<std::reference_wrapper<Note>> noteCollection();

signals:
    void newNoteCreated(int id);
    void noteContentChanged(int id);

public slots:
    void onNoteContentChanged(int id);

private:
    QList<std::unique_ptr<Note>> notes;
    int nextNoteId();

    QTextDocument* createTextDocumentForNote(const Note& note);
};