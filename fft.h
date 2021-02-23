#ifndef FFT_H
#define FFT_H

#include <math.h>
#include <fftw3.h>
#include <QObject>
#include <iostream>

class FFT : public QObject
{

    Q_OBJECT

public slots:

    void compute(double*);

signals:

    void done(double*);

public:

    FFT(int, double);
    ~FFT();

private:

    double* compute_hamming(double*, int);

    unsigned int fftSize, nfreq;
    double sampleRate;

    double *input, *window, *result;

    fftw_complex *output;
    fftw_plan plan;
};

#endif // FFT_H
