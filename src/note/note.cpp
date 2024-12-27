#include "note.h"
#include "ui_note.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>

Note::Note(QWidget *parent) : QDialog(parent), ui(new Ui::Note)
{
    ui->setupUi(this);

    // Подключаем слоты к кнопкам
    connect(ui->addNote, &QPushButton::clicked, this, &Note::addNote);
    connect(ui->removeNote, &QPushButton::clicked, this, &Note::removeNote);
}

Note::~Note()
{
    delete ui;
}

void Note::addNote()
{
    QString noteText = ui->noteTextEdit->toPlainText();

    if (noteText.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Заметка не может быть пустой.");

        return;
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyy-MM-dd HH:mm:ss");

    modelNote.setIdNote(notes.size());
    modelNote.setTitle(noteText);
    modelNote.setDescription(noteText);  // Можете задать описание как текст заметки
    modelNote.setDate(timestamp);

    // Добавляем заметку в список
    listModelNote.append(modelNote);

    // Создаем QListWidgetItem для отображения новой заметки
    QListWidgetItem *noteItem = new QListWidgetItem();

    // Формируем текст для отображения заметки (можно изменить формат)
    QString displayText = modelNote.getTitle() + "\n" + modelNote.getDate();

    // Устанавливаем текст заметки в item
    noteItem->setText(displayText);

    // Добавляем новый item в QListWidget
    ui->noteListWidget->addItem(noteItem);

    // Очищаем поле ввода после добавления заметки
    ui->noteTextEdit->clear();
}

void Note::removeNote()
{
    // Получаем текущий выбранный элемент из списка
    QListWidgetItem *selectedItem = ui->noteListWidget->currentItem();
    if (selectedItem) {
        // Удаляем элемент из списка
        delete selectedItem;

        // Удаляем заметку из модели
        int index = ui->noteListWidget->row(selectedItem);
        notes.removeAt(index);
    }
    else
    {
        QMessageBox::warning(this, "Ошибка", "Выберите заметку для удаления.");
    }
}
