#include "note.h"
#include "ui_note.h"
#include <qpushbutton.h>
#include "notemanager.h"
#include "notelistwidget.h"
#include <QMessageBox>

Note::Note(NoteManager& manager, QWidget *parent): QDialog(parent), ui(new Ui::Note), noteManager(&manager)
{

    ui->setupUi(this);

    makeConnection();
}

Note::Note(QWidget *parent)
    : QDialog(parent), ui(new Ui::Note), noteManager(nullptr)  // Инициализируем указатель как nullptr
{
    ui->setupUi(this);
    makeConnection();
}

Note::~Note()
{
    delete ui;
}

void Note::onAddNoteclicked()
{
    noteManager->createNewNote();
}

void Note::onRemoveNoteclicked()
{
    removeNote(ui->noteListWidget->currentNoteId());
}

void Note::onNewNoteCreated(int id)
{
    addNoteToList(noteManager->note(id));
}

void Note::onNoteContentChanged(int id)
{
    ui->noteListWidget->updateCurrentNote(noteManager->note(id));
}

void Note::onSelectedNoteChanged(int id)
{
    auto* document = noteManager->noteDocument(id);
    if(document)
    {
        ui->textEdit->setDocument(document);
        auto cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);
    }
}

void Note::onRemoveNote(int id)
{
    removeNote(id);
}



void Note::mainNote(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    NoteManager manager;
    Note note{manager};

}

void Note::addNoteToList(const Note &note)
{
    ui->noteListWidget->addNote(note);
}

void Note::removeNote(int id)
{
    QString noteTitle = noteManager->note(id).title;

    auto pressedBtn = QMessageBox::information(this, "Remove note?",
                                               QString("Are you sure you to remove %0?").arg(noteTitle),
                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if(pressedBtn == QMessageBox::Yes)
    {
        if(noteManager->count() == 1)
            ui->textEdit->setDocument(nullptr);

        ui->noteListWidget->removeChangedNote();
        noteManager->removeNote(id);
    }
}

void Note::init()
{
    auto noteList = noteManager->noteCollection();
    std::sort(noteList.begin(), noteList.end(),
              [](const Note& left, const Note& right)
              {
                    return left.lastModified < right.lastModified;
              });

    for(auto& note: noteList)
    {
        addNoteToList(note);
    }
}

void Note::makeConnection()
{
    connect(ui->AddNote, &QPushButton::clicked, this, &Note::onAddNoteclicked);
    connect(ui->RemoveNote, &QPushButton::clicked, this, &Note::onRemoveNoteclicked);

    if (noteManager && this) {
        connect(noteManager, &NoteManager::newNoteCreated, this, &Note::onNewNoteCreated);
        connect(noteManager, &NoteManager::noteContentChanged, this, &Note::onNoteContentChanged);
    }


    connect(ui->noteListWidget, &NoteListWidget::selectNoteChanged, this, &Note::onSelectedNoteChanged);
}

