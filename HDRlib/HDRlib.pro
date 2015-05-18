#-------------------------------------------------
#
# Project created by QtCreator 2015-03-03T18:52:47
#
#-------------------------------------------------

QT       += core

QT       += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HDRlib
CONFIG   -= console
CONFIG   -= app_bundle

TEMPLATE = app


LIBS += -pthread

QMAKE_CXXFLAGS += -std=c++11 -pthread
CONFIG += c++11

DESTDIR = .

release {
    OBJECTS_DIR = build/release
    MOC_DIR = build/release
}

debug {
    OBJECTS_DIR = build/debug
    MOC_DIR = build/debug
}


SOURCES += main.cpp \
    ldrimage.cpp \
    hdrcreator.cpp \
    weightfunction.cpp \
    debevecmalikwf.cpp \
    hdrimage.cpp \
    exceptions/notenoughimagesexception.cpp \
    exceptions/imagesizeexception.cpp \
    tonemapping/tonemapping.cpp \
    tonemapping/linearoperator.cpp \
    tonemapping/logoperator.cpp \
    tonemapping/expoperator.cpp \
    tonemapping/reinhardglobaloperator.cpp \
    multiexposureCamera/frame.cpp \
    multiexposureCamera/multi_exposure_camera_thrd.cpp \
    multiexposureCamera/multi_exposure_camera.cpp \
    multiexposureCamera/frame_assembler.cpp \
    multiexposureCamera/camera_viewer.cpp \
    multiexposureCamera/load_configuration.cpp \
    multiexposureCamera/camera_controller.cpp \
    gui/mainwindow.cpp \
    gui/imageframe.cpp \
    gui/camerasettingsdialog.cpp \
    gui/buildhdrthread.cpp \
    rgbe.cpp \
    tonemapping/adaptivelogoperator.cpp

HEADERS += \
    ldrimage.h \
    hdrcreator.h \
    weightfunction.h \
    debevecmalikwf.h \
    hdrimage.h \
    exceptions/notenoughimagesexception.h \
    exceptions/imagesizeexception.h \
    tonemapping/tonemapping.h \
    tonemapping/linearoperator.h \
    tonemapping/logoperator.h \
    tonemapping/expoperator.h \
    tonemapping/reinhardglobaloperator.h \
    multiexposureCamera/frame.h \
    multiexposureCamera/multi_exposure_camera_thrd.h \
    multiexposureCamera/multi_exposure_camera_common.h \
    multiexposureCamera/multi_exposure_camera.h \
    multiexposureCamera/frame_assembler.h \
    multiexposureCamera/camera_viewer.h \
    multiexposureCamera/load_configuration.h \
    multiexposureCamera/camera_controller.h \
    gui/mainwindow.h \
    gui/imageframe.h \
    gui/camerasettingsdialog.h \
    gui/buildhdrthread.h \
    rgbe.h \
    tonemapping/adaptivelogoperator.h

FORMS    += gui/mainwindow.ui


OTHER_FILES += \
    multiexposureCamera/TODO.txt \
    multiexposureCamera/NOTE.txt \
    multiexposureCamera/Binarni_struktura_paketu.txt \
    multiexposureCamera/Readme.txt \
    multiexposureCamera/configuration.txt \
    format/rgbe.txt


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


# Pro použítí ve windows je třeba mít nainstalované opencv a nastavit absolutní
# cesty ke knihovnám
win32 {
    INCLUDEPATH += C:/opencv/build/include

    LIBS += -L"C:/opencv/build/x86/mingw/bin/" \
        -llibopencv_core248 \
        -llibopencv_features2d248 \
        -llibopencv_highgui248 \
        -llibopencv_legacy248 \
        -llibopencv_nonfree248 \
        -llibopencv_flann248 \
        -llibopencv_imgproc248 \
        -llibopencv_calib3d248 \
        -llibopencv_video248

    LIBS += -luser32 \
            -lws2_32
}

unix {
    DEFINES += unix
}

win32 {
    DEFINES += win32
}





