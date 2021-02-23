#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include "tcpclient.h"
#include "fft.h"
#include "spectrogram.h"
#include <QSpinBox>
#include <QPushButton>
#include <QStatusBar>
#include <QMainWindow>
#include <QString>
#include <QDialog>
#include <QLineEdit>
#include <QIntValidator>
#include <QDialogButtonBox>
#include "palette.h"

#define FFTSIZE 16384
#define SAMPLERATE 5512.5
#define FREQFROM 0
#define FREQTO 2756

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void updateFreqRange();
    void resetFreqRange();
    void notifyBrightnessChange(int);
    void notifyContrastChange(int);

private:

    void resizeEvent(QResizeEvent*);
    void initialize();

    TcpClient* tcpClient;
    FFT *fft;
    Spectrogram *spectrogram;

    QLabel *spectrogramLabel, *paletteLabel, *scaleLabel, *scaleLabelRight;
    QSlider *brightnessSlider, *contrastSlider;
    QSpinBox *freqFrom, *freqTo;
    QPushButton *freqRangeButton, *defaultFreqRangeButton;


    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *settingsLayout;
    QGridLayout *sliderLayout, *freqLayout;
    QHBoxLayout *spectrogramLayout;

    QLineEdit *host;
    QLineEdit *port;

    bool initialized;

};

#endif // MAINWINDOW_H
