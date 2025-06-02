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
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::on_tabWidget_currentChanged); // Подключаем новый слот
    // Теперь deleteItem (если кнопка есть в UI) не будет подключена напрямую,
    // удаление будет через контекстное меню или клавишу Delete.

    // НОВОЕ: Включаем контекстное меню для QListWidget
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

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
    ui->listWidget->clear(); // Это вызовет "моргание" при полной перезагрузке

    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    dir.setSorting(QDir::Name | QDir::DirsFirst); // Сортируем папки первыми

    QDir autosaveRoot("autosave");
    if (dir.absolutePath() != autosaveRoot.absolutePath()) {
        QListWidgetItem *backItem = new QListWidgetItem("..");
        backItem->setData(Qt::UserRole, QVariant(".."));
        ui->listWidget->addItem(backItem);
    }

    QFileInfoList entries = dir.entryInfoList();
    for (const QFileInfo &entry : entries) {
        if (entry.fileName().startsWith(".")) continue; // Игнорируем скрытые файлы/папки

        QListWidgetItem *item = new QListWidgetItem(); // Создаем элемент без текста, текст установим далее
        if (entry.isDir()) {
            item->setText(entry.fileName()); // Для папок оставляем полное имя
            item->setIcon(QIcon::fromTheme("folder")); // Иконка для папки
            item->setData(Qt::UserRole, QVariant("folder")); // Тип: папка
        } else if (entry.isFile() && entry.suffix() == "md") {
            item->setText(entry.baseName()); // Для заметок убираем расширение .md
            item->setIcon(QIcon::fromTheme("text-x-markdown")); // Иконка для заметки
            item->setData(Qt::UserRole, QVariant("note")); // Тип: заметка
            item->setData(Qt::UserRole + 1, entry.absoluteFilePath()); // Путь к файлу
        } else {
            delete item; // Игнорируем другие файлы
            continue;
        }
        ui->listWidget->addItem(item);
    }
    m_currentDirectory = path; // Обновляем текущую директорию
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
        // QMessageBox::information(this, "Успех", "Папка успешно создана!"); // ЭТА СТРОКА УДАЛЕНА ИЛИ ЗАКОММЕНТИРОВАНА
        
        QListWidgetItem *item = new QListWidgetItem(dirName);
        item->setIcon(QIcon::fromTheme("folder"));
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

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение удаления",
                                  "Вы уверены, что хотите удалить \"" + itemName + "\"? Это действие нельзя отменить.",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return; // Пользователь отменил удаление
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
            // QMessageBox::information(this, "Удаление", "Заметка успешно удалена."); // Можно оставить, если нужно подтверждение
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
            // QMessageBox::information(this, "Удаление", "Папка и ее содержимое успешно удалены."); // Можно оставить
            loadItemsFromDirectory(m_currentDirectory); // Перезагружаем список, чтобы убедиться, что все корректно обновлено
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить папку рекурсивно. Проверьте права доступа.");
        }
    }
}

// НОВОЕ: Переопределение keyPressEvent для обработки клавиши Delete
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        deleteSelectedItems(); // Вызываем слот для удаления
    } else {
        QMainWindow::keyPressEvent(event); // Передаем событие базовому классу для других клавиш
    }
}

// НОВОЕ: Слот для отображения контекстного меню
void MainWindow::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = ui->listWidget->itemAt(pos);
    if (item) { // Показываем меню только если кликнули по элементу
        QMenu contextMenu(tr("Context menu"), this);

        QAction actionRename(tr("Переименовать"), this);
        QAction actionDelete(tr("Удалить"), this);

        connect(&actionRename, &QAction::triggered, this, &MainWindow::renameSelectedItem);
        connect(&actionDelete, &QAction::triggered, this, &MainWindow::deleteItemFromContextMenu);

        contextMenu.addAction(&actionRename);
        contextMenu.addAction(&actionDelete);

        contextMenu.exec(ui->listWidget->mapToGlobal(pos)); // Показываем меню в позиции курсора
    }
}

