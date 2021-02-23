#include "spectrogram.h"

Spectrogram::Spectrogram(QLabel *label, int fftSize, double sampleRate) : label(label), frameSize(fftSize / 2), current(0) {
    
    frameCount = 864;
    pixelHeight = label->height();

    image = new QImage(frameCount, frameSize, QImage::Format_RGB16);

    frames = new double* [frameCount];
    dframe = new double[pixelHeight];

    for(int i = 0; i < frameCount; ++i) {
        
        frames[i] = new double[frameSize]; 
    }

    df = sampleRate / fftSize;
    allFrames = false;
    average = 0;
    freqChanged = true;
    max = -std::numeric_limits<double>::max();
}

Spectrogram::~Spectrogram() {

    for(int i = 0; i < frameCount; ++i) {

        delete frames[i];
    }

    delete image;
    delete frames;
    delete dframe;
}

QImage Spectrogram::generatePalette(unsigned int width, unsigned int height) {
    
    QImage paletteImage(width, height, QImage::Format_RGB16);

    for(unsigned int i = 0; i < width; ++i)
    {
        for(unsigned int j = 0; j < height; ++j)
        {
            double s = (double)(height - j) / height;
            paletteImage.setPixel(i, j, palette[(int)(s * 255)].rgb());
        }
    }

    return paletteImage;
}

QImage Spectrogram::generateFreqScale(unsigned int width, unsigned int height) {

    QImage freqScaleImage(width, height, QImage::Format_RGB16);
    QPainter p;
    p.begin(&freqScaleImage);
    p.fillRect(0, 0, width, height, QColor(50, 50, 50));
    
    QPen pen;
    pen.setColor(QColor(255, 255, 255));
    p.setPen(pen);
    QFont f = p.font();
    f.setPixelSize(12);
    p.setFont(f);
    
    /*int px = hertzToPixel(1000, height);
    std::cout << px << " " << pixelToHertz(px, height) << std::endl;
    p.drawLine(45, height - px, 50, height - px);
    p.drawLine(0, height - px, 5, height - px);
    p.drawText(5, height - px - 20, 40, 40, Qt::AlignCenter, QString::number(1000));
    */

    p.end();

    /*
        for(int i = 0, j = 0; i < height; i += (fr * (1/df) * height) / frameSize, ++j)
        {
            p.drawLine(35, height - i, 40, height - i);
            p.drawText(0, height - i - 20, 35, 40, Qt::AlignCenter, QString::number(j * fr));
        }
    */
   
    return freqScaleImage;
}

double Spectrogram::dbfs(double data) {

    return 10 * log10(abs(data) / max);
}

long Spectrogram::computeColor(double dbfs)
{
    return ((dbfs - average) / abs(average)) * 255;
}

int Spectrogram::hertzToPixel(double hertz, unsigned int height) {
    
    return (hertz * height) / (df * frameSize);
}

double Spectrogram::pixelToHertz(int pixel, unsigned int height) {
    
    return (pixel * df * frameSize) / height;
}

void Spectrogram::draw(double *data) {

    /* copying fft data to current buffer */
    memcpy(frames[current], data, frameSize * sizeof(double));
    
    if(pixelHeight != label->height() || freqChanged) // resize or frequency changed
    {
        emit scalingSpectrogram("Rescaling spectrogram...", 0);
        pixelHeight = label->height();
        delete dframe;
        dframe = new double[pixelHeight];
        emit scalingSpectrogram("Spectrogram rescaled", 3000);
        freqChanged = false;
    }

    double valueAverage = 0;
    double f = ((freqTo - freqFrom) / df) / pixelHeight;
    double start = freqFrom / df;
    double end = freqTo / df;

    for(int k = 0; k < pixelHeight; ++k) {

        double m = 0;
        int n = 0;

        for(int j = start + (k * f); j < start + (k * f) + f; ++j) {
            
            if(j >= 0 && j < end) {

                m += frames[current][j];
                n++;
            }
        }

        dframe[k] = m / n;
        if(dframe[k] > max) max = dframe[k];
        double value = dbfs(dframe[k]);

        valueAverage += value;
        if(value > 0) value = 0;
        if(value < average) value = average;
        image->setPixel(frameCount - 1, pixelHeight - k, palette[computeColor(value)].rgb());
    }

    valueAverage /= pixelHeight;
    average = (valueAverage + average) / 2;

    for(int i = 0; i < frameCount - 1; ++i) {

        for(int j = 0; j < pixelHeight; ++j) {

            image->setPixel(i, j, image->pixel(i + 1, j));
        }
    } 

    label->setPixmap(QPixmap::fromImage(*image).copy(0, 0, frameCount, pixelHeight).scaled(label->width(), label->height(), Qt::IgnoreAspectRatio));

    current++;

    if(current >= frameCount) {

        allFrames = true;
        current = 0;
    }
}

void Spectrogram::adjustBrightness(int value) {
    
    palette.setBrightness(value);
}

void Spectrogram::adjustContrast(int value) {
    
    palette.setContrast(value);
}

void Spectrogram::setFreqRange(unsigned int freqFrom, unsigned int freqTo) {
    
    if(this->freqFrom != freqFrom || this->freqTo != freqTo) {

        this->freqFrom = freqFrom;
        this->freqTo = freqTo;
        freqChanged = true;
    }
}
