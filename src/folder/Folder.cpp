#include "Folder.h"
#include "src/folder/ui_Folder.h"

Folder::Folder(QWidget *parent): QDialog(parent), ui(new Ui::Folder)
{
    ui->setupUi(this);
}

Folder::~Folder()
{
    delete ui;
}
