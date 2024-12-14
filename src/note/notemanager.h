#pragma once

#include <QObject>
#include "note.h"
#include <QTextDocument>
#include <QSignalMapper>
#include <QString>
#include <QDateTime>

class QTextDocument;
class QSignalMapper;

int nextNoteId();


class NoteManager : public QObject
{
    Q_OBJECT

    public:
        explicit NoteManager(QObject *parent = nullptr);
        ~NoteManager();

        void createNewNote();
        void removeNote(int id);
        void renameNote(int id, const QString& newTitle);

        const Note& note(int id) const;

        QTextDocument* noteDocument(int id) const;
        std::vector<std::reference_wrapper<Note>> noteCollection();

        size_t count() const;

    public slots:
        void newNoteCreated(int id);
        void noteContentChanged(int id);

    private:
        QVector<QString> listNotes;

        void onNoteContentChanged(int id);

        void readNotes();
        void writeNotes();
        std::unique_ptr<QTextDocument> createNewTextDocument(const Note& note);

    private:
        std::unordered_map<int, std::pair<Note, std::unique_ptr<QTextDocument>>> notes;

        QSignalMapper* mapChangedSignalToNotId = nullptr;
};
