#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>
#include <QDir>
#include <QInputDialog>
#include <QFileInfo> // Для работы с информацией о файле
#include <QMessageBox> // Для сообщений пользователю
#include <QSet> // Для отслеживания открытых файлов/папок
#include <QDateTime> // Для генерации уникальных имен
#include <QDebug> // Для отладочных сообщений


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Подключаем сигналы к слотам
    connect(ui->addNote, &QPushButton::clicked, this, &MainWindow::createNewNote);
    connect(ui->addFolder, &QPushButton::clicked, this, &MainWindow::createNewDir);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::on_tabWidget_tabCloseRequested);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::onListItemClicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::on_tabWidget_currentChanged); // Подключаем новый слот

    // Убедимся, что корневая папка существует
    QDir rootDir("autosave");
    if (!rootDir.exists()) {
        rootDir.mkpath(".");
    }

    m_currentDirectory = rootDir.absolutePath(); // Инициализируем m_currentDirectory абсолютным путем к autosave
    loadItemsFromDirectory(m_currentDirectory); // Загружаем содержимое корневой директории
}

MainWindow::~MainWindow()
{
    // Очищаем открытые заметки
    for (Note* note : m_openNotes.values()) {
        delete note;
    }
    delete ui;
}

void MainWindow::loadItemsFromDirectory(const QString &path)
{
    qDebug() << "Loading items from directory:" << path;
    ui->listWidget->clear(); // Очищаем текущий список

    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    dir.setSorting(QDir::Name | QDir::DirsFirst); // Сортируем папки первыми

    // Добавляем ".." для навигации вверх, только если мы не в корневой директории "autosave"
    QDir autosaveRoot("autosave");
    if (dir.absolutePath() != autosaveRoot.absolutePath()) {
        QListWidgetItem *backItem = new QListWidgetItem("..");
        backItem->setData(Qt::UserRole, QVariant("..")); // Специальный флаг для ".."
        ui->listWidget->addItem(backItem);
    }

    QFileInfoList entries = dir.entryInfoList();
    for (const QFileInfo &entry : entries) {
        if (entry.fileName().startsWith(".")) continue; // Игнорируем скрытые файлы/папки

        QListWidgetItem *item = new QListWidgetItem(entry.fileName());
        if (entry.isDir()) {
            item->setIcon(QIcon::fromTheme("folder")); // Иконка для папки
            item->setData(Qt::UserRole, QVariant("folder")); // Тип: папка
        } else if (entry.isFile() && entry.suffix() == "md") {
            item->setIcon(QIcon::fromTheme("text-x-markdown")); // Иконка для заметки
            item->setData(Qt::UserRole, QVariant("note")); // Тип: заметка
            item->setData(Qt::UserRole + 1, entry.absoluteFilePath()); // Сохраняем абсолютный путь
        } else {
            delete item; // Игнорируем другие файлы
            continue;
        }
        ui->listWidget->addItem(item);
    }
    m_currentDirectory = path; // Обновляем текущую директорию после загрузки
    qDebug() << "Current directory set to:" << m_currentDirectory;
}

void MainWindow::onListItemClicked(QListWidgetItem *item)
{
    QString type = item->data(Qt::UserRole).toString();
    QString itemName = item->text();

    if (type == "folder") {
        QString newPath = QDir(m_currentDirectory).filePath(itemName);
        loadItemsFromDirectory(newPath);
    } else if (itemName == "..") {
        QDir currentDir(m_currentDirectory);
        if (currentDir.cdUp()) {
            loadItemsFromDirectory(currentDir.absolutePath()); // Используем absolutePath после cdUp
        }
    } else if (type == "note") {
        QString filePath = item->data(Qt::UserRole + 1).toString();
        openNoteInTab(filePath);
    }
}

