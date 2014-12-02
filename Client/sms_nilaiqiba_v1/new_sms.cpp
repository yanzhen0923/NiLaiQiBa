#include "new_sms.h"
#include "ui_new_sms.h"
#include "maindialog.h"

new_sms::new_sms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::new_sms)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    QPixmap bgImages("7.png");
    QPalette bgPalette  = this->palette();
    bgPalette.setBrush(QPalette::Background,bgImages);
    this->setPalette(bgPalette);
    setMask(bgImages.mask());
}

new_sms::~new_sms()
{
    delete ui;
}

string new_sms::get_system_time() {
    string ret("");
    time_t timer;
    struct tm* t_tm;
    timeval tBegin;
    time(&timer);
    t_tm = localtime(&timer);
    gettimeofday(&tBegin, 0);
    ret += m_to_string((unsigned int)t_tm->tm_year + 1900, 4);
    ret += m_to_string((unsigned int)t_tm->tm_mon + 1, 2);
    ret += m_to_string((unsigned int)t_tm->tm_mday, 2);
    ret += m_to_string((unsigned int)t_tm->tm_hour, 2);
    ret += m_to_string((unsigned int)t_tm->tm_min, 2);
    ret += m_to_string((unsigned int)t_tm->tm_sec, 2);
    ret += m_to_string((unsigned int)tBegin.tv_usec, 8);
    return ret;
}

void new_sms::on_sendButton_clicked()
{
    //get text
    string tel_num = ui->lineEdit->text().toStdString(); // 18810577401 - 18810577499

    //电话号码合法性检测
    bool flag = 0;
    string std_tel = "188105774";
    if(strlen(tel_num.c_str()) != 11) flag = 1;
    else if(strcmp(tel_num.substr(0,9).c_str(),std_tel.c_str()) != 0) flag = 1;
    else if(tel_num[9] < '0' || tel_num[9] > '9') flag = 1;
    else if(tel_num[10] < '1' || tel_num[10] > '9') flag = 1;
    if(flag)
    {
        QMessageBox::information(this,"Warning","The phone number is invalid\nPlease input:\n18810577401 - 18810577499");
        return;
    }

    string sms_text = ui->textEdit->toPlainText().toStdString();
    if(strlen(sms_text.c_str()) == 0) return; // 空短信不允许发送
    Sms message(db_from_parent->get_number(),tel_num,tel_num,sms_text,-1);
    cout << db_from_parent->get_number() << " hahahehe";
    if(ui->timedSendRadio->isChecked()) // 定时发送
    {
        message.set_time(year_s,month_s,day_s,hour_s,minute_s);
    }

   //send sms
   char buf_s[MAX_LEN+9];
   memset(buf_s, 0, sizeof(buf_s));
   message.sms_to_cstr(buf_s);
   qDebug() << tc_from_parent->sockfd << buf_s;
   if(!tc_from_parent->tcp_client_send(buf_s,MAX_LEN))
   {
       QMessageBox::information(this,"Warning","Sending failure!");
       db_from_parent->Add(message);
   }
   else
   {
       message.set_status(0);//发送成功
       db_from_parent->Add(message);
   }
   this->close();
}

void new_sms::on_setTimeButton_clicked()
{
    bool ok;
    QString sys_year = QString::fromStdString(get_system_time().substr(0,4));
    QString sys_month = QString::fromStdString(get_system_time().substr(4,2));
    QString sys_day = QString::fromStdString(get_system_time().substr(6,2));
    QString sys_hour = QString::fromStdString(get_system_time().substr(8,2));
    QString sys_minute = QString::fromStdString(get_system_time().substr(10,2));
    year_s = QInputDialog::getInt(this,"Timed send settings","YEAR:",sys_year.toInt(),2013,10000,1,&ok);
    if(!ok) return;
    month_s = QInputDialog::getInt(this,"Timed send settings","MONTH:",sys_month.toInt(),1,12,1,&ok);
    if(!ok) return;
    switch(month_s)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12: day_s = QInputDialog::getInt(this,"Timed send settings","DAY:",sys_day.toInt(),1,31,1,&ok);break;
    case 2: {
        if (((year_s%4==0)&&(year_s%100!=0)||(year_s%400==0)))
        {
            day_s = QInputDialog::getInt(this,"Timed send settings","DAY:",sys_day.toInt(),1,29,1,&ok);
            break;
        }
        else day_s = QInputDialog::getInt(this,"Timed send settings","DAY:",sys_day.toInt(),1,28,1,&ok); break;
    }
    case 4:
    case 6:
    case 9:
    case 11: day_s = QInputDialog::getInt(this,"Timed send settings","DAY:",sys_day.toInt(),1,30,1,&ok);break;
    default: break;
    }
    if(!ok) return;
    hour_s = QInputDialog::getInt(this,"Timed send settings","HOUR:",sys_hour.toInt(),0,23,1,&ok);
    if(!ok) return;
    minute_s = QInputDialog::getInt(this,"Timed send settings","MINUTE:",sys_minute.toInt(),0,59,1,&ok);
}
