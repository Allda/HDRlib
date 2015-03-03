#-------------------------------------------------
#
# Project created by QtCreator 2015-03-03T18:52:47
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = HDRlib
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32 {
    INCLUDEPATH  += $$quote(C:/MinGW/msys/1.0/local/include)
    LIBS         += $$quote(C:/MinGW/msys/1.0/local/lib/libexiv2.dll.a)
}

unix {
    INCLUDEPATH  += /usr/local/include
    LIBS         += -L/usr/local/lib -lexiv2
}

macx {
    INCLUDEPATH  += /usr/local/include
    LIBS         += -L/usr/local/lib -lexiv2
}

