#include "Note.h"
#include "src/note/ui_Note.h"

Note::Note(QWidget *parent): QDialog(parent), ui(new Ui::Note)
{

    ui->setupUi(this);
}

Note::~Note()
{
    delete ui;
}
