#include "note.h"
#include "ui_note.h"

Note::Note(QWidget *parent): QDialog(parent), ui(new Ui::Note)
{

    ui->setupUi(this);
}

Note::~Note()
{
    delete ui;
}
