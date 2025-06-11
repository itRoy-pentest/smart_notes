#pragma once

#include <QMainWindow>
#include "folder/folder.h"
#include "note/note.h"
#include <QFile>
#include <QMap>       // Для хранения открытых заметок по их пути
#include <QListWidgetItem> // Для работы с элементами списка
#include <QKeyEvent>    // Для обработки событий нажатия клавиш
#include <QDir> // Добавлено для QDir::SortFlags

// Макросы QT_BEGIN_NAMESPACE и QT_END_NAMESPACE
// Оборачивают пространство имен Qt, чтобы оно не вступало в конфликт с вашим кодом
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

// Новый класс для списка заметок с обработкой клавиши Delete
class NoteListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit NoteListWidget(QWidget *parent = nullptr) : QListWidget(parent) {}

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Delete) {
            emit deleteKeyPressed();
            event->accept();
        } else {
            QListWidget::keyPressEvent(event);
        }
    }

signals:
    void deleteKeyPressed();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateNoteTabTitle();
    void updateNoteTitle();

private slots:
    void createNewNote();
    void createNewDir();
    void on_tabWidget_tabCloseRequested(int index);
    void onListItemClicked(QListWidgetItem *item); // Слот для клика по элементу в listWidget
    void handleNoteRenamed(const QString &oldPath, const QString &newPath); // Слот для обработки переименования заметки
    void handleNoteClosed(const QString &filePath); // Слот для обработки закрытия заметки
    void on_tabWidget_currentChanged(int index); // Новый слот для обработки смены активной вкладки
    void deleteSelectedItems(); // Этот слот теперь будет вызываться из keyPressEvent

    void showContextMenu(const QPoint &pos); // НОВОЕ: Слот для отображения контекстного меню
    void renameSelectedItem();
    void deleteItemFromContextMenu();        // НОВОЕ: Слот для удаления из контекстного меню
    void toggleSortOrder(); // НОВЫЙ СЛОТ ДЛЯ СОРТИРОВКИ

signals:

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    // Удаляем Folder folder; Note note; так как будем управлять множеством заметок
    QMap<QString, Note*> m_openNotes; // Карта открытых заметок (путь -> объект Note)
    QMap<QString, int> m_noteTabIndices; // Карта путей заметок к индексам вкладок

    QString m_currentDirectory = "autosave"; // Текущая директория для отображения в списке
    QDir::SortFlags m_currentSortOrder; // НОВОЕ: Переменная для хранения текущего порядка сортировки
    
    void loadItemsFromDirectory(const QString &path); // Загружает заметки и папки из директории
    void openNoteInTab(const QString &filePath); // Открывает заметку во вкладке
    int getTabIndexForNote(const QString &filePath); // Получает индекс вкладки для заметки
};
