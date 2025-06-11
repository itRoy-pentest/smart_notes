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
#include <QListWidgetItem>
#include <QMenu>         // Для контекстного меню
#include <QAction>       // Для действий меню
#include <QKeyEvent>     // Для обработки событий нажатия клавиш
#include <QGuiApplication> // Для QGuiApplication::primaryScreen()->grabWindow() или других глобальных позиций


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Подключаем сигналы к слотам
    connect(ui->addNote, &QPushButton::clicked, this, &MainWindow::createNewNote);
    connect(ui->addFolder, &QPushButton::clicked, this, &MainWindow::createNewDir);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::on_tabWidget_tabCloseRequested);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::onListItemClicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::on_tabWidget_currentChanged);
    
    // НОВОЕ: Включаем контекстное меню для QListWidget
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    // НОВОЕ: Подключаем кнопку сортировки
    connect(ui->changeSortButton, &QPushButton::clicked, this, &MainWindow::toggleSortOrder);

    // Убедимся, что корневая папка существует
    QDir rootDir("autosave");
    if (!rootDir.exists()) {
        rootDir.mkpath(".");
    }

    m_currentDirectory = rootDir.absolutePath(); // Инициализируем m_currentDirectory абсолютным путем к autosave
    m_currentSortOrder = QDir::Name | QDir::DirsFirst; // ИНИЦИАЛИЗАЦИЯ: Сортировка по имени, папки вначале
    loadItemsFromDirectory(m_currentDirectory); // Загружаем содержимое корневой директории

    // Устанавливаем начальный фокус на список
    ui->listWidget->setFocus();
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
    qDebug() << "Loading items from directory:" << path << "with sort order:" << m_currentSortOrder;
    ui->listWidget->clear();

    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    dir.setSorting(m_currentSortOrder | QDir::DirsFirst); // Всегда держим папки вначале

    QDir autosaveRoot("autosave");
    if (dir.absolutePath() != autosaveRoot.absolutePath()) {
        QListWidgetItem *backItem = new QListWidgetItem("..");
        backItem->setData(Qt::UserRole, QVariant(".."));
        ui->listWidget->addItem(backItem);
    }

    QFileInfoList entries = dir.entryInfoList();
    for (const QFileInfo &entry : entries) {
        if (entry.fileName().startsWith(".")) continue;

        QListWidgetItem *item = new QListWidgetItem();
        if (entry.isDir()) {
            item->setText(entry.fileName());
            item->setIcon(QIcon(":/icon/addFolder.png")); // ИСПОЛЬЗУЕМ ВАШУ ИКОНКУ addFolder.png
            item->setData(Qt::UserRole, QVariant("folder"));
        } else if (entry.isFile() && entry.suffix() == "md") {
            item->setText(entry.baseName());
            item->setIcon(QIcon(":/icon/addNote.png"));   // ИСПОЛЬЗУЕМ ВАШУ ИКОНКУ addNote.png
            item->setData(Qt::UserRole, QVariant("note"));
            item->setData(Qt::UserRole + 1, entry.absoluteFilePath());
        } else {
            delete item;
            continue;
        }
        ui->listWidget->addItem(item);
    }
    m_currentDirectory = path;
    qDebug() << "Current directory set to:" << m_currentDirectory;

    // Устанавливаем фокус на список после загрузки элементов
    ui->listWidget->setFocus();
}

