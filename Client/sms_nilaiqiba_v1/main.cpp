#include "maindialog.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //launch pre-boot-movie
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap("OpenAnimation.png"));
    splash->show();
    for(int i = 0; i < 2; i++) {
        splash->repaint();
        sleep(1);
    }

    MainDialog w;
    w.show();

    //splash finish:
    delete splash;

    return a.exec();
}
