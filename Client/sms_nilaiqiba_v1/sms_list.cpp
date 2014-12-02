#include "sms_list.h"
#include "ui_sms_list.h"

sms_list::sms_list(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sms_list)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    QPixmap bgImages("7.png");
    QPalette bgPalette  = this->palette();
    bgPalette.setBrush(QPalette::Background,bgImages);
    this->setPalette(bgPalette);
    setMask(bgImages.mask());
    QPixmap p1("contact.png");
    QIcon ButtonIcon(p1);
    ui->contactButton->setIcon(ButtonIcon);
    ui->contactButton->setIconSize(QSize(50,50));

    timer_s = new QTimer();
    connect(timer_s,SIGNAL(timeout()),this,SLOT(refresh_s()));
    timer_s->start(100);
}

sms_list::~sms_list()
{
    delete ui;
}

string sms_list::get_system_time() {
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

void sms_list::on_sendButton_clicked()
{
    //get text
    string tel_num = contact_tel.toStdString();
    string sms_text = ui->textEdit->toPlainText().toStdString();
    if(strlen(sms_text.c_str()) == 0) return; // 空短信不允许发送
    Sms message(db_from_parent->get_number(),tel_num,tel_num,sms_text,-1);
    if(ui->timedSendRadio->isChecked()) // 定时发送
    {
        message.set_time(year_s,month_s,day_s,hour_s,minute_s);
    }
    cout << message << endl;

   //send sms
   char buf_s[MAXLEN+9];
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
   ui->textEdit->clear();
   refresh_s();
}

void sms_list::refresh_s()
{
    ui->listWidget->clear();
    if(db_from_parent->get_contact(contact_tel.toStdString()).get_name() != "未命名")
    {
        ui->nameLable->setText(QString::fromStdString(db_from_parent->get_contact(contact_tel.toStdString()).get_name()));
    }
    else ui->nameLable->setText("");

    ui->phone_numberLable->setText(contact_tel);//contact_list_dialog::contact_index
    map<string,int> smss = db_from_parent->get_sms_list(contact_tel.toStdString());
    map<string,int>::iterator it;
    Sms tem_sms;
    for( it = smss.begin();it != smss.end();++it){
        tem_sms = db_from_parent->get_sms(it->second);
        QListWidgetItem *item = new QListWidgetItem();

        stringstream ss;
        ss << tem_sms.get_id();
        string id = ss.str();
        item->setData(1,QVariant(QString::fromStdString(id)));

        string text = tem_sms.get_formated_text(26) + "\n\n";
        string time = (it->first).substr(0,4) + "/" + (it->first).substr(4,2) + "/" + (it->first).substr(6,2) + " "
                + (it->first).substr(8,2) + ":" + (it->first).substr(10,2);
        text.append("" + time + "\n");

        if(tem_sms.get_cont() != tem_sms.get_from())
        {
            item->setBackground(QColor::fromRgb(245,248,212));
        }
        else item->setBackground(QColor::fromRgb(250,230,250));

        if(tem_sms.get_status() != 0)
        {
            text.append("Sending failure");
        }

        item->setText(QString::fromStdString(text));

        ui->listWidget->addItem(item);
        ui->listWidget->addItem("");
    }
    ui->listWidget->scrollToBottom();
    timer_s->stop();
}

void sms_list::on_setTimeButton_clicked()
{
    bool ok;
    QString sys_year = QString::fromStdString(get_system_time().substr(0,4));
    QString sys_month = QString::fromStdString(get_system_time().substr(4,2));
    QString sys_day = QString::fromStdString(get_system_time().substr(6,2));
    QString sys_hour = QString::fromStdString(get_system_time().substr(8,2));
    QString sys_minute = QString::fromStdString(get_system_time().substr(10,2));
    year_s = QInputDialog::getInt(this,"Timed send settings","YEAR:",sys_year.toInt(),2013,10000,1,&ok);
    if(!ok) {
        qDebug() << year_s;
        return;
    }
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
        }
        else day_s = QInputDialog::getInt(this,"Timed send settings","DAY:",sys_day.toInt(),1,28,1,&ok);
        break;
    }
    case 4:
    case 6:
    case 9:
    case 11: day_s = QInputDialog::getInt(this,"Timed send settings","DAY:",sys_day.toInt(),1,30,1,&ok);
        break;
    }
    if(!ok) return;
    hour_s = QInputDialog::getInt(this,"Timed send settings","HOUR:",sys_hour.toInt(),0,23,1,&ok);
    if(!ok) return;
    minute_s = QInputDialog::getInt(this,"Timed send settings","MINUTE:",sys_minute.toInt(),0,59,1,&ok);
}

void sms_list::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Backspace)
    {
        if(ui->listWidget->currentItem() == NULL || ui->listWidget->currentItem()->text() == "") return;
        QString sms_id = ui->listWidget->currentItem()->data(1).toString();
        db_from_parent->Delete(sms_id.toInt());
        refresh_s();
    }
}

void sms_list::on_contactButton_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this,"Contact Name","Please Input Nick Name:",
                                         QLineEdit::Normal,"",&ok);
    if(ok)
    {
        db_from_parent->change_name(name.toStdString(),contact_tel.toStdString());
        ui->nameLable->setText(name);
    }
}
