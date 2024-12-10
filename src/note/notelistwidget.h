#ifndef NOTELISTWIDGET_H
#define NOTELISTWIDGET_H

#include <QWidget>

namespace Ui {
class NoteListWidget;
}

class NoteListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteListWidget(QWidget *parent = nullptr);
    ~NoteListWidget();

private:
    Ui::NoteListWidget *ui;
};

#endif // NOTELISTWIDGET_H
