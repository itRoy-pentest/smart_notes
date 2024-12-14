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

void NoteManager::renameNote(int id, const QString &newTitle)
{
    //TODO
    Q_UNUSED(id)
    Q_UNUSED(newTitle)
}

const Note &NoteManager::note(int id) const
{
    return notes.at(id).first;
}

QTextDocument *NoteManager::noteDocument(int id) const
{
    auto found = notes.find(id);
    if(found != notes.end())
    {
        return found->second.second.get();
    }
    return nullptr;
}

std::vector<std::reference_wrapper<Note> > NoteManager::noteCollection()
{
    std::vector<std::reference_wrapper<Note>> out;

    for(auto& i : notes)
    {
        auto& [note, textDocument] = i.second;
        note.content = textDocument->toPlainText();
        out.push_back(note);
    }
    return out;
}

size_t NoteManager::count() const
{
    return notes.size();
}





void NoteManager::onNoteContentChanged(int id)
{
    notes.at(id).first.lastModified = QDateTime::currentDateTime();

    emit noteContentChanged(id);
}

void NoteManager::readNotes()
{
    // TODO
}

void NoteManager::writeNotes()
{
    // TODO
}


std::unique_ptr<QTextDocument> NoteManager::createNewTextDocument(const Note& note)
{
    auto textDocument = std::make_unique<QTextDocument>(note.content);
    connect(textDocument.get(), &QTextDocument::contentsChange,
            mapChangedSignalToNotId, qOverload<>(&QSignalMapper::map));
    mapChangedSignalToNotId->setMapping(textDocument.get(), note.id);
    return textDocument;  // не забудь вернуть объект
}


int nextNoteId()
{
    static int Id = 0;
    return ++Id;
}
