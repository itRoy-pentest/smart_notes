#include "note.h"
#include "ui_note.h"
#include <qpushbutton.h>
#include <QMessageBox>
#include <QDebug>

Note::Note(QWidget *parent): QDialog(parent), ui(new Ui::Note)
{

    ui->setupUi(this);
}

Note::~Note()
{
    delete ui;
}
