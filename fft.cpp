#include "fft.h"

FFT::FFT(int fftSize, double sampleRate) : fftSize(fftSize), sampleRate(sampleRate) {
    
    nfreq = (fftSize / 2) + 1;

    result = new double [nfreq];

    /* allocating space for the input and for the window */
    input = (double *) fftw_malloc(2 * (size_t) fftSize * sizeof(double));
    output = (fftw_complex *) fftw_malloc((size_t) nfreq * sizeof(fftw_complex));
    window = compute_hamming(input + fftSize, fftSize);

    plan = fftw_plan_dft_r2c_1d(fftSize, input, output, FFTW_ESTIMATE);
}

FFT::~FFT()
{
    delete[] result;
    fftw_destroy_plan(plan);
    fftw_free(output);
    fftw_free(input);
}

/* Returns the N-point Hamming window.
 * w(i) = 0.54 - 0.46 * cos(2*pi*i/(N-1)), for i = 0..N-1. */
double* FFT::compute_hamming(double* data, int n) {

    int m = n - 1;
    int h = n & 1 ? (n + 1) / 2 : n / 2;

    if (n == 1) {

        data[0] = 1;
    }

    else {

        for (int i = 0; i < h; ++i) {

           double x = 0.54 - 0.46 * cos(2 * M_PI * i / m);
           data[i] = x;
           data[m - i] = x;
        }
    }

    return data;
}

void FFT::compute(double *data) {


    for (unsigned int i = 0; i < fftSize; ++i) {

        input[i] = window[i] * data[i];
    }

    fftw_execute(plan);

    for (unsigned int i = 1; i < nfreq; ++i) {

        result[i] = sqrt(output[i][0] * output[i][0] + output[i][1] * output[i][1]);
    }

    emit done(result + 1);
}
