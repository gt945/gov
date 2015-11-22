#-------------------------------------------------
#
# Project created by QtCreator 2015-10-15T11:16:47
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = getvpn
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    website.cpp \
    vpn.cpp

HEADERS  += mainwindow.h \
    website.h \
    vpn.h

FORMS    += mainwindow.ui
