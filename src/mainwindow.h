#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "folder/folder.h"
#include "note/note.h"


QT_BEGIN_NAMESPACE
namespace Ui {
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
    void on_NewNote_clicked();

private:
    Ui::MainWindow *ui;
    Folder folder;
    Note note;
};
#endif // MAINWINDOW_H
