#-------------------------------------------------
#
# Project created by QtCreator 2013-09-09T10:03:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sms_nilaiqiba_v1
TEMPLATE = app


SOURCES += main.cpp\
        maindialog.cpp \
    new_sms.cpp \
    sms_list.cpp \
    search_dialog.cpp \
    tcpclientclass.cpp

HEADERS  += maindialog.h \
    new_sms.h \
    sms_list.h \
    contact.h \
    database.h \
    sms.h \
    sms_file.h \
    search_dialog.h \
    tcpclientclass.h

FORMS    += maindialog.ui \
    new_sms.ui \
    sms_list.ui \
    search_dialog.ui
