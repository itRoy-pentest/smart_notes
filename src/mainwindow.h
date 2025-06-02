#pragma once

#include <QMainWindow>
#include "folder/folder.h"
#include "note/note.h"
#include <QFile>
#include <QMap>       // Для хранения открытых заметок по их пути
#include <QListWidgetItem> // Для работы с элементами списка
#include <QKeyEvent>    // Для обработки событий нажатия клавиш

// Макросы QT_BEGIN_NAMESPACE и QT_END_NAMESPACE
// Оборачивают пространство имен Qt, чтобы оно не вступало в конфликт с вашим кодом
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateNoteTabTitle();
    void updateNoteTitle();

protected:
    void keyPressEvent(QKeyEvent *event) override; // НОВОЕ: Переопределение для обработки клавиш

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
    void renameSelectedItem();               // НОВОЕ: Слот для переименования из контекстного меню
    void deleteItemFromContextMenu();        // НОВОЕ: Слот для удаления из контекстного меню

signals:


private:
    Ui::MainWindow *ui;
    // Удаляем Folder folder; Note note; так как будем управлять множеством заметок
    QMap<QString, Note*> m_openNotes; // Карта открытых заметок (путь -> объект Note)
    QMap<QString, int> m_noteTabIndices; // Карта путей заметок к индексам вкладок

    QString m_currentDirectory = "autosave"; // Текущая директория для отображения в списке
    
    void loadItemsFromDirectory(const QString &path); // Загружает заметки и папки из директории
    void openNoteInTab(const QString &filePath); // Открывает заметку во вкладке
    int getTabIndexForNote(const QString &filePath); // Получает индекс вкладки для заметки
};
