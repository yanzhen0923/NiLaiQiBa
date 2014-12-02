#ifndef NEW_SMS_H
#define NEW_SMS_H

#include <QDialog>
#include "maindialog.h"
#include "database.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QBitmap>
#include "tcpclientclass.h"
namespace Ui {
class new_sms;
}

class new_sms : public QDialog
{
    Q_OBJECT
    
public:
    explicit new_sms(QWidget *parent = 0);
    ~new_sms();

    Database* db_from_parent;
    TcpClientClass* tc_from_parent;
    
private slots:
    void on_sendButton_clicked();

    void on_setTimeButton_clicked();


private:
    Ui::new_sms *ui;
    int year_s, month_s, day_s, hour_s, minute_s;
    string get_system_time();
     QPoint last;
};

#endif // NEW_SMS_H
