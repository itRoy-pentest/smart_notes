#ifndef NOTE_H
#define NOTE_H

#include <QDialog>

namespace Ui {
class Note;
}

class Note : public QDialog
{
    Q_OBJECT

public:
    explicit Note(QWidget *parent = nullptr);
    ~Note();

private:
    Ui::Note *ui;
};

#endif // NOTE_H
