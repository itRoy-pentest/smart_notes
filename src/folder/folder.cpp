#include "folder.h"
#include "src/folder/ui_folder.h"
#include <QDir>

Folder::Folder(QWidget *parent): QDialog(parent), ui(new Ui::Folder)
{
    ui->setupUi(this);


}

Folder::~Folder()
{
    delete ui;
}

void Folder::createAndCheckMainFolder()
{
    QDir dir("autosave");

    if(!dir.exists())
        dir.mkpath("autosave");
}
