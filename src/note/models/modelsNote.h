#pragma once

#include <QString>

class ModelNote
{
    // Описание заметки
    int idNote; // Айди
    QString title; // Название
    QString description; // Описание
    QString date; // Дата создания

public:
    ModelNote();

    ModelNote(int id, QString title, QString description, QString date);

    // Возвращаем айди, название, описание и дату заметки
    int getIdNote() const;
    QString getTitle() const;
    QString getDescription() const;
    QString getDate() const;

    // Устанавливаем айди, название, описание и дату заметки
    void setIdNote(int newIdNote);
    void setTitle(const QString& newTitle);
    void setDescription(const QString& newDescription);
    void setDate(const QString& newDate);
};

