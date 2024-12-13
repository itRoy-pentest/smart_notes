#ifndef NOTE_H
#define NOTE_H

#include <QDialog>
#include <QDateTime>

namespace Ui
{
    class Note;
}

class Note : public QDialog
{
    Q_OBJECT

public:
    explicit Note(QWidget *parent = nullptr);
    ~Note();

    int id;                    // Идентификатор заметки
    QString title;             // Заголовок заметки
    QDateTime lastModified;    // Время последнего изменения
private slots:
    void onAddNoteclicked();
    void onRemoveNoteclicked();

private:
    void makeConnection();

    Ui::Note *ui;


};

#endif // NOTE_H
