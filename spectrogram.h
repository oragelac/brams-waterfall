#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <QObject>
#include <QImage>
#include <QLabel>
#include <iostream>
#include <cmath>
#include <QPainter>
#include "palette.h"

class Spectrogram : public QObject
{
    Q_OBJECT

public:

    Spectrogram(QLabel*, int, double);
    ~Spectrogram();
    void setFreqRange(unsigned int, unsigned int);
    QImage generatePalette(unsigned int, unsigned int);
    QImage generateFreqScale(unsigned int, unsigned int);

signals:

    void scalingSpectrogram(QString, int);
    void scalingDone();

public slots:

    void draw(double*);
    void adjustBrightness(int);
    void adjustContrast(int);

private:
    
    int hertzToPixel(double, unsigned int);
    double pixelToHertz(int, unsigned int);
    long computeColor(double);
    double dbfs(double);


    QLabel *label;
    int frameSize, current, fftSize;
    int pixelHeight;
    int frameCount;

    unsigned int freqFrom, freqTo;
    
    Palette palette;
    QImage *image;
    double **frames;
    double *dframe;

    double df;
    
    bool allFrames;
    bool freqChanged;
    double average;
    double max;
};

#endif // SPECTROGRAM_H