void MainWindow::openNoteInTab(const QString &filePath)
{
    if (m_openNotes.contains(filePath)) {
        // Заметка уже открыта, просто переключаемся на ее вкладку
        int tabIndex = m_noteTabIndices.value(filePath, -1);
        if (tabIndex != -1) {
            ui->tabWidget->setCurrentIndex(tabIndex);
        }
        return;
    }

    Note *note = new Note(this);
    note->setNoteData(filePath); // Загружаем данные из файла

    // Подключаем сигналы от заметки
    connect(note, &Note::noteRenamed, this, &MainWindow::handleNoteRenamed);
    connect(note, &Note::noteClosed, this, &MainWindow::handleNoteClosed);
    // Подключаем изменение заголовка для обновления текста вкладки
    connect(note, &Note::noteContentChanged, this, [this, note, filePath]() {
        int tabIndex = m_noteTabIndices.value(filePath, -1); // Используем исходный путь для поиска вкладки
        if (tabIndex != -1) {
            ui->tabWidget->setTabText(tabIndex, note->getCurrentTitle());
        }
    });

    int tabIndex = ui->tabWidget->addTab(note->getNoteWidget(), QFileInfo(filePath).baseName());
    ui->tabWidget->setCurrentIndex(tabIndex);

    m_openNotes.insert(filePath, note);
    m_noteTabIndices.insert(filePath, tabIndex);

    note->getTitleEdit()->setFocus();
}

int MainWindow::getTabIndexForNote(const QString &filePath)
{
    return m_noteTabIndices.value(filePath, -1);
}

void MainWindow::createNewNote()
{
    // Генерируем уникальное имя файла на основе временной метки, чтобы избежать перезаписи
    QString fileBasedTitle = "Untitled-" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString displayTitle = "Untitled"; // Это то, что пользователь увидит изначально

    QString noteFileName = fileBasedTitle + ".md"; // Имя файла на диске будет уникальным
    QString fullPath = QDir(m_currentDirectory).filePath(noteFileName);

    Note *newNote = new Note(this);
    newNote->setInitialNoteContent(fullPath, displayTitle, ""); // Передаем отображаемое название

    connect(newNote, &Note::noteRenamed, this, &MainWindow::handleNoteRenamed);
    connect(newNote, &Note::noteClosed, this, &MainWindow::handleNoteClosed);
    // Подключаем изменение содержимого заметки для обновления текста вкладки
    connect(newNote, &Note::noteContentChanged, this, [this, newNote]() {
        // Чтобы найти индекс вкладки, нам нужно найти текущий путь заметки
        // в m_openNotes, а затем получить его индекс из m_noteTabIndices.
        QString currentNotePath = newNote->getCurrentFilePath();
        int tabIndex = m_noteTabIndices.value(currentNotePath, -1);
        
        if (tabIndex != -1) {
            ui->tabWidget->setTabText(tabIndex, newNote->getCurrentTitle());
        }
    });

    // Добавляем вкладку с отображаемым названием "Untitled"
    int tabIndex = ui->tabWidget->addTab(newNote->getNoteWidget(), displayTitle); 
    ui->tabWidget->setCurrentIndex(tabIndex);

    // Сохраняем в картах, используя уникальный путь к файлу
    m_openNotes.insert(fullPath, newNote);
    m_noteTabIndices.insert(fullPath, tabIndex);

    // Добавляем элемент в список с отображаемым названием "Untitled" и уникальным путем к файлу
    QListWidgetItem *item = new QListWidgetItem(displayTitle); 
    item->setIcon(QIcon::fromTheme("text-x-markdown"));
    item->setData(Qt::UserRole, QVariant("note"));
    item->setData(Qt::UserRole + 1, fullPath); // Храним уникальный путь к файлу в данных элемента
    ui->listWidget->addItem(item);

    newNote->getTitleEdit()->setFocus();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    QWidget *widget = ui->tabWidget->widget(index);
    if (widget) {
        // Ищем заметку по ее виджету
        QString filePathToRemove;
        for (auto it = m_openNotes.constBegin(); it != m_openNotes.constEnd(); ++it) {
            if (it.value()->getNoteWidget() == widget) {
                filePathToRemove = it.key();
                break;
            }
        }

        if (!filePathToRemove.isEmpty()) {
            Note* noteToClose = m_openNotes.take(filePathToRemove); // Удаляем из карты
            m_noteTabIndices.remove(filePathToRemove); // Удаляем из карты индексов вкладок
            
            // Важно: emit noteClosed перед delete noteToClose, чтобы сигнал успел пройти
            emit noteToClose->noteClosed(filePathToRemove); 
            delete noteToClose; // Удаляем объект заметки
        }
        ui->tabWidget->removeTab(index);
    }
}