// НОВОЕ: Слот для переименования элемента
void MainWindow::renameSelectedItem()
{
    QListWidgetItem *selectedItem = ui->listWidget->currentItem();
    if (!selectedItem || selectedItem->data(Qt::UserRole).toString() == "..") {
        QMessageBox::information(this, "Переименование", "Пожалуйста, выберите заметку или папку для переименования.");
        return;
    }

    QString type = selectedItem->data(Qt::UserRole).toString();
    QString oldDisplayName = selectedItem->text(); // Текущее отображаемое имя (базовое имя)
    QString oldFullPath;

    if (type == "note") {
        oldFullPath = selectedItem->data(Qt::UserRole + 1).toString();
        oldDisplayName = QFileInfo(oldFullPath).baseName(); // Берем базовое имя для диалога
    } else if (type == "folder") {
        oldFullPath = QDir(m_currentDirectory).filePath(oldDisplayName); // Полный путь к старой папке
    } else {
        return;
    }

    bool ok;
    QString newName = QInputDialog::getText(this, tr("Переименовать"),
                                            tr("Введите новое имя для '%1':").arg(oldDisplayName),
                                            QLineEdit::Normal,
                                            oldDisplayName, &ok);

    if (ok && !newName.isEmpty() && newName != oldDisplayName) {
        bool success = false;

        if (type == "note") {
            QString newFullPath = QFileInfo(oldFullPath).dir().filePath(newName + ".md");
            QFile oldFile(oldFullPath);
            success = oldFile.rename(newFullPath);
            if (success) {
                handleNoteRenamed(oldFullPath, newFullPath);
            } else {
                QMessageBox::critical(this, "Ошибка переименования", "Не удалось переименовать заметку. Ошибка: " + oldFile.errorString());
            }
        } else if (type == "folder") {
            // ИЗМЕНЕНИЕ ЗДЕСЬ:
            // 1. Создаем объект QDir, представляющий родительскую директорию.
            //    В данном случае m_currentDirectory - это и есть родительская директория.
            QDir parentDir(m_currentDirectory); 
            
            // 2. Вызываем метод rename() на этом объекте, передавая старое базовое имя
            //    и новое базовое имя.
            success = parentDir.rename(oldDisplayName, newName); 

            if (success) {
                // Обновляем отображаемое имя в списке
                selectedItem->setText(newName); 
                // Перезагружаем список, чтобы убедиться, что все пути и порядок корректны.
                // Это также обновит m_currentDirectory, если это было переименование
                // текущей директории, что крайне редко, но важно для консистентности.
                loadItemsFromDirectory(m_currentDirectory); 
            } else {
                QMessageBox::critical(this, "Ошибка переименования", "Не удалось переименовать папку. Проверьте права доступа.");
            }
        }
    } else if (ok && newName.isEmpty()) {
        QMessageBox::warning(this, "Переименование", "Имя не может быть пустым.");
    }
}

// НОВОЕ: Слот для удаления элемента из контекстного меню или клавиши Delete
void MainWindow::deleteItemFromContextMenu()
{
    QListWidgetItem *selectedItem = ui->listWidget->currentItem();
    if (!selectedItem || selectedItem->data(Qt::UserRole).toString() == "..") {
        return; // Ничего не выбрано или выбран ".."
    }

    QString type = selectedItem->data(Qt::UserRole).toString();
    QString itemName = selectedItem->text();
    QString fullPath;

    if (type == "note") {
        fullPath = selectedItem->data(Qt::UserRole + 1).toString();
    } else if (type == "folder") {
        fullPath = QDir(m_currentDirectory).filePath(itemName);
    } else {
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение удаления",
                                  "Вы уверены, что хотите удалить '" + itemName + "'? Это действие нельзя отменить.",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return; // Пользователь отменил удаление
    }

    if (type == "note") {
        QFile file(fullPath);
        if (file.remove()) {
            if (m_openNotes.contains(fullPath)) {
                int tabIndex = m_noteTabIndices.value(fullPath, -1);
                if (tabIndex != -1) {
                    on_tabWidget_tabCloseRequested(tabIndex);
                }
            }
            delete ui->listWidget->takeItem(ui->listWidget->row(selectedItem));
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить заметку. Ошибка: " + file.errorString());
        }
    } else if (type == "folder") {
        QDir dir(fullPath);
        
        if (dir.removeRecursively()) {
            QList<QString> notesToClose;
            for(const QString& notePath : m_openNotes.keys()) {
                if (notePath.startsWith(fullPath)) {
                    notesToClose.append(notePath);
                }
            }
            for(const QString& notePath : notesToClose) {
                int tabIndex = m_noteTabIndices.value(notePath, -1);
                if (tabIndex != -1) {
                    on_tabWidget_tabCloseRequested(tabIndex);
                }
            }
            delete ui->listWidget->takeItem(ui->listWidget->row(selectedItem));
            loadItemsFromDirectory(m_currentDirectory);
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить папку рекурсивно. Проверьте права доступа.");
        }
    }
}

