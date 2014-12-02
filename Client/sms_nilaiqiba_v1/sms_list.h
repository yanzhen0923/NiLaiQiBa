#ifndef SMS_LIST_H
#define SMS_LIST_H

#include <QDialog>
#include "tcpclientclass.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QBitmap>
#include "maindialog.h"
namespace Ui {
class sms_list;
}

class sms_list : public QDialog
{
    Q_OBJECT
    
public:
    explicit sms_list(QWidget *parent = 0);
    ~sms_list();

    QString contact_tel;
    Database* db_from_parent;
    TcpClientClass* tc_from_parent;
    QTimer* timer_s;
    
private slots:
    void on_sendButton_clicked();
    void refresh_s();

    void on_setTimeButton_clicked();

    void on_contactButton_clicked();

private:
    Ui::sms_list *ui;
    int year_s, month_s, day_s, hour_s, minute_s;
    string get_system_time();
    QPoint last;

protected:
    virtual void keyPressEvent(QKeyEvent *e);
};

#endif // SMS_LIST_H


