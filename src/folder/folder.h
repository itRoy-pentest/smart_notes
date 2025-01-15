#ifndef FOLDER_H
#define FOLDER_H

#include <QDialog>

namespace Ui {
class Folder;
}

class Folder : public QDialog
{
    Q_OBJECT

public:
    explicit Folder(QWidget *parent = nullptr);
    ~Folder();



    void createAndCheckMainFolder();

private:
    Ui::Folder *ui;
};

#endif // FOLDER_H
