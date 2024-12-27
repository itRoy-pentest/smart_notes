#include "modelsNote.h"

ModelNote::ModelNote() {}

ModelNote::ModelNote(int id, QString title, QString description, QString date)
{
    this->idNote = id;
    this->title = title;
    this->description = description;
    this->date = date;
}

// Get id note
int ModelNote::getIdNote() const
{
    return idNote;
}

// Get title note
QString ModelNote::getTitle() const
{
    return title;
}

// Get description note
QString ModelNote::getDescription() const
{
    return description;
}

// Get date Note
QString ModelNote::getDate() const
{
    return date;
}

// Set id note
void ModelNote::setIdNote(int newIdNote)
{
    idNote = newIdNote;
}

// Set title note
void ModelNote::setTitle(const QString &newTitle)
{
    title = newTitle;
}

// Set description note
void ModelNote::setDescription(const QString &newDescription)
{
    description = newDescription;
}

// Set date note
void ModelNote::setDate(const QString &newDate)
{
    date = newDate;
}

