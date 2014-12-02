#include "maindialog.h"
#include "ui_maindialog.h"

//server ip
char serverip[20];

// Sms, to inset into db
Sms nsms;

// counter to check new message
int new_msg_s = 0;

//counter to check if server is available
int server_avlb = 0;

//thread to recevice messga from server
void*
run(void* arg)
{
        TcpClientClass* tctc = (TcpClientClass * )arg;

        //init params
        fd_set fdcli;
        int maxsock;
        char buf[MAX_LEN] = "hehe I wake up";
        maxsock = tctc->sockfd;

        //receiving msg
        while(1)
        {
            FD_ZERO(&fdcli);

            FD_SET(tctc->sockfd, &fdcli);

            int ret = select(maxsock+1, &fdcli, NULL, NULL, NULL);
            if(ret < 0)
            {
                perror("select");
                break;
            }

            if(ret == 0)
            {
                printf("time out\n");
                continue;
            }

            if(FD_ISSET(tctc->sockfd, &fdcli))
            {

                if(!tctc->tcp_client_recv(buf, MAX_LEN)){

                    if(tctc->reconnect())
                        tctc->tcp_client_recv(buf, MAX_LEN);
                }
                qDebug() << "receive";
                nsms.cstr_to_sms(buf);


                cout << tctc->telNum << endl;
                if(nsms.get_to() == tctc->telNum){
                    /*************************************insert to db********************************/
                    new_msg_s = 1;

                }
            }

        }
        return (void *)0;
}

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    QPixmap bgImages("7.png");
    QPalette bgPalette  = this->palette();
    bgPalette.setBrush(QPalette::Background,bgImages);
    this->setPalette(bgPalette);
    setMask(bgImages.mask());


    //to set icons over buttons
    QPixmap p1("search.png");
    QIcon ButtonIcon(p1);
    ui->searchButton->setIcon(ButtonIcon);
    ui->searchButton->setIconSize(QSize(28,28));
    QPixmap p2("add.png");
    QIcon ButtonIcon2(p2);
    ui->addButton->setIcon(ButtonIcon2);
    ui->addButton->setIconSize(QSize(28,28));
    QPixmap p3("refresh.png");
    QIcon ButtonIcon3(p3);
    ui->refreshButton->setIcon(ButtonIcon3);
    ui->refreshButton->setIconSize(QSize(28,28));

    //timer to check new message
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(show_sms_list()));
    QTimer * timer_s  = new QTimer();
    connect(timer_s,SIGNAL(timeout()),this,SLOT(new_msg()));
    timer_s->start(20);

    //get local phone number
    tc.telNum = db.get_number();

    //set server ip
    strcpy(serverip, db.get_ip_server().c_str());

    //initialize tcpclient

    if (tc.tcp_client_init(serverip,4217, tc.telNum) == false)
    {
        server_avlb = 1;
    }

    db.set_number(tc.telNum);


    //run the receiving thread
    pthread_t pt_run;
    pthread_create(&pt_run,NULL,run,(void *)&tc);

    //you know...
    refresh_list();
}

MainDialog::~MainDialog()
{
    //functions called when the client shut down

    delete ui;
}

void MainDialog::refresh_list()
{
    ui->listWidget->clear();
    map<string,string> map_list = db.get_contact_list();
    map<string,string>::iterator it = map_list.end();
    string show_text;
    int i = 0;
    while(it != map_list.begin()){
        it --;
        ++ i;
        string tem_name = (db.get_contact(it->second)).get_name();
        if(tem_name != "未命名")
        {
            show_text = (db.get_contact(it->second)).get_name();
        }
        else show_text = (db.get_contact(it->second)).get_ip();
        show_text.append(":\n");
        show_text.append(db.get_sms((-- db.get_contact(it->second).Sms_self.end())->second).get_text());//update
        show_text.append("\n             ");
        string time;
        time = (it->first).substr(0,4) + "/" + (it->first).substr(4,2) + "/" + (it->first).substr(6,2) + " "
                + (it->first).substr(8,2) + ":" + (it->first).substr(10,2);
        show_text.append(time);

        QListWidgetItem *item = new QListWidgetItem((QString::fromStdString(show_text)));//tr(show_text.c_str()//245,248,212
        item->setSizeHint(QSize(60,60));
        if(i & 1) item->setBackground(QColor::fromRgb(245,248,212));
        item->setData(1,QVariant(QString::fromStdString(it->second)));
        ui->listWidget->addItem(item);
    }
}

void MainDialog::on_addButton_clicked()
{
    // show new_sms dialog
    new_sms *dialog = new new_sms(this);
    dialog->db_from_parent = &db;
    dialog->tc_from_parent = &tc;
    dialog->show();
    dialog->setAutoFillBackground(true);
}

void MainDialog::on_searchButton_clicked()
{
    search_dialog * dialog  =new search_dialog(this);
    dialog->db_from_parent = &db;
    dialog->show();
    dialog->setAutoFillBackground(true);

}

void MainDialog::show_sms_list()
{
    QListWidgetItem *t = ui->listWidget->currentItem();
        sms_list *dialog = new sms_list(this);
        dialog->db_from_parent = &db;
        dialog->tc_from_parent = &tc;
        dialog->contact_tel =t->data(1).toString();
        dialog->show();
        dialog->setAutoFillBackground(true);
}

void MainDialog::new_msg()
{   
    if(server_avlb == 1)
    {
        QMessageBox::information(this,"Warning","Cannot connect to server");
         server_avlb = 0;
    }

    if(new_msg_s == 1)
    {
        new_msg_s = 0;
        QPixmap p3("refresh_new.png");
        QIcon ButtonIcon3(p3);
        ui->refreshButton->setIcon(ButtonIcon3);
        ui->refreshButton->setIconSize(QSize(28,28));
        nsms.set_status(0);
        db.Add(nsms);
        QMessageBox::information(this,"","You have a new message");
    }
}

void MainDialog::on_refreshButton_clicked()
{
    QPixmap p3("refresh.png");
    QIcon ButtonIcon3(p3);
    ui->refreshButton->setIcon(ButtonIcon3);
    ui->refreshButton->setIconSize(QSize(28,28));
    MainDialog::refresh_list();
}

void MainDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Backspace)
    {
        if(ui->listWidget->currentItem() == NULL) return;
        QListWidgetItem *t = ui->listWidget->currentItem();
        int ret = QMessageBox::question(this,"Warning","Are you sure to delete all messages with"+ t->data(1).toString() + "?",QMessageBox::Yes,QMessageBox::No);
        if(ret == QMessageBox::Yes)
        {
               db.Delete(t->data(1).toString().toStdString());
               refresh_list();
        }
    }
}

void MainDialog::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}

void MainDialog::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();

    last = e->globalPos();
    move(x()+dx, y()+dy);
}

void MainDialog::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();

    move(x()+dx, y()+dy);
}

void MainDialog::on_pushButton_clicked()
{
    db.shut_down();
    tc.tcp_client_close();
    this->close();
}
