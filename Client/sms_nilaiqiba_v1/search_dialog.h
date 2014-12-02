#ifndef SEARCH_DIALOG_H
#define SEARCH_DIALOG_H

#include <QDialog>
#include "maindialog.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QBitmap>
namespace Ui {
class search_dialog;
}

class search_dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit search_dialog(QWidget *parent = 0);
    ~search_dialog();

    Database* db_from_parent;
    
private slots:
    void on_searchButton_clicked();

private:
    Ui::search_dialog *ui;
    QPoint last;
};

#endif // SEARCH_DIALOG_H


