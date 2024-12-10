#include "folder.h"
#include "src/folder/ui_folder.h"

Folder::Folder(QWidget *parent): QDialog(parent), ui(new Ui::Folder)
{
    ui->setupUi(this);
}

Folder::~Folder()
{
    delete ui;
}
