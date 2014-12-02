#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QSplashScreen>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <pthread.h>
#include <QKeyEvent>
#include <QInputDialog>
#include <stdlib.h>
#include "contact.h"
#include "sms.h"
#include "database.h"
#include "sms_file.h"
#include "new_sms.h"
#include "sms_list.h"
#include "search_dialog.h"
#include "tcpclientclass.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QBitmap>
#define MAX_LEN 1000

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();
    
    Database db;
    TcpClientClass tc;
private slots:
    void on_addButton_clicked();
    void on_searchButton_clicked();
    void show_sms_list();
    void refresh_list();
    void new_msg();
    void on_refreshButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainDialog *ui;
       QPoint last;
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
     void mouseMoveEvent(QMouseEvent *e);
     void mouseReleaseEvent(QMouseEvent *e);

};

#endif // MAINDIALOG_H
