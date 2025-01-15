#pragma once

#include <QDialog>
#include <QDateTime>
#include <QTextDocument>
#include <QList>  // Для хранения списка заметок
#include <QString>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QTextEdit>
#include <folder/folder.h>

namespace Ui
{
    class Note;
}

class Note : public QDialog
{
    Q_OBJECT

    void loadFromFile(const QString& filePath);

public:
    explicit Note(QWidget *parent = nullptr);
    explicit Note(Ui::Note *ui) : ui(ui) {}
    ~Note();

    void autoNoteSave();

    /// Setters ///

    // Set title text
    void setTitle(const QString &title);

    // Set main text
    void setText(const QString &text);

    /// Getters ///

    // Get title text
    QTextEdit* getTitleEdit();

    // Get main text
    QTextEdit* getTextEdit();

    // Get note widget (widget have title and text)
    QWidget* getNoteWidget();

    void saveToFile();

private slots:

private:
    Ui::Note *ui;
    Folder folder;

    QString setTitleText;
    QString setTextMain;

    QTextEdit* titleText;
    QTextEdit* textMain;

    int currentNoteIndex = -1; // Current note index
};
