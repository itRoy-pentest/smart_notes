#include "notemanager.h"


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

    note.id = id;
    note.title = "New Note";
    note.lastModified = QDateTime::currentDateTime();
    textDocument = createNewTextDocument(note);

    emit newNoteCreated(id);
}

void NoteManager::removeNote(int id)
{
    notes.erase(id);

    if(notes.empty())
        createNewNote();
}

int nextNoteId()
{
    static int Id = 0;
    return ++Id;
}

void NoteManager::onNoteContentChanged(int id)
{
    // Реализуйте обработку изменения содержимого заметки
}

void NoteManager::readNotes()
{
    // Реализуйте логику чтения заметок, например, из файла
}

void NoteManager::writeNotes()
{
    // Реализуйте логику записи заметок
}


std::unique_ptr<QTextDocument> NoteManager::createNewTextDocument(const Note& note)
{
    // Пример создания нового текстового документа
    std::unique_ptr<QTextDocument> doc = std::make_unique<QTextDocument>();
    // Здесь можно добавить логику инициализации документа, например, из содержимого заметки
    return doc;
}


