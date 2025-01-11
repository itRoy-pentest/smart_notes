#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "folder/folder.h"
#include "note/note.h"

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

    void tabWidgetClosed();

    void autoNoteSave();

signals:
    void titleChanged();
    void textChanged();

private:
    Ui::MainWindow *ui;
    Folder folder;
    Note note;
    int noteIndex = 0; // Current note index

    QTextEdit* titleEdit;
    QTextEdit* textEdit;
};
#endif // MAINWINDOW_H