void MainWindow::onListItemClicked(QListWidgetItem *item)
{
    QString type = item->data(Qt::UserRole).toString();
    QString itemName = item->text();

    // Устанавливаем фокус на список при клике
    ui->listWidget->setFocus();

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
            QString title = note->getCurrentTitle();
            ui->tabWidget->setTabText(tabIndex, title.isEmpty() ? "" : title);
            // Обновляем название в списке слева
            for (int i = 0; i < ui->listWidget->count(); ++i) {
                QListWidgetItem *item = ui->listWidget->item(i);
                if (item->data(Qt::UserRole).toString() == "note" && item->data(Qt::UserRole + 1).toString() == filePath) {
                    item->setText(title.isEmpty() ? "" : title);
                    break;
                }
            }
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
    // Подбор уникального имени
    QDir dir(m_currentDirectory);
    QString base = "Untitled";
    QString displayTitle = base;
    int idx = 0;
    while (true) {
        bool exists = false;
        QString checkName = (idx == 0) ? base : base + " " + QString::number(idx);
        QString checkPath = dir.filePath(checkName + ".md");
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (item->data(Qt::UserRole).toString() == "note" && item->text() == checkName) {
                exists = true;
                break;
            }
        }
        if (QFileInfo::exists(checkPath) || exists) {
            ++idx;
            continue;
        }
        displayTitle = checkName;
        break;
    }
    QString fileBasedTitle = displayTitle;
    QString noteFileName = fileBasedTitle + ".md";
    QString fullPath = dir.filePath(noteFileName);

    Note *newNote = new Note(this);
    newNote->setInitialNoteContent(fullPath, displayTitle, "");

    connect(newNote, &Note::noteRenamed, this, &MainWindow::handleNoteRenamed);
    connect(newNote, &Note::noteClosed, this, &MainWindow::handleNoteClosed);
    connect(newNote, &Note::noteContentChanged, this, [this, newNote]() {
        QString currentNotePath = newNote->getCurrentFilePath();
        int tabIndex = m_noteTabIndices.value(currentNotePath, -1);
        if (tabIndex != -1) {
            QString title = newNote->getCurrentTitle();
            ui->tabWidget->setTabText(tabIndex, title.isEmpty() ? "" : title);
            // Обновляем название в списке слева
            for (int i = 0; i < ui->listWidget->count(); ++i) {
                QListWidgetItem *item = ui->listWidget->item(i);
                if (item->data(Qt::UserRole).toString() == "note" && item->data(Qt::UserRole + 1).toString() == currentNotePath) {
                    item->setText(title.isEmpty() ? "" : title);
                    break;
                }
            }
        }
    });

    int tabIndex = ui->tabWidget->addTab(newNote->getNoteWidget(), displayTitle);
    ui->tabWidget->setCurrentIndex(tabIndex);

    m_openNotes.insert(fullPath, newNote);
    m_noteTabIndices.insert(fullPath, tabIndex);

    QListWidgetItem *item = new QListWidgetItem(displayTitle);
    item->setIcon(QIcon(":/icon/addNote.png")); // ИСПОЛЬЗУЕМ ВАШУ ИКОНКУ addNote.png
    item->setData(Qt::UserRole, QVariant("note"));
    item->setData(Qt::UserRole + 1, fullPath);
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

        // Обновляем текст вкладки (без .md)
        ui->tabWidget->setTabText(tabIndex, QFileInfo(newPath).baseName());

        // Обновляем элемент в QListWidget (без .md)
        bool itemUpdated = false;
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (item->data(Qt::UserRole).toString() == "note" && item->data(Qt::UserRole + 1).toString() == oldPath) {
                item->setText(QFileInfo(newPath).baseName()); // Обновляем текст без .md
                item->setData(Qt::UserRole + 1, newPath); // Обновляем путь в данных элемента
                itemUpdated = true;
                break;
            }
        }
        
        if (QFileInfo(oldPath).dir().path() != QFileInfo(newPath).dir().path() || !itemUpdated) {
            qDebug() << "Note moved to a different directory or not found in current view. Reloading listWidget.";
            loadItemsFromDirectory(m_currentDirectory);
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
    QString dirName = QInputDialog::getText(this, "Создать новую папку", "Введите имя новой папки");
    if (dirName.isEmpty()) {
        return;
    }

    QString fullPath = QDir(m_currentDirectory).filePath(dirName);
    QDir newDir(fullPath);

    if (newDir.exists()) {
        QMessageBox::warning(this, "Ошибка", "Папка с таким именем уже существует!");
        return;
    }

    if (newDir.mkpath(".")) {
        QListWidgetItem *item = new QListWidgetItem(dirName);
        item->setIcon(QIcon(":/icon/addFolder.png")); // ИСПОЛЬЗУЕМ ВАШУ ИКОНКУ addFolder.png
        item->setData(Qt::UserRole, QVariant("folder"));
        ui->listWidget->addItem(item);
        
        ui->listWidget->sortItems(Qt::AscendingOrder);

    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать папку. Проверьте права доступа.");
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

// НОВЫЙ СЛОТ: Удаление выбранных заметок или папок
void MainWindow::deleteSelectedItems()
{
    QListWidgetItem *selectedItem = ui->listWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "Удаление", "Пожалуйста, выберите заметку или папку для удаления.");
        return;
    }

    QString type = selectedItem->data(Qt::UserRole).toString();
    QString itemName = selectedItem->text();

    QString fullPath;
    if (type == "note") {
        fullPath = selectedItem->data(Qt::UserRole + 1).toString(); // Получаем полный путь для заметки
    } else if (type == "folder") {
        fullPath = QDir(m_currentDirectory).filePath(itemName); // Формируем полный путь для папки
    } else {
        // Это может быть ".." или неизвестный тип, ничего не делаем
        return;
    }

    if (type == "note") {
        QFile file(fullPath);
        if (file.remove()) {
            // Если заметка была открыта во вкладке, закрываем ее
            if (m_openNotes.contains(fullPath)) {
                int tabIndex = m_noteTabIndices.value(fullPath, -1);
                if (tabIndex != -1) {
                    // Удаляем вкладку и объект Note
                    on_tabWidget_tabCloseRequested(tabIndex);
                }
            }
            // Удаляем элемент из QListWidget
            delete ui->listWidget->takeItem(ui->listWidget->row(selectedItem));
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить заметку. Ошибка: " + file.errorString());
        }
    } else if (type == "folder") {
        QDir dir(fullPath);
        
        // Спрашиваем, если папка не пуста
        if (!dir.isEmpty() && QMessageBox::question(this, "Подтверждение удаления папки",
                                                "Папка \"" + itemName + "\" не пуста. Вы хотите удалить ее и все ее содержимое рекурсивно?",
                                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            return; // Пользователь отменил рекурсивное удаление
        }

        if (dir.removeRecursively()) {
            // Закрываем все заметки, которые были открыты из этой папки или ее подпапок
            QList<QString> notesToClose;
            for(const QString& notePath : m_openNotes.keys()) {
                if (notePath.startsWith(fullPath)) {
                    notesToClose.append(notePath);
                }
            }
            for(const QString& notePath : notesToClose) {
                // Ищем индекс вкладки по пути заметки
                int tabIndex = -1;
                for (auto it = m_noteTabIndices.constBegin(); it != m_noteTabIndices.constEnd(); ++it) {
                    if (it.key() == notePath) {
                        tabIndex = it.value();
                        break;
                    }
                }
                if (tabIndex != -1) {
                    on_tabWidget_tabCloseRequested(tabIndex); // Используем уже существующий слот для закрытия
                }
            }

            // Удаляем элемент из QListWidget
            delete ui->listWidget->takeItem(ui->listWidget->row(selectedItem));
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить папку рекурсивно. Проверьте права доступа.");
        }
    }
}

// Переопределение keyPressEvent для обработки клавиши Delete
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        qDebug() << "Delete key pressed. List widget has focus:" << ui->listWidget->hasFocus();
        // Проверяем, находится ли фокус на listWidget
        if (ui->listWidget->hasFocus()) {
            deleteSelectedItems(); // Вызываем слот для удаления
        } else {
            QMainWindow::keyPressEvent(event); // Передаем событие базовому классу для других виджетов
        }
    } else {
        QMainWindow::keyPressEvent(event); // Передаем событие базовому классу для других клавиш
    }
}

void MainWindow::showContextMenu(const QPoint &) {}
void MainWindow::renameSelectedItem() {}
void MainWindow::deleteItemFromContextMenu() {}
void MainWindow::toggleSortOrder() {}

