#pragma once

#include <QWidget>
#include <QDateTime>
#include <QTextDocument>
#include <QList>  // Для хранения списка заметок
#include <QString>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QTextEdit>
#include <folder/folder.h>
#include <QTimer>

namespace Ui
{
    class Note;
}

class Note : public QWidget
{
    Q_OBJECT

public:
    explicit Note(QWidget *parent = nullptr);
    ~Note();

    void setNoteData(const QString &filePath); // Для загрузки существующих заметок
    void setInitialNoteContent(const QString &filePath, const QString &initialTitle, const QString &initialText); // Для новых заметок
    
    // Методы для обновления UI, чтобы избежать путаницы с внутренними строками
    void setNoteTitleInUI(const QString &title);
    void setNoteTextInUI(const QString &text);

    QTextEdit* getTitleEdit();
    QTextEdit* getTextEdit();
    QWidget* getNoteWidget();

    QString getCurrentFilePath() const { return m_currentFilePath; }
    QString getCurrentTitle() const;
    QString getCurrentText() const;

signals:
    void noteContentChanged(const QString &filePath); // Сигнал об изменении контента (текста или заголовка)
    void noteRenamed(const QString &oldPath, const QString &newPath); // Сигнал о переименовании файла
    void noteClosed(const QString &filePath); // Сигнал о закрытии заметки (вкладки)

private slots:
    void autoNoteSave();
    void onTitleTextChanged(); // Слот для обработки изменения заголовка
    void onTextTextChanged();  // Слот для обработки изменения основного текста

private:
    Ui::Note *ui;
    QString m_currentFilePath; // Путь к файлу заметки на диске
    QString m_initialTitle;    // Используется для отслеживания изменения заголовка для переименования
    QTextEdit* titleText;
    QTextEdit* textMain;
    QTimer* autoSaveTimer;
};
