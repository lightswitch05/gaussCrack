#-------------------------------------------------
#
# Project created by QtCreator 2012-08-15T16:21:58
#
#-------------------------------------------------

QT       += core gui

TARGET = GaussCheck
TEMPLATE = app


SOURCES += main.cpp\
            mainwindow.cpp \
            keypairqueue.cpp \
            hashgenerator.cpp \
            hashthread.cpp

HEADERS  += mainwindow.h \
            keypairqueue.h \
            hashgenerator.h \
            hashthread.h

FORMS    += mainwindow.ui

RC_FILE = resources.rc

DEFINES += QT_NO_DEBUG_OUTPUT
