#include "note.h"
#include "ui_note.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QWidget>


Note::Note(QWidget *parent) : QWidget(parent), ui(new Ui::Note)
{
    ui->setupUi(this);

    titleText = ui->titleText;
    textMain = ui->textEdit;

    m_currentFilePath = ""; // Изначально нет пути к файлу
    m_initialTitle = "";    // Нет начального заголовка

    // Подключаем изменения текста к слотам
    connect(titleText, &QTextEdit::textChanged, this, &Note::onTitleTextChanged);
    connect(textMain, &QTextEdit::textChanged, this, &Note::onTextTextChanged);

    // Настраиваем таймер автосохранения
    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(5000); // 5 секунд
    connect(autoSaveTimer, &QTimer::timeout, this, &Note::autoNoteSave);
    autoSaveTimer->start(); // Запускаем таймер
}

Note::~Note()
{
    autoSaveTimer->stop(); // Останавливаем таймер
    // Убедимся, что последнее сохранение произошло перед удалением, если заметка была изменена
    if (!m_currentFilePath.isEmpty() && (ui->titleText->document()->isModified() || ui->textEdit->document()->isModified())) {
        qDebug() << "Saving note before closing:" << m_currentFilePath;
        autoNoteSave();
    }
    delete ui;
}

// Загружает содержимое существующего файла заметки
void Note::setNoteData(const QString &filePath)
{
    m_currentFilePath = filePath;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        ui->textEdit->setText(content);
        
        QFileInfo fileInfo(filePath);
        QString title = fileInfo.baseName();
        ui->titleText->setText(title);
        m_initialTitle = title; // Устанавливаем начальный заголовок из имени файла
        qDebug() << "Note loaded from" << filePath;
        // Помечаем документ как неизмененный после загрузки
        ui->titleText->document()->setModified(false);
        ui->textEdit->document()->setModified(false);
    } else {
        qWarning() << "Could not open file for reading:" << filePath << ". Error:" << file.errorString();
        ui->titleText->clear();
        ui->textEdit->clear();
        m_initialTitle = "";
        m_currentFilePath = ""; // Очищаем путь, если файл не найден
    }
}

// Инициализирует новую заметку с заданным путем, заголовком и содержимым
void Note::setInitialNoteContent(const QString &filePath, const QString &initialTitle, const QString &initialText)
{
    m_currentFilePath = filePath;
    m_initialTitle = initialTitle;
    ui->titleText->setText(initialTitle);
    ui->textEdit->setText(initialText);
    qDebug() << "New note initialized with path:" << filePath;
    // Помечаем документ как измененный, чтобы автосохранение сработало
    ui->titleText->document()->setModified(true);
    ui->textEdit->document()->setModified(true);
    autoNoteSave(); // Выполняем первоначальное сохранение немедленно
}

void Note::setNoteTitleInUI(const QString &title)
{
    ui->titleText->setText(title);
    m_initialTitle = title;
    ui->titleText->document()->setModified(false); // Предполагаем, что это программное изменение, а не пользовательское
}

void Note::setNoteTextInUI(const QString &text)
{
    ui->textEdit->setText(text);
    ui->textEdit->document()->setModified(false); // Предполагаем программное изменение
}

QString Note::getCurrentTitle() const {
    return ui->titleText->toPlainText();
}

QString Note::getCurrentText() const {
    return ui->textEdit->toPlainText();
}

void Note::onTitleTextChanged()
{
    // Помечаем документ как измененный
    ui->titleText->document()->setModified(true);
    autoNoteSave(); // Запускаем логику сохранения, включая проверку переименования
    emit noteContentChanged(m_currentFilePath);
}

void Note::onTextTextChanged()
{
    // Помечаем документ как измененный
    ui->textEdit->document()->setModified(true);
    autoNoteSave(); // Запускаем логику сохранения
    emit noteContentChanged(m_currentFilePath);
}

void Note::autoNoteSave()
{
    // Сохраняем только если документ был изменен
    if (!ui->titleText->document()->isModified() && !ui->textEdit->document()->isModified()) {
        return; // Нет изменений для сохранения
    }

    QString currentTitle = ui->titleText->toPlainText();
    QString currentText = ui->textEdit->toPlainText();

    // Если m_currentFilePath пуст, это означает, что заметка не была должным образом инициализирована с путем к файлу.
    // Этого в идеале не должно происходить, если MainWindow правильно обрабатывает новые заметки.
    // В качестве запасного варианта, сохраняем во временный файл в корне autosave.
    if (m_currentFilePath.isEmpty()) {
        if (currentTitle.isEmpty()) {
            currentTitle = "Untitled-" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
        }
        QDir rootDir("autosave");
        if (!rootDir.exists()) rootDir.mkpath(".");
        m_currentFilePath = rootDir.filePath(currentTitle + ".md");
        m_initialTitle = currentTitle;
        qWarning() << "Note: m_currentFilePath was empty, setting to a fallback path:" << m_currentFilePath;
    }

    // --- Логика переименования ---
    QString oldFileName = QFileInfo(m_currentFilePath).baseName();
    // Пытаемся переименовать только если заголовок действительно изменился и новый заголовок не пуст
    if (currentTitle != oldFileName && !currentTitle.isEmpty()) {
        QString oldPath = m_currentFilePath;
        QString newPath = QFileInfo(m_currentFilePath).dir().filePath(currentTitle + ".md");

        QFile oldFile(oldPath);
        if (oldFile.rename(newPath)) {
            m_currentFilePath = newPath; // Обновляем внутренний путь
            m_initialTitle = currentTitle; // Обновляем начальный заголовок на новое имя
            emit noteRenamed(oldPath, newPath); // Сигнал для обновления UI
            qDebug() << "Note renamed from" << oldPath << "to" << newPath;
        } else {
            qWarning() << "Failed to rename note from" << oldPath << "to" << newPath << ". Error:" << oldFile.errorString();
            // Если переименование не удалось, мы, вероятно, не должны продолжать сохранять под новым именем.
            // Пока продолжим сохранять по старому пути, но это потенциальная проблема.
            // Лучшим решением было бы отменить изменение заголовка в UI или предотвратить переименование.
        }
    }

    // --- Логика сохранения содержимого ---
    // Убедимся, что директория для текущего пути к файлу существует
    QDir noteDir(QFileInfo(m_currentFilePath).dir().path());
    if (!noteDir.exists()) {
        if (!noteDir.mkpath(".")) {
            qWarning() << "Failed to create directory for note:" << noteDir.path() << ". Check permissions.";
            return; // Критическая ошибка, невозможно сохранить
        }
    }

    QFile file(m_currentFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << currentText;
        file.close();
        // Помечаем документы как неизмененные после успешного сохранения
        ui->titleText->document()->setModified(false);
        ui->textEdit->document()->setModified(false);
        qDebug() << "Note content saved to" << m_currentFilePath;
    } else {
        qWarning() << "Could not open file for writing:" << m_currentFilePath << ". Error:" << file.errorString();
    }
}

QTextEdit *Note::getTitleEdit()
{
    return ui->titleText;
}

QTextEdit *Note::getTextEdit()
{
    return ui->textEdit;
}

QWidget *Note::getNoteWidget()
{
    return ui->noteWidget;
}
