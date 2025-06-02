#include "notemanager.h"
#include <QTextDocument>
#include <vector>       // Для std::vector
#include <functional>   // Для std::reference_wrapper

NoteManager::NoteManager(QObject *parent) : QObject(parent)
{
    // Инициализация при необходимости
}

NoteManager::~NoteManager()
{
    notes.clear();
}

void NoteManager::createNewNote() 
{
    // Реализация создания заметки
}

void NoteManager::removeNote(int id) 
{
    // Реализация удаления заметки
}

void NoteManager::renameNote(int id, const QString &newTitle) 
{
    // Реализация переименования заметки
}

const Note& NoteManager::note(int id) const 
{
    // Реализация получения заметки
    static Note dummy; // Временная заглушка
    return dummy;
}

size_t NoteManager::count() const 
{ 
    return notes.size(); 
}

QTextDocument* NoteManager::noteDocument(int id) const 
{
    // Реализация получения документа
    return nullptr; // Временная заглушка
}

std::vector<std::reference_wrapper<Note>> NoteManager::noteCollection() 
{
    // Реализация получения коллекции
    return {}; // Временная заглушка
}

int NoteManager::nextNoteId() 
{
    // Реализация генерации ID
    return 0; // Временная заглушка
}

void NoteManager::onNoteContentChanged(int id)
{
    emit noteContentChanged(id);
}

QTextDocument* NoteManager::createTextDocumentForNote(const Note& note)
{
    QTextDocument* document = new QTextDocument();
    document->setPlainText(note.getCurrentText());
    return document;
}