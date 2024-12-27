#pragma once

#include <QWidget>

namespace Ui
{
class NoteListWidget;
}

class NoteListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteListWidget(QWidget *parent = nullptr);
    ~NoteListWidget();

    // Set title note
    void setTextTitle(const QString& text);

    // Set date note
    void setTextDate(const QString& text);

private:
    Ui::NoteListWidget *ui;
};
