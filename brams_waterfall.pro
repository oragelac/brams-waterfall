#-------------------------------------------------
#
# Project created by QtCreator 2017-05-12T11:18:53
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = brams_waterfall
TEMPLATE = app
INCLUDEPATH += D:\oragelac\Softwares\fftw
LIBS += -LD:\oragelac\Softwares\fftw -lfftw3-3

SOURCES += \
    tcpclient.cpp \
    fft.cpp \
    main.cpp \
    mainwindow.cpp \
    palette.cpp \
    spectrogram.cpp

HEADERS  += \
    tcpclient.h \
    fft.h \
    mainwindow.h \
    palette.h \
    spectrogram.h

SUBDIRS += \
    spectrogram.pro
