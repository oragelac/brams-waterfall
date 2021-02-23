#-------------------------------------------------
#
# Project created by QtCreator 2017-04-18T13:16:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = spectrogram
TEMPLATE = app
LIBS += -lfftw3
SOURCES += main.cpp\
        mainwindow.cpp \
    memorymap.cpp \
    fft.cpp \
    spectrogram.cpp \
    palette.cpp

HEADERS  += mainwindow.h \
    memorymap.h \
    fft.h \
    spectrogram.h \
    palette.h

