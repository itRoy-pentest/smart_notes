#include "notemanager.h"
#include "note.h"
#include <QTextDocument>
#include <QSignalMapper>

NoteManager::NoteManager(QObject *parent): QObject{parent}
{
    mapChangedSignalToNotId = new QSignalMapper(this);
    connect(mapChangedSignalToNotId, &QSignalMapper::mappedInt, this, &NoteManager::onNoteContentChanged);

    readNotes();

    if(notes.size() == 0)
        createNewNote();
}

NoteManager::~NoteManager()
{
    writeNotes();
}

void NoteManager::createNewNote()
{
    int id = nextNoteId();
    auto& [note, textDocument] = notes[id];
    notes.id = id;
    note.title = "New Note";
    note.lastModified = QDateTime::currentDataTime();
    textDocument = createNewTextDocument(note);

    emit newNoteCreated(id);
}

void NoteManager::removeNote(int id)
{
    notes.erase(id);
}

std::unique_ptr<QTextDocument> NoteManager::createNewTextDocument(const Note &note)
{
}

int nextNoteId()
{
    static int Id = 0;
    return ++Id;
}

