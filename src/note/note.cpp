#include "note.h"
#include "ui_note.h"
#include <qpushbutton.h>

Note::Note(QWidget *parent): QDialog(parent), ui(new Ui::Note)
{

    ui->setupUi(this);

    makeConnection();
}



Note::~Note()
{
    delete ui;
}

void Note::onAddNoteclicked()
{

}


void Note::onRemoveNoteclicked()
{

}

void Note::makeConnection()
{
    connect(ui->AddNote, &QPushButton::clicked, this, &Note::onAddNoteclicked);
    connect(ui->RemoveNote, &QPushButton::clicked, this, &Note::onRemoveNoteclicked);
}

