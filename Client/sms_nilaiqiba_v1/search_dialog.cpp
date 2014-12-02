#include "search_dialog.h"
#include "ui_search_dialog.h"
#include "sms.h"

search_dialog::search_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::search_dialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    QPixmap bgImages("7.png");
    QPalette bgPalette  = this->palette();
    bgPalette.setBrush(QPalette::Background,bgImages);
    this->setPalette(bgPalette);
    setMask(bgImages.mask());
    QPixmap p("search.png");
    QIcon ButtonIcon(p);
    ui->searchButton->setIcon(ButtonIcon);
    ui->searchButton->setIconSize(QSize(28,28));
}

search_dialog::~search_dialog()
{
    delete ui;
}

void search_dialog::on_searchButton_clicked()
{
    ui->listWidget->clear();
    string key_word = ui->lineEdit->text().toStdString();
    map<string,int> ans_search = db_from_parent->search(key_word);
    map<string,int>::iterator itc;
    string text;
    int i;
    for(i = 0, itc = ans_search.begin();itc != ans_search.end();++itc,++i){
        Sms tem_sms = db_from_parent->get_sms(itc->second);//update

        text = tem_sms.get_cont();
        text.append(":\n\n");
        text.append(tem_sms.get_formated_text());
        text.append("\n");
        string time = (itc->first).substr(0,4) + "/" + (itc->first).substr(4,2) + "/" + (itc->first).substr(6,2) + " "
                + (itc->first).substr(8,2) + ":" + (itc->first).substr(10,2);
        text.append("            " + time);
        QListWidgetItem *itm = new QListWidgetItem(QString::fromStdString(text));
        if(i & 1) itm->setBackground(QColor::fromRgb(239,247,190)); // 239,247,146
        ui->listWidget->addItem(itm);
        ui->listWidget->addItem("");
    }
}
