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

CONFIG += c++11


SOURCES += main.cpp \
    ldrimage.cpp \
    hdrcreator.cpp \
    weightfunction.cpp \
    debevecmalikwf.cpp \
    hdrimage.cpp \
    exceptions/notenoughimagesexception.cpp \
    exceptions/imagesizeexception.cpp

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
INCLUDEPATH += /usr/local/include/opencv2

LIBS += `pkg-config opencv --libs`

HEADERS += \
    ldrimage.h \
    hdrcreator.h \
    weightfunction.h \
    debevecmalikwf.h \
    hdrimage.h \
    exceptions/notenoughimagesexception.h \
    exceptions/imagesizeexception.h

