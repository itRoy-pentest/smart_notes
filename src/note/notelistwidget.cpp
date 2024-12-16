#include "notelistwidget.h"
#include "ui_notelistwidget.h"

NoteListWidget::NoteListWidget(QWidget *parent): QWidget(parent), ui(new Ui::NoteListWidget)
{
    ui->setupUi(this);

    //connect(ui->noteList, &QListWidget::itemSelectionChanged, this, &NoteListWidget::onItemSelectionChanged);
}

NoteListWidget::~NoteListWidget()
{
    delete ui;
}

// void NoteListWidget::addNote(const Note &note)
// {
//     auto* item = new QListWidgetItem();
//     ui->noteList->insertItem(0, item);

//     setupNoteItem(note, item);
// }

// void NoteListWidget::removeChangedNote()
// {
//     auto* currentItem = ui->noteList->currentItem();
//     if(currentItem != nullptr)
//     {
//         delete currentItem;
//     }
// }

// void NoteListWidget::updateCurrentNote(const Note &note)
// {
//     if(ui->noteList->currentRow() != 0)
//     {
//         moveCurrentItemToTop(note);
//     }
// }

// int NoteListWidget::currentNoteId()
// {
//     auto* currentItem = ui->noteList->currentItem();
//     int noteId = currentItem->data(Qt::UserRole).toInt();
//     return noteId;
// }

// void NoteListWidget::onItemSelectionChanged()
// {
//     auto* currentItem = ui->noteList->currentItem();
//     if(currentItem)
//     {
//         int id = currentNoteId();

//         emit selectNoteChanged(id);
//     }
// }

// void NoteListWidget::moveCurrentItemToTop(const Note &note)
// {
//     blockSignals(true);
//     auto* item = ui->noteList->takeItem(ui->noteList->currentRow());

//     ui->noteList->insertItem(0, item);
//     setupNoteItem(note, item);
//     blockSignals(false);
// }

// void NoteListWidget::setupNoteItem(const Note& note, QListWidgetItem* item)
// {
//     item->setText(note.title);             // setText должен быть вызван на item
//     item->setData(Qt::UserRole, note.id);  // setData должен быть вызван на item
//     ui->noteList->setCurrentItem(item);    // setCurrentItem на QListWidget, но передаем item
// }





