#pragma once

#include <QObject>
#include <QList>
#include <QTextDocument>
#include <memory>
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

    QTextDocument* noteDocument(int id) const;  // Оставляем const, т.к. он не меняет состояние объекта
    std::vector<std::reference_wrapper<Note>> noteCollection();

signals:
    void newNoteCreated(int id);
    void noteContentChanged(int id);

public slots:
    void onNoteContentChanged(int id);  // Этот слот должен быть неконстантным

private:
    QList<std::unique_ptr<Note>> notes;
    int nextNoteId();

    QTextDocument* createTextDocumentForNote(const Note& note); // Убираем const
    // Этот метод также будет неконстантным
};
