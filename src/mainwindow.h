#pragma once

#include <QMainWindow>
#include "folder/folder.h"
#include "note/note.h"
#include <QFile>

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

private slots:
    void createNewNote();
    void createNewDir();

    void on_tabWidget_tabCloseRequested(int index);

signals:
    void titleChanged();
    void textChanged();

private:
    Ui::MainWindow *ui;
    Folder folder;
    Note note;
    int currentNoteIndex = -1; // Current note index

    QTextEdit* titleText;
    QTextEdit* textMain;
};
