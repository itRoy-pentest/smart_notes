#pragma once

#include <QDialog>
#include <QDateTime>
#include <QTextDocument>
#include <QList>  // Для хранения списка заметок
#include <QString>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QTextEdit>

namespace Ui
{
    class Note;
}

class Note : public QDialog
{
    Q_OBJECT

public:
    explicit Note(QWidget *parent = nullptr);
    explicit Note(Ui::Note *ui) : ui(ui) {}
    ~Note();

    QTextEdit* getTitleEdit();
    QTextEdit* getTextEdit();
    QWidget* getNoteWidget();

    QString setTitle(const QString &title);
    QString setText(const QString &text);

private slots:

private:
    Ui::Note *ui;
};