void MainWindow::handleNoteRenamed(const QString &oldPath, const QString &newPath)
{
    qDebug() << "Handling note renamed from" << oldPath << "to" << newPath;
    // Обновляем карту m_openNotes с новым путем
    if (m_openNotes.contains(oldPath)) {
        Note* note = m_openNotes.take(oldPath);
        m_openNotes.insert(newPath, note);

        // Обновляем карту m_noteTabIndices
        int tabIndex = m_noteTabIndices.take(oldPath);
        m_noteTabIndices.insert(newPath, tabIndex);

        // Обновляем текст вкладки
        ui->tabWidget->setTabText(tabIndex, QFileInfo(newPath).baseName());

        // Обновляем элемент в QListWidget
        // Перезагрузка директории - самый безопасный способ обеспечить согласованность списка
        // с изменениями файловой системы, особенно если заметка перемещалась между папками.
        // Для простых переименований внутри одной папки достаточно обновить элемент.
        // Пока просто обновим элемент, если он находится в текущем представлении.
        bool itemUpdated = false;
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            // Проверяем, является ли это заметкой и совпадает ли ее сохраненный путь со старым путем
            if (item->data(Qt::UserRole).toString() == "note" && item->data(Qt::UserRole + 1).toString() == oldPath) {
                item->setText(QFileInfo(newPath).baseName());
                item->setData(Qt::UserRole + 1, newPath); // Важно: Обновляем сохраненный путь
                itemUpdated = true;
                break;
            }
        }
        if (!itemUpdated) {
            // Если переименованная заметка в данный момент не видна в QListWidget (например, она была перемещена в другую папку)
            // или QListWidget был уже очищен/перезагружен, возможно, потребуется обновление.
            // Пока просто логируем. Для сложных сценариев может потребоваться полная перезагрузка.
            qDebug() << "Renamed note not found in current listWidget view, path:" << oldPath;
        }

        // После переименования, желательно обновить виджет списка, ЕСЛИ переименование включало изменение директории.
        // Пока, если это просто переименование в той же директории, мы обновили элемент.
        // Если путь директории изменился, мы должны перезагрузить.
        if (QFileInfo(oldPath).dir().path() != QFileInfo(newPath).dir().path()) {
            qDebug() << "Note moved to a different directory. Reloading listWidget.";
            loadItemsFromDirectory(m_currentDirectory); // Перезагружаем для отражения изменений
        }
    }
}

void MainWindow::handleNoteClosed(const QString &filePath)
{
    // Этот слот можно использовать для дополнительной логики, если нужно что-то сделать
    // после закрытия заметки, например, очистить кеш или обновить состояние.
    // На данный момент основная логика удаления уже в on_tabWidget_tabCloseRequested.
    qDebug() << "Note closed (from signal):" << filePath;
}

void MainWindow::createNewDir()
{
    QString dirName = QInputDialog::getText(this, "Create new folder", "Enter the name for the new folder");
    if (dirName.isEmpty()) {
        return;
    }

    QString fullPath = QDir(m_currentDirectory).filePath(dirName);
    QDir newDir(fullPath);

    if (newDir.exists()) {
        QMessageBox::warning(this, "Error", "A folder with that name already exists!");
        return;
    }

    // Пытаемся создать директорию
    if (newDir.mkpath(".")) { // QDir::mkpath работает относительно объекта QDir, поэтому здесь нужен fullPath
        QMessageBox::information(this, "Success", "Folder successfully created!");
        
        // Добавляем новую папку в QListWidget, если мы находимся в родительской директории
        // Перезагрузка обычно безопаснее для изменений файловой системы, чтобы обеспечить согласованность
        loadItemsFromDirectory(m_currentDirectory); 

    } else {
        QMessageBox::critical(this, "Error", "Cannot create folder. Check permissions.");
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == -1) { // Если вкладок нет
        return;
    }

    QWidget* currentTabWidget = ui->tabWidget->widget(index);

    // Ищем соответствующий объект Note по его виджету
    for (Note* note : m_openNotes.values()) {
        if (note->getNoteWidget() == currentTabWidget) {
            // Устанавливаем фокус на основной текстовый редактор заметки
            note->getTextEdit()->setFocus();
            break;
        }
    }
}

