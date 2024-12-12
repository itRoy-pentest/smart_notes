#pragma once

#include <QObject>
#include <unordered_map>
#include <utility>
#include <memory>
#include <vector>

class QTextDocument;
class QSignalMapper;

struct Note;

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

    signals:

    private:
        std::unordered_map<int, std::pair<Note, std::unique_ptr<QTextDocument>>> notes;
};
